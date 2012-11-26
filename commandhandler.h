#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <QTimer>
#include <QMessageBox>
#include <QString>

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
    void close_port();
    void update_timer();
    void error_select_channel();
    void error_start_capture();
    void error_stop_capture();
    void set_message_box(QString text);
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
    QMap <int, void (CommandHandler::*)(void) > command_delegate_, ack_delegate_, error_delegate_;
    int channel_number_, last_sent_command_id_;
    QTimer* timer_;
    bool check_complete_;
    char result_field_;
    QMessageBox msgBox_;
};

#endif // COMMANDHANDLER_H
