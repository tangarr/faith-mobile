#include "partition.h"
#include "disk.h"
#include <QDebug>

void Partition::_read(QDataStream &stream)
{
    stream >> _primary;
    stream >> _bootable;
    stream >> _minSize;
    stream >> _maxSize;
    stream >> _mountpoint;
    stream >> _fstype;
    stream >> _preserve;
}

void Partition::_write(QDataStream &stream)
{
    stream << _primary;
    stream << _bootable;
    stream << _minSize;
    stream << _maxSize;
    stream << _mountpoint;
    stream << _fstype;
    stream << _preserve;
}

void Partition::_clearParent()
{
    _disk = 0;
}

Partition::Partition(Disk *disk) : QObject(0)
{
    _disk = disk;
}

Partition::~Partition()
{
    if (_disk) _disk->_deletePartitionFromList(this);
}

Partition::Partition(Disk *disk, bool bootable, bool primary, bool preserve, int minSize, int maxSize, QString mountpoint, QString fstype) : QObject(0)
{
    _disk=disk;
    _bootable=bootable;
    _primary=primary;
    _preserve=preserve;
    _minSize=minSize;
    _maxSize=maxSize;
    _mountpoint=mountpoint;
    _fstype=fstype;
}

bool Partition::isBootable() const
{
    return _bootable;
}

QString Partition::name()
{
    QString name = "";
    if (_disk)
    {
        name= _disk->partitionName(this);
    }
    return name;
}

QString Partition::fstype()
{
    return _fstype;
}

QString Partition::mountpoint()
{
    return _mountpoint;
}

int Partition::minSize()
{
    return _minSize;
}

int Partition::maxSize()
{
    return _maxSize;
}

int Partition::isPreserved()
{
    return _preserve;
}

bool Partition::isPrimary() const
{
    return _primary;
}

void Partition::setBootable(bool bootable)
{
    if (bootable)
    {
        _disk->setBootablePartition(this);
    }
    else
    {
        _bootable = bootable;
    }
    emit isBootableChanged(bootable);
}

void Partition::setFstype(QString arg)
{
    if (_fstype != arg)
    {
        _fstype = arg;
        emit fstypeChanged(arg);
    }
}

void Partition::setMountpoint(QString arg)
{
    if (_mountpoint!=arg)
    {
        _mountpoint=arg;
        emit mountpointChanged(arg);
    }
}

void Partition::setMinSize(int arg)
{
    if (_minSize!=arg)
    {
        _minSize=arg;
        emit minSizeChanged(arg);
        _disk->emitMinimumSizeChanged();
    }
}

void Partition::setMaxSize(int arg)
{
    if (_maxSize!=arg)
    {
        _maxSize=arg;
        emit maxSizeChanged(arg);
    }

}

void Partition::setIsPreserved(bool arg)
{
    if (_preserve != arg)
    {
        _preserve=arg;
        emit isPreservedChanged(arg);
    }
}
