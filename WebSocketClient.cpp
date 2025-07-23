#include "WebSocketClient.h"
#include <QDebug>

WebSocketClient::WebSocketClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    connect(&m_webSocket, &QWebSocket::errorOccurred, this, &WebSocketClient::onErrorOccurred);


}

WebSocketClient::~WebSocketClient() {
    m_webSocket.close();
}

void WebSocketClient::connectToServer(const QUrl& url) {
    m_webSocket.open(url);
}

void WebSocketClient::disconnectFromServer() {
    m_webSocket.close();
}

void WebSocketClient::onConnected() {
    qDebug() << "WebSocket connected";
    emit connected();
}

void WebSocketClient::onDisconnected() {
    qDebug() << "WebSocket disconnected";
    emit disconnected();
}

void WebSocketClient::onTextMessageReceived(const QString& message) {
    emit messageReceived(message);
}

void WebSocketClient::onErrorOccurred(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    qWarning() << "WebSocket error:" << m_webSocket.errorString();
    emit errorOccurred(m_webSocket.errorString());
}

