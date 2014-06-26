#ifndef HOST_H
#define HOST_H

#include "laboratoriesmodelelement.h"
#include "laboratoriesmodel.h"

class ComputerLab;

class Host : public LaboratoriesModelElement
{
    Q_GADGET
    QString _hostname;
    QString _ip;
    ComputerLab *_lab;
    DiskLayout::Layout _diskLayout;
public:
    Host(QString ip, QString hostname);
    // LaboratoriesModelElement interface
public:
    bool isParent() const;
    QString name() const;
    void setLab(ComputerLab* lab);
    ComputerLab *lab() const;
    DiskLayout::Layout diskLayout() const;
    void checkDiskLayout();
    QString configFileName() const;
};

#endif // HOST_H
