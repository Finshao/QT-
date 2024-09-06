#include "kxmusiclist.h"
#include "kxmusiclistwidget.h"
#include "mainwindow.h"
#include <QString>
#include <QProgressDialog>
#include <QMimeDatabase>


KxMusicList::KxMusicList(QString& name) : m_name(name)
{
}

KxMusicList& KxMusicList::operator=(const KxMusicList& other)
{
    if (this != &other) {
        m_name = other.m_name;
        m_musics = other.m_musics;
    }
    return *this;
}

void KxMusicList::addMusic(KxMusic music)
{
    for (auto a : m_musics)
    {
        if (a == music) return;
    }
	m_musics.push_back(music);
}

void KxMusicList::addMusic(const QList<QUrl>& urls)
{
    QProgressDialog proDialog(u8"添加进度", u8"取消", 0, urls.size());
    proDialog.setMinimumSize(350, 150);
    proDialog.setWindowModality(Qt::WindowModal);
    proDialog.setWindowTitle("添加中...请稍后");
    proDialog.show();
    int x = 0;
    foreach(QUrl i, urls) {
        x++;
        proDialog.setValue(x);
        //过滤Url的类型
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(i.toLocalFile());
        if (mime.name() != "audio/mpeg" && mime.name() != "audio/flac") {
            continue;
        }
        //剩下的符合类型
        m_musics.push_back(KxMusic(i));
        //if (sql_flag) {
        //    music[music.size() - 1].insertSQL(name);
        //}
        if (proDialog.wasCanceled()) break;
    }
}


KxMusic KxMusicList::getMusicpostion(int postion)
{
	return m_musics[postion];
}

void KxMusicList::addToPlayMusicList(QMediaPlaylist* playlist, QList<KxMusic>& current)
{
	for (auto a : m_musics)
	{
		playlist->addMedia(a.getUrl());
        current.push_back(a);
	}
}

void KxMusicList::showThisMusiclist(KxMusicListWidget* listWidget)
{
	for (auto a : m_musics)
	{
		QListWidgetItem* item = new QListWidgetItem;
		item->setIcon(listWidget->getIcon());
		QString str = a.getInfo();
		item->setText(str);
		listWidget->addItem(item);

	}
}

void KxMusicList::removeMusic(int postion)
{
}

void KxMusicList::showMusicUrl(int postion)
{
}

void KxMusicList::showMusicInfo(int postion)
{
}

void KxMusicList::clear()
{
}

void KxMusicList::setName(QString name)
{
	m_name = name;
}


