#include "disk.h"
#include "partition.h"
#include <QRegExp>

void Disk::_read(QDataStream &stream)
{
    foreach (Partition *p, _partitions) delete p;
    _partitions.clear();
    int count, label;
    stream >> _devName;
    stream >> label;
    _diskLabel = (DiskLabel::Label) label;
    stream >> count;
    for (int i=0; i<count; i++)
    {
        Partition* p=new Partition(this);
        p->_read(stream);
        _partitions.append(p);
    }
    emit partitionCountChanged(_partitions.count());
}

void Disk::_write(QDataStream &stream)
{
    stream << _devName << (int)_diskLabel << (int)_partitions.count();
    foreach (Partition *p, _partitions) p->_write(stream);
}

Disk::Disk(DiskLabel::Label diskLabel, QString devName) : QObject(0)
{
    _diskLabel = diskLabel;
    _devName = devName;
}

Disk::~Disk()
{
    foreach (Partition* p, _partitions) {
        delete p;
    }
    _partitions.clear();
}

void Disk::emitMinimumSizeChanged()
{
    emit minimumSizeChanged(minimumSize());
}

int Disk::partitionCount() const
{
    return _partitions.count();
}

QObject *Disk::partition(int partitionNumber)
{
    if (partitionNumber>=0 && partitionNumber<_partitions.count())
        return _partitions.at(partitionNumber);
    else
        return 0;

}

QString Disk::devName() const
{
    return _devName;
}

DiskLabel::Label Disk::diskLayout() const
{
    return _diskLabel;
}

QString Disk::partitionName(int partitionNumber)
{
    if (partitionNumber>=_partitions.count() || partitionNumber < 0) return "NOT SUCH DEVICE";
    if (_diskLabel==DiskLabel::GPT)
    {
        return _devName+QString::number(partitionNumber+1);
    }
    else
    {
        Partition* p = _partitions.at(partitionNumber);
        if (p->isPrimary())
        {
            return _devName+QString::number(partitionNumber+1);
        }
        else
        {
            int first_logical = 0;
            for (int i=0; i<_partitions.count(); i++)
            {
                if (!_partitions.at(i)->isPrimary())
                {
                    first_logical = i;
                    break;
                }
            }
            int number = 5 + partitionNumber - first_logical;
            return _devName+QString::number(number);
        }
    }
}

QString Disk::partitionName(Partition *p)
{
    if (_partitions.contains(p))
    {
        int index = _partitions.indexOf(p);
        return partitionName(index);
    }
    else return "";
}

void Disk::setBootablePartition(int index)
{
    if (index>0 && index < _partitions.count())
    {
        for (int i=0; i<partitionCount(); i++)
        {
            _partitions.at(i)->_bootable = false;
        }
        _partitions.at(index)->_bootable = true;
    }
}

void Disk::setBootablePartition(Partition *partition)
{
    for (int i=0; i<partitionCount(); i++)
    {
        _partitions.at(i)->_bootable = false;
    }
    partition->_bootable = true;
}

void Disk::createPartition(int minSize, int maxSize, QString fstype, QString mountpoint, bool bootable, bool preserved)
{
    bool primary=true;
    if (_partitions.count()>0)
    {
        if (!_partitions.last()->isPrimary()) primary=false;
        else if (_partitions.count()>=3) primary=false;
        if (bootable)
        {
            foreach (Partition* p, _partitions) {
                if (p->isBootable()) p->setBootable(false);
            }
        }
    }
    Partition* p = new Partition(this, bootable, primary, preserved, minSize, maxSize, mountpoint, fstype);
    _partitions.append(p);
    emit partitionCountChanged(_partitions.count());
    emit minimumSizeChanged(minimumSize());
}

void Disk::appendPartition(Partition *p)
{
    _partitions.append(p);
    emit partitionCountChanged(_partitions.count());
    emit minimumSizeChanged(minimumSize());
}

bool Disk::removePartition(int index)
{
    if (index>=0 && index<_partitions.count())
    {
        Partition *p = _partitions.at(index);
        delete p;
        _partitions.removeAt(index);
        emit partitionCountChanged(_partitions.count());
        emit minimumSizeChanged(minimumSize());
        return true;
    }
    else return false;
}

bool Disk::canCreatePartition(QString mountpoint)
{
    if (mountpoint.isEmpty()) return false;
    else if (mountpoint.trimmed()=="-") return true;
    else
    {
        QRegExp regexp("/([a-zA-Z0-9_](/[a-zA-Z0-9_]+)*)*");
        if (!regexp.exactMatch(mountpoint.trimmed())) return false;
        else
        {
            foreach (Partition* p, _partitions) {
                if (p->mountpoint().trimmed()==mountpoint.trimmed()) return false;
            }
            return true;
        }
    }
}



int Disk::minimumSize()
{
    int out=0;
    foreach (Partition* p, _partitions) {
        out+=p->minSize();
    }
    return out;
}
