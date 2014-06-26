#ifndef COMPUTERLAB_H
#define COMPUTERLAB_H

#include "laboratoriesmodelelement.h"
#include "laboratoriesmodel.h"
#include "host.h"
#include "disk.h"
#include "user.h"

class LaboratoriesModel;

class ComputerLab : public LaboratoriesModelElement
{
    Q_OBJECT
    Q_PROPERTY(int diskCount READ diskCount NOTIFY diskCountChanged)
    Q_PROPERTY(int userCount READ userCount NOTIFY userCountChanged)
private: // fields
    LaboratoriesModel *_parent;
    QString _name;
    bool _expanded;
    QList<Host*> _hosts;
    QList<Disk*> _disks;
    QList<User*> _users;
    QStringList _softwareList;
    QString _rootPasswordHash;
    QByteArray _backup;
private: // functions
    bool _readConfiguration(const QByteArray& data);
    QByteArray &_writeConfiguration() const;
    QString _configurationFilename() const;

public:
    ComputerLab(QString name, LaboratoriesModel* parent);
    bool isParent() const;
    QString name() const;
    int expand();
    int colapse();

    int indexWhereHostFit(Host* host);
    bool insertHost(Host* host, int index);
    bool isExpanded();
    QList<Host *> hosts() const;
    int checkedHosts() const;
    bool isPartiallyChecked() const;        
    int diskCount();
    int userCount();
signals:
    void diskCountChanged(int arg);
    void userCountChanged(int arg);
public slots:
    bool hasValidPartitionLayout();    
    QObject *disk(int index);
    int partitionCount(int diskNumber);
    QString proposedNewDiskName();    
    bool createDisk(QString devname, DiskLabel::Label diskLabel);
    bool createDisk(QString devname, int diskLabel);
    void backupCreate();
    void backupRevert();
    void backupDelete();
    bool readConfiguration();
    bool writeConfiguration();
    bool removeDisk(int index);
    bool removePartition(int diskIndex, int partitionIndex);
    bool removeDisks();
    int hostsCount() const;
    QString hostName(int index) const;
    bool loadDiskSchemaFromHost(int index);

};

#endif // COMPUTERLAB_H
