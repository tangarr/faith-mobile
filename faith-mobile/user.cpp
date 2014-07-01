#include "user.h"
#include <unistd.h>

void User::_read(QDataStream &stream)
{
    stream >> m_username >> password_hash;
    emit usernameChanged(m_username);
}

void User::_write(QDataStream &stream)
{
    stream << m_username << password_hash;
}

User::User(QObject *parent) :
    QObject(parent)
{
}

QString User::username() const
{
    return m_username;
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
    if (m_username != arg) {
        m_username = arg;
        emit usernameChanged(arg);
    }
}


void User::setPassword(QString password)
{
    password_hash = hashPassword(password);
}
