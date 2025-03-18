#include "stdafx.h"
#include "BaseFinderView.h"
#include <QtWidgets/QApplication>
#include <utility>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BaseFinderView w;
    w.show();
    return a.exec();
}
