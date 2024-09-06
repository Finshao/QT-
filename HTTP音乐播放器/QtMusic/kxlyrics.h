#ifndef KX_LYRICS_H
#define KX_LYRIC_H


#include <QString>
#include <QStringList>
#include <QFile>
#include <QMap>
#include <QTextStream>
#include <QDebug>

class KxLyrics
{
public:
	bool readLyricsFile(QString lyricsPath);
	bool analysisLyricsFile(QString line);
	QMap<qint64, QString>& getListLyricsMap();
private:
	QMap<qint64, QString> m_listLyricsMap;
};

#endif // !KX_LYRICS_H


