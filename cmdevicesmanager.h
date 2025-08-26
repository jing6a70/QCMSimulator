#ifndef CMDEVICESMANAGER_H
#define CMDEVICESMANAGER_H

#include <QObject>
#include <QThread>
#include "cmdevice.h"
#include "cmprotocolhandler.h"

class cmDevicesManager : public QObject {
    Q_OBJECT
public:
    void addSerialPort(const QString &portName, qint32 baudRate) {
        QThread *thread = new QThread(this);
        cmDevices *device = new cmDevices;
        cmProtocolHandler *protocol = new cmProtocolHandler;

        device->moveToThread(thread);
        threads.append(thread);
        devices.append(device);
        protocols.append(protocol);

        // 连接 cmDevices 和 ProtocolHandler
        connect(device, &cmDevices::dataReceived, protocol, &cmProtocolHandler::processReceivedData);
        connect(protocol, &cmProtocolHandler::dataToSend, device, &cmDevices::writeData);
        connect(protocol, &cmProtocolHandler::dataParsed, this, [=](const QByteArray &data) {
            emit dataParsed(portName, data);
        });

        // 启动线程
        connect(thread, &QThread::started, device, [=]() {
            device->openSerialPort(portName, baudRate);
        });
        thread->start();
    }

signals:
    void dataParsed(const QString &portName, const QByteArray &data);

private:
    QList<QThread*> threads;
    QList<cmDevices*> devices;
    QList<cmProtocolHandler*> protocols;
};

#endif // CMDEVICESMANAGER_H
