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
	void sendGetRequest(const QUrl& url);// ���� get ����
	void sendGetImageRequest(const QUrl& url);// ���� get ����
	void sendPostRequest(const QUrl& url, const QByteArray& data); // ���� post ����

signals:
	void requestFinished(QNetworkReply* reply);

private:
	QNetworkAccessManager m_manager;
};

#endif