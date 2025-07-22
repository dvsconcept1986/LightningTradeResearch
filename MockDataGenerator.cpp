#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "MockDataGenerator.h"

MockDataGenerator::MockDataGenerator()
    : randomGenerator(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())),
    priceMovement(-0.02, 0.02),  // ±2% price movement
    volumeRange(100, 10000),     // Volume between 100-10000
    spreadRange(0.001, 0.01)     // Bid-ask spread 0.1%-1%
{
    // Initialize with some common trading pairs
    addSymbol("BTCUSD", 45000.0);
    addSymbol("ETHUSD", 3000.0);
    addSymbol("AAPL", 150.0);
    addSymbol("MSFT", 300.0);
    addSymbol("GOOGL", 2500.0);
}

void MockDataGenerator::addSymbol(const std::string& symbol, double initialPrice) {
    symbols.push_back(symbol);
    currentPrices.push_back(initialPrice);
    dailyOpen.push_back(initialPrice);
    dailyHigh.push_back(initialPrice);
    dailyLow.push_back(initialPrice);
}

long long MockDataGenerator::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

double MockDataGenerator::generatePriceMovement(double currentPrice, double volatility) {
    double movement = priceMovement(randomGenerator) * volatility;
    return currentPrice * (1.0 + movement);
}

int MockDataGenerator::generateVolume() {
    return volumeRange(randomGenerator);
}

MarketTick MockDataGenerator::generateTick(const std::string& symbol) {
    // Find symbol index
    auto it = std::find(symbols.begin(), symbols.end(), symbol);
    if (it == symbols.end()) {
        throw std::runtime_error("Symbol not found: " + symbol);
    }

    size_t index = std::distance(symbols.begin(), it);

    // Generate new price
    double newPrice = generatePriceMovement(currentPrices[index]);
    currentPrices[index] = newPrice;

    // Update daily high/low
    if (newPrice > dailyHigh[index]) {
        dailyHigh[index] = newPrice;
    }
    if (newPrice < dailyLow[index]) {
        dailyLow[index] = newPrice;
    }

    // Generate bid/ask spread
    double spread = newPrice * spreadRange(randomGenerator);
    double bid = newPrice - (spread / 2.0);
    double ask = newPrice + (spread / 2.0);

    MarketTick tick;
    tick.symbol = symbol;
    tick.price = newPrice;
    tick.volume = generateVolume();
    tick.timestamp = getTimestamp();
    tick.bid = bid;
    tick.ask = ask;
    tick.high = dailyHigh[index];
    tick.low = dailyLow[index];
    tick.open = dailyOpen[index];

    return tick;
}

std::vector<MarketTick> MockDataGenerator::generateBatch(int count) {
    std::vector<MarketTick> batch;
    batch.reserve(count);

    for (int i = 0; i < count; ++i) {
        // Randomly select a symbol
        std::uniform_int_distribution<size_t> symbolSelector(0, symbols.size() - 1);
        size_t symbolIndex = symbolSelector(randomGenerator);

        batch.push_back(generateTick(symbols[symbolIndex]));
    }

    return batch;
}

std::string MockDataGenerator::generateTickJSON(const std::string& symbol) {
    MarketTick tick = generateTick(symbol);

    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "{"
        << "\"symbol\":\"" << tick.symbol << "\","
        << "\"price\":" << tick.price << ","
        << "\"volume\":" << tick.volume << ","
        << "\"timestamp\":" << tick.timestamp << ","
        << "\"bid\":" << tick.bid << ","
        << "\"ask\":" << tick.ask << ","
        << "\"high\":" << tick.high << ","
        << "\"low\":" << tick.low << ","
        << "\"open\":" << tick.open
        << "}";

    return json.str();
}

std::string MockDataGenerator::generateBatchJSON(int count) {
    std::vector<MarketTick> batch = generateBatch(count);

    std::ostringstream json;
    json << std::fixed << std::setprecision(2);
    json << "[";

    for (size_t i = 0; i < batch.size(); ++i) {
        const MarketTick& tick = batch[i];
        json << "{"
            << "\"symbol\":\"" << tick.symbol << "\","
            << "\"price\":" << tick.price << ","
            << "\"volume\":" << tick.volume << ","
            << "\"timestamp\":" << tick.timestamp << ","
            << "\"bid\":" << tick.bid << ","
            << "\"ask\":" << tick.ask << ","
            << "\"high\":" << tick.high << ","
            << "\"low\":" << tick.low << ","
            << "\"open\":" << tick.open
            << "}";

        if (i < batch.size() - 1) {
            json << ",";
        }
    }

    json << "]";
    return json.str();
}

std::vector<MarketTick> MockDataGenerator::generateHistoricalData(const std::string& symbol, int minutes) {
    std::vector<MarketTick> history;
    history.reserve(minutes);

    long long currentTime = getTimestamp();
    long long startTime = currentTime - (minutes * 60 * 1000); // Go back 'minutes' minutes

    for (int i = 0; i < minutes; ++i) {
        MarketTick tick = generateTick(symbol);
        tick.timestamp = startTime + (i * 60 * 1000); // One tick per minute
        history.push_back(tick);
    }

    return history;
}

void MockDataGenerator::printTick(const MarketTick& tick) {
    std::cout << std::fixed << std::setprecision(2)
        << "Symbol: " << tick.symbol
        << " | Price: $" << tick.price
        << " | Volume: " << tick.volume
        << " | Bid: $" << tick.bid
        << " | Ask: $" << tick.ask
        << " | High: $" << tick.high
        << " | Low: $" << tick.low
        << " | Timestamp: " << tick.timestamp << std::endl;
}

std::vector<std::string> MockDataGenerator::getAvailableSymbols() {
    return symbols;
}

double MockDataGenerator::getCurrentPrice(const std::string& symbol) {
    auto it = std::find(symbols.begin(), symbols.end(), symbol);
    if (it == symbols.end()) {
        return 0.0;
    }

    size_t index = std::distance(symbols.begin(), it);
    return currentPrices[index];
}

void MockDataGenerator::setVolatility(double volatility) {
    priceMovement = std::uniform_real_distribution<double>(-volatility, volatility);
}