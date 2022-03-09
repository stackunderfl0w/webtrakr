

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCore/QProcess>
#include <QLabel>


class QPushButton;
class QTextBrowser;
class QGridLayout;
class QScrollArea;
class QLabel;
class QFormLayout;
class QLineEdit;


// This is the declaration of our MainWidget class
// The definition/implementation is in mainwidget.cpp
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0); //Constructor
    ~MainWidget(); // Destructor

private slots:
    //void onButtonReleased(); // Handler for button presses
    //void onCaptureProcessOutput(); // Handler for Process output
public slots:
    void setValue();
    void clickChapter();
    void rightClickChapter();
    void back_button_clicked();
    void add_button_clicked();
    void add_new_manga();
    void reload_manga();
    void reload_chapter_buttons();
    void scrollDown();

signals:
    void reload_chapter_buttons_signal();
    void valueChanged(int newValue);

private:
    void manga_loader();
    QGridLayout *mainLayout;
    QGridLayout *subLayout;
    QGridLayout *titleLayout;
    QGridLayout *buttonLayout;

    QLabel *title;
    QPushButton *backButton;
    QPushButton *addButton;
    QPushButton *reloadButton;
    QPushButton *scrollDownButton;
    QScrollArea *scrollArea;
    QScrollArea *subScrollArea;

    QLabel *loadingicon;
    QMovie *loadingmovie;

    QGridLayout *new_manga;//fuck from layou
    QWidget *new_manga_carrier;
    QLineEdit *titleLineEdit;
    QLineEdit *addressLineEdit;

    QProcess process_;   // This is the process the button will fire off
};

#endif // MAINWIDGET_H

