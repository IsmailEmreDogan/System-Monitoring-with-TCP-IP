#include "memclient.h"

ClientMemory::ClientMemory(QObject *parent) : QObject(parent), stx(0x02), etx(0x03)
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &ClientMemory::sendData);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &ClientMemory::handleDisconnect);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &ClientMemory::retrieveMemoryUsage);
    connect(updateTimer, &QTimer::timeout, this, &ClientMemory::sendData);

    updateTimer->start(1000);

    connectToServer();
}

uint16_t ClientMemory::crc16(const QByteArray &data) {
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



int ClientMemory::retrieveMemoryUsage()
{
    QProcess process;
    process.start("bash", QStringList() << "-c" << "free | awk '/^Mem:/{print int(($2-$7)/$2 * 100)}'");
    process.waitForFinished();
    QByteArray output = process.readAllStandardOutput();
    int result = output.trimmed().toInt();
    emit memoryUsageUpdated(result);
    return result;
}

QByteArray ClientMemory::retrieveMemoryPacket()
{
    int memUsage = retrieveMemoryUsage();
    QByteArray data = QByteArray::number(memUsage);
    uint16_t len = data.size();
    uint16_t crcValue = crc16(data);
    QByteArray packet;
    packet.append(stx);
    packet.append((char)(len & 0xFF));
    packet.append((char)((len >> 8) & 0xFF));
    packet.append(data);
    packet.append((char)((crcValue >> 8) & 0xFF)); // Önce yüksek byte
    packet.append((char)(crcValue & 0xFF));        // Sonra düşük byte
    packet.append(etx);
    return packet;
}

QByteArray ClientMemory::packetToJson(const QByteArray &packet)
{
    QJsonObject jsonObj;
    jsonObj["stx"] = (unsigned char)packet[0];
    jsonObj["len"] = ((unsigned char)packet[2] << 8) + (unsigned char)packet[1];
    jsonObj["data"] = QString(packet.mid(3, jsonObj["len"].toInt()));
    jsonObj["crc"] = ((unsigned char)packet[packet.size() - 3] << 8) + (unsigned char)packet[packet.size() - 2];
    jsonObj["etx"] = (unsigned char)packet[packet.size() - 1];
    QJsonDocument doc(jsonObj);
    return doc.toJson();
}

void ClientMemory::sendData()
{
    QByteArray packet = retrieveMemoryPacket();
    QByteArray jsonPacket = packetToJson(packet);
    tcpSocket->write(jsonPacket);
}

void ClientMemory::handleDisconnect()
{
    qDebug() << "Disconnected from server. Trying to reconnect...";
    QTimer::singleShot(3000, this, &ClientMemory::connectToServer);
}

void ClientMemory::connectToServer()
{
    tcpSocket->connectToHost(QHostAddress::LocalHost, 12345);
}
