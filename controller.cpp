#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent)
{
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogFileName("/var/log/MySensors/M2L.log", 10*10000, 10);
    QSettings settings("MySensors", "M2L");
    if(QFile(settings.fileName()).exists())
    {
        L_INFO("Settings file found at "+settings.fileName());
    }
    else
    {
        L_WARN("Settings file not found at "+settings.fileName());
        L_WARN("Switch back to default settings");
    }
    m_url = settings.value("Larahome/url","http://larahome.dev/api").toString();
    m_api_id = settings.value("Larahome/api_id","").toString();
    m_api_key = settings.value("Larahome/api_key","").toString();

    m_mqtt_ip = settings.value("MQTT/ip", "192.168.105.8").toString();
    m_mqtt_port = settings.value("MQTT/port", "1883").toInt();
    m_mqtt_client_id = settings.value("MQTT/client_id", "M2L").toString();
    m_mqtt_channel=  settings.value("MQTT/channel", "mysensors-out/#").toString();
    client = new QMQTT::Client(QHostAddress(m_mqtt_ip), m_mqtt_port);
    client->setClientId(m_mqtt_client_id);
    client->setAutoReconnect(true);
    if(settings.contains("MQTT/user") && settings.contains("MQTT/password"))
    {
        m_mqtt_user = settings.value("MQTT/user").toString();
        m_mqtt_password = settings.value("MQTT/password").toString();
        client->setUsername(m_mqtt_user);
        client->setPassword(m_mqtt_password);
    }
    L_INFO("Connecting to host: "+m_mqtt_ip+" on port: "+m_mqtt_port);
    client->connectToHost();
    QObject::connect(client, SIGNAL(connected()), this, SLOT(onConnected()));

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

void Controller::onConnected()
{
    L_INFO("Connected to mqtt server");
    L_INFO("Subscribe to channel "+ m_mqtt_channel);
    client->subscribe(m_mqtt_channel,0);
    QObject::connect(client, SIGNAL(received(QMQTT::Message)), this, SLOT(onReceived(QMQTT::Message)));
}

void Controller::onReceived(QMQTT::Message message)
{
    QStringList topics = message.topic().split("/");
    L_INFO("Incomming message from topic: "+message.topic()+" with payload: "+message.payload());

    QUrl url(m_url+m_api_id);//Init URL
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");//POST FORM DATA
    QUrlQuery params;
    if(topics.size()==6)//If topic match the MySensors Protocol
    {
        params.addQueryItem("node_id", topics[1]);
        params.addQueryItem("child_sensor_id", topics[2]);
        params.addQueryItem("command", topics[3]);
        params.addQueryItem("ack", topics[4]);
        params.addQueryItem("type", topics[5]);
        params.addQueryItem("payload", QString(message.payload()));
        params.addQueryItem("api_key", m_api_key);
        url.setQuery(params);
        QByteArray data;
        data.append(params.toString());
        L_INFO("Sending the message to LaraHome");
        manager->post(request, url.query().toUtf8());
    }
    else
    {
        L_WARN("Message does not correspond to MySensors specification. SKIP IT");
    }

}

void Controller::replyFinished(QNetworkReply *reply)
{
    QString rep = QString(reply->readAll());
    L_INFO("Reply from larahome "+rep);

    QJsonDocument doc = QJsonDocument::fromJson(rep.toUtf8());
    QJsonObject obj;
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
            if(obj.value("status")=="ok")
            {
                L_INFO("Data correctly send to LaraHome");
            }
            else
            {
                L_WARN("Reponse error from LaraHome");
            }
        }
        else
        {
            L_ERROR("Reponse error from LaraHome. Reponse is not an object");
        }
    }
    else
    {
        L_ERROR("Reponse error from LaraHome. Document is null");
    }
}
