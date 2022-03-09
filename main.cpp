
#include <iostream>
#include <QtWidgets/QtWidgets>
#include <chrono>
#include <thread>
#include "mainwidget.h"

void other_thread(MainWidget *app){
    std::this_thread::sleep_for(std::chrono::seconds(5));
    //QMetaObject::invokeMethod( task, "doTask", Q_ARG( int, param ) );
    std::cout<<"hi"<<std::endl;
    //app->setValue();


}

int main(int argc, char *argv[])
{
    // Creates an instance of QApplication
    QApplication a(argc, argv);

    // This is our MainWidget class containing our GUI and functionality
    MainWidget w;
    w.setMinimumHeight(600);
    w.setMinimumWidth(360);
    w.show(); // Show main window

    //std::thread app(other_thread, &w);

    // run the application and return execs() return value/code
    return a.exec();
}

