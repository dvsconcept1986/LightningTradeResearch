// main.cpp - Test program for Mock Data Generator
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QTimer>
#include <Qdatetime.h>
#include <QWidget>
#include <iostream>
#include "MockDataGenerator.h"

// Include our Mock Data Generator
// (You'll need to add MockDataGenerator.cpp to your Visual Studio project)
class MockDataGenerator; // Forward declaration - include the actual header

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setupUI();
        generator = new MockDataGenerator();

        // Connect timer for real-time simulation
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {
            generateAndDisplayTick();
            });
    }

    ~MainWindow() {
        delete generator;
    }

private:
    MockDataGenerator* generator;
    QTimer* timer;
    QTextEdit* dataDisplay;
    QComboBox* symbolSelector;
    QLabel* priceLabel;
    QLabel* volumeLabel;
    QLabel* timestampLabel;
    QPushButton* startButton;
    QPushButton* stopButton;
    QPushButton* generateBatchButton;

    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

        // Title
        QLabel* title = new QLabel("Lightning Trade - Mock Data Generator Test");
        title->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
        mainLayout->addWidget(title);

        // Controls section
        QHBoxLayout* controlsLayout = new QHBoxLayout();

        // Symbol selector
        controlsLayout->addWidget(new QLabel("Symbol:"));
        symbolSelector = new QComboBox();
        symbolSelector->addItems({ "BTCUSD", "ETHUSD", "AAPL", "MSFT", "GOOGL" });
        controlsLayout->addWidget(symbolSelector);

        // Control buttons
        startButton = new QPushButton("Start Real-time");
        stopButton = new QPushButton("Stop");
        generateBatchButton = new QPushButton("Generate Batch");

        controlsLayout->addWidget(startButton);
        controlsLayout->addWidget(stopButton);
        controlsLayout->addWidget(generateBatchButton);
        controlsLayout->addStretch();

        mainLayout->addLayout(controlsLayout);

        // Current data display
        QHBoxLayout* currentDataLayout = new QHBoxLayout();
        priceLabel = new QLabel("Price: --");
        volumeLabel = new QLabel("Volume: --");
        timestampLabel = new QLabel("Last Update: --");

        priceLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: blue;");
        volumeLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: green;");

        currentDataLayout->addWidget(priceLabel);
        currentDataLayout->addWidget(volumeLabel);
        currentDataLayout->addWidget(timestampLabel);
        currentDataLayout->addStretch();

        mainLayout->addLayout(currentDataLayout);

        // Data display area
        dataDisplay = new QTextEdit();
        dataDisplay->setPlainText("Mock market data will appear here...\n\nFeatures tested:\n- Real-time price simulation\n- JSON data generation\n- Market data validation\n- Performance timing\n");
        dataDisplay->setMaximumHeight(400);
        mainLayout->addWidget(dataDisplay);

        // Connect buttons
        connect(startButton, &QPushButton::clicked, [this]() {
            timer->start(1000); // Update every second
            startButton->setEnabled(false);
            stopButton->setEnabled(true);
            });

        connect(stopButton, &QPushButton::clicked, [this]() {
            timer->stop();
            startButton->setEnabled(true);
            stopButton->setEnabled(false);
            });

        connect(generateBatchButton, &QPushButton::clicked, [this]() {
            generateBatchData();
            });

        // Set window properties
        setWindowTitle("Lightning Trade Research - Mock Data Generator");
        resize(800, 600);
    }

    void generateAndDisplayTick() {
        std::string symbol = symbolSelector->currentText().toStdString();

        // Measure performance (as per your research requirements)
        auto startTime = std::chrono::high_resolution_clock::now();

        // Generate data
        MarketTick tick = generator->generateTick(symbol);
        std::string jsonData = generator->generateTickJSON(symbol);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

        // Update current data display
        priceLabel->setText(QString("Price: $%1").arg(tick.price, 0, 'f', 2));
        volumeLabel->setText(QString("Volume: %1").arg(tick.volume));
        timestampLabel->setText(QString("Last Update: %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss")));

        // Add to data display
        QString displayText = QString("=== %1 ===\n").arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
        displayText += QString("Symbol: %1 | Price: $%2 | Volume: %3\n")
            .arg(QString::fromStdString(tick.symbol))
            .arg(tick.price, 0, 'f', 2)
            .arg(tick.volume);
        displayText += QString("Bid: $%1 | Ask: $%2 | Spread: $%3\n")
            .arg(tick.bid, 0, 'f', 2)
            .arg(tick.ask, 0, 'f', 2)
            .arg(tick.ask - tick.bid, 0, 'f', 3);
        displayText += QString("Processing Time: %1 ?s\n").arg(duration.count());
        displayText += QString("JSON: %1\n\n").arg(QString::fromStdString(jsonData));

        dataDisplay->append(displayText);

        // Scroll to bottom
        QTextCursor cursor = dataDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        dataDisplay->setTextCursor(cursor);
    }

    void generateBatchData() {
        // Measure batch processing performance
        auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<MarketTick> batch = generator->generateBatch(5);
        std::string batchJSON = generator->generateBatchJSON(5);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

        QString displayText = QString("\n=== BATCH GENERATION (%1) ===\n")
            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"));
        displayText += QString("Generated %1 ticks in %2 ?s\n").arg(batch.size()).arg(duration.count());
        displayText += QString("Average: %1 ?s per tick\n").arg(duration.count() / batch.size());

        for (const auto& tick : batch) {
            displayText += QString("%1: $%2 (Vol: %3)\n")
                .arg(QString::fromStdString(tick.symbol))
                .arg(tick.price, 0, 'f', 2)
                .arg(tick.volume);
        }

        displayText += "\nBatch JSON:\n" + QString::fromStdString(batchJSON) + "\n";

        dataDisplay->append(displayText);

        // Scroll to bottom
        QTextCursor cursor = dataDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        dataDisplay->setTextCursor(cursor);
    }
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}