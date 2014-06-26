#include "config.h"
#include "QSettings"

Config::Config(QObject *parent) :
    QObject(parent)
{
    _settings = new QSettings("config.ini", QSettings::IniFormat);
}

Config::~Config()
{
    delete _settings;
}

QString Config::server_address() const
{
    if (_settings->contains("server_address"))
        return _settings->value("server_address").toString();
    else return "";
}

int Config::server_port() const
{
    if (_settings->contains("server_port"))
    {
        bool ok;
        int ret = _settings->value("server_port").toInt(&ok);
        if (ok) return ret;
        else return 0;
    }
    else return 0;
}

Config *Config::instance()
{
    static Config config;
    return &config;
}

void Config::setServer_address(QString arg)
{
    _settings->setValue("server_address", arg);
}

void Config::setServer_port(int arg)
{
    _settings->setValue("server_port", arg);
}
