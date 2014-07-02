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
    bool _hasValidDiskLayout;
private: // functions
    bool _readConfiguration(const QByteArray& data);
    QByteArray &_writeConfiguration() const;
    QString _configurationFilename() const;
    bool _writeDiskLayoutConfiguration();

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
    Disk *diskByName(QString devName);
    void removeHost(Host* h);
    void clear();
signals:
    void diskCountChanged(int arg);
    void userCountChanged(int arg);
    void diskLayoutStatusChanged(ComputerLab*);
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
    bool removeUsers();
    int hostsCount() const;
    QString hostName(int index) const;
    bool loadDiskSchemaFromHost(int index);
    void checkDiskLayout();
    void changeSoft(QString soft, bool checked);
    bool containsSoft(QString soft);
    bool hasRootPassword();
    void setRootPassword(QString password);
    bool userExist(QString username);
    bool addUser(QString username, QString password, QString shell, QString home);
    bool updateUser(QString username, QString password, QString shell, QString home);
    bool updateUser(int index, QString password, QString shell, QString home);
    bool removeUser(QString username);
    bool removeUser(int index);
    QObject* user(int index);
};

#endif // COMPUTERLAB_H
