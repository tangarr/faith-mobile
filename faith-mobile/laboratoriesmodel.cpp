#include "laboratoriesmodel.h"
#include "computerlab.h"
#include "host.h"
#include "faithMacro.h"

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
    }
    return true;
}

bool LaboratoriesModel::updateHost(QString ip)
{
    NOT_IMPLEMENTED_YET
}

bool LaboratoriesModel::removeHost(QString ip)
{
    NOT_IMPLEMENTED_YET
}

int LaboratoriesModel::selectedHostCount()
{
    int out=0;
    foreach (LaboratoriesModelElement *el, elements) {
        ComputerLab* lab = static_cast<ComputerLab*>(el);
        if (lab->inherits("ComputerLab"))
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
        if (lab->inherits("ComputerLab"))
        {
            foreach (Host* h, lab->hosts()) {
                if (h->checked()) out.append(h->ipUint32());
            }
        }
    }
    return out;
}

bool LaboratoriesModel::expand(int index)
{
    ComputerLab *l = static_cast<ComputerLab*>(elements.at(index));
    if (l)
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
    if (l)
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
    if (l)
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
    if (h)
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
    if (l)
    {
        return l;
    }
    else return 0;
}
