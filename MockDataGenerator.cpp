#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include "MockDataGenerator.h"
#include "LightningTradeMainWindow.h"
#include <chrono>
#include <cmath>
#include <thread>

MockDataGenerator::MockDataGenerator()
    : gen(rd()), priceDist(-1.0, 1.0), volumeDist(100, 10000), spreadDist(0.01, 0.05),
    basePrice(50000.0), lastPrice(50000.0), currentSymbol("BTCUSD") {
}

MockDataGenerator::MockDataGenerator(const MockDataGenerator& other)
    : QObject(),
    gen(other.gen),
    priceDist(other.priceDist),
    volumeDist(other.volumeDist),
    spreadDist(other.spreadDist),
    basePrice(other.basePrice),
    lastPrice(other.lastPrice),
    currentSymbol(other.currentSymbol)
{
}

MockDataGenerator::MockDataGenerator(LightningTradeMainWindow* parent)
    : QObject(parent),
    gen(rd()),
    priceDist(-1.0, 1.0),
    volumeDist(100, 1000),
    spreadDist(0.01, 0.2),
    basePrice(30000.0),
    lastPrice(30000.0),
    currentSymbol("BTCUSD")
{
}



void MockDataGenerator::updateBasePrice(const std::string& symbol) {
    if (currentSymbol != symbol) {
        currentSymbol = symbol;

        // Set realistic base prices for different symbols
        if (symbol == "BTCUSD") {
            basePrice = 45000.0 + (std::uniform_real_distribution<double>(0, 10000))(gen);
        }
        else if (symbol == "ETHUSD") {
            basePrice = 2500.0 + (std::uniform_real_distribution<double>(0, 1000))(gen);
        }
        else if (symbol == "AAPL") {
            basePrice = 150.0 + (std::uniform_real_distribution<double>(0, 50))(gen);
        }
        else if (symbol == "MSFT") {
            basePrice = 300.0 + (std::uniform_real_distribution<double>(0, 100))(gen);
        }
        else if (symbol == "GOOGL") {
            basePrice = 2500.0 + (std::uniform_real_distribution<double>(0, 500))(gen);
        }
        else {
            basePrice = 100.0 + (std::uniform_real_distribution<double>(0, 900))(gen);
        }

        lastPrice = basePrice;
    }
}

double MockDataGenerator::generateRealisticPriceMovement() {
    // Generate more realistic price movements using a random walk
    double change = priceDist(gen) * (basePrice * 0.001); // 0.1% max change per tick

    // Add some trend bias (slight upward bias)
    double trendBias = (std::uniform_real_distribution<double>(-0.1, 0.2))(gen);
    change += trendBias * (basePrice * 0.0005);

    // Apply the change with some momentum
    lastPrice += change;

    // Prevent price from going below a reasonable minimum
    double minPrice = basePrice * 0.8;
    double maxPrice = basePrice * 1.2;
    lastPrice = std::max(minPrice, std::min(maxPrice, lastPrice));

    return lastPrice;
}

MarketTick MockDataGenerator::generateTick(const std::string& symbol) {
    updateBasePrice(symbol);

    // Generate realistic timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();

    // Generate price with realistic movement
    double price = generateRealisticPriceMovement();

    // Generate bid/ask spread
    double spreadPercent = spreadDist(gen);
    double spreadAmount = price * (spreadPercent / 100.0);
    double bid = price - (spreadAmount / 2.0);
    double ask = price + (spreadAmount / 2.0);

    // Generate volume
    int volume = volumeDist(gen);

    // Generate OHLC data (simplified)
    double high = price + (price * 0.001 * std::uniform_real_distribution<double>(0, 1)(gen));
    double low = price - (price * 0.001 * std::uniform_real_distribution<double>(0, 1)(gen));
    double open = price + (price * 0.0005 * priceDist(gen));

    return MarketTick(symbol, price, volume, timestamp, bid, ask, high, low, open);
}

std::vector<MarketTick> MockDataGenerator::generateBatch(int count, const std::string& symbol) {
    std::vector<MarketTick> batch;
    batch.reserve(count);

    for (int i = 0; i < count; ++i) {
        batch.push_back(generateTick(symbol));

        // Add small delay between batch items for realistic timestamps
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return batch;
}

void MockDataGenerator::setVolatility(double volatility) {
    // Clamp volatility between 0.0 and 1.0
    volatility = std::max(0.0, std::min(1.0, volatility));

    // Adjust price distribution based on volatility
    double range = 2.0 * volatility; // Scale the range
    priceDist = std::uniform_real_distribution<double>(-range, range);
}

void MockDataGenerator::reset() {
    basePrice = 50000.0;
    lastPrice = basePrice;
    currentSymbol = "BTCUSD";
}

void MockDataGenerator::setSymbol(const std::string& symbol) {
    currentSymbol = symbol;
    updateBasePrice(symbol);
}

MarketTick parseMarketTickFromJson(const QString& jsonString){
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "JSON Parse Error:" << parseError.errorString();
        return MarketTick(); 
    }

    QJsonObject obj = doc.object();

    return MarketTick{
        obj.value("symbol").toString("BTCUSD").toStdString(),
        obj.value("price").toDouble(0.0),
        obj.value("volume").toInt(0),
        obj.value("timestamp").toVariant().toLongLong(),
        obj.value("bid").toDouble(0.0),
        obj.value("ask").toDouble(0.0),
        obj.value("high").toDouble(0.0),
        obj.value("low").toDouble(0.0),
        obj.value("open").toDouble(0.0)
    };
}
