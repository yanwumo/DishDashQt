#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QStringList>
#include <QStringListIterator>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager1 = new QNetworkAccessManager(this);
    connect(manager1, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished1(QNetworkReply*)));
    manager2 = new QNetworkAccessManager(this);
    connect(manager2, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished2(QNetworkReply*)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(orderRequest()));
    timer->start(10000);
    orderRequest();

    QBluetoothDeviceDiscoveryAgent *discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
                this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
    discoveryAgent->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::orderRequest()
{
    qDebug() << "Request1";
    manager1->get(QNetworkRequest(QUrl("http://localhost/get_orders.php")));
}

void MainWindow::replyFinished1(QNetworkReply *reply)
{
    ui->treeWidget->clear();

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QString all = codec->toUnicode(reply->readAll());
    reply->deleteLater();

    QStringList sl = all.split(' ', QString::SkipEmptyParts);
    QStringListIterator it(sl);
    while (it.hasNext()) {
        QStringList temp;
        temp << it.next();
        temp << it.next();
        temp << it.next();
        new QTreeWidgetItem(ui->treeWidget, temp);
    }
}

void MainWindow::replyFinished2(QNetworkReply *reply)
{
    ui->treeWidget->clear();

    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QString all = codec->toUnicode(reply->readAll());
    reply->deleteLater();

    QStringList sl = all.split(' ', QString::SkipEmptyParts);
    qDebug() << sl[0] << sl[1];
    if (sl[0] == "success") {
        ui->pushButton->setEnabled(false);
        QByteArray instruction = "^^^^^^^";
        instruction.append(sl[1]);
        instruction.append("$$$$$$$");
        qDebug() << instruction;
        socket->write(instruction);
    } else {
        qDebug() << "Error";
    }
    timer->start(10000);
    orderRequest();
}

void MainWindow::on_pushButton_clicked()
{
    timer->start(10000);
    QString table = "";
    QString request = "";
    for (QTreeWidgetItem *item : ui->treeWidget->selectedItems()) {
        if (request == "") {
            request += "?id[]=";
        } else {
            request += "&id[]=";
        }
        request += item->text(0);
        if (table != "" && table != item->text(2)) {
            qDebug() << "error";
            return;
        }
        table = item->text(2);
    }
    qDebug() << "Request2";
    qDebug() << request;
    manager2->get(QNetworkRequest(QUrl("http://localhost/finish_orders.php" + request)));
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *, int)
{
    timer->start(10000);
}

void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    qDebug() << "Found new device:" << device.name() << '(' << device.address().toString() << ')';
    if (device.name() == "BEN") {
        socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
        socket->connectToService(device.address(), QBluetoothUuid::SerialPort);
        connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
        connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    }
}

void MainWindow::readSocket() {
    if (socket == nullptr) return;

    qDebug() << "readSocket()";
    char c;
    socket->getChar(&c);
    qDebug() << c;
    if (c == 'f') ui->pushButton->setEnabled(true);
}

void MainWindow::connected() {
    qDebug() << "connected";
    socket->write("Hello\r\n", 7);
}
