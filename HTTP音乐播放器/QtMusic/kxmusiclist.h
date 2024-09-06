#ifndef KX_MUSICLIST_H
#define KX_MUSICLIST_H
#include <QMediaPlaylist>
#include <QListWidget>
#include "kxmusic.h"
#include <QList>

class KxMusicListWidget;
class KxMusicList
{
public:
	KxMusicList() {};
	explicit KxMusicList(QString& name);
	~KxMusicList() {};
	KxMusicList& operator=(const KxMusicList& other);

	void addMusic(KxMusic music);
	void addMusic(const QList<QUrl>& urls);
	KxMusic getMusicpostion(int postion);
	void addToPlayMusicList(QMediaPlaylist* playlist, QList<KxMusic>& current);
	void showThisMusiclist(KxMusicListWidget* listWidget);
	void removeMusic(int postion);
	void showMusicUrl(int postion);
	void showMusicInfo(int postion);
	void clear();

	void setName(QString name);
private:
	QString m_name;
	QList<KxMusic> m_musics;
};
#endif // !KX_MUSICLIST_H
