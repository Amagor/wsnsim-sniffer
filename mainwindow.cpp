#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialport-1.2beta1/src/qextserialport.h"
#include <QTimer>
#include <QDateTime>
#include <qiterator.h>
#include <QDebug>
#include <QXmlStreamWriter>
#include <QUdpSocket>
#include <QtConcurrentRun>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), ack_timer_(new QTimer(this)), file_("data.bin"),
    ack_start_flag_(false), ack_req_flag_(true), file_settings_("MAI", "sniffer")
{

    ui->setupUi(this);

    //set ui settings
    ui->portInfoEdit->setReadOnly(true);
    ui->captureButton->setCheckable(true);
    ui->captureButton->setDisabled(true);
    disable_settings();
    scan_ports();

    file_.open(QIODevice::WriteOnly);

    ui->spinBox->setRange(12,21);

    ui->BaudRateBox->addItem(QLatin1String("1200"), BAUD1200);
    ui->BaudRateBox->addItem(QLatin1String("2400"), BAUD2400);
    ui->BaudRateBox->addItem(QLatin1String("4800"), BAUD4800);
    ui->BaudRateBox->addItem(QLatin1String("9600"), BAUD9600);
    ui->BaudRateBox->addItem(QLatin1String("19200"), BAUD19200);
    ui->BaudRateBox->setCurrentIndex(3);

    ui->parityBox->addItem(QLatin1String("none"), PAR_NONE);
    ui->parityBox->addItem(QLatin1String("odd"), PAR_ODD);
    ui->parityBox->addItem(QLatin1String("even"), PAR_EVEN);
    ui->parityBox->setCurrentIndex(1);

    ui->dataBitsBox->addItem(QLatin1String("5"), DATA_5);
    ui->dataBitsBox->addItem(QLatin1String("6"), DATA_6);
    ui->dataBitsBox->addItem(QLatin1String("7"), DATA_7);
    ui->dataBitsBox->addItem(QLatin1String("8"), DATA_8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->stopBitsBox->addItem(QLatin1String("1"), STOP_1);
    ui->stopBitsBox->addItem(QLatin1String("2"), STOP_2);

    ui->crcModeBox->addItem(QLatin1String("auto"));
    ui->crcModeBox->addItem(QLatin1String("manual"));
    ui->crcModeBox->setCurrentIndex(0);

    //set port settings
    ack_timer_->setInterval(2000);

    PortSettings settings = {BAUD9600, DATA_8, PAR_ODD, STOP_1, FLOW_OFF, 10};
    port_ = new QextSerialPort(settings, QextSerialPort::EventDriven);

    acks_ << "ack start" << "ack stop" << "ack time";

//    delegate_.insert("ack start", boost::bind(&MainWindow::on_ack_start_received, this));
//    delegate_.insert("ack stop", boost::bind(&MainWindow::on_ack_stop_received, this));
//    delegate_.insert("ack stop", boost::bind(&MainWindow::on_ack_time_received, this));

    delegate_.insert("ack start", &MainWindow::on_ack_start_received);
    delegate_.insert("ack stop", &MainWindow::on_ack_stop_received);
    delegate_.insert("ack time", &MainWindow::on_ack_time_received);

    set_project_file();
    file_settings_.setValue("sniffer/absolutepath", file_information_.absoluteFilePath());

    connect(ui->PortlistWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(show_port_info(QListWidgetItem*)));
    connect(ui->BaudRateBox, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    connect(ui->parityBox, SIGNAL(currentIndexChanged(int)), SLOT(onParityChanged(int)));
    connect(ui->dataBitsBox, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(ui->stopBitsBox, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));
    connect(ui->captureButton, SIGNAL(toggled(bool)), this, SLOT(on_captureButton_toggled(bool)));
    connect(ack_timer_.data(), SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port_, SIGNAL(readyRead()), SLOT(onReadyRead()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scan_ports(){
    ui->PortlistWidget->clear();
    ports_ = QextSerialEnumerator::getPorts();
    for(int i=0; i<ports_.size(); ++i){
        ui->PortlistWidget->addItem(ports_.at(i).portName);
    }
}

void MainWindow::show_port_info(QListWidgetItem* item){
    for(QList<QextPortInfo>::iterator iter = ports_.begin(); iter!=ports_.end(); ++iter){
        if((*iter).portName == item->text()){
            QString add_prefix = "Friendly name: " + (*iter).friendName;
            ui->portInfoEdit->setPlainText(add_prefix);
            add_prefix = "Physical name: " + (*iter).physName;
            ui->portInfoEdit->append(add_prefix);
            add_prefix = "Enumerator name: " + (*iter).enumName;
            ui->portInfoEdit->append(add_prefix);
            add_prefix = "Vendor ID: " + QString::number((*iter).vendorID, 16);
            ui->portInfoEdit->append(add_prefix);
            add_prefix = "Product ID: " + QString::number((*iter).productID, 16);
            ui->portInfoEdit->append(add_prefix);
        }
    }
    ui->captureButton->setDisabled(false);
    enable_settings();
}

void MainWindow::on_captureButton_toggled(bool check){
    if(ui->PortlistWidget->currentItem()->isSelected())
        check ? on_captureButton_pressed() : on_captureButton_released();
}

void MainWindow::on_captureButton_pressed(){
    disable_settings();
    ui->captureButton->setText("Stop capture");
    ui->PortlistWidget->setDisabled(true);
    ui->portInfoEdit->setDisabled(true);
    if(!port_->isOpen()){
        port_->setPortName(ui->PortlistWidget->currentItem()->text());
        port_->open(QIODevice::ReadWrite);
    }
    if (port_->isOpen()) {
        if (port_->queryMode() == QextSerialPort::EventDriven){
            ack_timer_->start();
        }
        QString command = "start" + ui->crcModeBox->currentText() + " " + QString::number(ui->spinBox->value()).toAscii() + "\\";
        port_->write(command.toLatin1());
    }
}

void MainWindow::on_captureButton_released(){
    enable_settings();
    ui->captureButton->setText("Start capture");
    ui->PortlistWidget->setDisabled(false);
    ui->portInfoEdit->setDisabled(false);
    QString command = "stop\\";
    port_->write(command.toLatin1());
}

void MainWindow::onBaudRateChanged(int idx)
{
    port_->setBaudRate((BaudRateType)ui->BaudRateBox->itemData(idx).toInt());
}

void MainWindow::onParityChanged(int idx)
{
    port_->setParity((ParityType)ui->parityBox->itemData(idx).toInt());
}

void MainWindow::onDataBitsChanged(int idx)
{
    port_->setDataBits((DataBitsType)ui->dataBitsBox->itemData(idx).toInt());
}

void MainWindow::onStopBitsChanged(int idx)
{
    port_->setStopBits((StopBitsType)ui->stopBitsBox->itemData(idx).toInt());
}

void MainWindow::onReadyRead(){
    if(port_->bytesAvailable()){
        buffer_.append(port_->readAll());
        foreach(QString current_command, acks_){
            if(buffer_.contains(current_command.toStdString().c_str())){
                (this->*delegate_[current_command])();
                    return;
            }
            if(ack_start_flag_)
                write_and_clear_buffer();
        }
    }
}


void MainWindow::check_sniffer(){
    if(ack_req_flag_){
        port_->write("ack\\");
        ack_req_flag_ = false;
    }
    else{
        port_->close();
        ack_timer_->stop();
    }
}

void MainWindow::disable_settings(){
    ui->BaudRateBox->setDisabled(true);
    ui->dataBitsBox->setDisabled(true);
    ui->parityBox->setDisabled(true);
    ui->stopBitsBox->setDisabled(true);
    ui->spinBox->setDisabled(true);
    ui->crcModeBox->setDisabled(true);
}

void MainWindow::enable_settings(){
    ui->BaudRateBox->setDisabled(false);
    ui->dataBitsBox->setDisabled(false);
    ui->parityBox->setDisabled(false);
    ui->stopBitsBox->setDisabled(false);
    ui->spinBox->setDisabled(false);
    ui->crcModeBox->setDisabled(false);
}

void MainWindow::write_and_clear_buffer(){
    if(buffer_.size()>buffer_.data()[0] && buffer_.data()[0]){
        qint64 current_time = QDateTime::currentMSecsSinceEpoch();
        static char current_time_in_bytes[8];
        for(int i=0; i<8; i++)
            current_time_in_bytes[7-i] = current_time>>8*i;
        QByteArray message(current_time_in_bytes, 8);
        message.append('\0');
        message.append(buffer_.left(buffer_.data()[0]+1));
        buffer_.remove(0, buffer_.data()[0]+1);
        file_.write(message);
        QtConcurrent::run(this, &MainWindow::send_message, message);
    }
}

void MainWindow::send_message(const QByteArray& message){
    QUdpSocket socket;
    socket.writeDatagram(message, QHostAddress("127.0.0.1"), 10000);
}



void MainWindow::on_ack_start_received(){
    buffer_.replace("ack start", "");
    ack_timer_->start();
    ack_start_flag_ = true;
}

void MainWindow::on_ack_stop_received(){
    buffer_.replace("ack stop", "");
    port_->close();
    ack_timer_->stop();
}

void MainWindow::on_ack_time_received(){
    buffer_.replace("ack time", "");
    ack_req_flag_ = true;
}

void MainWindow::set_project_file(){
    QFile project_file("project_sniffer.xml");
    file_information_.setFile(project_file);
    project_file.open(QIODevice::WriteOnly);
    QXmlStreamWriter stream(&project_file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("project");
    stream.writeAttribute("version", "0.6.0");
    stream.writeStartElement("events");
    stream.writeStartElement("systemEvents");
    stream.writeStartElement("event");
    stream.writeAttribute("group", "");
    stream.writeAttribute("ID", "0");
    stream.writeAttribute("name", "MessageRecieved");
    stream.writeStartElement("argument");
    stream.writeAttribute("type", "ByteArray");
    stream.writeAttribute("ID", "0");
    stream.writeAttribute("name", "NodeID");
    for(int i=0; i<4; ++i)
        stream.writeEndElement();
    stream.writeStartElement("logFiles");
    stream.writeStartElement("logFile");
    stream.writeAttribute("ID", "0");
    stream.writeAttribute("name", file_.fileName());
    for(int i=0; i<2; ++i)
        stream.writeEndElement();
    stream.writeEndDocument();

}
