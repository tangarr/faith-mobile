#include "host.h"
#include "computerlab.h"
#include "faithMacro.h"
#include "faithcore.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include "partition.h"

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

DiskLayout::Layout Host::diskLayoutStatus() const
{
    return _diskLayout;
}

void Host::checkDiskLayoutStatus()
{
    DiskLayout::Layout out = DiskLayout::Unknown;

    if (lab()->hasValidPartitionLayout())
    {
        QList<QString*> disks;
        QSqlDatabase db = QSqlDatabase::database();
        QString filename = "config/";
        filename+=QString::number(Faithcore::ipFromString(_ip), 16).rightJustified(8, '0');
        filename+=".db";
        db.setDatabaseName(filename);
        if (db.open())
        {
            out = DiskLayout::Ready;
            QSqlQuery query("select dev, size, pt_type from disk;");
            while (query.next())
            {
                QString *d = new QString[3];
                d[0] = query.value(0).toString();
                d[1] = query.value(1).toString();
                d[2] = query.value(2).toString();
                disks.append(d);
            }
            if (disks.count())
            {
                foreach (QString* arr, disks) {
                    Disk* d = _lab->diskByName(arr[0]);
                    if (d)
                    {
                        int size = Disk::stringToMiB(arr[1]);
                        if (size < d->minimumSize())
                        {
                            out = DiskLayout::NotEnoughtSpace;
                            break;
                        }
                        else
                        {
                            DiskLabel::Label tmp = DiskLabel::MsDos;
                            if (arr[2]=="gpt") tmp = DiskLabel::GPT;
                            if (tmp != d->diskLabel())
                            {
                                out = DiskLayout::NeedRepartition;
                            }
                            else
                            {
                                query.prepare("select size, fs_type, flags, id from partition where dev=?;");
                                query.bindValue(0, arr[0]);
                                query.exec();
                                int i;
                                for (i=0; query.next(); i++)
                                {
                                    Partition *p = static_cast<Partition*>(d->partition(i));
                                    if (p)
                                    {
                                        int size = Disk::stringToMiB(query.value(0).toString());
                                        QString fs_type = query.value(1).toString();
                                        QString flags = query.value(2).toString();
                                        int id = query.value(3).toInt();
                                        if (d->diskLabel()==DiskLabel::MsDos)
                                        {
                                            if (flags.contains("extended"))
                                            {
                                                i--;
                                                continue;
                                            }
                                            bool primary = (id <= 4);
                                            if (primary!=p->isPrimary())
                                            {
                                                out = DiskLayout::NeedRepartition;
                                                break;
                                            }
                                        }
                                        bool bootable = flags.contains("boot");
                                        if (size < p->minSize())
                                        {
                                            out = DiskLayout::NeedRepartition;
                                            break;
                                        }
                                        if (p->maxSize()>0 && size > p->maxSize())
                                        {
                                            out = DiskLayout::NeedRepartition;
                                            break;
                                        }
                                        if (bootable!=p->isBootable())
                                        {
                                            out = DiskLayout::NeedRepartition;
                                            break;
                                        }
                                        if (fs_type!=p->fstype())
                                        {
                                            out = DiskLayout::NeedRepartition;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        out = DiskLayout::NeedRepartition;
                                        break;
                                    }
                                }
                                if (i!=d->partitionCount())
                                {
                                    out = DiskLayout::NeedRepartition;
                                }
                            }
                        }
                    }
                    else
                    {
                        out = DiskLayout::NotEnoughtSpace;
                        break;
                    }
                }

            }
            else out = DiskLayout::NotEnoughtSpace;
            db.close();
        }
        foreach (QString *d, disks) {
            delete [] d;
        }
        disks.clear();
    }

    if (_diskLayout!=out)
    {
        _diskLayout = out;
        emit diskLayoutStatusChanged(this);
    }
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
