#ifndef CMDEVICE_H
#define CMDEVICE_H

#include <QObject>

#include <QObject>
#include <QSerialPort>

#include <QObject>
#include <QSerialPort>

class cmDevices : public QObject {
    Q_OBJECT
public:
    explicit cmDevices(QObject *parent = nullptr) : QObject(parent) {
        serialPort = new QSerialPort(this);
        connect(serialPort, &QSerialPort::readyRead, this, &cmDevices::readData);
    }

    ~cmDevices() {
        if (serialPort->isOpen()) {
            serialPort->close();
        }
    }

public slots:
    void openSerialPort(const QString &portName, qint32 baudRate) {
        serialPort->setPortName(portName);
        serialPort->setBaudRate(baudRate);
        if (serialPort->open(QIODevice::ReadWrite)) {
            emit serialPortOpened(true);
        } else {
            emit serialPortOpened(false, serialPort->errorString());
        }
    }

    void closeSerialPort() {
        if (serialPort->isOpen()) {
            serialPort->close();
        }
    }

    void writeData(const QByteArray &data) {
        if (serialPort->isOpen()) {
            serialPort->write(data);
        }
    }

private slots:
    void readData() {
        if (serialPort->isOpen()) {
            QByteArray data = serialPort->readAll();
            emit dataReceived(data); // 直接发送原始数据给协议层
        }
    }

signals:
    void serialPortOpened(bool success, const QString &errorString = "");
    void dataReceived(const QByteArray &data);

private:
    QSerialPort *serialPort;
};

#endif // CMDEVICE_H
