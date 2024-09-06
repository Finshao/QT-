#include "musicmanager.h"


KxMusicManager::KxMusicManager()
{
	m_player = new QMediaPlayer();
	m_playList = new QMediaPlaylist();

	m_playList->setPlaybackMode(QMediaPlaylist::Loop);
	m_player->setPlaylist(m_playList);
}

KxMusicManager::~KxMusicManager()
{
	m_player->stop();
	delete m_player;
	delete m_playList;
}

QMediaPlayer* KxMusicManager::getPlayer()
{
	return m_player;
}

QMediaPlaylist* KxMusicManager::getPlayList()
{
	return m_playList;
}

void KxMusicManager::addMusic(const QString& filePath)
{
	QUrl url = QUrl::fromLocalFile(filePath);
	if (!url.isValid())
	{
		qDebug() << "error url";
		return;
	}
	m_playList->addMedia(url);
}

void KxMusicManager::playMusic()
{
	if (m_playList->mediaCount() > 0)
	{
		m_player->play();
	}
	else
	{
		qDebug() << "error playlist empty";
	}
}

void KxMusicManager::pauseMusic()
{
	m_player->pause();
}


