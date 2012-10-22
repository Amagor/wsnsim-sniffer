#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qextserialport-1.2beta1/src/qextserialenumerator.h"
#include <QMainWindow>
#include <QList>
#include <QListWidget>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QSharedPointer>                                       //удалить
#include <QSettings>



namespace Ui {
class MainWindow;
}

class QTimer;
class QextSerialPort;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void scan_ports();
    void disable_settings();
    void enable_settings();
    void on_captureButton_pressed();
    void on_captureButton_released();
    void write_and_clear_buffer();
    void on_ack_start_received();
    void on_ack_stop_received();
    void on_ack_time_received();
    void set_project_file();
    void send_message(const QByteArray& message);

public slots:
    void show_port_info(QListWidgetItem* item);
    void on_captureButton_toggled(bool);
    void onBaudRateChanged(int idx);
    void onParityChanged(int idx);
    void onDataBitsChanged(int idx);
    void onStopBitsChanged(int idx);
    void onReadyRead();
    void check_sniffer();

    
private:
    Ui::MainWindow *ui;
    QList<QextPortInfo> ports_;
    QextSerialPort* port_;
    QSharedPointer<QTimer> ack_timer_;
    QByteArray buffer_;
    QFile file_;
    bool ack_start_flag_, ack_req_flag_;
    QMap <QString, void (MainWindow::*)(void) > delegate_;
    QStringList acks_;
    QSettings file_settings_;
    QFileInfo file_information_;
};

#endif // MAINWINDOW_H
