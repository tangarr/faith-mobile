#ifndef DISK_H
#define DISK_H

#include <QObject>
#include <QList>

class Partition;

class DiskLabel : public QObject
{
    Q_OBJECT
    Q_ENUMS(Label)
public:
    enum Label
    {
        MsDos,
        GPT
    };
};

class Disk : public QObject
{
public:

private:
    Q_OBJECT
    QList <Partition*> _partitions;
    DiskLabel::Label _diskLabel;
    QString _devName;    
    Q_PROPERTY(int partitionCount READ partitionCount NOTIFY partitionCountChanged)
    Q_PROPERTY(int minimumSize READ minimumSize NOTIFY minimumSizeChanged)
private:
    void _read(QDataStream &stream);
    void _write(QDataStream &stream);
public:
    explicit Disk(DiskLabel::Label diskLabel, QString devName, QObject* parent);
    ~Disk();
    void emitMinimumSizeChanged();
    const QList <Partition*> partitions() const;
    static int stringToMiB(QString text);
signals:
    void partitionCountChanged(int arg);
    void minimumSizeChanged(int arg);
public slots:
    int partitionCount() const;
    QObject *partition(int partitionNumber);
    QString devName() const;
    DiskLabel::Label diskLabel() const;
    QString partitionName(int partitionNumber);
    QString partitionName(Partition* p);    
    void setBootablePartition(int index);
    void setBootablePartition(Partition* partition);
    void createPartition(int minSize, int maxSize, QString fstype, QString mountpoint, bool bootable, bool preserved);
    void appendPartition(Partition* p);
    bool removePartition(int index);
    bool canCreatePartition(QString mountpoint);
    int minimumSize();

    friend class ComputerLab;
    friend class Partition;
};

#endif // DISK_H
