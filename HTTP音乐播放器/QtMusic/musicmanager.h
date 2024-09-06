#ifndef KX_MUSICMANAGER_H
#define KX_MUSICMANAGER_H


#include <QMediaPlayer>  
#include <QMediaPlaylist>  
#include <QUrl> 
#include <QDebug>
#include "kxmusiclist.h"

class KxMusicManager
{
public:
	KxMusicManager();
	~KxMusicManager();
	QMediaPlayer* getPlayer();
	QMediaPlaylist* getPlayList();

	void addMusic(const QString &filePath);
	void playMusic();
	void pauseMusic();

private:
	QMediaPlayer* m_player;
	QMediaPlaylist* m_playList;
};

#endif // !KX_MUSICMANAGER_H