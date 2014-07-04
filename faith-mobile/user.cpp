#include "user.h"
#include <QDebug>
#include "sha512crypt.h"


QString User::shell() const
{
    return _shell;
}

void User::setShell(const QString &shell)
{
    _shell = shell;
}

QString User::homeDir() const
{
    return _homeDir;
}

void User::setHomeDir(const QString &homeDir)
{
    _homeDir = homeDir;
}
void User::_read(QDataStream &stream)
{
    stream >> _username >> _password >> _shell >> _homeDir;
}

void User::_write(QDataStream &stream)
{
    stream << _username << _password << _shell << _homeDir;
}

User::User(QObject *parent) :
    QObject(parent)
{
}

QString User::username() const
{
    return _username;
}

QString User::hashPassword(QString password)
{

    static const QString posibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int salt_length = qrand()%8+4;
    QString salt;
    for (int i=0;i<salt_length;i++)
    {
        int index = qrand()%posibleCharacters.length();
        salt.append(posibleCharacters.at(index));
    } 
    QString password_hash = QString(sha512_crypt(password.toStdString().c_str(), salt.toStdString().c_str()));
    qDebug() << Q_FUNC_INFO << password_hash;
    return password_hash;
}

void User::setUsername(QString arg)
{
    _username = arg;
}


void User::setPassword(QString password)
{
    _password = hashPassword(password);
}
