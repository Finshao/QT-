#ifndef KX_MUSIC_H
#define KX_MUSIC_H

#include <QDebug>
#include <QUrl>
#include <QString>
#include <QMediaplayer>
#include <QCoreApplication>
#include <QImage>

class KxMusic
{
public:
	KxMusic(const QString& url);
	KxMusic(QUrl url);
	~KxMusic() {};
	bool operator == (const KxMusic& other);
	QUrl getUrl();
	void serUrl(const QString& url);
	QString getInfo();
	QString formatTime(qint64 milliseconds);
public:
	bool m_ifLocal = true;
	bool m_ifLove = false;
	QString m_albumName;
	int m_duration;
	QString m_musicName;
	QUrl m_imgurl;
	QUrl m_lyricurl;
	QImage m_image;
private:
	QUrl m_url;


};
#endif // !KX_MUSIC_H
