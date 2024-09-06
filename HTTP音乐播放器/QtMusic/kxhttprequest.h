#ifndef KX_HTTP_REQUEST_H
#define KX_HTTP_REQUEST_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>


class KxHttpRequest : public QObject
{
	Q_OBJECT

public:
	KxHttpRequest(QObject* parent = nullptr);
	~KxHttpRequest();
	void sendGetRequest(const QUrl& url);// 发送 get 请求
	void sendGetImageRequest(const QUrl& url);// 发送 get 请求
	void sendPostRequest(const QUrl& url, const QByteArray& data); // 发送 post 请求

signals:
	void requestFinished(QNetworkReply* reply);

private:
	QNetworkAccessManager m_manager;
};

#endif