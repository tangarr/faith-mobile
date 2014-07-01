#include "laboratoriesmodel.h"
#include "computerlab.h"
#include "host.h"
#include "faithMacro.h"
#include "faithcore.h"

LaboratoriesModel::LaboratoriesModel() : QAbstractListModel(0)
{    

}

bool LaboratoriesModel::insertHost(QString lab, QString ip, QString hostname)
{
    int index;
    ComputerLab *l=0;
    for (index=0; index<elements.count(); index++)
    {
        l = static_cast<ComputerLab*>(elements.at(index));
        if (l)
        {
            if (lab < l->name())
            {
                l = 0;
                break;
            }
            else if (lab == l->name()) break;
            else l = 0;
        }
    }
    if (!l)
    {
        l = new ComputerLab(lab, this);
        beginInsertRows(QModelIndex(), index, index);
        elements.append(l);
        endInsertRows();
        connect(l, SIGNAL(diskLayoutStatusChanged(ComputerLab*)), this, SLOT(_labDiskLayoutChanged(ComputerLab*)));
    }
    Host *host = new Host(ip, hostname);
    int host_index = l->indexWhereHostFit(host);
    l->insertHost(host, host_index);

    if (l->isExpanded())
    {
        int new_index = index+host_index;
        beginInsertRows(QModelIndex(), new_index, new_index);
        elements.insert(new_index, host);
        endInsertRows();
        connect(host, SIGNAL(diskLayoutStatusChanged(Host*)), this, SLOT(_hostDiskLayoutChanged(Host*)));
    }
    return true;
}

bool LaboratoriesModel::updateHost(QString ip_str)
{
    Host *host=0;
    quint32 ip = Faithcore::ipFromString(ip_str);
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab && lab->inherits("ComputerLab"))
        {
            foreach (Host* h, lab->hosts()) {
                if (h->ipUint32()==ip)
                {
                    host = h;
                    break;
                }
            }
        }
    }
    if (host)
    {
        host->checkDiskLayoutStatus();
        return true;
    }
    else return false;
}

bool LaboratoriesModel::removeHost(QString ip)
{
    Host *host=0;
    quint32 ip_int = Faithcore::ipFromString(ip);
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab && lab->inherits("ComputerLab"))
        {
            foreach (Host* h, lab->hosts()) {
                if (h->ipUint32()==ip_int)
                {
                    host = h;
                    break;
                }
            }
        }
    }
    if (host)
    {
        int index = elements.indexOf(host);
        if (index!=-1)
        {
            beginRemoveRows(QModelIndex(), index, index);
            disconnect(host, SIGNAL(diskLayoutStatusChanged(Host*)), this, SLOT(_hostDiskLayoutChanged(Host*)));
            elements.removeAt(index);
            endRemoveRows();
        }
        host->lab()->removeHost(host);
        delete host;
        return true;
    }
    else return false;
}

int LaboratoriesModel::selectedHostCount()
{
    int out=0;
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab && lab->inherits("ComputerLab"))
        {
            foreach (Host* h, lab->hosts()) {
                if (h->checked()) out++;
            }
        }
    }
    return out;
}

int LaboratoriesModel::rowCount(const QModelIndex&) const
{
    return elements.count();
}

QVariant LaboratoriesModel::data(const QModelIndex &index, int role) const
{    
    switch (role) {
    case RoleIsParent:
        return QVariant(elements.at(index.row())->isParent());
    case RoleName:
        return elements.at(index.row())->name();
    case RoleIsExpanded:
    {
        ComputerLab *l = static_cast<ComputerLab*>(elements.at(index.row()));
        if (l) return l->isExpanded();
        else return false;
    }
    case RoleIsChecked:
    {
        return elements.at(index.row())->checked();
    }
    case RoleIsPartiallyChecked:
    {
        ComputerLab *l = static_cast<ComputerLab*>(elements.at(index.row()));
        if (l) return l->isPartiallyChecked();
        else return QVariant();
    }
    case RoleHasDiskLayout:
    {
        ComputerLab *l = static_cast<ComputerLab*>(elements.at(index.row()));
        if (l) return l->hasValidPartitionLayout();
        else return QVariant();
    }
    case RoleDiskLayoutStatus:
    {
        Host *h = static_cast<Host *>(elements.at(index.row()));
        if (h) return h->diskLayoutStatus();
        else return QVariant();
    }
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> LaboratoriesModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty())
    {
        roles[RoleIsParent] = "isParent";
        roles[RoleName] = "name";
        roles[RoleIsExpanded] = "isExpanded";
        roles[RoleIsChecked] = "isChecked";
        roles[RoleIsPartiallyChecked] = "isPartiallyChecked";
        roles[RoleHasDiskLayout] = "hasDiskLayout";
        roles[RoleDiskLayoutStatus] = "diskLayoutStatus";
    }
    return roles;

}

QList<quint32> LaboratoriesModel::selectedHosts()
{
    QList<quint32> out;
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab && lab->inherits("ComputerLab"))
        {
            foreach (Host* h, lab->hosts()) {
                if (h->checked()) out.append(h->ipUint32());
            }
        }
    }
    return out;
}

bool LaboratoriesModel::updateLaboratoryConfig(QString name)
{
    ComputerLab* l=0;
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab && lab->inherits("ComputerLab") && lab->name()==name)
        {
            l=lab;
            break;
        }
    }
    if (l)
    {
        l->readConfiguration();
        l->checkDiskLayout();
        return true;
    }
    else return false;
}

bool LaboratoriesModel::removeLaboratoryConfig(QString name)
{
    ComputerLab* l=0;
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab && lab->inherits("ComputerLab") && lab->name()==name)
        {
            l=lab;
            break;
        }
    }
    if (l)
    {
        l->clear();
        l->checkDiskLayout();
        return true;
    }
    else return false;
}

void LaboratoriesModel::_labDiskLayoutChanged(ComputerLab *e)
{
    int index = elements.indexOf(e);
    if (index!=-1)
    {
        QVector<int> vec;
        vec << RoleHasDiskLayout;
        QModelIndex m_index = createIndex(index,index);
        emit dataChanged(m_index, m_index, vec);
    }
}

void LaboratoriesModel::_hostDiskLayoutChanged(Host *e)
{
    int index = elements.indexOf(e);
    if (index!=-1)
    {
        QVector<int> vec;
        vec << RoleDiskLayoutStatus;
        QModelIndex m_index = createIndex(index,index);
        emit dataChanged(m_index, m_index, vec);
    }
}

bool LaboratoriesModel::expand(int index)
{
    ComputerLab *l = static_cast<ComputerLab*>(elements.at(index));
    if (l && l->inherits("ComputerLab"))
    {
        if (l->isExpanded()) return false;
        else
        {
            int rows = l->expand();
            if (rows > 0)
            {
                beginInsertRows(QModelIndex(), index+1, index+rows);
                QList<Host *> hosts = l->hosts();
                for (int i=0; i<hosts.count(); i++)
                {                    
                    elements.insert(index+i+1, hosts.at(i));
                    connect(hosts.at(i), SIGNAL(diskLayoutStatusChanged(Host*)), this, SLOT(_hostDiskLayoutChanged(Host*)));
                }
                endInsertRows();
            }
            return true;
        }
    }
    else return false;
}

bool LaboratoriesModel::colapse(int index)
{
    ComputerLab *l = static_cast<ComputerLab*>(elements.at(index));
    if (l && l->inherits("ComputerLab"))
    {
        if (!l->isExpanded()) return false;
        else
        {
            int rows = l->colapse();
            if (rows > 0)
            {
                beginRemoveRows(QModelIndex(), index+1, index+rows);
                for (int i=0; i<rows; i++)
                {
                    Host *host = static_cast<Host*>(elements.at(index+1));
                    disconnect(host, SIGNAL(diskLayoutStatusChanged(Host*)), this, SLOT(_hostDiskLayoutChanged(Host*)));
                    elements.removeAt(index+1);                    
                }
                endRemoveRows();
            }
            return true;
        }
    }
    else return false;
}

void LaboratoriesModel::changeCheckedLab(int index, bool checked)
{
    ComputerLab *l = static_cast<ComputerLab *>(elements.at(index));
    if (l && l->inherits("ComputerLab"))
    {
        l->setChecked(checked);
        foreach (Host* h, l->hosts()) {
            h->setChecked(checked);
        }
        if (l->isExpanded())
        {
            QModelIndex first = this->index(index);
            QModelIndex last = this->index(index+l->hostsCount());
            QVector<int> roles;
            roles << RoleIsChecked << RoleIsPartiallyChecked;
            emit dataChanged(first, last, roles);
        }
        emit selectedHostCountChanged(selectedHostCount());
    }
}

void LaboratoriesModel::changeCheckedHost(int index, bool checked)
{
    Host *h = static_cast<Host *>(elements.at(index));
    if (h && h->inherits("Host"))
    {
        ComputerLab *l = h->lab();
        bool p0 = l->isPartiallyChecked();
        h->setChecked(checked);
        bool p1 = l->isPartiallyChecked();
        QVector<int> roles;
        roles << RoleIsChecked << RoleIsPartiallyChecked;
        QModelIndex first = this->index(index);
        emit dataChanged(first, first, roles);
        if (p0!=p1)
        {
            l->setChecked(p1 || checked);
            first = this->index(elements.indexOf(l));
            emit dataChanged(first, first, roles);
        }
        emit selectedHostCountChanged(selectedHostCount());
    }
}

QObject *LaboratoriesModel::getLab(int index)
{
    ComputerLab *l = static_cast<ComputerLab *>(elements.at(index));
    if (l && l->inherits("ComputerLab"))
    {
        return l;
    }
    else return 0;
}
