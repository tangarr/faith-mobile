#ifndef LABORATORIESMODELELEMENT_H
#define LABORATORIESMODELELEMENT_H

#include <QString>
#include <QObject>

class LaboratoriesModelElement : public QObject
{
    Q_OBJECT
    bool _checked;
public:
    LaboratoriesModelElement();
    ~LaboratoriesModelElement();
    virtual bool isParent() const = 0;
    virtual QString name() const = 0;
    bool checked() const;
    void setChecked(bool checked);
};

#endif // LABORATORIESMODELELEMENT_H
