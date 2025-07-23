#ifndef LIGHTNINGTRADEMAINWINDOW_H
#define LIGHTNINGTRADEMAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QTextEdit>
#include <QSplitter>
#include <chrono>
#include "MockDataGenerator.h"
#include "ChartManager.h"

class LightningTradeMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit LightningTradeMainWindow(QWidget* parent = nullptr);
    ~LightningTradeMainWindow();

private slots:
    void startRealtimeFeed();
    void stopRealtimeFeed();
    void generateBatchData();
    void clearChart();
    void generateRealtimeUpdate();
    void onSymbolChanged(const QString& symbol);
    void onUpdateIntervalChanged(int interval);
    void onMaxDataPointsChanged(int points);
    void onThemeChanged(bool darkTheme);

private:
    // Core components
    MockDataGenerator* generator;
    ChartManager* chartManager;
    QTimer* realTimeTimer;

    // UI Components
    QWidget* centralWidget;
    QSplitter* mainSplitter;
    QSplitter* rightSplitter;

    // Control Panel
    QGroupBox* controlGroup;
    QComboBox* symbolSelector;
    QPushButton* startRealtimeButton;
    QPushButton* stopRealtimeButton;
    QPushButton* generateBatchButton;
    QPushButton* clearChartButton;
    QSpinBox* updateIntervalSpinBox;
    QSpinBox* maxDataPointsSpinBox;
    QCheckBox* darkThemeCheckBox;
    QCheckBox* performanceLoggingCheckBox;

    // Data Display
    QGroupBox* dataGroup;
    QLabel* currentPriceLabel;
    QLabel* currentVolumeLabel;
    QLabel* bidAskSpreadLabel;
    QLabel* lastUpdateLabel;
    QLabel* performanceLabel;

    // Log Display
    QGroupBox* logGroup;
    QTextEdit* logDisplay;

    // Chart Display
    QGroupBox* chartGroup;

    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastUpdateTime;
    int totalUpdates;
    double totalUpdateTime; // in microseconds

    // Private methods
    void setupUI();
    void setupControlPanel();
    void setupDataDisplay();
    void setupLogDisplay();
    void setupChartDisplay();
    void initializeComponents();
    void connectSignals();
    void updateDataDisplay(const MarketTick& tick);
    void updatePerformanceMetrics(double updateTimeMicros);
    void addLogMessage(const QString& message);
    void updateStatusBar(const QString& message);
};

#endif // LIGHTNINGTRADEMAINWINDOW_H
