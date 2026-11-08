#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    resize(1200,1100);
    setWindowTitle("无人购物超市管理系统");

    dial=new LoadingDialog(this);
    timer=new QTimer(this);
    socket=new QTcpSocket(this);

    myconnect();
    timer->start(800);
    dial->setTipsText("正在等待服务器响应.....");
    dial->show();

    any_init();
}

void Widget::myconnect()
{
    connect(socket,&QTcpSocket::connected,this,&Widget::connected_function);
    connect(socket,&QTcpSocket::readyRead,this,&Widget::read_buf);
    connect(socket,&QTcpSocket::disconnected,this,&Widget::disconnected_function);
    connect(timer,&QTimer::timeout,[=](){
     socket->connectToHost(QHostAddress(M_IP),8889);
    });
}


void Widget::disconnected_function()
{
    dial->show();
    timer->start();
    //socket->deleteLater(); //deleteLater 会在当前对象的所有事件处理完成后再删除对象
}

void Widget::read_buf()
{
     QByteArray array=socket->readLine();//读取管理员端发送来的信息
     QString sw=array;
     sw.chop(1);//去除/n
     switch (sw.toInt())
     {
     case 1:case1_getcode();  //申请注册会员卡成功
         break;

     case 2:case2_commodity_zhuce();       //申请商品注册成功
         break;

     case 3:case3_admin_zhuce();            //会员注册
         break;

     case 4:case4_chongzhi();               //充值
        break;

     case 5:case5_select();          //会员查询
        break;

     case 6:case6_commodity_add();    //添加商品结果
         break;

     case 7:case7_text1();            //销售记录
         break;

     case 8:case8_admin_init();         //更新会员信息
         break;

     case 9:case9_commodity_init();         //更新商品信息
         break;

     case 10:case10_error();                  //刷卡失败
         break;

     case 11:case11_text2();
         break;

     default:
         QMessageBox::critical(this,"失败","操作,未知操作,稍后重试");
         break;
      }

     socket->readAll();   //清除缓冲区数据
}

void Widget::connected_function()
{
   socket->write("100101");
   timer->stop();
   dial->close();
   QMessageBox::information(this,"成功","连接服务器成功，开始使用客户端");
}

void Widget::table1_Init(int row,QString str)
{
    ui->tableWidget->setRowCount(0);
    
    QStringList li=str.split("\n");

    ui->tableWidget->setRowCount(row);
    for(int i=0;i<row;i++)
    {
        for (int j=0;j<4;j++) {
            QTableWidgetItem *name=new QTableWidgetItem();//定义一个表格元素的类
            QString DI_Number =li.at(i).section("#",j,j);          //定义一个字符串 要添加会变化的字符
            name->setText(DI_Number);//赋予到表格元素中
            name->setFont(QFont("黑体", 14));
            ui->tableWidget->setItem(i,j,name);//显示到N行N列
            ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignCenter);//设置文字居中
        }
    }
}

void Widget::table2_Init(int row,QString str)
{
    QStringList li=str.split("\n");

    ui->tableWidget_2->setRowCount(row);
    for(int i=0;i<row;i++)
    {
        for (int j=0;j<4;j++) {

            QTableWidgetItem *name=new QTableWidgetItem();//定义一个表格元素的类
            QString DI_Number = li.at(i).section("#",j,j);//定义一个字符串 要添加会变化的字符
            name->setText(DI_Number);//赋予到表格元素中
            name->setFont(QFont("黑体", 14));
            ui->tableWidget_2->setItem(i,j,name);//显示到N行N列
            ui->tableWidget_2->item(i,j)->setTextAlignment(Qt::AlignCenter);//设置文字居中
        }
    }
}

void Widget::any_init()
{
    ui->tableWidget->horizontalHeader()->setMinimumHeight(50);  //设置表头高
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//设置整行选中
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(65); //设置行高
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//单元格不可编辑
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);		//设置只选择一行

    ui->tableWidget_2->horizontalHeader()->setMinimumHeight(50);  //设置表头高
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows);//设置整行选中
    ui->tableWidget_2->verticalHeader()->setDefaultSectionSize(65); //设置行高
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);//单元格不可编辑
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);		//设置只选择一行
}

void Widget::case1_getcode()
{
        QByteArray array=socket->readLine();//读取MCU线程发送来的卡号
        QString sw=array;
        sw.chop(1);
        if(sw.toInt())
        {
            ui->lineEdit->setText("请刷卡!");
            ui->pushButton_11->setText("等待刷卡....");
            ui->pushButton_11->setEnabled(false);
        } else     //接受到卡号
        {
            QByteArray arr=socket->readLine();
            ui->lineEdit->setText(arr.remove(arr.size(),1));
            ui->pushButton_11->setText("请求资源");
            ui->pushButton_11->setEnabled(true);
        }

}

void Widget::case2_commodity_zhuce()
{
        QByteArray array=socket->readLine();//
        QString sw=array;
        sw.chop(1);//将\n删除
        if(sw.toInt())
        {
            ui->lineEdit_7->setText("请刷卡!");
            ui->pushButton_7->setText("等待刷卡....");
            ui->pushButton_7->setEnabled(false);
        } else                    //接受到卡号
        {
            QByteArray arr=socket->readLine();//读取卡号
            ui->lineEdit_7->setText(arr.remove(arr.size(),1));
            ui->pushButton_7->setText("请求资源");
            ui->pushButton_7->setEnabled(true);
        }

}

void Widget::case3_admin_zhuce()
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();

    QByteArray array=socket->readLine();
    QString sw=array;
    sw.chop(1);
    if(sw.toInt())        //注册成功
    {
       ui->tableWidget->insertRow(ui->tableWidget->rowCount());

       QStringList li;
       li<<ui->lineEdit->text()<<ui->lineEdit_2->text()<<ui->lineEdit_3->text()<<"0";

       for(int i=0;i<4;i++)
       {
           QTableWidgetItem *name=new QTableWidgetItem();//定义一个表格元素的类
           QString DI_Number=li.at(i);
           name->setText(DI_Number);//赋予到表格元素中
           name->setFont(QFont("黑体", 14));
           ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,i,name);//显示到N行N列
           ui->tableWidget->item(ui->tableWidget->rowCount()-1,i)->setTextAlignment(Qt::AlignCenter);//设置文字居中
       }
          QMessageBox::information(this,"成功","注册成功");
      }
       else               //注册失败
       {
          QMessageBox::critical(this,"失败","注册失败,电子标签或者电话已被注册");
       }

}

void Widget::case4_chongzhi()
{

        QByteArray array=socket->readLine();
        QString sw=array;
        sw.chop(1);
        QString mess=QString("用户:%1 \n成功充值:%2 元\n当前可余额: %3 元").arg(sw.section("#",0,0)).
        arg(ui->lineEdit_4->text()).arg(sw.section("#",3,3));
        QMessageBox::information(this,"成功",mess);

        QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
        if(items.size()==0) return ;

        items.at(3)->setText(sw.section("#",3,3));
        ui->lineEdit_4->clear();
}

void Widget::case5_select()
{

        QByteArray array=socket->readLine();
        QString sw=array;
        sw.chop(1);
        if(sw.toInt())        //查询失败
        {
         QMessageBox::information(this,"失败","未查询到该用户，请检查号码后重试");
        } else               //查询成供
        {
            QByteArray array=socket->readLine();
            QString sw=array;
            sw.chop(1);
            ui->tableWidget->clearContents();//清空除了表头之外的内容
            ui->tableWidget->setRowCount(1);
            for (int j=0;j<4;j++) {
                QTableWidgetItem *name=new QTableWidgetItem();//定义一个表格元素的类
                QString DI_Number =sw.section("#",j,j);//定义一个字符串 要添加会变化的字符
                name->setText(DI_Number);//赋予到表格元素中
                name->setFont(QFont("黑体", 14));
                ui->tableWidget->setItem(0,j,name);//显示到N行N列
                ui->tableWidget->item(0,j)->setTextAlignment(Qt::AlignCenter);//设置文字居中
            }
        }
}

void Widget::case6_commodity_add()
{

    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();

    ui->lineEdit_7->clear();
    ui->lineEdit_9->clear();

        QByteArray array=socket->readLine();
        QString sw=array;
        sw.chop(1);
        if(sw.toInt())        //商品add成功
        {

            QMessageBox::information(this,"成功","商品添加成功");
        } else               //注册失败
        {
         QMessageBox::critical(this,"失败","添加失败,电子标签已被注册");
        }
}

void Widget::case7_text1()
{
        QByteArray array=socket->readAll();
        ui->textEdit->setText(array);
}

void Widget::case8_admin_init()
{
        QByteArray array=socket->readAll();
        QString sw=array;
        table1_Init(sw.count("\n"),sw);
}

void Widget::case9_commodity_init()
{
     QByteArray array=socket->readAll();
     QString sw=array;
     table2_Init(sw.count("\n"),sw);
}

void Widget::case10_error()
{
    QMessageBox::warning(this,"失败","读卡器资源被占用,请稍后再试");
}

void Widget::case11_text2()
{
     QByteArray array=socket->readAll();
     ui->textEdit_2->append(array);
}


Widget::~Widget()
{
    delete ui;
}


void Widget::on_pushButton_3_clicked()   //注册
{
    if(ui->lineEdit_3->text().size()!=11 ||
      ui->lineEdit_2->text().isEmpty() || ui->lineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"错误","电话号码格式错误,或信息未填完整");
        return;
    }
    QString sendbuf=QString("02%1%2%3").arg(ui->lineEdit->text()).arg(ui->lineEdit_2->text()).arg(ui->lineEdit_3->text());
    socket->write(sendbuf.toUtf8());
}

void Widget::on_pushButton_2_clicked()    //查询
{
    if( ui->lineEdit_4->text().isEmpty() || ui->lineEdit_4->text().size()!=11)
    {
      QMessageBox::warning(this,"错误","输入框不允许为空,请输入11电话号码格式");
      ui->lineEdit_4->clear();
      return ;
    }

    socket->write(QString("04%1").arg(ui->lineEdit_4->text()).toUtf8());
}

void Widget::on_pushButton_clicked()        //充值
{
    if( ui->lineEdit_4->text().isEmpty() || ui->lineEdit_4->text().toLong()==0)
    {
      QMessageBox::warning(this,"错误","输入框不允许为空,或者格式错误");
      return ;
    }

  QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
  if(items.size()==0)
  {
      QMessageBox::warning(this,"错误","请选择充值的账户");
      return ;
  }
  socket->write(QString("03%1%2").arg(items.at(0)->text()).arg(ui->lineEdit_4->text().toLong()).toUtf8());
}

void Widget::on_pushButton_5_clicked()        //注销卡
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
    if(items.size()==0)
    {
        return ;
    }
    int count = items.count();
    for(int i = 0; i < count; i++)
    {
        QTableWidgetItem *item = items.at(i);
        QString text = item->text(); //获取内容
        qDebug()<<text<<"::::"<<items.at(i)->row();
    }
    // 删除表格第行
    socket->write(QString("05%1").arg(items.at(0)->text()).toUtf8());

    ui->tableWidget->removeRow(items.at(0)->row());
    ui->tableWidget->clearSelection();        //清除选中状态


}

void Widget::on_pushButton_4_clicked()           //删除全部
{
    ui->tableWidget->clearContents();//清空除了表头之外的内容
    ui->tableWidget->setRowCount(0);

    socket->write("06");
}

void Widget::on_pushButton_9_clicked()          //商品删除
{
    QList<QTableWidgetItem*> items = ui->tableWidget_2->selectedItems();
    if(items.size()==0)
    {
        return ;
    }
    int count = items.count();
    for(int i = 0; i < count; i++)
    {
        QTableWidgetItem *item = items.at(i);
        QString text = item->text(); //获取内容

    }
    // 删除表格第行
    qDebug()<<"sdfsdfsd:"<<items.at(0)->text();

    socket->write(QString("08%1").arg(items.at(0)->text()).toUtf8());

    ui->tableWidget_2->removeRow(items.at(0)->row());
    ui->tableWidget_2->clearSelection();        //清除选中状态
}

void Widget::on_pushButton_11_clicked()    //扫描卡号用户注册
{
    socket->write("12");
}

void Widget::on_pushButton_7_clicked()      //扫描商品添加注册
{
    socket->write("01");
}

void Widget::on_pushButton_6_clicked()     //添加商品
{
    if(ui->lineEdit_5->text().isEmpty()||ui->lineEdit_6->text().isEmpty()||
      ui->lineEdit_7->text().isEmpty() || ui->lineEdit_9->text().isEmpty())
    {
        QMessageBox::warning(this,"错误","信息未填完整");
        return;
    }
    QString sendbuf=QString("07%1%2$%3&%4").arg(ui->lineEdit_7->text()).arg(ui->lineEdit_6->text())
                             .arg(ui->lineEdit_9->text()).arg(ui->lineEdit_5->text());
    socket->write(sendbuf.toUtf8());
}

void Widget::on_pushButton_10_clicked()      //销售记录
{
    socket->write("09");
}

void Widget::on_pushButton_8_clicked()       //更新商品数据
{
   socket->write("10");
}


void Widget::on_pushButton_12_clicked()
{
    socket->write("11");
}
