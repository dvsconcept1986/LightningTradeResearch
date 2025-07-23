# LightningTradeResearch

LightningTradeResearch is a proof-of-concept application developed to evaluate the viability and integration of specific technologies intended for a larger-scale financial trading project. This lightweight research tool demonstrates real-time data streaming, parsing, and visualization using a streamlined modern C++/Qt stack.

## 🛠 Tech Stack

- **Visual Studio 2022**: Primary development environment
- **Qt 6.9.1**: Used for GUI development and charting (ChartStack)
- **WebSockets**: For live market data stream (connected to Kraken exchange)
- **JSON Parser**: Custom logic for handling Kraken's real-time messaging protocol

## 🎯 Purpose

This application serves as a prototype to validate that the chosen tech stack can support:
- Real-time communication with a crypto exchange
- Parsing and interpreting WebSocket JSON messages
- Displaying financial data using custom charts

## 📈 Features

- Connects to Kraken’s WebSocket API
- Parses JSON data streams for market updates
- Visualizes price feeds in a Qt-based chart system

## 📌 Notes

Originally developed to connect with Binance, the project encountered WebSocket compatibility issues. A pivot to Kraken was made, requiring reimplementation of parsing logic due to protocol differences.

## 📂 Structure

- `MockDataGenerator.cpp/h`: Generates fake price data for testing
- `ChartManager.cpp/h`: Manages Qt charting logic
- `LightningTradeMainWindow.cpp/h`: UI entry point and signal management

## 🧪 Future Work

- Replace mock data with full WebSocket integration
- Improve resilience and reconnection logic for live feeds
- Expand JSON parsing to include depth and volume metrics

---
