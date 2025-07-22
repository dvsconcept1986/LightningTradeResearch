#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>

struct MarketTick {
    std::string symbol;
    double price;
    int volume;
    long long timestamp;  // Unix timestamp in milliseconds
    double bid;
    double ask;
    double high;
    double low;
    double open;
};

class MockDataGenerator {
private:
    std::mt19937 randomGenerator;
    std::uniform_real_distribution<double> priceMovement;
    std::uniform_int_distribution<int> volumeRange;
    std::uniform_real_distribution<double> spreadRange;

    // Current market state
    std::vector<std::string> symbols;
    std::vector<double> currentPrices;
    std::vector<double> dailyOpen;
    std::vector<double> dailyHigh;
    std::vector<double> dailyLow;

    long long getTimestamp();
    double generatePriceMovement(double currentPrice, double volatility = 0.02);
    int generateVolume();

public:
    MockDataGenerator();

    // Configuration
    void addSymbol(const std::string& symbol, double initialPrice);
    void setVolatility(double volatility);

    // Data generation
    MarketTick generateTick(const std::string& symbol);
    std::vector<MarketTick> generateBatch(int count = 10);
    std::string generateTickJSON(const std::string& symbol);
    std::string generateBatchJSON(int count = 10);

    // Historical data simulation
    std::vector<MarketTick> generateHistoricalData(const std::string& symbol,
        int minutes = 60);

    // Utility functions
    void printTick(const MarketTick& tick);
    std::vector<std::string> getAvailableSymbols();
    double getCurrentPrice(const std::string& symbol);
};