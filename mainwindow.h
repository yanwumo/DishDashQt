#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QtBluetooth/QtBluetooth>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager1, *manager2;
    QTimer *timer;
    QBluetoothSocket *socket;

private slots:
    void replyFinished1(QNetworkReply *);
    void replyFinished2(QNetworkReply *);
    void orderRequest();
    void on_pushButton_clicked();
    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void readSocket();
    void connected();
};

#endif // MAINWINDOW_H
