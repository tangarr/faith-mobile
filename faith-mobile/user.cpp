#include "user.h"
#include <unistd.h>


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
    /*
    static const QString posibleCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int salt_length = qrand()%8+4;
    QString salt = "$6$";
    for (int i=0;i<salt_length;i++)
    {
        int index = qrand()%posibleCharacters.length();
        salt.append(posibleCharacters.at(index));
    }
    salt.append("$");
    password_hash = QString(crypt(password.toStdString().c_str(), salt.toStdString().c_str()));
    */
    return password;
}

void User::setUsername(QString arg)
{
    _username = arg;
}


void User::setPassword(QString password)
{
    _password = hashPassword(password);
}
