#include "commandhandler.h"
#include "qextserialenumerator.h"
#include "mainwindow.h"
#include <QDebug>               //убрать

CommandHandler::CommandHandler():transmit_stream_(&transmit_message_, QIODevice::ReadWrite)
{
    command_delegate_.insert(0x38, &CommandHandler::on_query_response_packet);
    command_delegate_.insert(0x35, &CommandHandler::on_ack_packet);
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
    transmit_stream_.device()->reset();
    transmit_stream_ <<  quint8(0x00) << qint8(0x01) << qint8(0x04) << qint8(0x32)
                      << qint8(channel_number_) << qint8(0xff) << qint8(0x04);
//    emit log_message(transmit_message_);
    emit send_message(transmit_message_);
}

void CommandHandler::on_ack_packet(){
    qDebug() << "ack packet received";
    //check result field

    emit log_message(receive_message_);
}
