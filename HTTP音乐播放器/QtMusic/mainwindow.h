#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlayList>
#include <QSlider>
#include <QString>
#include <QImage>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

#include "musicmanager.h"
#include "kxhttprequest.h"
#include "kxhttpresponse.h"
#include "kxresourcedownloader.h"
#include "kxlyrics.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
// 初始化函数
private:
    void initPlayManager();
    void initButtonClicked();
    void initButtonValueChanged();
    void initMusicList();
    void initListWidget();
    void initDown();

// 槽函数
private:
    void setPosition(qint64 position);
    void updatePosition(qint64 position);
    void updateDuration(qint64 position);
    void updateInfo();
    void updatePlayBtn();


    void on_volumeSlider_valueChanged(int value);
    void on_ButtonPlayer_Clicked();
    void on_btnAdd_clicked();
    void on_ButtonRequset_Clicked();    // 这里调用请求对象请求资源
    void handleImageInfo(const QImage& img);
    void handleSongInfo(const QString& albumName, int duration, const QString& imgPath, const QString& lyricPath, const QString& mp3Path, const QString& musicName, const QString& path);
    void on_current_itemDoubleClicked(QListWidgetItem* item);
    void on_local_itemdoubleClicked(QListWidgetItem* item);
    void on_internet_itemdoubleClicked(QListWidgetItem* item);
    void on_listWidget_itemClicked(QListWidgetItem* item);
    void on_ButtonNext_Clicked();
    void on_ButtonPre_Clicked();
    void on_MusicButton_Clicked();

//协助处理函数
private:
    // 将毫秒换算分钟QString
    QString formatTime(qint64 milliseconds);
    void refreshCurrentMusicList();
    void setBackgroundImage(QPushButton* button, const QString& imagePath);
    void displayLyrics(QTextEdit* lyricsTextEdit);
    void updateTextTime(qint64 position);
    void textChanged(const QStringList& lyrics);
    
private:
    Ui::MainWindow* ui;
    KxMusicManager* m_manager;
    KxHttpRequest m_request;
    KxResponse m_response;

    QList<KxMusic> m_currentMusic;

    KxLyrics m_lyrics;
    QMap<qint64, QString> m_listLyricsMap;
    QVBoxLayout* m_lyricsLayout;

};
#endif // MAINWINDOW_H
