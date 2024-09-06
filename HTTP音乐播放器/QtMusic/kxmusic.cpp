#include "kxmusic.h"

KxMusic::KxMusic(const QString& url)
{
	QUrl tmpurl = QUrl::fromLocalFile(url);
	if (!tmpurl.isValid())
	{
		qDebug() << "error url";
	}
	m_url = tmpurl;
}

KxMusic::KxMusic(QUrl url): m_url(url)
{
}

bool KxMusic::operator==(const KxMusic& other)
{
	if (m_url == other.m_url) return true;
	return false;
}

QUrl KxMusic::getUrl()
{
	return m_url;
}

void KxMusic::serUrl(const QString& url)
{

	if (!url.isEmpty())
	{
		QUrl tmpurl = QUrl::fromLocalFile(url);
		if (!tmpurl.isValid())
		{
			qDebug() << "error url";
		}
		m_url = tmpurl;
	}
	else
	{
		qDebug() << "error url empty";
	}
}

QString KxMusic::getInfo()
{
	QString info = "";
	if (!m_ifLocal)
	{
		info.append(m_musicName);
		qDebug() << m_duration;
		info.append(" [" + formatTime(m_duration * 1000) + "]");
	}
	else
	{
		QMediaPlayer tmpPlayer;
		tmpPlayer.setMedia(m_url);

		// 这里等待元数据解析结束，事件通知，否则程序会提前渲染看不到info
		while (!tmpPlayer.isMetaDataAvailable()) {
			QCoreApplication::processEvents();
		}

		// 作者可能有多个，所以这里author为QStringList
		QString author = tmpPlayer.metaData(QStringLiteral("Author")).toStringList().join(",");
		info.append(author);
		QString title = tmpPlayer.metaData(QStringLiteral("Title")).toString();
		QString albumTitle = tmpPlayer.metaData(QStringLiteral("AlbumTitle")).toString();
		info.append(" - " + title);
		info.append(" [" + formatTime(tmpPlayer.duration()) + "]");
	}


	return info;
}

QString KxMusic::formatTime(qint64 milliseconds)
{
	qint64 seconds;
	if (!m_ifLocal) {
		seconds = m_duration;
	}
	else seconds = milliseconds / 1000;
	qint64 minutes = seconds / 60;
	seconds %= 60;

	return QString("%1:%2")
		.arg(minutes, 2, 10, QChar('0')) // 分钟，至少两位，不足补0  
		.arg(seconds, 2, 10, QChar('0')); // 秒，至少两位，不足补0 
}


