#include "LightningTradeMainWindow.h"
#include <QDateTime>
#include <QTextCursor>
#include <QString>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include <chrono>

// Constructor
LightningTradeMainWindow::LightningTradeMainWindow(QWidget* parent)
    : QMainWindow(parent),
    generator(nullptr),
    chartManager(nullptr),
    realTimeTimer(nullptr),
    totalUpdates(0),
    totalUpdateTime(0.0)
{
    setupUI();
    initializeComponents();
    connectSignals();
    updateStatusBar("Ready - Lightning Trade Research App");
}

// Destructor
LightningTradeMainWindow::~LightningTradeMainWindow() {
    delete generator;
    delete chartManager;
}

// UI Setup Methods
void LightningTradeMainWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create main splitter (horizontal)
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);

    // Create right splitter (vertical) for controls and data
    rightSplitter = new QSplitter(Qt::Vertical);

    setupControlPanel();
    setupDataDisplay();
    setupLogDisplay();
    setupChartDisplay();

    // Add widgets to splitters
    mainSplitter->addWidget(chartGroup);      // Chart takes left side
    mainSplitter->addWidget(rightSplitter);   // Controls/data take right side

    rightSplitter->addWidget(controlGroup);   // Controls at top
    rightSplitter->addWidget(dataGroup);      // Data in middle
    rightSplitter->addWidget(logGroup);       // Log at bottom

    // Set splitter proportions
    mainSplitter->setSizes({ 600, 300 });       // Chart gets more space
    rightSplitter->setSizes({ 200, 150, 200 }); // Balanced right side

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);

    // Window properties
    setWindowTitle("Lightning Trade Research Platform v1.0");
    resize(1200, 800);

    // Status bar
    statusBar()->showMessage("Initializing Lightning Trade Research Platform...");
}

void LightningTradeMainWindow::setupControlPanel() {
    controlGroup = new QGroupBox("Control Panel");
    QGridLayout* controlLayout = new QGridLayout(controlGroup);

    // Symbol selection
    controlLayout->addWidget(new QLabel("Symbol:"), 0, 0);
    symbolSelector = new QComboBox();
    symbolSelector->addItems({ "BTCUSD", "ETHUSD", "AAPL", "MSFT", "GOOGL" });
    controlLayout->addWidget(symbolSelector, 0, 1);

    // Update interval
    controlLayout->addWidget(new QLabel("Update Interval (ms):"), 1, 0);
    updateIntervalSpinBox = new QSpinBox();
    updateIntervalSpinBox->setRange(100, 5000);
    updateIntervalSpinBox->setValue(1000);
    updateIntervalSpinBox->setSuffix(" ms");
    controlLayout->addWidget(updateIntervalSpinBox, 1, 1);

    // Max data points
    controlLayout->addWidget(new QLabel("Max Data Points:"), 2, 0);
    maxDataPointsSpinBox = new QSpinBox();
    maxDataPointsSpinBox->setRange(50, 500);
    maxDataPointsSpinBox->setValue(100);
    controlLayout->addWidget(maxDataPointsSpinBox, 2, 1);

    // Theme selection
    darkThemeCheckBox = new QCheckBox("Dark Theme");
    darkThemeCheckBox->setChecked(true);
    controlLayout->addWidget(darkThemeCheckBox, 3, 0, 1, 2);

    // Performance logging
    performanceLoggingCheckBox = new QCheckBox("Performance Logging");
    performanceLoggingCheckBox->setChecked(true);
    controlLayout->addWidget(performanceLoggingCheckBox, 4, 0, 1, 2);

    // Control buttons
    startRealtimeButton = new QPushButton("Start Real-time Feed");
    stopRealtimeButton = new QPushButton("Stop Feed");
    generateBatchButton = new QPushButton("Generate Batch Data");
    clearChartButton = new QPushButton("Clear Chart");

    startRealtimeButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; }");
    stopRealtimeButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; }");

    controlLayout->addWidget(startRealtimeButton, 5, 0, 1, 2);
    controlLayout->addWidget(stopRealtimeButton, 6, 0, 1, 2);
    controlLayout->addWidget(generateBatchButton, 7, 0, 1, 2);
    controlLayout->addWidget(clearChartButton, 8, 0, 1, 2);

    stopRealtimeButton->setEnabled(false);
}

void LightningTradeMainWindow::setupDataDisplay() {
    dataGroup = new QGroupBox("Current Market Data");
    QVBoxLayout* dataLayout = new QVBoxLayout(dataGroup);

    currentPriceLabel = new QLabel("Price: --");
    currentVolumeLabel = new QLabel("Volume: --");
    bidAskSpreadLabel = new QLabel("Bid/Ask Spread: --");
    lastUpdateLabel = new QLabel("Last Update: --");
    performanceLabel = new QLabel("Avg Update Time: --");

    currentPriceLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2196F3;");
    currentVolumeLabel->setStyleSheet("font-size: 14px; color: #4CAF50;");
    bidAskSpreadLabel->setStyleSheet("font-size: 14px; color: #FF9800;");
    lastUpdateLabel->setStyleSheet("font-size: 12px; color: #757575;");
    performanceLabel->setStyleSheet("font-size: 12px; color: #9C27B0;");

    dataLayout->addWidget(currentPriceLabel);
    dataLayout->addWidget(currentVolumeLabel);
    dataLayout->addWidget(bidAskSpreadLabel);
    dataLayout->addWidget(lastUpdateLabel);
    dataLayout->addWidget(performanceLabel);
}

void LightningTradeMainWindow::setupLogDisplay() {
    logGroup = new QGroupBox("Activity Log");
    QVBoxLayout* logLayout = new QVBoxLayout(logGroup);

    logDisplay = new QTextEdit();
    logDisplay->setMaximumHeight(150);
    logDisplay->setReadOnly(true);
    logDisplay->setStyleSheet("font-family: Consolas, Monaco, monospace; font-size: 10px;");

    logLayout->addWidget(logDisplay);
}

void LightningTradeMainWindow::setupChartDisplay() {
    chartGroup = new QGroupBox("Price Chart");
    QVBoxLayout* chartLayout = new QVBoxLayout(chartGroup);
    // ChartManager will add chart view here in initializeComponents()
}

void LightningTradeMainWindow::initializeComponents() {
    // Initialize performance monitoring
    totalUpdates = 0;
    totalUpdateTime = 0.0;

    // Create core components
    generator = new MockDataGenerator(this);
    chartManager = new ChartManager(this);
    realTimeTimer = new QTimer(this);

    // Add chart view to chart group
    QVBoxLayout* chartLayout = qobject_cast<QVBoxLayout*>(chartGroup->layout());
    if (chartLayout) {
        chartLayout->addWidget(chartManager->getChartView());
    }

    // Set initial values
    chartManager->setMaxDataPoints(maxDataPointsSpinBox->value());
    chartManager->setDarkTheme(darkThemeCheckBox->isChecked());

    addLogMessage("Lightning Trade Research Platform initialized successfully.");
}

void LightningTradeMainWindow::connectSignals() {
    connect(startRealtimeButton, &QPushButton::clicked, this, &LightningTradeMainWindow::startRealtimeFeed);
    connect(stopRealtimeButton, &QPushButton::clicked, this, &LightningTradeMainWindow::stopRealtimeFeed);
    connect(generateBatchButton, &QPushButton::clicked, this, &LightningTradeMainWindow::generateBatchData);
    connect(clearChartButton, &QPushButton::clicked, this, &LightningTradeMainWindow::clearChart);

    connect(symbolSelector, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
        this, &LightningTradeMainWindow::onSymbolChanged);
    connect(updateIntervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &LightningTradeMainWindow::onUpdateIntervalChanged);
    connect(maxDataPointsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
        this, &LightningTradeMainWindow::onMaxDataPointsChanged);
    connect(darkThemeCheckBox, &QCheckBox::toggled, this, &LightningTradeMainWindow::onThemeChanged);

    connect(realTimeTimer, &QTimer::timeout, this, &LightningTradeMainWindow::generateRealtimeUpdate);
}

// Slot implementations
void LightningTradeMainWindow::startRealtimeFeed() {
    realTimeTimer->start(updateIntervalSpinBox->value());
    startRealtimeButton->setEnabled(false);
    stopRealtimeButton->setEnabled(true);

    updateStatusBar("Real-time feed started");
    addLogMessage(QString("Started real-time feed for %1 (interval: %2ms)")
        .arg(symbolSelector->currentText())
        .arg(updateIntervalSpinBox->value()));
}

void LightningTradeMainWindow::stopRealtimeFeed() {
    realTimeTimer->stop();
    startRealtimeButton->setEnabled(true);
    stopRealtimeButton->setEnabled(false);

    updateStatusBar("Real-time feed stopped");
    addLogMessage("Stopped real-time feed");
}

void LightningTradeMainWindow::generateBatchData() {
    addLogMessage("Generating batch data...");

    QString currentSymbol = symbolSelector->currentText();

    for (int i = 0; i < 20; ++i) {
        MarketTick tick = generator->generateTick(currentSymbol.toStdString());
        tick.timestamp = QDateTime::currentDateTime().addSecs(-20 + i).toMSecsSinceEpoch();

        chartManager->addDataPoint(tick.bid, tick.timestamp);

        if (i == 19) { // Update display with last tick
            updateDataDisplay(tick);
        }
    }

    updateStatusBar("Batch data generated");
    addLogMessage("Generated 20 historical data points");
}

void LightningTradeMainWindow::clearChart() {
    chartManager->clearChart();
    currentPriceLabel->setText("Price: --");
    currentVolumeLabel->setText("Volume: --");
    bidAskSpreadLabel->setText("Bid/Ask Spread: --");
    lastUpdateLabel->setText("Last Update: --");

    updateStatusBar("Chart cleared");
    addLogMessage("Chart data cleared");
}

void LightningTradeMainWindow::generateRealtimeUpdate() {
    auto startTime = std::chrono::high_resolution_clock::now();

    QString currentSymbol = symbolSelector->currentText();

    MarketTick tick = generator->generateTick(currentSymbol.toStdString());

    chartManager->addDataPoint(tick.bid, tick.timestamp);
    updateDataDisplay(tick);

    auto endTime = std::chrono::high_resolution_clock::now();
    double updateTimeMicros = std::chrono::duration<double, std::micro>(endTime - startTime).count();

    if (performanceLoggingCheckBox->isChecked()) {
        updatePerformanceMetrics(updateTimeMicros);
    }
}

void LightningTradeMainWindow::onSymbolChanged(const QString& symbol) {
    generator->setSymbol(symbol.toStdString());
    chartManager->clearChart();

    updateStatusBar(QString("Symbol changed to %1").arg(symbol));
    addLogMessage(QString("Switched to symbol: %1").arg(symbol));
}

void LightningTradeMainWindow::onUpdateIntervalChanged(int interval) {
    if (realTimeTimer->isActive()) {
        realTimeTimer->setInterval(interval);
    }
    addLogMessage(QString("Update interval changed to %1ms").arg(interval));
}

void LightningTradeMainWindow::onMaxDataPointsChanged(int points) {
    chartManager->setMaxDataPoints(points);
    addLogMessage(QString("Max data points changed to %1").arg(points));
}

void LightningTradeMainWindow::onThemeChanged(bool darkTheme) {
    chartManager->setDarkTheme(darkTheme);
    addLogMessage(QString("Theme changed to %1").arg(darkTheme ? "Dark" : "Light"));
}

void LightningTradeMainWindow::updateDataDisplay(const MarketTick& tick) {
    currentPriceLabel->setText(QString("Price: $%1").arg(tick.price, 0, 'f', 2));
    currentVolumeLabel->setText(QString("Volume: %1").arg(tick.volume));

    double bidValue = tick.bid;
    double askValue = tick.ask;
    double spread = askValue - bidValue;

    bidAskSpreadLabel->setText(QString("Bid/Ask: $%1 / $%2 (?%3)")
        .arg(bidValue, 0, 'f', 2)
        .arg(askValue, 0, 'f', 2)
        .arg(spread, 0, 'f', 4));

    lastUpdateLabel->setText(
        QString("Last Update: %1")
        .arg(QDateTime::fromMSecsSinceEpoch(tick.timestamp).toString("hh:mm:ss.zzz"))
    );
}

void LightningTradeMainWindow::updatePerformanceMetrics(double updateTimeMicros) {
    totalUpdates++;
    totalUpdateTime += updateTimeMicros;

    double avgUpdateTime = totalUpdateTime / totalUpdates;
    performanceLabel->setText(QString("Avg Update Time: %1 ?s").arg(QString::number(avgUpdateTime, 'f', 1)));

    if (totalUpdates % 100 == 0) {
        addLogMessage(QString("Performance: %1 updates, avg %2 ?s")
            .arg(QString::number(totalUpdates))
            .arg(QString::number(avgUpdateTime, 'f', 1)));
    }
}

void LightningTradeMainWindow::addLogMessage(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    logDisplay->append(QString("[%1] %2").arg(timestamp).arg(message));

    // Auto-scroll to bottom
    QTextCursor cursor = logDisplay->textCursor();
    cursor.movePosition(QTextCursor::End);
    logDisplay->setTextCursor(cursor);
}

void LightningTradeMainWindow::updateStatusBar(const QString& message) {
    statusBar()->showMessage(message, 3000);
}
