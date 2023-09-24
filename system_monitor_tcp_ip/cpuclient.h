#ifndef CPUMEMCLIENT_H
#define CPUMEMCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QProcess>
#include <QJsonObject>
#include <QJsonDocument>

class CpuClient : public QObject
{
    Q_OBJECT

public:
    explicit CpuClient(QObject *parent = nullptr);
    int retrieveCpuUsage();

signals:
    void cpuUsageUpdated(int usage);

private slots:
    void sendData();
    void handleDisconnect();
    void connectToServer();
    QByteArray retrieveCpuPacket();
    QByteArray packetToJson(const QByteArray &packet);

private:
    QTcpSocket *tcpSocket;
    QTimer *updateTimer;
    const char stx;
    const char etx;
    uint16_t crc16(const QByteArray &data);
};

#endif // CPUMEMCLIENT_H
