#ifndef KX_MUSICLISTWIDGET_H
#define KX_MUSICLISTWIDGET_H

#include <QListWidget>
#include "kxmusiclist.h"

class KxMusicListWidget :public QListWidget
{
    Q_OBJECT


public:
    KxMusicListWidget(QWidget* parent = Q_NULLPTR);
    void refresh();
    void setMusicList(const KxMusicList& music);
    void setMusicList_playing(const KxMusicList& music);
    void removeMusic();
    void detail();
    QIcon getIcon() { return icon; }
    KxMusicList& getMusicList() { return m_musicList; }
private:
    KxMusicList m_musicList;
    QIcon icon = QIcon(":/image/musicsignal.png");
};


#endif // !KX_MUSICLISTWIDGET_H
