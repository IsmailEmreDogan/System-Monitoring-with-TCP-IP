#include "cpuclient.h"

CpuClient::CpuClient(QObject *parent) : QObject(parent), stx(0x02), etx(0x03)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &CpuClient::sendData);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &CpuClient::handleDisconnect);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &CpuClient::retrieveCpuUsage);
    connect(updateTimer, &QTimer::timeout, this, &CpuClient::sendData);

    updateTimer->start(1000);

    connectToServer();
}

uint16_t CpuClient::crc16(const QByteArray &data) {
    const uint16_t poly = 0x1021;
    uint16_t crc = 0xFFFF;

    for (int j = 0; j < data.length(); ++j) {
        uint8_t byte = data[j];
        for (int i = 0; i < 8; i++) {
            bool bit = ((byte >> (7 - i) & 1) == 1);
            bool c15 = ((crc >> 15 & 1) == 1);
            crc <<= 1;
            if (c15 ^ bit) crc ^= poly;
        }
    }

    crc &= 0xFFFF;
    return crc;
}

int CpuClient::retrieveCpuUsage()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "vmstat 1 2 | tail -n 1 | awk '{print 100 - $15}'");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    int result = output.trimmed().toInt();
    emit cpuUsageUpdated(result);
    return result;
}

QByteArray CpuClient::retrieveCpuPacket()
{
    int cpuUsage = retrieveCpuUsage();
    QByteArray data = QByteArray::number(cpuUsage);
    uint16_t len = data.size();
    uint16_t crcValue = crc16(data);
    QByteArray packet;
    packet.append(stx);
    packet.append((char)(len & 0xFF));
    packet.append((char)((len >> 8) & 0xFF));
    packet.append(data);
    packet.append((char)((crcValue >> 8) & 0xFF));
    packet.append((char)(crcValue & 0xFF));
    packet.append(etx);
    return packet;
}

QByteArray CpuClient::packetToJson(const QByteArray &packet)
{
    QJsonObject jsonObj;
    jsonObj["stx"] = (unsigned char)packet[0];
    jsonObj["len"] = ((unsigned char)packet[2] << 8) + (unsigned char)packet[1];
    jsonObj["data"] = QString(packet.mid(3, jsonObj["len"].toInt()));
    jsonObj["crc"] = ((unsigned char)packet[packet.size() - 3] << 8) + (unsigned char)packet[packet.size() - 2];
    jsonObj["etx"] = (unsigned char)packet[packet.size() - 1];
    QJsonDocument doc(jsonObj);
    return doc.toJson(QJsonDocument::JsonFormat::Compact);
}

void CpuClient::sendData()
{
    QByteArray packet = retrieveCpuPacket();
    QByteArray jsonPacket = packetToJson(packet);
    tcpSocket->write(jsonPacket);
}

void CpuClient::handleDisconnect()
{
    qDebug() << "Disconnected from server. Trying to reconnect...";
    QTimer::singleShot(3000, this, &CpuClient::connectToServer);
}

void CpuClient::connectToServer()
{
    tcpSocket->connectToHost(QHostAddress::LocalHost, 12344);
}
