#include "mywidget.h"
#include <QStyleOption>
#include <QPainter>
#include <QPixmap>
#include <QImage>

MyWidget::MyWidget(QWidget *parent)
    :QWidget(parent)
{
    this->setAutoFillBackground(true);
}

MyWidget::~MyWidget()
{

}

void MyWidget::paintEvent(QPaintEvent *)
{

    QImage image("://Image/cut.png");
    if(!image.isNull())
    {
        QPainter painter(this);
        QPixmap pixmap = QPixmap::fromImage(image).scaled(this->size(), Qt::KeepAspectRatio);

        int x = (this->width() - pixmap.width()) / 2;
        int y = (this->height() - pixmap.height()) / 2;
        painter.drawPixmap(x, y, pixmap);
    }

}
