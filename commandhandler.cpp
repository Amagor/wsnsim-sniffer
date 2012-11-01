#include "commandhandler.h"
#include "qextserialenumerator.h"
#include "mainwindow.h"
#include <QDebug>               //убрать

CommandHandler::CommandHandler():transmit_stream_(&transmit_message_, QIODevice::ReadWrite), start_command_sent_(false)
{
    command_delegate_.insert(0x38, &CommandHandler::on_query_response_packet);
    command_delegate_.insert(0x35, &CommandHandler::on_ack_packet);
    command_delegate_.insert(0x31, &CommandHandler::on_data_packet_received);
}

CommandHandler::~CommandHandler(){
}


void CommandHandler::get_message(QByteArray& receive_buffer){
    if(receive_buffer.size()>3){
        char message_length = receive_buffer.data()[2];
        if(receive_buffer.size()>=message_length+3){
            qDebug() << "message received";
            receive_message_.push_back(receive_buffer.left(message_length+3));
            receive_buffer.remove(0, message_length+3);
            parse_message();
        }
    }
}

void CommandHandler::set_channel_number(int channel){
    channel_number_ = channel;
    send_query_request_packet();
}

void CommandHandler::send_query_request_packet(){
    qDebug() << channel_number_;
    qDebug() << "send query request packet";
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x03)
                     << qint8(0x37) << qint8(0xff) << qint8(0x04);
    emit send_message(transmit_message_);
}

void CommandHandler::parse_message(){
    //add check checksum
    (this->*command_delegate_[receive_message_.data()[3]])();
}

void CommandHandler::on_query_response_packet(){
    qDebug() << "query response packet received";
    receive_message_.clear();
    send_channel_select_command();
}

void CommandHandler::send_channel_select_command(){
    qDebug() << "send channel select command";
    transmit_stream_.device()->reset();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x04) << qint8(0x32)
                      << qint8(channel_number_) << qint8(0xff) << qint8(0x04);
    emit send_message(transmit_message_);
}

void CommandHandler::on_ack_packet(){
    qDebug() << "ack packet received";
    //check result field
    //add some checks
    receive_message_.clear();
    if(!start_command_sent_){
        send_start_command();
        start_command_sent_ = true;
    }
}

void CommandHandler::send_start_command(){
    qDebug() << "send start command";
    transmit_stream_.device()->reset();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x03) << qint8(0x33)
                     << qint8(0xff) << qint8(0x04);
    emit send_message(transmit_message_);
}

void CommandHandler::send_stop_command(){
    qDebug() << "send stop command";
    transmit_stream_.device()->reset();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x03) << qint8(0x34)
                     << qint8(0xff) << qint8(0x04);
    emit send_message(transmit_message_);
}


void CommandHandler::on_data_packet_received(){
    qDebug() << "data packet received";

    //test
//    unsigned long long some_number = 0;
    QByteArray timestamp(receive_message_.left(9));
    timestamp.remove(0, 4);
    QDataStream test(&timestamp, QIODevice::ReadOnly);
    test.setByteOrder(QDataStream::LittleEndian);
    unsigned int some_time;
    test >> some_time;
    qDebug() << some_time;

//    some_number >> 8;
//    some_number |= receive_message_.data()[5];
//    some_number |= receive_message_.data()[6];
//    some_number >> 8;

//    some_number >> 8;


//    qDebug() << some_number;

//    emit log_message(receive_message_);
    emit log_message(timestamp);
    receive_message_.clear();
}

