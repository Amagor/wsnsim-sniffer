#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qextserialenumerator.h"
#include <QMainWindow>
#include <QFileInfo>
#include <QListWidget>
#include <QFile>



//forward declarations
class SerialPort;
class CommandHandler;
class TransferNetwork;
class QAction;
class QMenu;
class QFileDialog;


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
    void set_project_file();
    void create_actions();
    void create_menus();

signals:
    void channel_number_selected(int);
    void stop_capture_command();

public slots:
    void show_port_info(QListWidgetItem* item);
    void captureButton_clicked(bool);
    void baud_rate_changed(int idx);
    void parity_changed(int idx);
    void data_bits_changed(int idx);
    void stop_bits_changed(int idx);

    void write_to_log(QByteArray);
    void log_file_location_triggered();
    void directory_changed(QString current_dir);

    
private:
    Ui::MainWindow *ui;
    SerialPort* port_;
    CommandHandler* command_handler_;
    TransferNetwork* transfer_network_;
    QList<QextPortInfo> ports_info_;
    QFile log_file_;
    QFile project_file_;
    QFileInfo project_file_info_;
    QAction* actionLogFile_;
    QAction* actionQuit_;
    QAction* actionAboutQt_;
    QMenu* fileMenu_;
    QFileDialog* dialog_;
    QString current_dir_;
};

#endif // MAINWINDOW_H
