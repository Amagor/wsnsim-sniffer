#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QMap>

class CommandHandler : public QObject{

    Q_OBJECT

public:
    CommandHandler();
    ~CommandHandler();
    void send_query_request_packet();
    void parse_message();
    void on_query_response_packet();
    void on_ack_packet();
    void on_data_packet_received();
    void send_channel_select_command();
    void send_start_command();
signals:
    void log_message(QByteArray);
    void send_message(QByteArray);
public slots:
    void get_message(QByteArray& receive_buffer);
    void set_channel_number(int channel);
    void send_stop_command();
private:
    QByteArray receive_message_, transmit_message_;
    QDataStream transmit_stream_;
    QMap <int, void (CommandHandler::*)(void) > command_delegate_;
    int channel_number_;
    bool start_command_sent_;
};

#endif // COMMANDHANDLER_H
