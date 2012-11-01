#ifndef TRANSFERNETWORK_H
#define TRANSFERNETWORK_H

#include <QtGlobal>
#include <QUdpSocket>

//forward declaration
class IClientRealTimeSettings;
class QUdpSocket;
class QHostAddress;


class TransferNetwork : public QObject
{
    Q_OBJECT

public:
    TransferNetwork();
    ~TransferNetwork();
public slots:
    void send_message(QByteArray message);
private:
    IClientRealTimeSettings* udp_settings_;
    QUdpSocket socket_;
    QHostAddress* udp_ip_;
    qint64 udp_port_;

};

#endif // TRANSFERNETWORK_H
