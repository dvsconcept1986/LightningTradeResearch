#include <QApplication>
#include "LightningTradeMainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    qRegisterMetaType<MarketTick>("MarketTick");

    LightningTradeMainWindow window;
    window.show();

    return app.exec();
}
