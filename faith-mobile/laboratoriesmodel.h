#ifndef LABORATORIESMODEL_H
#define LABORATORIESMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "laboratoriesmodelelement.h"

class ComputerLab;

class DiskLayout : public QObject
{
    Q_OBJECT
    Q_ENUMS(Layout)
public:
    enum Layout
    {
        Unknown,
        Ready,
        NeedRepartition,
        NotEnoughtSpace
    };
};

class LaboratoriesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int selectedHostCount READ selectedHostCount NOTIFY selectedHostCountChanged)

    QList<LaboratoriesModelElement*> elements;

public:
    enum LaboratoriesElementRoles
    {
        RoleIsParent = Qt::UserRole+1,
        RoleName,
        RoleIsExpanded,
        RoleIsChecked,
        RoleIsPartiallyChecked,
        RoleHasDiskLayout,
        RoleDiskLayoutStatus
    };

    LaboratoriesModel();
    bool insertHost(QString lab, QString ip, QString hostname);
    bool updateHost(QString ip);
    bool removeHost(QString ip);
    int selectedHostCount();
    int rowCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

signals:
    void selectedHostCountChanged(int arg);

public slots:
    int count() { return elements.count(); }
    bool expand(int index);
    bool colapse(int index);
    void changeCheckedLab(int index, bool checked);
    void changeCheckedHost(int index, bool checked);
    QObject *getLab(int index);
};

#endif // LABORATORIESMODEL_H
