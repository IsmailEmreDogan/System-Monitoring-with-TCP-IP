#ifndef MEMSERVER_H
#define MEMSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class MemoryServer : public QObject
{
    Q_OBJECT
public:
    explicit MemoryServer(QObject *parent = nullptr);
    uint16_t crc16(const QByteArray &data);

signals:
    void dataPartReceived(QByteArray dataPart);

private slots:
    void startServer();
    void handleNewConnection();
    void readData();

private:
    QTcpServer* serverSocket;
    QTcpSocket* clientSocket;
    QTimer* updateTimer;
    QByteArray jsonToPacket(const QByteArray &jsonData);
    QByteArray buffer;
    int len;
};

#endif // MEMSERVER_H
