#include "ChartManager.h"
#include "MockDataGenerator.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCore/QDateTime>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <algorithm>
#include <chrono>
#include <iostream>

ChartManager::ChartManager(QWidget* parent)
    : maxDataPoints(100), minPrice(0), maxPrice(0) {

    // Create chart and series
    priceChart = new QChart();
    chartView = new QChartView(priceChart, parent);

    priceSeries = new QLineSeries();
    bidSeries = new QLineSeries();
    askSeries = new QLineSeries();

    // Configure series
    priceSeries->setName("Price");
    bidSeries->setName("Bid");
    askSeries->setName("Ask");

    // Add series to chart
    priceChart->addSeries(priceSeries);
    priceChart->addSeries(bidSeries);
    priceChart->addSeries(askSeries);

    // Create and configure axes
    timeAxis = new QDateTimeAxis();
    timeAxis->setFormat("hh:mm:ss");
    timeAxis->setTitleText("Time");

    priceAxis = new QValueAxis();
    priceAxis->setTitleText("Price ($)");
    priceAxis->setLabelFormat("%.2f");

    // Attach axes
    priceChart->addAxis(timeAxis, Qt::AlignBottom);
    priceChart->addAxis(priceAxis, Qt::AlignLeft);

    priceSeries->attachAxis(timeAxis);
    priceSeries->attachAxis(priceAxis);
    bidSeries->attachAxis(timeAxis);
    bidSeries->attachAxis(priceAxis);
    askSeries->attachAxis(timeAxis);
    askSeries->attachAxis(priceAxis);

    // Configure chart appearance
    setChartTheme(true); // Dark theme by default
    enableAntialiasing(true);

    // Configure chart view
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(600, 400);

    // Set initial title
    priceChart->setTitle("Lightning Trade - Real-time Market Data");
}

ChartManager::~ChartManager() {
    // QChart and series are automatically deleted by Qt's parent-child system
}

void ChartManager::setSymbol(const QString& symbol) {
    currentSymbol = symbol;
    priceChart->setTitle(QString("Lightning Trade - %1 Real-time Data").arg(symbol));
    clearChart();
}

void ChartManager::clearChart() {
    priceSeries->clear();
    bidSeries->clear();
    askSeries->clear();

    priceData.clear();
    bidData.clear();
    askData.clear();

    minPrice = 0;
    maxPrice = 0;
}

void ChartManager::addMarketTick(const MarketTick& tick) {
    // Performance measurement (as per research requirements)
    auto startTime = std::chrono::high_resolution_clock::now();

    // Convert timestamp to QDateTime
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(tick.timestamp);
    qint64 timeValue = dateTime.toMSecsSinceEpoch();

    // Create data points
    QPointF pricePoint(timeValue, tick.price);
    QPointF bidPoint(timeValue, tick.bid);
    QPointF askPoint(timeValue, tick.ask);

    // Add to data storage
    priceData.push_back(pricePoint);
    bidData.push_back(bidPoint);
    askData.push_back(askPoint);

    // Update series
    priceSeries->append(pricePoint);
    bidSeries->append(bidPoint);
    askSeries->append(askPoint);

    // Trim old data for performance
    trimOldData();

    // Update axis ranges
    updateAxisRanges();

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    // Output performance data (for research validation)
    static int updateCount = 0;
    updateCount++;
    if (updateCount % 10 == 0) { // Log every 10 updates
        std::cout << "Chart update #" << updateCount
            << " took " << duration.count() << " ?s" << std::endl;
    }
}

void ChartManager::addPricePoint(double price, qint64 timestamp) {
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
    QPointF point(dateTime.toMSecsSinceEpoch(), price);

    priceData.push_back(point);
    priceSeries->append(point);

    trimOldData();
    updateAxisRanges();
}

void ChartManager::addBidAskPoints(double bid, double ask, qint64 timestamp) {
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
    qint64 timeValue = dateTime.toMSecsSinceEpoch();

    QPointF bidPoint(timeValue, bid);
    QPointF askPoint(timeValue, ask);

    bidData.push_back(bidPoint);
    askData.push_back(askPoint);

    bidSeries->append(bidPoint);
    askSeries->append(askPoint);

    trimOldData();
    updateAxisRanges();
}

void ChartManager::trimOldData() {
    // Remove old data points to maintain performance
    while (priceData.size() > static_cast<size_t>(maxDataPoints)) {
        priceData.pop_front();
        priceSeries->removePoints(0, 1);
    }

    while (bidData.size() > static_cast<size_t>(maxDataPoints)) {
        bidData.pop_front();
        bidSeries->removePoints(0, 1);
    }

    while (askData.size() > static_cast<size_t>(maxDataPoints)) {
        askData.pop_front();
        askSeries->removePoints(0, 1);
    }
}

void ChartManager::updateAxisRanges() {
    if (priceData.empty()) return;

    // Find min/max values for price axis
    double newMinPrice = std::numeric_limits<double>::max();
    double newMaxPrice = std::numeric_limits<double>::lowest();

    for (const auto& point : priceData) {
        newMinPrice = std::min(newMinPrice, point.y());
        newMaxPrice = std::max(newMaxPrice, point.y());
    }

    for (const auto& point : bidData) {
        newMinPrice = std::min(newMinPrice, point.y());
        newMaxPrice = std::max(newMaxPrice, point.y());
    }

    for (const auto& point : askData) {
        newMinPrice = std::min(newMinPrice, point.y());
        newMaxPrice = std::max(newMaxPrice, point.y());
    }

    // Add some padding
    double padding = (newMaxPrice - newMinPrice) * 0.05; // 5% padding
    newMinPrice -= padding;
    newMaxPrice += padding;

    // Update price axis range
    priceAxis->setRange(newMinPrice, newMaxPrice);

    // Update time axis range
    if (!priceData.empty()) {
        QDateTime startTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(priceData.front().x()));
        QDateTime endTime = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(priceData.back().x()));
        timeAxis->setRange(startTime, endTime);
    }
}

void ChartManager::setChartTheme(bool darkMode) {
    if (darkMode) {
        priceChart->setTheme(QChart::ChartThemeDark);

        // Customize series colors for dark theme
        QPen pricePen(QColor("#00BFFF"), 2); // Deep sky blue
        QPen bidPen(QColor("#32CD32"), 1.5);   // Lime green  
        QPen askPen(QColor("#FF6347"), 1.5);   // Tomato red

        priceSeries->setPen(pricePen);
        bidSeries->setPen(bidPen);
        askSeries->setPen(askPen);

        // Chart background
        priceChart->setBackgroundBrush(QBrush(QColor("#1E1E1E")));
        chartView->setStyleSheet("background-color: #1E1E1E;");

    }
    else {
        priceChart->setTheme(QChart::ChartThemeLight);

        // Customize series colors for light theme
        QPen pricePen(QColor("#0066CC"), 2); // Blue
        QPen bidPen(QColor("#009900"), 1.5); // Green
        QPen askPen(QColor("#CC0000"), 1.5); // Red

        priceSeries->setPen(pricePen);
        bidSeries->setPen(bidPen);
        askSeries->setPen(askPen);

        chartView->setStyleSheet("background-color: white;");
    }
}

void ChartManager::enableAntialiasing(bool enable) {
    if (enable) {
        chartView->setRenderHint(QPainter::Antialiasing, true);
        chartView->setRenderHint(QPainter::TextAntialiasing, true);
    }
    else {
        chartView->setRenderHint(QPainter::Antialiasing, false);
        chartView->setRenderHint(QPainter::TextAntialiasing, false);
    }
}

void ChartManager::measureUpdatePerformance(bool enable) {
    // This method can be used to toggle performance measurement
    // Currently the performance measurement is always enabled in addMarketTick
    Q_UNUSED(enable);
}

void ChartManager::saveChartImage(const QString& filename) {
    QPixmap pixmap = chartView->grab();
    pixmap.save(filename);
    std::cout << "Chart saved to: " << filename.toStdString() << std::endl;
}

void ChartManager::exportChartData(const QString& filename) {
    // This would export the chart data to CSV - placeholder for research features
    std::cout << "Chart data export to: " << filename.toStdString() << " (feature placeholder)" << std::endl;
}

void ChartManager::setDarkTheme(bool dark){
    if (!priceChart) return;

    if (dark) {
        priceChart->setTheme(QChart::ChartThemeDark);
    }
    else {
        priceChart->setTheme(QChart::ChartThemeLight);
    }
}

void ChartManager::addDataPoint(double price, qint64 timestamp){
    if (!priceSeries || !priceAxis || !timeAxis)
        return;

    // Convert timestamp to QDateTime for X-axis (QDateTimeAxis expects QDateTime)
    QDateTime time = QDateTime::fromMSecsSinceEpoch(timestamp);
    QPointF newPoint(time.toMSecsSinceEpoch(), price);

    // Store the point for later trimming and axis calculations
    priceData.push_back(newPoint);
    priceSeries->append(newPoint);

    // Enforce max number of points
    if (priceData.size() > static_cast<size_t>(maxDataPoints)) {
        priceData.pop_front();
        priceSeries->remove(0);
    }

    // Update min/max for price axis
    minPrice = std::numeric_limits<double>::max();
    maxPrice = std::numeric_limits<double>::lowest();

    for (const auto& pt : priceData) {
        minPrice = std::min(minPrice, pt.y());
        maxPrice = std::max(maxPrice, pt.y());
    }

    // Add some padding
    double padding = (maxPrice - minPrice) * 0.1;
    if (padding == 0.0) padding = 1.0; // Avoid zero range
    priceAxis->setRange(minPrice - padding, maxPrice + padding);

    // Update time axis range (e.g., last N seconds)
    if (!priceData.empty()) {
        qint64 startTime = priceData.front().x();
        qint64 endTime = priceData.back().x();
        timeAxis->setRange(QDateTime::fromMSecsSinceEpoch(startTime),
            QDateTime::fromMSecsSinceEpoch(endTime));
    }
}
