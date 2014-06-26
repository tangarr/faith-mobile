#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>
#include <QList>
#include "laboratoriesmodel.h"

class ApplicationManager : public QObject
{
    Q_OBJECT
    ApplicationManager();
    LaboratoriesModel *laboratoriesModel;    
private:
    static const QString dir_path;
    bool configFileRead(QString filename);
    bool configFileUpdate(QString filename);
    bool configFileDelete(QString filename);
    bool downloadConfigFile(QString filename);
public:
    static ApplicationManager *instance();

public slots:
    bool readConfig();
    bool updateConfig();
    LaboratoriesModel *model();

};

#endif // APPLICATIONMANAGER_H
