#pragma once
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QCandlestickSet>
#include <QtCore/QDateTime>
#include <QWidget>
#include <memory>
#include <deque>
#include <limits>

// Forward declaration
struct MarketTick;

class ChartManager {
private:
    QChart* priceChart;
    QChartView* chartView;
    QLineSeries* priceSeries;
    QLineSeries* bidSeries;
    QLineSeries* askSeries;
    QValueAxis* priceAxis;
    QDateTimeAxis* timeAxis;

    // Data storage for performance
    std::deque<QPointF> priceData;
    std::deque<QPointF> bidData;
    std::deque<QPointF> askData;

    // Chart configuration
    int maxDataPoints;
    QString currentSymbol;
    double minPrice, maxPrice;

    void updateAxisRanges();
    void trimOldData();

public:
    ChartManager(QWidget* parent = nullptr);
    ~ChartManager();

    // Chart setup and configuration
    QChartView* getChartView() { return chartView; }
    void setSymbol(const QString& symbol);
    void setMaxDataPoints(int points) { maxDataPoints = points; }
    void clearChart();

    // Data updates
    void addMarketTick(const MarketTick& tick);
    void addPricePoint(double price, qint64 timestamp);
    void addBidAskPoints(double bid, double ask, qint64 timestamp);

    // Chart styling
    void setChartTheme(bool darkMode = true);
    void enableAntialiasing(bool enable = true);

    // Performance monitoring
    void measureUpdatePerformance(bool enable = true);

    // Utility functions
    void saveChartImage(const QString& filename);
    void exportChartData(const QString& filename);

    void setDarkTheme(bool dark);
    void addDataPoint(double price, qint64 timestamp);
};