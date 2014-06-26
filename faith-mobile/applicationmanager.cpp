#include "applicationmanager.h"
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QRegExp>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include "config.h"
#include <QTcpSocket>
#include "faithmessage.h"
#include "fdstringlist.h"
#include <QCryptographicHash>
#include "fdfile.h"

const QString ApplicationManager::dir_path = "config";

ApplicationManager::ApplicationManager() : QObject()
{
    QSqlDatabase::addDatabase("QSQLITE");
    laboratoriesModel = new LaboratoriesModel();
}

bool ApplicationManager::configFileRead(QString filename)
{
    QString extension="";
    QString tmp = filename.split("//").last();
    if (tmp.contains("."))
        extension = tmp.split(".").last();

    if (extension=="db")
    {
        QSqlDatabase db = QSqlDatabase::database();
        db.setDatabaseName(filename);
        if (db.open())
        {
            QSqlQuery querry("select * from general;");
            QHash<QString, QString>result;
            while (querry.next()) {
                result.insert(querry.value(0).toString(), querry.value(1).toString());
            }
            db.close();
            QString lab = result.value("lab");
            QString ip = result.value("ip");
            QString hostname = result.value("hostname");
            if (lab.isEmpty() || ip.isEmpty() || hostname.isEmpty())
            {
                qDebug() << "some 'general' information is missing in database " << filename;  
                return false;
            }
            laboratoriesModel->insertHost(lab, ip, hostname);
            return true;
        }
        return false;
    }
    else
    {
        qDebug() << "EXTENSION " << extension << " NOT SUPPORTED";
        return false;
    }
}

bool ApplicationManager::configFileUpdate(QString filename)
{
    QString extension="";
    QString tmp = filename.split("//").last();
    if (tmp.contains("."))
        extension = tmp.split(".").last();

    if (extension=="db")
    {
        QSqlDatabase db = QSqlDatabase::database();
        db.setDatabaseName(filename);
        if (db.open())
        {
            QString ip;
            QSqlQuery querry("select key from general where value='ip';");
            if (querry.next()) ip = querry.value(0).toString();
            db.close();
            laboratoriesModel->updateHost(ip);
            return true;
        }
        return false;
    }
    else
    {
        qDebug() << "EXTENSION " << extension << " NOT SUPPORTED";
        return false;
    }
}

bool ApplicationManager::configFileDelete(QString filename)
{
    QString extension="";
    QString tmp = filename.split("//").last();
    if (tmp.contains("."))
        extension = tmp.split(".").last();

    if (extension=="db")
    {
        QSqlDatabase db = QSqlDatabase::database();
        db.setDatabaseName(filename);
        if (db.open())
        {
            QString ip;
            QSqlQuery querry("select key from general where value='ip';");
            if (querry.next()) ip = querry.value(0).toString();
            db.close();
            laboratoriesModel->removeHost(ip);
            return true;
        }
        return false;
    }
    else
    {
        qDebug() << "EXTENSION " << extension << " NOT SUPPORTED";
        return false;
    }

}

bool ApplicationManager::downloadConfigFile(QString filename)
{
    QString address = Config::instance()->server_address();
    int port = Config::instance()->server_port();
    QTcpSocket socket;
    socket.connectToHost(address, port);
    if (socket.state()!=QTcpSocket::ConnectedState) socket.waitForConnected();
    if (socket.state()!=QTcpSocket::ConnectedState)
    {
        qDebug() << "error: " << socket.errorString();
        return false;
    }
    FaithMessage::MsgGetFile(filename).send(&socket);
    FaithMessage msg;
    msg.recive(&socket);
    socket.disconnectFromHost();
    if (socket.state()!=QTcpSocket::UnconnectedState) socket.waitForDisconnected();
    if (msg.getMessageCode() != Faithcore::SEND_FILE) return false;
    FdFile* fdFile = static_cast<FdFile*>(msg.getData());
    if (!fdFile) return false;
    return fdFile->saveFile(dir_path+"/"+filename);
}

ApplicationManager *ApplicationManager::instance()
{
    static ApplicationManager applicationManager;
    return &applicationManager;
}

bool ApplicationManager::readConfig()
{    
    QDir config_dir(dir_path);
    if (config_dir.exists())
    {        
        QStringList files = config_dir.entryList(QDir::Files);
        QRegExp regexp("[0-9a-f]{8}.db");

        foreach (QString filename, files) {
            if (regexp.exactMatch(filename))
            {
                configFileRead(dir_path+"/"+filename);
            }
        }
        return true;
    }
    else
    {
        return QDir::current().mkpath(dir_path);
    }
}

bool ApplicationManager::updateConfig()
{
    QString address = Config::instance()->server_address();
    int port = Config::instance()->server_port();

    QTcpSocket socket;
    socket.connectToHost(address, port);
    if (socket.state()!=QTcpSocket::ConnectedState) socket.waitForConnected();
    if (socket.state()!=QTcpSocket::ConnectedState)
    {
        qDebug() << "error: " << socket.errorString();
        return false;
    }
    FaithMessage::MsgGetFileList().send(&socket);
    if (!socket.waitForReadyRead())
    {
        socket.disconnectFromHost();
        if (socket.state()!=QTcpSocket::UnconnectedState) socket.waitForDisconnected();
        return false;
    }
    FaithMessage msg;
    msg.recive(&socket);
    socket.disconnectFromHost();
    if (socket.state()!=QTcpSocket::UnconnectedState) socket.waitForDisconnected();
    if (msg.getMessageCode()!=Faithcore::FILE_LIST) return false;
    FdStringList* list = static_cast<FdStringList*>(msg.getData());
    if (!list) return false;

    QDir config_dir("config");
    QHash<QString, QString> files;
    foreach(QFileInfo fileInfo, config_dir.entryInfoList())
    {
        QFile file(fileInfo.absoluteFilePath());
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray data = file.readAll();
            file.close();
            QString md5sum = QString(QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex());
            files.insert(fileInfo.fileName(), md5sum);
        }
    }
    for (int i=0; i<list->count(); i++)
    {
        QStringList tmp = list->at(i).split("#");
        QString key = tmp[0];
        QString md5 = tmp[1];

        if (files.contains(key))
        {
            if (files.value(key)!=md5)
            {
                if (downloadConfigFile(key)) configFileUpdate(config_dir.filePath(key));
            }
            files.remove(key);
        }
        else
        {
            bool downloaded = downloadConfigFile(key);
            if (downloaded) configFileRead(config_dir.filePath(key));
        }
    }
    foreach (QString key, files.keys())
    {
        configFileDelete(config_dir.filePath(key));
        QFile file(config_dir.filePath(key));
        file.remove();
    }
    return true;
}

LaboratoriesModel *ApplicationManager::model()
{
    return laboratoriesModel;
}
