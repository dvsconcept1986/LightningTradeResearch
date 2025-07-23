#pragma once
#pragma once

#include <QObject>
#include <QWebSocket>

class WebSocketClient : public QObject {
    Q_OBJECT
public:
    explicit WebSocketClient(QObject* parent = nullptr);
    ~WebSocketClient();

    void connectToServer(const QUrl& url);
    void disconnectFromServer();
	QString errorString() const { return m_webSocket.errorString(); }
    void sendMessage(const QString& message);
    bool isConnected() const;

signals:
    void messageReceived(const QString& message);
    void connected();
    void disconnected();
    void errorOccurred(const QString& errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:
    QWebSocket m_webSocket;
};
