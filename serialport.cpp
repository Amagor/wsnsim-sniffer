#include "serialport.h"
#include "qextserialenumerator.h"
#include "mainwindow.h"
#include <QDebug>               //убрать

SerialPort::SerialPort()
{
    PortSettings settings = {BAUD9600, DATA_8, PAR_ODD, STOP_1, FLOW_OFF, 10};
    current_port_ = new QextSerialPort(settings, QextSerialPort::EventDriven);
    connect(current_port_, SIGNAL(readyRead()), SLOT(on_ready_read()));
}

SerialPort::~SerialPort(){
    delete current_port_;
}

void SerialPort::on_ready_read(){
    if(current_port_->bytesAvailable()){
        receive_buffer_.append(current_port_->readAll());
        emit data_received(receive_buffer_);
    }
}

void SerialPort::send_message(QByteArray message){
    current_port_->write(message);
}

void SerialPort::start_port_session(QString port_name){
    if(!current_port_->isOpen()){
        current_port_->setPortName(port_name);
        current_port_->open(QIODevice::ReadWrite);
        qDebug() << "port is open";
    }
}

void SerialPort::set_baud_rate(BaudRateType baud_rate){
    current_port_->setBaudRate(baud_rate);
}

void SerialPort::set_data_bits(DataBitsType data_bits){
    current_port_->setDataBits(data_bits);
}

void SerialPort::set_parity(ParityType parity){
    current_port_->setParity(parity);
}

void SerialPort::set_stop_bits(StopBitsType stop_bits){
    current_port_->setStopBits(stop_bits);
}
