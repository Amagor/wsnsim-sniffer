#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialport.h"
#include "qextserialport.h"
#include "commandhandler.h"
#include "transfernetwork.h"
#include <QXmlStreamWriter>
#include <QFile>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    log_file_.setFileName("data.txt");
    log_file_.open(QIODevice::WriteOnly);

    port_ = new SerialPort;
    command_handler_ = new CommandHandler;
    transfer_network_ = new TransferNetwork;


    //set ui settings
    ui->portInfoEdit->setReadOnly(true);
    ui->captureButton->setCheckable(true);
    ui->captureButton->setDisabled(true);
    disable_settings();
    scan_ports();


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

    set_project_file();


//    udp_settings_ = StaticTools::getClientRealTimeSettings();
//    udp_ip_ = udp_settings_->ip("Sniffer");
//    udp_port_ = udp_settings_->port("Sniffer");
//    qDebug() << udp_ip_;
//    udp_settings_->setProjectPath("Sniffer", file_information_.absoluteFilePath());
//    qDebug() << file_information_.absoluteFilePath();


    connect(ui->PortlistWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(show_port_info(QListWidgetItem*)));
    connect(ui->BaudRateBox, SIGNAL(currentIndexChanged(int)), SLOT(baud_rate_changed(int)));
    connect(ui->parityBox, SIGNAL(currentIndexChanged(int)), SLOT(parity_changed(int)));
    connect(ui->dataBitsBox, SIGNAL(currentIndexChanged(int)), SLOT(data_bits_changed(int)));
    connect(ui->stopBitsBox, SIGNAL(currentIndexChanged(int)), SLOT(stop_bits_changed(int)));
    connect(ui->captureButton, SIGNAL(clicked(bool)), SLOT(captureButton_clicked(bool)));

    connect(this, SIGNAL(channel_number_selected(int)), command_handler_, SLOT(set_channel_number(int)));
    connect(this, SIGNAL(stop_capture_command()), command_handler_, SLOT(send_stop_command()));

    connect(command_handler_, SIGNAL(log_message(QByteArray)), this, SLOT(write_to_log(QByteArray)));
    connect(command_handler_, SIGNAL(log_message(QByteArray)), transfer_network_, SLOT(send_message(QByteArray)));
    connect(command_handler_, SIGNAL(send_message(QByteArray)), port_, SLOT(send_message(QByteArray)));

    connect(port_, SIGNAL(data_received(QByteArray&)), command_handler_, SLOT(get_message(QByteArray&)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete port_;
    delete command_handler_;
}

void MainWindow::scan_ports(){
    ui->PortlistWidget->clear();
    ports_info_ = QextSerialEnumerator::getPorts();
    for(int i=0; i<ports_info_.size(); ++i){
        ui->PortlistWidget->addItem(ports_info_.at(i).portName);
    }
}

void MainWindow::show_port_info(QListWidgetItem* item){
    for(QList<QextPortInfo>::iterator iter = ports_info_.begin(); iter!=ports_info_.end(); ++iter){
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

void MainWindow::captureButton_clicked(bool check){
    if(ui->PortlistWidget->currentItem()->isSelected())
        check ? on_captureButton_pressed() : on_captureButton_released();
}

void MainWindow::on_captureButton_pressed(){
    disable_settings();
    ui->captureButton->setText("Stop capture");
    ui->PortlistWidget->setDisabled(true);
    ui->portInfoEdit->setDisabled(true);
    port_->start_port_session(ui->PortlistWidget->currentItem()->text());
    emit channel_number_selected(ui->spinBox->value());
}

void MainWindow::on_captureButton_released(){
    enable_settings();
    ui->captureButton->setText("Start capture");
    ui->PortlistWidget->setDisabled(false);
    ui->portInfoEdit->setDisabled(false);
    emit stop_capture_command();
}

void MainWindow::write_to_log(QByteArray message){
    log_file_.write(message);
}


void MainWindow::baud_rate_changed(int idx)
{
    port_->set_baud_rate((BaudRateType)ui->BaudRateBox->itemData(idx).toInt());
}

void MainWindow::parity_changed(int idx)
{
    port_->set_parity((ParityType)ui->parityBox->itemData(idx).toInt());
}
void MainWindow::data_bits_changed(int idx)
{
    port_->set_data_bits((DataBitsType)ui->dataBitsBox->itemData(idx).toInt());
}

void MainWindow::stop_bits_changed(int idx)
{
    port_->set_stop_bits((StopBitsType)ui->stopBitsBox->itemData(idx).toInt());
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

void MainWindow::set_project_file(){
    QFile project_file("project_sniffer.xml");
    project_file_info_.setFile(project_file);
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
    stream.writeAttribute("name", log_file_.fileName());
    for(int i=0; i<2; ++i)
        stream.writeEndElement();
    stream.writeEndDocument();

}
