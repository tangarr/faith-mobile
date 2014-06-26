#include "host.h"
#include "computerlab.h"
#include "faithMacro.h"
#include "faithcore.h"

Host::Host(QString ip, QString hostname)
{
    _ip = ip;
    _hostname = hostname;
    _diskLayout = DiskLayout::Unknown;
}

bool Host::isParent() const
{
    return false;
}

QString Host::name() const
{
    return _ip + " - " +_hostname;
}

void Host::setLab(ComputerLab *lab)
{
    _lab = lab;
}

ComputerLab *Host::lab() const
{
    return _lab;
}

DiskLayout::Layout Host::diskLayout() const
{
    return _diskLayout;
}

void Host::checkDiskLayout()
{
    NOT_IMPLEMENTED_YET;
}

QString Host::configFileName() const
{
    QStringList tmp = _ip.split(".");
    QString out;
    for (int i=0; i<4; i++)
    {
        QVariant octet = tmp[i];
        int x = octet.toInt();
        out+=QString::number(x, 16).rightJustified(2, '0');
    }
    return out+".db";
}

quint32 Host::ipUint32() const
{
    return Faithcore::ipFromString(_ip);
}
