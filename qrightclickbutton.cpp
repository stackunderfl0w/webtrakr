#include "qrightclickbutton.h"
#include <iostream>

QRightClickButton::QRightClickButton(const QString &text, QWidget *parent) :
        QPushButton(text, parent)
{
}

void QRightClickButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton)
        emit rightClicked();
    if(e->button()==Qt::MiddleButton)
        emit rightClicked();

    //call super function to not miss out on left clicks
    QPushButton::mousePressEvent(e);
}