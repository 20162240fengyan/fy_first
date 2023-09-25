#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include <QFile>

struct table
{
    QString column_1;
    QString column_2;
    QString column_3;
    QString column_4;
    QString column_5;
    table()
    {
        column_1 = "";
        column_2 = "";
        column_3 = "";
        column_4 = "";
        column_5 = "";
    }
    void operator = (const table value)
    {
        column_1 = value.column_1;
        column_2 = value.column_2;
        column_3 = value.column_3;
        column_4 = value.column_4;
        column_5 = value.column_5;
    }
};


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DealFileThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_action_triggered();

    void on_actOpen_triggered();

    void on_actNew_triggered();

    void on_actSave_triggered();

    void on_actUndo_triggered();

    void on_actCut_triggered();

    void on_pushButton_clicked();

    void on_btnChange_clicked();

    void on_btnDelete_clicked();

    void on_btnSendTcpData_clicked();

    void on_btnSelect_clicked();

public slots:
    void onNewConnect();

    void onReadMsg();

    void onDisplayError(QAbstractSocket::SocketError e);

    void onTimeout();

private:
    /**
     * @brief setStatusBar  设置状态栏
     */
    void setStatusBar();

    /**
     * @brief initTableWidget 初始化table widget
     */
    void initTableWidget();

    /**
     * @brief getCurrentData  获取tableWidget指定行数据
     * @param i      指定的行
     * @param value  获取的数据
     */
    void getCurrentData(int i, table &value);

    /**
     * @brief setCurrentData 设置//设置指定行数据指定行数据
     * @param i      指定的行
     * @param value  设置的数据
     */
    void setCurrentData(int i, table value);

    /**
     * @brief initTreeView   初始化tree view
     */
    void initTreeView();

    void initTcpServer();

    void initTcpClient();

private:
    Ui::MainWindow *ui;

    QWidget *newWidget;
    QLabel *m_status1;
    QLabel *m_status2;
    QLabel *m_status3;

    QStandardItemModel* m_treeModel;

    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSerSock;
    QTcpSocket *m_tcpCliSock;
    int m_cliSendCount;
    QTimer *m_tcpSendTimer;
    DealFileThread *m_dealFileThread;
};


class DealFileThread : public QThread
{
    Q_OBJECT
public:
    DealFileThread(QString filepath);
    ~DealFileThread();

    void run();
public slots:
    void DealFile();

private:
    QTimer *m_timer;
    QString m_filePath;
    QFile *m_file;
    int count;
};

#endif // MAINWINDOW_H
