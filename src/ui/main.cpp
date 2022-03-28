#include "labeltool.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    labelTool w;
    w.show();
    return a.exec();
}
