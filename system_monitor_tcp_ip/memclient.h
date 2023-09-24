#ifndef MEMCLIENT_H
#define MEMCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include <QDebug>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
class ClientMemory : public QObject
{
    Q_OBJECT

public:
    explicit ClientMemory(QObject *parent = nullptr);
    int retrieveMemoryUsage();

private slots:
    void sendData();
    void handleDisconnect();

private:
    QTcpSocket *tcpSocket;
    QTimer *updateTimer;
    const char stx;
    const char etx;

    uint16_t crc16(const QByteArray &data);
    QByteArray retrieveMemoryPacket();
    QByteArray packetToJson(const QByteArray &packet);
    void connectToServer();

signals:
    void memoryUsageUpdated(int value);
};

#endif // MEMCLIENT_H
