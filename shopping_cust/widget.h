#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QListWidgetItem>
#include<QTcpSocket>
#include<QDebug>
#include<QHostAddress>
#include<QMessageBox>
#include "dialog.h"
#include<QTimer>
#include<QDateTime>

#define M_IP "127.0.0.1"



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void connect_function();
    void table_init();
    void table_insert(QStringList li);
    void cheout_timeout();

    void case3_Payment();
    void case1_insert();
    void case2_error1();
    void case4_error2();
    void case5_nobalance();
    void case6_wiatchekout();
    void case7_wenshidu();


private slots:

    void connected_function();
    void disconnected_function();
    void read_buf();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();



private:
    int money=0;
    QTimer * timer=NULL;
    LoadingDialog * log=NULL;
    LoadingDialog * log2=NULL;

    QString wendu="";
    QString shidu="";
    QTcpSocket *socket=NULL;
    QTimer *cheout=NULL;

    Ui::Widget *ui;
};
#endif // WIDGET_H
