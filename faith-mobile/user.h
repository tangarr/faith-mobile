#ifndef USER_H
#define USER_H

#include <QObject>
#include <QDataStream>

class User : public QObject
{
    Q_OBJECT
private:    
    QString _password;
    QString _username;
    QString _shell;
    QString _homeDir;

    void _read(QDataStream &stream);
    void _write(QDataStream &stream);
public:
    explicit User(QObject *parent);
    static QString hashPassword(QString password);
    void setPassword(QString password);
    void setUsername(QString arg);
    void setShell(const QString &shell);
    void setHomeDir(const QString &homeDir);

public slots:    
    QString username() const;
    QString homeDir() const;
    QString shell() const;

    friend class ComputerLab;
};



#endif // USER_H
