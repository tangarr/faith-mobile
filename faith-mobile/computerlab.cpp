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
    qDebug() << Q_FUNC_INFO << _disks.count();
    if (!data.size()) return false;
    foreach (Disk *d, _disks) delete d;
    foreach (User *u, _users) delete u;
    _disks.clear();
    _users.clear();

    QDataStream stream(data);
    int count;
    stream >> _rootPasswordHash >> _softwareList;
    stream >> count;
    for (int i=0; i<count; i++)
    {
        Disk *d = new Disk(DiskLabel::MsDos, "dev");
        d->_read(stream);
        _disks.append(d);
    }
    emit diskCountChanged(_disks.count());
    stream >> count;
    for (int i=0; i<count; i++)
    {
        User *u = new User();
        u->_read(stream);
        _users.append(u);
    }
    return true;
}

QByteArray &ComputerLab::_writeConfiguration() const
{    
    qDebug() << _disks.count();
    QByteArray *array = new QByteArray();
    QDataStream stream(array, QIODevice::WriteOnly);
    stream << _rootPasswordHash << _softwareList;
    stream << (int)_disks.count();
    foreach (Disk *d, _disks) d->_write(stream);
    stream << (int)_users.count();
    foreach (User *u, _users) u->_write(stream);
    return *array;
}

QString ComputerLab::_configurationFilename() const
{
    return "config/"+_name+".labconfig";
}

ComputerLab::ComputerLab(QString name, LaboratoriesModel *parent)
{
    _name = name;
    _parent = parent;
    _expanded = false;
    readConfiguration();
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
    if (hasValidPartitionLayout()) host->checkDiskLayout();
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
    NOT_IMPLEMENTED_YET
    return false;
}

int ComputerLab::diskCount()
{
    return _disks.count();
}

int ComputerLab::userCount()
{
    return _users.count();
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
    Disk *d = new Disk(diskLabel, devname);
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
    return (msg.getMessageCode()==Faithcore::OK);
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

QString ComputerLab::hostName(int index) const
{
    if (index>=0 && index<_hosts.count()) return _hosts.at(index)->name();
    else return "";
}

int stringToMB(QString text)
{
    for (int i=text.length()-1; i>=0 ; i--)
    {
        if (text[i].isDigit() || text[i]=='.')
        {
            QString unit = text.mid(i+1);
            QString number = text.left(i+1);

            float tmp = QVariant(number).toFloat();
            if (unit == "TB")
            {
                tmp*=1000000;
            }
            if (unit == "GB")
            {
                tmp*=1000;
            }
            else if (unit == "KB")
            {
                tmp/=1000;
            }
            else if (unit == "B")
            {
                tmp/=1000000;
            }
            int ret = (int)tmp;
            qDebug() << number<< unit << ret;
            return ret;
        }
    }
    return 0;
}

bool ComputerLab::loadDiskSchemaFromHost(int index)
{
    bool ret = false;
    qDebug() << Q_FUNC_INFO;
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
                qDebug() << tmp;
                DiskLabel::Label label = DiskLabel::MsDos;
                if (tmp.at(1)!="msdos") label = DiskLabel::GPT;
                createDisk(tmp.at(0), label);
                Disk* disk = _disks.last();
                query.prepare("select size, fs_type, flags from partition where dev=? order by id;");
                query.bindValue(0, tmp.at(0));
                query.exec();
                bool primary = true;
                while (query.next()) {
                    int size = stringToMB(query.value(0).toString());
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

