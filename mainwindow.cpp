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
    qDebug() << "Request";
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
        QString instruction = "mmmmmm";
        instruction << sl[1];
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
    QString table = nullptr;
    for (QTreeWidgetItem *item : ui->treeWidget->selectedItems()) {
        QString temp = item->text(2);
        if (table && table != temp) {
            qDebug() << "error";
            return;
        }
        table = temp;
    }
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
    qDebug() << "readSocket()";
    while (socket && socket->canReadLine()) {
        QByteArray line = socket->readLine();
        qDebug() << line;
    }
}

void MainWindow::connected() {
    qDebug() << "connected";
    socket->write("Hello\r\n", 7);
}
