#include "transfernetwork.h"
#include "statictools.h"
#include <QDebug>
//#include <QUdpSocket>


TransferNetwork::TransferNetwork()
{
    udp_settings_ = StaticTools::getClientRealTimeSettings();
    udp_ip_ = new QHostAddress(udp_settings_->ip("Sniffer"));
    udp_port_ = udp_settings_->port("Sniffer");
}

TransferNetwork::~TransferNetwork(){
    delete udp_settings_;
    delete udp_ip_;
}

void TransferNetwork::send_message(QByteArray message){
    qDebug() << "send message via udp";
    socket_.writeDatagram(message, *udp_ip_, udp_port_);
    //add qdebug send message
}
