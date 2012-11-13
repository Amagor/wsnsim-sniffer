#include "commandhandler.h"
#include "qextserialenumerator.h"
#include "mainwindow.h"
#include <QDateTime>
#include <QDebug>               //убрать


CommandHandler::CommandHandler():transmit_stream_(&transmit_message_, QIODevice::ReadWrite)
{
    command_delegate_.insert(0x38, &CommandHandler::on_query_response_packet);
    command_delegate_.insert(0x35, &CommandHandler::on_ack_packet);
    command_delegate_.insert(0x31, &CommandHandler::on_data_packet_received);

    ack_delegate_.insert(0x32, &CommandHandler::send_start_command);
    ack_delegate_.insert(0x33, &CommandHandler::do_nothing);
    ack_delegate_.insert(0x34, &CommandHandler::close_port);
}

CommandHandler::~CommandHandler(){
}


void CommandHandler::get_message(QByteArray& receive_buffer){
    if(receive_buffer.size()>3){
        unsigned char message_length = receive_buffer.data()[2];
        if(receive_buffer.size()>=message_length+3){
            receive_message_.push_back(receive_buffer.left(message_length+3));
            if(*(receive_message_.end()-2)==check_crc(receive_message_.right(receive_message_.data()[2]+1))){
                receive_buffer.remove(0, message_length+3);
                (this->*command_delegate_[receive_message_.data()[3]])();
            }
            else{
                receive_message_.clear();
                receive_buffer.clear();
            }
        }
    }
}

void CommandHandler::set_channel_number(int channel){
    channel_number_ = channel;
    send_query_request_packet();
}

void CommandHandler::send_query_request_packet(){
    qDebug() << "send query request packet";
    clear_transmit();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x03)
                     << qint8(0x37) << qint8(0xff) << qint8(0x04);
    insert_crc();
    emit send_message(transmit_message_);
}


void CommandHandler::on_query_response_packet(){
    qDebug() << "query response packet received";
    receive_message_.clear();
    send_channel_select_command();
}

void CommandHandler::send_channel_select_command(){
    qDebug() << "send channel select command";
    clear_transmit();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x04) << qint8(0x32)
                      << qint8(channel_number_) << qint8(0xff) << qint8(0x04);
    insert_crc();
    last_sent_command_id_ = 0x32;
    emit send_message(transmit_message_);
}

void CommandHandler::on_ack_packet(){
    qDebug() << "ack packet received";
    //check result field
    //add some checks
    receive_message_.clear();
   (this->*ack_delegate_[last_sent_command_id_])();
}

void CommandHandler::send_start_command(){
    qDebug() << "send start command";
    clear_transmit();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x03) << qint8(0x33)
                     << qint8(0xff) << qint8(0x04);
    insert_crc();
    last_sent_command_id_ = 0x33;
    emit send_message(transmit_message_);
}

void CommandHandler::send_stop_command(){
    qDebug() << "send stop command";
    clear_transmit();
    transmit_stream_ << quint8(0x00) << qint8(0x01) << qint8(0x03) << qint8(0x34)
                     << qint8(0xff) << qint8(0x04);
    insert_crc();
    last_sent_command_id_ = 0x34;
    emit send_message(transmit_message_);
}


void CommandHandler::on_data_packet_received(){
    qDebug() << "data packet received";

    qint64 current_time = QDateTime::currentMSecsSinceEpoch();
    static char current_time_in_bytes[8];
    for(int i=0; i<8; i++)
        current_time_in_bytes[7-i] = current_time>>8*i;

    unsigned char length = receive_message_.data()[9];
    receive_message_.insert(4, 0x05);
    receive_message_.insert(10, length);

    receive_message_.prepend('\0');
    receive_message_.prepend(current_time_in_bytes, 8);

    emit log_message(receive_message_);
    receive_message_.clear();
}


//расчет CRC8
char CommandHandler::check_crc(QByteArray message){
    *(message.end()-2)=0xFF;
    char crc = 0xFF;
    foreach(char c, message){
        crc ^= c;
        for (char i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }
    return crc;
}

void CommandHandler::insert_crc(){
    transmit_message_.replace(0xff, check_crc(transmit_message_.right(transmit_message_.data()[2]+1)));
}

void CommandHandler::clear_transmit(){
    transmit_message_.clear();
    transmit_stream_.device()->reset();
}

void CommandHandler::do_nothing(){}

void CommandHandler::close_port(){
    qDebug() << "close port";
    emit close_current_port();
}
