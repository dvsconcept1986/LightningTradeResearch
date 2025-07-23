#include <QApplication>
#include "LightningTradeMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    LightningTradeMainWindow window;
    window.show();

    return app.exec();
}
