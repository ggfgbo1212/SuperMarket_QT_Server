#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    resize(1024,800);
    socket=new QTcpSocket(this);
    log=new LoadingDialog(this);
    log2=new LoadingDialog(this);
    timer=new QTimer(this);
    cheout=new QTimer(this);

    timer->start(800);

    table_init();

    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);

    log->setTipsText("正在等待服务器响应.....");
    log2->setTipsText("支付中,等待刷卡...");
    log->show();

    connect_function();
    ui->label_2->setText(QString("温度: %1 ℃             湿度: %2%               总计:%3  ").arg(wendu).arg(shidu).arg(money));
}

void Widget::connect_function()
{
    connect(socket,&QTcpSocket::connected,this,&Widget::connected_function);
    connect(socket,&QTcpSocket::readyRead,this,&Widget::read_buf);
    connect(socket,&QTcpSocket::disconnected,this,&Widget::disconnected_function);
    connect(cheout,&QTimer::timeout,this,&Widget::cheout_timeout);

    connect(timer,&QTimer::timeout,[=](){
        socket->connectToHost(QHostAddress(M_IP),8889);
    });

    connect(log2,&LoadingDialog::cancelWaiting,[=](){

    });

}

void Widget::table_init()
{
    ui->tableWidget->horizontalHeader()->setMinimumHeight(50);  //设置表头高
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置整行选中
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(65); //设置行高
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//单元格不可编辑
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//设置只选择一行
}

void Widget::table_insert(QStringList li)
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

      for(int i=0;i<4;i++)
      {
        QTableWidgetItem *name=new QTableWidgetItem();//定义一个表格元素的类
        QString DI_Number=li.at(i);
        name->setText(DI_Number);//赋予到表格元素中
        name->setFont(QFont("黑体", 14));
        if(i==2)
        {
          money+= DI_Number.toLong();
        }
        ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i,name);//显示到N行N列
        ui->tableWidget->item(ui->tableWidget->rowCount()-1,i)->setTextAlignment(Qt::AlignCenter);//设置文字居中
        ui->label_2->setText(QString("温度: %1 ℃             湿度: %2%               总计:%3  ").arg(wendu).arg(shidu).arg(money));
        }
}


void Widget::disconnected_function()
{
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);

    timer->start();
    ui->label_4->setText("等待读卡器有信号!!!");
    log->show();
}

void Widget::read_buf()
{
     QByteArray array=socket->readLine();
     QString swi=array;
     swi.chop(1);
     int a=swi.toInt();

     switch (a)
     {
     case 1: case1_insert();           //接受到卡号
            break;

     case 2:case2_error1();
            break;

     case 3:case3_Payment();
           break;

     case 4:case4_error2();
           break;

     case 5:case5_nobalance();//用户#123
           break;

     case 6:case6_wiatchekout();
           break ;
     case 7:case7_wenshidu();
         break;

     default:QMessageBox::warning(this,"错误","未定义操作");
         break;
     }

        socket->readAll();       //清空缓冲区
}


void Widget::connected_function()
{
    log->close();
   ui->pushButton->setEnabled(true);
   ui->pushButton_2->setEnabled(true);
   ui->pushButton_3->setEnabled(true);
   socket->write("100111");
   timer->stop();
   ui->label_4->setText("请刷电子标签!!!");

   QMessageBox::information(this,"成功","连接服务器成功，开始使用客户端");
}

Widget::~Widget()
{
     log->close();
     delete ui;
}




void Widget::on_pushButton_2_clicked()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
        if(items.size()==0)
        {
            return ;
        }
        // 删除表格第行
        money-=items.at(2)->text().toInt();
        ui->label_2->setText(QString("温度: %1 ℃             湿度: %2%               总计:%3  ").arg(wendu).arg(shidu).arg(money));


        ui->tableWidget->removeRow(items.at(0)->row());
        ui->tableWidget->clearSelection();        //清除选中状态

}

void Widget::on_pushButton_clicked()
{
    ui->tableWidget->clearContents();//清空除了表头之外的内容
    ui->tableWidget->setRowCount(0);
    money=0;
    ui->label_2->setText(QString("温度: %1 ℃             湿度: %2%               总计:%3  ").arg(wendu).arg(shidu).arg(money));
}



void Widget::on_pushButton_3_clicked()
{
    if(money==0)
    {
        QMessageBox::information(this,"错误","请不要乱结账");
         return ;
    }

     QString sendbuf=QString("1%1").arg(money);
     socket->write(sendbuf.toUtf8());

}

void Widget::cheout_timeout()
{
     cheout->stop();
     log2->close();
     QMessageBox::information(this,"错误","支付超时，重新支付");
     socket->write("2");

}

void Widget::case3_Payment()
{
    log2->close();
    cheout->stop();

    QByteArray array=socket->readLine();
    QString str=array;
    str.chop(1);           //用户#电话#余额
    QString f= QString("当前用户: %1 \n电话:%2 \n成功消费:%3 \n当前余额:%4 ").arg(str.section("#",0,0))
    .arg(str.section("#",1,1)).arg(money).arg(str.section("#",2,2));



    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString time_tr = dateTime .toString("yyyy-MM-dd hh:mm:ss");//格式化时间

    QString sendbuf="3"+str.section("#",1,1)+time_tr+"\r\n"+"用户:"+str.section("#",0,0)+"\r\n"+"购买了商品:\r\n";

    int count = ui->tableWidget->rowCount();
    for(int i = 0; i < count; i++)
    {
        QString text1= ui->tableWidget->item(i,1)->text();//设置文字居中
        QString text2= ui->tableWidget->item(i,2)->text();//设置文字居中
        sendbuf.append("           "+text1+" ------ "+text2+"元\r\n");
    }


    sendbuf.append(QString("总消费:%1 元 ---------------- 剩余余额:%2 元\r\n\r\n\r\n").arg(money).arg(str.section("#",2,2)));
    if(sendbuf.size()>980)
    {
        QMessageBox::warning(this,"错误","数据过多，请分多次购买");
        return;
    }
    socket->write(sendbuf.toUtf8());
     //用户#电话#余额
    money=0;
    ui->tableWidget->clearContents();//清空除了表头之外的内容
    ui->tableWidget->setRowCount(0);
    ui->label_2->setText("总计:"+QString::number(money));

    QMessageBox::information(this,"成功",f);
}

void Widget::case1_insert()
{
    QByteArray arr=socket->readAll();
    QString str=arr;
    table_insert(str.split("#")); //插入数据

}

void Widget::case2_error1()
{
   QMessageBox::warning(this,"错误","读卡器正在被占用！！！");
}

void Widget::case4_error2()
{
    log2->close();
    QMessageBox::warning(this,"错误","无效卡");
}

void Widget::case5_nobalance()
{
    log2->close();
    QByteArray array=socket->readLine();
    QString str=array;
    str.chop(1);

    //用户#电话#余额
    QMessageBox::information(this,"余额不足",
    QString("当前用户 %1 余额不足\n 电话:%2 \r\n当前余额: %3").arg(str.section("#",0,0)).arg(str.section("#",1,1)).arg(str.section("#",2,2)));

}

void Widget::case6_wiatchekout()
{
    cheout->start(15000);
    log2->show();
}

void Widget::case7_wenshidu()
{
    QByteArray array=socket->readLine();
    QString str=array;
    wendu=str.section("#",0,0);
    shidu=str.section("#",1,1);
    ui->label_2->setText(QString("温度: %1 ℃             湿度: %2%               总计:%3  ").arg(wendu).arg(shidu).arg(money));
}
