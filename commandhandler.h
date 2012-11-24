#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <QTimer>

class CommandHandler : public QObject{

    Q_OBJECT

public:
    CommandHandler();
    ~CommandHandler();
    void send_query_request_packet();
    void on_query_response_packet();
    void on_ack_packet();
    void on_data_packet_received();
    void send_channel_select_command();
    void send_start_command();
    char check_crc(QByteArray message);
    void insert_crc();
    void clear_transmit();
    void do_nothing();
    void close_port();
    void update_timer();
signals:
    void log_message(QByteArray);
    void send_message(QByteArray);
    void close_current_port();
public slots:
    void get_message(QByteArray& receive_buffer);
    void set_channel_number(int channel);
    void send_stop_command();
    void send_ack_request_packet();
private:
    QByteArray receive_message_, transmit_message_;
    QDataStream transmit_stream_;
    QMap <int, void (CommandHandler::*)(void) > command_delegate_;
    QMap <int, void (CommandHandler::*)(void) > ack_delegate_;
    int channel_number_, last_sent_command_id_;
    QTimer* timer_;
    bool check_complete_;
};

#endif // COMMANDHANDLER_H
