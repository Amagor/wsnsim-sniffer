#include "transfernetwork.h"
#include "statictools.h"
#include <QDebug>


TransferNetwork::TransferNetwork(QString path):absolute_file_path_(path)
{
    udp_settings_ = StaticTools::getClientRealTimeSettings();
    udp_ip_ = new QHostAddress(udp_settings_->ip("Sniffer"));
    udp_port_ = udp_settings_->port("Sniffer");
    udp_settings_->setProjectPath("Sniffer", absolute_file_path_);
}

TransferNetwork::~TransferNetwork(){
    delete udp_settings_;
    delete udp_ip_;
}

void TransferNetwork::send_message(QByteArray message){
    qDebug() << "send message via udp";
    socket_.writeDatagram(message, *udp_ip_, udp_port_);
}
