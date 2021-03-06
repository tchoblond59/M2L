#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include "qmqtt.h"
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>

#include "simpleQtLogger.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
private:
    QMQTT::Client *client;
    QNetworkAccessManager *manager;
    QString m_url;
    QString m_api_id;
    QString m_api_key;
    QString m_mqtt_ip;
    int m_mqtt_port;
    QString m_mqtt_client_id;
    QString m_mqtt_user;
    QString m_mqtt_password;
    QString m_mqtt_channel;

signals:

public slots:
    void onConnected();
    void onReceived(QMQTT::Message message);
    void replyFinished(QNetworkReply *reply);
};

#endif // CONTROLLER_H
