#ifndef CMPROTOCOLHANDLER_H
#define CMPROTOCOLHANDLER_H

#include <QObject>

#include <QObject>

class cmProtocolHandler : public QObject {
    Q_OBJECT
public:
    explicit cmProtocolHandler(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    // 处理从 cmDevices 接收的原始数据
    void processReceivedData(const QByteArray &rawData) {
        // 示例协议：STX(0x02) + 数据 + 校验和 + ETX(0x03)
        QByteArray buffer = receivedBuffer + rawData;
        receivedBuffer.clear();

        while (buffer.size() >= 4) { // 最小帧长度：STX(1) + 数据(1) + 校验和(1) + ETX(1)
            // 查找帧头
            int stxIndex = buffer.indexOf(0x02);
            if (stxIndex == -1) {
                buffer.clear(); // 无帧头，丢弃
                break;
            }
            buffer = buffer.mid(stxIndex); // 从帧头开始

            // 查找帧尾
            int etxIndex = buffer.indexOf(0x03, 1);
            if (etxIndex == -1) {
                receivedBuffer = buffer; // 帧不完整，缓存等待下一批数据
                break;
            }

            // 提取完整帧
            QByteArray frame = buffer.mid(0, etxIndex + 1);
            buffer = buffer.mid(etxIndex + 1); // 剩余数据继续处理

            // 校验帧
            if (isValidFrame(frame)) {
                QByteArray payload = frame.mid(1, frame.size() - 3); // 提取数据部分
                emit dataParsed(payload); // 发送解析后的数据
            }
        }
        receivedBuffer = buffer; // 保存未处理完的数据
    }

    // 封装要发送的数据
    void sendData(const QByteArray &payload) {
        // 构造协议帧：STX + 数据 + 校验和 + ETX
        QByteArray frame;
        frame.append(0x02); // STX
        frame.append(payload); // 数据
        frame.append(calculateChecksum(payload)); // 校验和
        frame.append(0x03); // ETX
        emit dataToSend(frame); // 发送给 cmDevices
    }

signals:
    // 解析后的数据，供上层使用
    void dataParsed(const QByteArray &data);
    // 封装后的数据，发送给 cmDevices
    void dataToSend(const QByteArray &data);

private:
    // 校验帧（示例：校验和为数据部分的异或）
    bool isValidFrame(const QByteArray &frame) {
        if (frame.size() < 4 || frame[0] != 0x02 || frame[frame.size() - 1] != 0x03) {
            return false;
        }
        char receivedChecksum = frame[frame.size() - 2];
        char calculatedChecksum = calculateChecksum(frame.mid(1, frame.size() - 3));
        return receivedChecksum == calculatedChecksum;
    }

    // 计算校验和（示例：简单异或）
    char calculateChecksum(const QByteArray &data) {
        char checksum = 0;
        for (char byte : data) {
            checksum ^= byte;
        }
        return checksum;
    }

    QByteArray receivedBuffer; // 缓存未处理完的数据
};

#endif // CMPROTOCOLHANDLER_H
