

#ifndef MOCKDATAGENERATOR_H
#define MOCKDATAGENERATOR_H

#include <random>
#include <vector>
#include <string>
#include <QObject>
#include <QString>


// Forward declarations 
class LightningTradeMainWindow;


struct MarketTick {
    std::string symbol;
    double price;
    int volume;
    qint64 timestamp;
    double bid;
    double ask;
    double high;
    double low;
    double open;

    MarketTick()  
        : symbol(""), price(0.0), volume(0), timestamp(0),
        bid(0.0), ask(0.0), high(0.0), low(0.0), open(0.0) {
    }

    MarketTick(const std::string& sym, double p, int v, qint64 ts,
        double b, double a, double h, double l, double o)
        : symbol(sym), price(p), volume(v), timestamp(ts),
        bid(b), ask(a), high(h), low(l), open(o) {
    }
};


class MockDataGenerator : public QObject { // Inherit from QObject if you need Qt functionality
    Q_OBJECT 

private:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<double> priceDist;
    std::uniform_int_distribution<int> volumeDist;
    std::uniform_real_distribution<double> spreadDist;

    // Base prices for different symbols
    double basePrice;
    double lastPrice;
    std::string currentSymbol;

    void updateBasePrice(const std::string& symbol);
    double generateRealisticPriceMovement();

public:
    MockDataGenerator();
    explicit MockDataGenerator(LightningTradeMainWindow* parent = nullptr);
    ~MockDataGenerator() = default;
    MockDataGenerator(const MockDataGenerator& other);

    // Generate single market tick
    MarketTick generateTick(const std::string& symbol);

    // Generate batch of ticks
    std::vector<MarketTick> generateBatch(int count, const std::string& symbol = "BTCUSD");

    // Set price volatility (0.0 to 1.0)
    void setVolatility(double volatility);

    // Reset generator state
    void reset();

    // FIXED: Change parameter type to match your usage
    void setSymbol(const std::string& symbol); 
    
};


MarketTick parseMarketTickFromJson(const QString& jsonString);

#endif // MOCKDATAGENERATOR_H

