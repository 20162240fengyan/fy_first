#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mywidget.h"

#include <QList>
#include <QTime>
#include <QtGlobal>
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("界面程序-冯艳-2023.9.21");
    newWidget = nullptr;
    m_status1 = nullptr;
    m_status2 = nullptr;
    m_status3 = nullptr;
    m_treeModel = nullptr;
    m_tcpServer = nullptr;
    m_tcpSerSock = nullptr;
    m_tcpCliSock = nullptr;
    m_cliSendCount = 0;
    m_tcpSendTimer = nullptr;
    m_dealFileThread = nullptr;

    //设置状态栏
    setStatusBar();

    //初始化table widget
    initTableWidget();

    //初始化tree view
    initTreeView();

    initTcpServer();

    initTcpClient();

}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_tcpSendTimer)
    {
        m_tcpSendTimer->stop();
        delete m_tcpSendTimer;
    }

    if(newWidget)
    {
        delete newWidget;
        newWidget = nullptr;
    }


    m_tcpServer->close();
    m_tcpSerSock->disconnectFromHost();
    if(m_tcpSerSock->state()!=QAbstractSocket::UnconnectedState){
        m_tcpSerSock->abort();
    }

    m_tcpCliSock->abort();

    if(m_dealFileThread)
    {
        m_dealFileThread->quit();
        m_dealFileThread->wait();
        delete m_dealFileThread;
    }

}

//设置状态栏
void MainWindow::setStatusBar()
{
    m_status1 = new QLabel("右对齐_1",this);
    m_status1->setAlignment(Qt::AlignRight);
    m_status1->setFixedSize(100,40);

    m_status2 = new QLabel("右对齐_2",this);
    m_status2->setAlignment(Qt::AlignRight);
    m_status2->setFixedSize(100,40);

    m_status3 = new QLabel("左对齐",this);
    m_status3->setAlignment(Qt::AlignLeft);
    m_status3->setFixedSize(100,40);


    ui->statusbar->addPermanentWidget(m_status3);
    ui->statusbar->addWidget(m_status1);
    ui->statusbar->addWidget(m_status2);
    ui->statusbar->setSizeGripEnabled(false);

}

//初始化列表
void MainWindow::initTableWidget()
{
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setRowCount(8);
    for(int i = 0; i < 8; i++)
    {
        QString msg = QString("%1行").arg(i+1);

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(msg+"1列"));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(msg+"2列"));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(msg+"3列"));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(msg+"4列"));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(msg+"5列"));

    }
}


//打开新的空白界面
void MainWindow::on_action_triggered()
{
    if(newWidget != nullptr)
    {
        newWidget->close();
        delete newWidget;
    }
    newWidget = new QWidget;
    newWidget->show();
}


void MainWindow::on_actOpen_triggered()
{
    ui->lineEdit->setText("打开");
}


void MainWindow::on_actNew_triggered()
{
    ui->lineEdit->setText("新建");
}


void MainWindow::on_actSave_triggered()
{
    ui->lineEdit->setText("保存");
}


void MainWindow::on_actUndo_triggered()
{
    ui->lineEdit->setText("撤回");
}


void MainWindow::on_actCut_triggered()
{
    ui->lineEdit->setText("剪切");
}


void MainWindow::on_pushButton_clicked()
{
    int count = ui->tableWidget->rowCount();
    for(int i = 0; i < count/2; i++)
    {
        table value1;
        getCurrentData(i, value1);

        table value2;
        getCurrentData(count-i-1,value2);

        setCurrentData(i,value2);
        setCurrentData(count-i-1, value1);

    }
}


void MainWindow::getCurrentData(int i, table &value)
{
    value.column_1 = ui->tableWidget->item(i,0)->text();
    value.column_2 = ui->tableWidget->item(i,1)->text();
    value.column_3 = ui->tableWidget->item(i,2)->text();
    value.column_4 = ui->tableWidget->item(i,3)->text();
    value.column_5 = ui->tableWidget->item(i,4)->text();
}


void MainWindow::setCurrentData(int i, table value)
{
    ui->tableWidget->setItem(i,0,new QTableWidgetItem(value.column_1));
    ui->tableWidget->setItem(i,1,new QTableWidgetItem(value.column_2));
    ui->tableWidget->setItem(i,2,new QTableWidgetItem(value.column_3));
    ui->tableWidget->setItem(i,3,new QTableWidgetItem(value.column_4));
    ui->tableWidget->setItem(i,4,new QTableWidgetItem(value.column_5));
}

void MainWindow::initTreeView()
{
    m_treeModel = new QStandardItemModel(ui->treeView);
    QStringList headList;
    headList<<"序号"<<"列1"<<"列2"<<"列3"<<"列4"<<"列5";
    m_treeModel->setHorizontalHeaderLabels(headList);

    QList<QStandardItem*> items1;
    QStandardItem* item0 = new QStandardItem("一级节点");
    m_treeModel->appendRow(item0);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    for(int i = 0; i < 8; i++)
    {
        QList<QStandardItem*> items2;
        QStandardItem* item1 = new QStandardItem(QString::number(i+1));
        items2.append(item1);
        for (int j = 0; j < 5 ; j++ )
        {
            QStandardItem* item2 = new QStandardItem(QString::number(qrand()%50));
            items2.append(item2);
        }
        item0->appendRow(items2);
    }

    ui->treeView->setModel(m_treeModel);
}

void MainWindow::on_btnChange_clicked()
{
    QStandardItem* itemRoot = m_treeModel->item(0);
    for(int i = 0; i < itemRoot->rowCount(); i++)
    {
        for(int j = 1; j < 5; j++)
        {
            QStandardItem* item = itemRoot->child(i,j);
            if(item)
            {
                int num = item->text().toInt();
                if(num <= 25)
                {
                    item->setText("0");
                }
            }

        }
    }
}


void MainWindow::on_btnDelete_clicked()
{
    QStandardItem* itemRoot = m_treeModel->item(0);
    for(int i = itemRoot->rowCount(); i > 0; i--)
    {
        if(i%2)
        {
            QStandardItem* item = itemRoot->child(i-1,0);
            if(item)
            {
                itemRoot->removeRow(item->row());
            }
        }

    }
}

void MainWindow::onNewConnect()
{
    m_tcpSerSock = m_tcpServer->nextPendingConnection();
    connect(m_tcpSerSock,&QTcpSocket::disconnected,m_tcpSerSock,&QTcpSocket::deleteLater);
    connect(m_tcpSerSock,&QTcpSocket::readyRead,this,&MainWindow::onReadMsg);
}

void MainWindow::onReadMsg()
{
    QByteArray ba = m_tcpSerSock->readAll();
    //界面显示
    ui->textEdit->append(QString::fromUtf8(ba));
}

void MainWindow::onDisplayError(QAbstractSocket::SocketError e)
{
    ui->label->setText(m_tcpSerSock->errorString());
}

void MainWindow::onTimeout()
{
    m_cliSendCount++;
    m_tcpCliSock->write(QString("第%1次发送").arg(m_cliSendCount).toUtf8());
    m_tcpCliSock->flush();
}


void MainWindow::initTcpServer()
{
    m_tcpServer = new QTcpServer(this);
    if(!m_tcpServer->listen(QHostAddress::AnyIPv4, 6666))
    {
        ui->label->setText("TCP监听失败");
        delete m_tcpServer;
    }
    connect(m_tcpServer,&QTcpServer::newConnection,this,&MainWindow::onNewConnect);



}

void MainWindow::initTcpClient()
{
    m_tcpCliSock = new QTcpSocket(this);
    connect(m_tcpCliSock,SIGNAL(QAbstractSocket::SocketError),this,SLOT(onDisplayError(QAbstractSocket::SocketError)));

    m_tcpCliSock->abort();
    m_tcpCliSock->connectToHost("127.0.0.1",6666);

}

void MainWindow::on_btnSendTcpData_clicked()
{
    if(m_tcpSendTimer == nullptr)
    {
        m_tcpSendTimer = new QTimer;
        m_tcpSendTimer->setInterval(500);
        connect(m_tcpSendTimer,&QTimer::timeout,this,&MainWindow::onTimeout);
        m_tcpSendTimer->start();
    }
}


DealFileThread::DealFileThread(QString filepath)
    :m_filePath(filepath)
{
    m_timer = nullptr;
    m_file = nullptr;
    count = 0;
}

DealFileThread::~DealFileThread()
{
    if(m_timer)
    {
        delete m_timer;
    }
    if(m_file)
    {
        m_file->close();
        delete m_file;
    }
}

void DealFileThread::run()
{
    m_timer = new QTimer;
    m_timer->setInterval(500);
    connect(m_timer,&QTimer::timeout,this,&DealFileThread::DealFile,Qt::DirectConnection);
    m_timer->start();
    this->exec();
}

void DealFileThread::DealFile()
{
    if(m_file == nullptr)
    {
        m_file = new QFile(m_filePath);
        m_file->open(QIODevice::WriteOnly|QIODevice::Append|QIODevice::Text);
    }
    if(m_file->isOpen())
    {
        count++;
        m_file->write("你好，“文件”");
        m_file->flush();

        if(count == 100)
        {
            m_timer->stop();
            m_file->close();
        }
        qDebug()<<"count"<<count;

    }
}

void MainWindow::on_btnSelect_clicked()
{
    if(m_dealFileThread == nullptr)
    {
        QString str = QString("选择“hello.txt”的路径");
        QString strPath = QFileDialog::getExistingDirectory(this,str,QDir::currentPath(),QFileDialog::ShowDirsOnly);
        m_dealFileThread = new DealFileThread(strPath + "/hello.txt");
        m_dealFileThread->start();
    }
    else
    {
        QMessageBox box(QMessageBox::Warning, QString("警告"),QString("已选择“hello.txt”的路径"),QMessageBox::Ok);
        box.exec();
    }


}

