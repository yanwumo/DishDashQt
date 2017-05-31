#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QStringList>
#include <QStringListIterator>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(orderRequest()));
    timer->start(30000);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::orderRequest()
{
    manager->get(QNetworkRequest(QUrl("http://localhost/get_orders.php")));
}

void replyFinished(QNetworkReply *reply)
{
    QTextCodec *codec = QTextCodec::codecForName("utf8");
    QString all = codec->toUnicode(reply->readAll());
    reply->deleteLater();

    QStringList sl = all.split(' ', QString::SkipEmptyParts);
    QStringListIterator it(sl);
    while (it.hasNext()) {
        QStringList temp;
        temp << it.next() << it.next() << it.next();
        new QTreeWidgetItem(ui->treeWidget, temp);
    }
}

void MainWindow::on_pushButton_clicked()
{
    for (const QTreeWidgetItem &item : ui->treeWidget->selectedItems()) {
        QString s = item.text(0);
    }
}
