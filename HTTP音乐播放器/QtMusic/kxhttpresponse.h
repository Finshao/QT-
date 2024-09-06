#ifndef KX_HTTPRESPONSE_H
#define KX_HTTPRESPONSE_H

#include <QObject>
#include <QNetworkReply>
#include <QImage>


class QNetworkReply;

class KxResponse : public QObject
{
	Q_OBJECT

public:
	KxResponse(QObject* parent = nullptr);
	~KxResponse();
	void onHandleResponse(QNetworkReply* reply);// ���� http Ӧ������
	void onHandleImg(QNetworkReply* reply);
signals:
	void dataReady(QByteArray& data);
	void imgInfoReady(const QImage& img);
	void songInfoReady(const QString& albumName, int duration, const QString& imgPath, const QString& lyricPath, const QString& mp3Path, const QString& musicName, const QString& path);
};

#endif