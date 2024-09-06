#include "kxlyrics.h"

bool KxLyrics::readLyricsFile(QString lyricsPath)
{
    m_listLyricsMap.clear();
    QFile file(lyricsPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_listLyricsMap.clear();
        qDebug() << "file open error";
        return false;
    }
    QTextStream in(&file);
    in.setCodec("utf-8");

    // һ���Զ�ȡ�����ļ�
    QString allContent = in.readAll();

    // ʹ�� \r\n �ָ��ַ���
    QStringList lines = allContent.split("\\r\\n", Qt::SkipEmptyParts);

    foreach(const QString & line, lines)
    {
        qDebug() << line;
        analysisLyricsFile(line);
    }

    return true;
}

bool KxLyrics::analysisLyricsFile(QString line)
{
    if (line == NULL || line.isEmpty())
    {
        return false;
    }
    QRegExp timeRegExp("\\[(\\d+):(\\d+\\.\\d+)\\]");

    if (timeRegExp.indexIn(line) != -1)
    {
        qint64 totalTime = timeRegExp.cap(1).toInt() * 60000 + // ����
            timeRegExp.cap(2).toFloat() * 1000; // ��

        QString lyricText = line.mid(timeRegExp.matchedLength());
        m_listLyricsMap.insert(totalTime, lyricText);
    }
    return true;
}

QMap<qint64, QString>& KxLyrics::getListLyricsMap()
{
    return m_listLyricsMap;
}

