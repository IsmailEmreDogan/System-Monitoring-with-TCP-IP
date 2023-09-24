#ifndef CPUSERVER_H
#define CPUSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class CpuServer : public QObject
{
    Q_OBJECT

public:
    explicit CpuServer(QObject *parent = nullptr);

private slots:
    void startServer();
    void handleNewConnection();
    void readData();
    uint16_t crc16(const QByteArray &data);
    QByteArray jsonToPacket(const QByteArray &jsonData);

signals:
    void dataPartReceived(QByteArray dataPart);

private:
    QTcpServer *serverSocket;
    QTcpSocket *clientSocket;
    QByteArray buffer;
};

#endif // CPUSERVER_H
