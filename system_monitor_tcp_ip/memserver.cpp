#include "memserver.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

MemoryServer::MemoryServer(QObject *parent) : QObject(parent)
{
    startServer();
}

void MemoryServer::startServer()
{
    serverSocket = new QTcpServer(this);
    if (!serverSocket->listen(QHostAddress::LocalHost, 12345)) {
        qDebug() << "Server could not start!";
        return;
    }
    qDebug() << "Server started!";
    connect(serverSocket, &QTcpServer::newConnection, this, &MemoryServer::handleNewConnection);
}

void MemoryServer::handleNewConnection()
{
    clientSocket = serverSocket->nextPendingConnection();
    qDebug() << "New client connected from" << clientSocket->peerAddress();
    connect(clientSocket, &QTcpSocket::readyRead, this, &MemoryServer::readData);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
}

uint16_t MemoryServer::crc16(const QByteArray &data) {
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


QByteArray MemoryServer::jsonToPacket(const QByteArray &jsonData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "JSON Parse Error:" << parseError.errorString();
        return QByteArray();
    }

    QJsonObject jsonObj = doc.object();
    if (!jsonObj.contains("stx") || !jsonObj.contains("len") ||
        !jsonObj.contains("data") || !jsonObj.contains("crc") ||
        !jsonObj.contains("etx")) {
        qDebug() << "Incomplete JSON!";
        return QByteArray();
    }

    // Validate range for STX, ETX
    int stx = jsonObj["stx"].toInt();
    int etx = jsonObj["etx"].toInt();


    // Data conversion and length checks
    QByteArray dataBytes = jsonObj["data"].toString().toUtf8();
    int len = jsonObj["len"].toInt();
    if (len != dataBytes.size()) {
        qDebug() << "Mismatch in data length!";
        return QByteArray();
    }

    int crc = jsonObj["crc"].toInt();


    QByteArray packet;
    packet.append((char)stx);
    packet.append((char)(len & 0xFF));
    packet.append((char)((len >> 8) & 0xFF));
    packet.append(dataBytes);
    packet.append((char)((crc >> 8) & 0xFF)); // Önce yüksek byte
    packet.append((char)(crc & 0xFF));        // Sonra düşük byte
    packet.append((char)etx);

    qDebug() << "Constructed packet:" << packet.toHex();
    return packet;
}


void MemoryServer::readData()
{
    QTcpSocket* senderSocket = qobject_cast<QTcpSocket*>(sender());
    if (!senderSocket) return;

    buffer += senderSocket->readAll();

    int startIndex = buffer.indexOf("{");
    int endIndex = buffer.indexOf("}");

    while (startIndex != -1 && endIndex != -1 && endIndex > startIndex) {
        QByteArray jsonData = buffer.mid(startIndex, endIndex - startIndex + 1);
        buffer.remove(startIndex, endIndex - startIndex + 1);

        qDebug() << "Received JSON data:" << jsonData;
        QByteArray packet = jsonToPacket(jsonData);
        if (packet.isEmpty()) continue;

        qDebug() << "Converted to packet:" << packet.toHex();

        int len = ((unsigned char)packet[2] << 8) + (unsigned char)packet[1];
        QByteArray dataPart = packet.mid(3, len);
        qDebug() << "Data Part:" << dataPart.toHex();

        uint16_t receivedCRC = ((unsigned char)packet[packet.size() - 3] << 8) + (unsigned char)packet[packet.size() - 2];

        qDebug() << "Received CRC (High Byte):" << QString::number((unsigned char)packet[packet.size() - 3], 16);
        qDebug() << "Received CRC (Low Byte):" << QString::number((unsigned char)packet[packet.size() - 2], 16);

        uint16_t calculatedCRC = crc16(dataPart);
        qDebug() << "Received CRC:" << receivedCRC;
        qDebug() << "Calculated CRC:" << calculatedCRC;

        if (receivedCRC == calculatedCRC) {
            qDebug() << "Data is correct:" << dataPart;
            emit dataPartReceived(dataPart);
        } else {
            qDebug() << "CRC mismatch!";
            senderSocket->write("CRC mismatch!");
        }

        startIndex = buffer.indexOf("{");
        endIndex = buffer.indexOf("}");
    }
}

