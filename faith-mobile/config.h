#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
class QSettings;

class Config : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString server_address READ server_address WRITE setServer_address NOTIFY server_addressChanged)
    Q_PROPERTY(int server_port READ server_port WRITE setServer_port NOTIFY server_portChanged)

private:
    QSettings *_settings;
    explicit Config(QObject *parent = 0);

public:
    ~Config();
    QString server_address() const;
    int server_port() const;
    static Config *instance();
signals:
    void server_addressChanged(QString arg);
    void server_portChanged(int arg);

public slots:
    void setServer_address(QString arg);
    void setServer_port(int arg);
};

#endif // CONFIG_H
