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
#include <QVBoxLayout>
#include <QStackedWidget>
#include <chrono>
#include "MockDataGenerator.h"
#include "ChartManager.h"
#include "WebSocketClient.h"

namespace Ui {
    class LightningTradeMainWindow;
}

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
    void handleWebSocketMessage(const QString& message);
    void startWebSocket();
    void onWebSocketConnected();
    void onWebSocketDisconnected();
    void onWebSocketError(const QString& errorString);

private:
    // Core components - REMOVED duplicate chartManager pointer
    MockDataGenerator* generator;
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
    QStackedWidget* chartStack;

    // SINGLE chart manager - no map needed for single main chart
    std::unique_ptr<ChartManager> mainChartManager;

    enum class DataSourceMode {
        MockData,
        LiveFeed
    };

    DataSourceMode currentDataSource = DataSourceMode::MockData;
    QComboBox* dataSourceSelector;

    QString previousSymbol;
    QString currentSymbol;

    // New helper method
    void switchDataSource(DataSourceMode mode);

    // Performance monitoring
    std::chrono::high_resolution_clock::time_point lastUpdateTime;
    int totalUpdates;
    double totalUpdateTime; // in microseconds

    WebSocketClient* websocketClient;

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
    void subscribeToSymbol(const QString& symbol);
    void unsubscribeFromSymbol(const QString& symbol);
    void onDataSourceChanged(DataSourceMode newMode);

    // Helper functions for symbol mapping
    QString normalizeSymbol(const QString& symbol) {
        QString sym = symbol.toUpper();
        sym.remove('/');
        return sym;
    }

    // Convert UI symbol to Kraken format
    QString toKrakenSymbol(const QString& uiSymbol) {
        QString normalized = normalizeSymbol(uiSymbol);
        if (normalized == "BTCUSD") return "XBT/USD";
        if (normalized == "ETHUSD") return "ETH/USD";
        // Add more mappings as needed
        return normalized;
    }

    // Convert Kraken symbol to UI format
    QString fromKrakenSymbol(const QString& krakenSymbol) {
        QString normalized = normalizeSymbol(krakenSymbol);
        if (normalized == "XBTUSD") return "BTCUSD";
        if (normalized == "ETHUSD") return "ETHUSD";
        // Add more mappings as needed
        return normalized;
    }
};

#endif // LIGHTNINGTRADEMAINWINDOW_H