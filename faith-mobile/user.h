#ifndef USER_H
#define USER_H

#include <QObject>
#include <QDataStream>

class User : public QObject
{
    Q_OBJECT
private:
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    QString password_hash;
    QString m_username;
    void _read(QDataStream &stream);
    void _write(QDataStream &stream);
public:
    explicit User(QObject *parent = 0);
    QString username() const;
signals:
    void usernameChanged(QString arg);
public slots:
    void setPassword(QString password);
    void setUsername(QString arg);

    friend class ComputerLab;
};



#endif // USER_H
