#include <QApplication>

#include "senoid_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SenoidWidget main;

    main.resize(1280, 720);

    main.show();

    return a.exec();
}
