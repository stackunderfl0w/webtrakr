#ifndef QRIGHTCLICKBUTTON_H
#define QRIGHTCLICKBUTTON_H

#include <QPushButton>
#include <QMouseEvent>

class QRightClickButton : public QPushButton
{
    Q_OBJECT

public:
    explicit QRightClickButton(const QString &text, QWidget *parent = nullptr);
    char* title;
    char* address;

private slots:
            void mousePressEvent(QMouseEvent *e);

    signals:
            void rightClicked();

public slots:

};

#endif // QRIGHTCLICKBUTTON_H
