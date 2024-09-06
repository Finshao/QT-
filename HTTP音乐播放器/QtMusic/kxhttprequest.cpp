#include "kxhttprequest.h"

#include <QNetworkReply>

KxHttpRequest::KxHttpRequest(QObject* parent)
	: QObject(parent)
{
	(void)connect(&m_manager, &QNetworkAccessManager::finished, this, &KxHttpRequest::requestFinished);
}

KxHttpRequest::~KxHttpRequest()
{
}
// 发送 get 请求
void KxHttpRequest::sendGetRequest(const QUrl& url)
{
	QNetworkRequest request(url);
	request.setRawHeader("X-Data-Type", "application/json");
	m_manager.get(request);
}

void KxHttpRequest::sendGetImageRequest(const QUrl& url)
{
	QNetworkRequest request(url);
	request.setRawHeader("X-Data-Type", "image/jpeg");
	m_manager.get(request);
}

// 发送 post 请求
void KxHttpRequest::sendPostRequest(const QUrl& url, const QByteArray& data)
{
	QNetworkRequest request(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	m_manager.post(request, data);
}