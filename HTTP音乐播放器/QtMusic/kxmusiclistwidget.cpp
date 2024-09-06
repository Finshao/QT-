#include "kxmusiclistwidget.h"


KxMusicListWidget::KxMusicListWidget(QWidget* parent)
    :QListWidget(parent)
{
}
void KxMusicListWidget::refresh()
{
    clear();
    m_musicList.showThisMusiclist(this);
}

void KxMusicListWidget::setMusicList(const KxMusicList& music)
{
    m_musicList = music;
    refresh();
}

void KxMusicListWidget::setMusicList_playing(const KxMusicList& music)
{

}

void KxMusicListWidget::removeMusic()
{
}

void KxMusicListWidget::detail()
{
}

