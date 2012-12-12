#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QByteArray>
#include "qextserialport.h"

class SerialPort : public QObject{

    Q_OBJECT

public:
    SerialPort();
    ~SerialPort();
    void set_baud_rate(BaudRateType);
    void set_data_bits(DataBitsType);
    void set_parity(ParityType);
    void set_stop_bits(StopBitsType);
    void set_flow_control(FlowType);
    void open_port_session(QString port_name);
signals:
    void data_received(QByteArray&);
public slots:
    void send_message(QByteArray message);
    void close_port_session();
private slots:
    void on_ready_read();
private:
    Q_DISABLE_COPY(SerialPort)
private:
    QextSerialPort* current_port_;
    QByteArray receive_buffer_;
};


#endif // SERIALPORT_H
