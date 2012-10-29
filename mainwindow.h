#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qextserialenumerator.h"
#include <QMainWindow>
//#include <QFileInfo>
#include <QListWidget>
#include <QFile>


//forward declarations
class SerialPort;
class CommandHandler;


namespace Ui {
class MainWindow;
}

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
//    void set_project_file();

signals:
    void channel_number_selected(int);

public slots:
    void show_port_info(QListWidgetItem* item);
    void captureButton_clicked(bool);
    void baud_rate_changed(int idx);
    void parity_changed(int idx);
    void data_bits_changed(int idx);
    void stop_bits_changed(int idx);

    void write_to_log(QByteArray);

    
private:
    Ui::MainWindow *ui;
    SerialPort* port_;
    CommandHandler* command_handler_;
    QList<QextPortInfo> ports_info_;
    QFile test_file_;
//    QFileInfo project_file_info_;

};

#endif // MAINWINDOW_H
