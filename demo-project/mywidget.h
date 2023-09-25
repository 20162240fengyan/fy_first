#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>

class MyWidget :public QWidget
{
    Q_OBJECT
public:
    MyWidget(QWidget *parent);
    ~MyWidget();

    void paintEvent(QPaintEvent *);
};

#endif // MYWIDGET_H
