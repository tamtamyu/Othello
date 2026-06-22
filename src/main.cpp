#include <QApplication>
#include "OthelloWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    OthelloWindow window;
    window.show();

    return app.exec();
}