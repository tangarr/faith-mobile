#ifndef PARTITION_H
#define PARTITION_H

#include <QObject>
#include <QDataStream>

class Disk;

class Partition : public QObject
{
    Q_OBJECT
    Disk* _disk;
    bool _primary;
    bool _bootable;
    int _minSize;
    int _maxSize;
    QString _mountpoint;
    QString _fstype;
    bool _preserve;

    Q_PROPERTY(bool isBootable READ isBootable WRITE setBootable NOTIFY isBootableChanged)    
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString fstype READ fstype WRITE setFstype NOTIFY fstypeChanged)
    Q_PROPERTY(QString mountpoint READ mountpoint WRITE setMountpoint NOTIFY mountpointChanged)
    Q_PROPERTY(int minSize READ minSize WRITE setMinSize NOTIFY minSizeChanged)
    Q_PROPERTY(int maxSize READ maxSize WRITE setMaxSize NOTIFY maxSizeChanged)
    Q_PROPERTY(bool isPreserved READ isPreserved WRITE setIsPreserved NOTIFY isPreservedChanged)

private:
    void _read(QDataStream &stream);
    void _write(QDataStream &stream);
public:
    explicit Partition(Disk* disk);
    Partition(Disk* disk, bool bootable, bool primary, bool preserve, int minSize, int maxSize, QString mountpoint, QString fstype);
    bool isBootable() const;
    QString name();
    QString fstype();
    QString mountpoint();
    int minSize();
    int maxSize();
    int isPreserved();
signals:
    void isBootableChanged(bool arg);
    void nameChanged(QString arg);
    void fstypeChanged(QString arg);
    void mountpointChanged(QString arg);
    void minSizeChanged(int arg);
    void maxSizeChanged(int arg);
    void isPreservedChanged(bool arg);
public slots:
    bool isPrimary() const;
    void setBootable(bool bootable=true);
    void setFstype(QString arg);
    void setMountpoint(QString arg);
    void setMinSize(int arg);
    void setMaxSize(int arg);
    void setIsPreserved(bool arg);


    friend class Disk;
};

#endif // PARTITION_H
