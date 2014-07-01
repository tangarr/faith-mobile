#ifndef HOST_H
#define HOST_H

#include "laboratoriesmodelelement.h"
#include "laboratoriesmodel.h"

class ComputerLab;

class Host : public LaboratoriesModelElement
{
    Q_OBJECT
    QString _hostname;
    QString _ip;
    ComputerLab *_lab;
    DiskLayout::Layout _diskLayout;
public:
    Host(QString ipUint32, QString hostname);
    // LaboratoriesModelElement interface
public:
    bool isParent() const;
    QString name() const;
    void setLab(ComputerLab* lab);
    ComputerLab *lab() const;
    DiskLayout::Layout diskLayoutStatus() const;
    void checkDiskLayoutStatus();
    QString configFileName() const;
    quint32 ipUint32() const;
signals:
    void diskLayoutStatusChanged(Host*);
};

#endif // HOST_H
