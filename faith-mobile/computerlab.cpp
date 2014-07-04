#include "computerlab.h"
#include "faithMacro.h"
#include <QFile>
#include <QDataStream>
#include "faithmessage.h"
#include "config.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "disk.h"
#include "partition.h"

bool ComputerLab::_readConfiguration(const QByteArray &data)
{
    if (!data.size()) return false;
    clear();

    QDataStream stream(data);
    int count;
    stream >> _rootPasswordHash >> _softwareList;
    stream >> count;
    for (int i=0; i<count; i++)
    {
        Disk *d = new Disk(DiskLabel::MsDos, "dev", this);
        d->_read(stream);
        _disks.append(d);
    }
    emit diskCountChanged(_disks.count());
    stream >> count;
    for (int i=0; i<count; i++)
    {
        User *u = new User(this);
        u->_read(stream);
        _users.append(u);
    }
    stream >> _softwareList;
    return true;
}

QByteArray &ComputerLab::_writeConfiguration() const
{
    QByteArray *array = new QByteArray();
    QDataStream stream(array, QIODevice::WriteOnly);
    stream << _rootPasswordHash << _softwareList;
    stream << (int)_disks.count();
    foreach (Disk *d, _disks) d->_write(stream);
    stream << (int)_users.count();
    foreach (User *u, _users) u->_write(stream);
    stream << _softwareList;
    return *array;
}

QString ComputerLab::_configurationFilename() const
{
    return "config/"+_name+".labconfig";
}

bool ComputerLab::_writeDiskLayoutConfiguration()
{
    QString file_path = "config/"+_name.toUpper()+".diskconfig";
    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream stream(&file);
    foreach (Disk* d, _disks) {
        QString dev = d->devName();
        QString disklabel = (d->diskLabel()==DiskLabel::MsDos)?"msdos":"gpt";
        QStringList preserve;
        QString boot;
        foreach (Partition* p, d->partitions()) {
            if (p->isBootable()) boot=p->name();
            if (p->isPreserved()) preserve.append(p->name());
        }
        QString line = "disk_config "+dev+" ";
        if (!preserve.isEmpty())
        {
            line+= "preserve_lazy:";
            foreach (QString str, preserve) {
                line+=str.remove(dev)+",";
            }
            line.remove(line.length()-1, 1);
            line+=" ";
        }
        line+="disklabel:"+disklabel+" ";
        if (!boot.isEmpty()) line+="bootable:"+boot.remove(dev);
        line += "\n";
        stream << line << "\n";
        foreach (Partition *p, d->partitions()) {
            int minSize = p->minSize() -1;
            line = (p->isPrimary())?"primary ":"logical ";
            line+= (p->mountpoint().isEmpty())?"- ":p->mountpoint()+" ";
            line+= QString::number(minSize)+"MiB";
            if (p->maxSize()==0) line+="- ";
            else if (p->maxSize()>minSize) line+="-"+QString::number(p->maxSize())+"MiB ";
            else line+=" ";
            line+= p->fstype()+" ";
            if (p->mountpoint().isEmpty() || p->mountpoint()=="-") line+="-";
            else line+="defaults";
            stream << line << "\n";
        }
    }
    file.close();
    QString address = Config::instance()->server_address();
    int port = Config::instance()->server_port();
    QTcpSocket socket;
    socket.connectToHost(address, port);
    if (socket.state()!=QTcpSocket::ConnectedState) socket.waitForConnected();
    if (socket.state()!=QTcpSocket::ConnectedState)
    {
        return false;
    }
    FaithMessage::MsgSendFile(file_path).send(&socket);
    FaithMessage msg;
    msg.recive(&socket);
    socket.disconnectFromHost();
    if (socket.state()!=QTcpSocket::UnconnectedState) socket.waitForDisconnected();
    return  (msg.getMessageCode()==Faithcore::OK);
}

ComputerLab::ComputerLab(QString name, LaboratoriesModel *parent)
{
    _name = name;
    _parent = parent;
    _expanded = false;
    readConfiguration();
    checkDiskLayout();
    _softwareList.append("BASE");
}

bool ComputerLab::isParent() const
{
    return true;
}

QString ComputerLab::name() const
{
    return _name;
}

int ComputerLab::expand()
{    
    if (_expanded) return 0;
    else
    {
        _expanded = true;
        return _hosts.count();
    }
}

int ComputerLab::colapse()
{
    if (_expanded)
    {
        _expanded = false;
        return _hosts.count();
    }
    else return 0;

}

int ComputerLab::hostsCount() const
{
    return _hosts.count();
}

int ComputerLab::indexWhereHostFit(Host *host)
{
    int index;
    for (index=0; index<_hosts.count(); index++)
    {
        if (host->name()<_hosts.at(index)->name()) break;
    }
    return index;
}

bool ComputerLab::insertHost(Host *host, int index)
{
    host->setLab(this);
    _hosts.insert(index, host);
    if (hasValidPartitionLayout()) host->checkDiskLayoutStatus();
    return true;
}

bool ComputerLab::isExpanded()
{
    return _expanded;
}

QList<Host *> ComputerLab::hosts() const
{
    return _hosts;
}

int ComputerLab::checkedHosts() const
{
    int x=0;
    foreach (Host* h, hosts()) {
        if (h->checked()) x++;
    }
    return x;
}

bool ComputerLab::isPartiallyChecked() const
{
    int x = checkedHosts();
    if (x==0 || x==hosts().count()) return false;
    else return true;
}

bool ComputerLab::hasValidPartitionLayout()
{
    return _hasValidDiskLayout;
}

int ComputerLab::diskCount()
{
    return _disks.count();
}

int ComputerLab::userCount()
{
    return _users.count();
}

Disk *ComputerLab::diskByName(QString devName)
{
    foreach (Disk *d, _disks) {
        if (d->devName()==devName) return d;
    }
    return 0;
}

void ComputerLab::removeHost(Host *h)
{
    _hosts.removeOne(h);
}

void ComputerLab::clear()
{
    removeDisks();
    removeUsers();
    _rootPasswordHash="";
    _softwareList.clear();
    _softwareList.append("BASE");
}

QObject *ComputerLab::disk(int index)
{
    if (index >= 0 && index < _disks.count())
    {
        return _disks.at(index);
    }
    else return 0;
}

int ComputerLab::partitionCount(int diskNumber)
{
    if (diskNumber >= 0 && diskNumber < _disks.count())
    {
        return _disks.at(diskNumber)->partitionCount();
    }
    else return 0;
}

QString ComputerLab::proposedNewDiskName()
{
    if (_disks.empty()) return "sda";
    else
    {
        QString last = _disks.last()->devName();
        return last.left(last.length()-1).append(QChar(last.at(last.length()-1).unicode()+1));
    }
}

bool ComputerLab::createDisk(QString devname, DiskLabel::Label diskLabel)
{
    foreach (Disk *d, _disks) {
        if (d->devName() == devname) return false;
    }
    Disk *d = new Disk(diskLabel, devname, this);
    _disks.append(d);
    emit diskCountChanged(_disks.count());
    return true;
}

bool ComputerLab::createDisk(QString devname, int diskLabel)
{
    return createDisk(devname, (DiskLabel::Label)diskLabel);
}

void ComputerLab::backupCreate()
{
    _backup = _writeConfiguration();
}

void ComputerLab::backupRevert()
{
    if (_backup.size())
        _readConfiguration(_backup);
}

void ComputerLab::backupDelete()
{
    _backup.clear();
}

bool ComputerLab::readConfiguration()
{
    QFile file(_configurationFilename());
    if (!file.open(QIODevice::ReadOnly)) return false;
    QByteArray tmp = file.readAll();
    file.close();
    return _readConfiguration(tmp);
}

bool ComputerLab::writeConfiguration()
{
    QFile file(_configurationFilename());
    if (!file.open(QIODevice::WriteOnly)) return false;
    QByteArray tmp = _writeConfiguration();
    file.write(tmp);
    file.close();

    QString address = Config::instance()->server_address();
    int port = Config::instance()->server_port();
    QTcpSocket socket;
    socket.connectToHost(address, port);
    if (socket.state()!=QTcpSocket::ConnectedState) socket.waitForConnected();
    if (socket.state()!=QTcpSocket::ConnectedState)
    {
        qDebug() << "error: " << socket.errorString();
        return false;
    }
    FaithMessage::MsgSendFile(_configurationFilename()).send(&socket);
    FaithMessage msg;
    msg.recive(&socket);
    socket.disconnectFromHost();
    if (socket.state()!=QTcpSocket::UnconnectedState) socket.waitForDisconnected();
    if (msg.getMessageCode()!=Faithcore::OK) return false;
    else return _writeDiskLayoutConfiguration();
}

bool ComputerLab::removeDisk(int index)
{
    if (index>=0 && index<_disks.count())
    {
        Disk* d = _disks.at(index);
        delete d;
        _disks.removeAt(index);
        emit diskCountChanged(_disks.count());
        return true;
    }
    else return true;
}

bool ComputerLab::removePartition(int diskIndex, int partitionIndex)
{
    if (diskIndex>=0 && diskIndex<_disks.count())
    {
        return _disks.at(diskIndex)->removePartition(partitionIndex);
    }
    return false;
}

bool ComputerLab::removeDisks()
{
    if (_disks.count()>0)
    {
        foreach (Disk* d, _disks) {
            delete d;
        }
        _disks.clear();
        emit diskCountChanged(0);
        return true;
    }
    else return false;
}

bool ComputerLab::removeUsers()
{
    if (_users.count())
    {
        foreach (User *u, _users) {
            delete u;
        }
        _users.clear();
        return true;
    }
    else return false;
}

QString ComputerLab::hostName(int index) const
{
    if (index>=0 && index<_hosts.count()) return _hosts.at(index)->name();
    else return "";
}

bool ComputerLab::loadDiskSchemaFromHost(int index)
{
    bool ret = false;
    removeDisks();
    emit diskCountChanged(_disks.count());
    if (index>=0 && index<_hosts.count())
    {
        QString filename = "config/"+_hosts.at(index)->configFileName();
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(filename);
        if (db.open())
        {
            QStringList diskList;
            QSqlQuery query("select dev, pt_type from disk;");
            while (query.next()) {
                diskList << query.value(0).toString()+"#"+query.value(1).toString();
            }

            foreach (QString d, diskList) {
                QStringList tmp = d.split("#");
                DiskLabel::Label label = DiskLabel::MsDos;
                if (tmp.at(1)!="msdos") label = DiskLabel::GPT;
                createDisk(tmp.at(0), label);
                Disk* disk = _disks.last();
                query.prepare("select size, fs_type, flags from partition where dev=? order by id;");
                query.bindValue(0, tmp.at(0));
                query.exec();
                bool primary = true;
                while (query.next()) {
                    int size = Disk::stringToMiB(query.value(0).toString());
                    QString fstype = query.value(1).toString();
                    QString flags = query.value(2).toString();
                    if (flags.contains("extended"))
                    {
                        primary = false;
                        continue;
                    }
                    Partition *p = new Partition(disk, flags.contains("boot"), primary, false, size, 0, "-", fstype);
                    disk->appendPartition(p);
                }
            }
            ret = true;
            db.close();
        }
    }
    emit diskCountChanged(_disks.count());
    return ret;
}

void ComputerLab::checkDiskLayout()
{
    bool oldLayout = _hasValidDiskLayout;
    _hasValidDiskLayout = false;
    foreach (Disk* d, _disks)
    {
        if (_hasValidDiskLayout) break;
        foreach (Partition* p, d->partitions()) {
            if(p->mountpoint()=="/")
            {
                _hasValidDiskLayout = true;
                break;
            }
        }
    }
    if (_hasValidDiskLayout!=oldLayout)
    {
        emit diskLayoutStatusChanged(this);
    }
    foreach (Host* h, _hosts) {
        h->checkDiskLayoutStatus();
    }
}

void ComputerLab::changeSoft(QString soft, bool checked)
{
    if (checked)
    {
        if (!_softwareList.contains(soft)) _softwareList.append(soft);
    }
    else
    {
        _softwareList.removeAll(soft);
    }
}

bool ComputerLab::containsSoft(QString soft)
{
    return _softwareList.contains(soft);
}

bool ComputerLab::hasRootPassword()
{
    return !_rootPasswordHash.isEmpty();
}

void ComputerLab::setRootPassword(QString password)
{
    qDebug() << _rootPasswordHash;
    _rootPasswordHash = User::hashPassword(password);
}

bool ComputerLab::userExist(QString username)
{
    if (username=="root") return true;
    foreach (User *u, _users) {
        if (u->username()==username) return true;
    }
    return false;
}

bool ComputerLab::addUser(QString username, QString password, QString shell, QString home)
{
    if (userExist(username)) return false;
    User *u = new User(this);
    u->setUsername(username);
    u->setPassword(password);
    u->setHomeDir(home);
    u->setShell(shell);
    _users.append(u);
    emit userCountChanged(_users.count());
    return true;
}

bool ComputerLab::updateUser(QString username, QString password, QString shell, QString home)
{
    foreach (User *u, _users) {
        if (u->username()==username)
        {
            if (!password.isEmpty()) u->setPassword(password);
            u->setShell(shell);
            u->setHomeDir(home);
            return true;
        }
    }
    return false;
}

bool ComputerLab::updateUser(int index, QString password, QString shell, QString home)
{
    if (index>=0 && index < _users.count())
    {
        User *u = _users.at(index);
        if (!password.isEmpty()) u->setPassword(password);
        u->setShell(shell);
        u->setHomeDir(home);
        return true;
    }
    else return false;

}

bool ComputerLab::removeUser(QString username)
{
    foreach (User *u, _users) {
        if (u->username()==username)
        {
            _users.removeAll(u);
            delete u;
            emit userCountChanged(_users.count());
            return true;
        }
    }
    return false;
}

bool ComputerLab::removeUser(int index)
{
    if (index>=0 && index < _users.count())
    {
        User *u = _users.at(index);
        delete u;
        _users.removeAt(index);
        emit userCountChanged(_users.count());
        return true;
    }
    else return false;
}

QObject *ComputerLab::user(int index)
{
    if (index>=0 && index < _users.count()) return _users.at(index);
    else return 0;
}
