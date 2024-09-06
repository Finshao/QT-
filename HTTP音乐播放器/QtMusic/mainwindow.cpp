#pragma execution_character_set("utf-8")
#include <QLabel>
#include <QSlider>
#include <QDateTime>
#include <QImage>
#include <QFileDialog>
#include <QStandardPaths>
#include "kxqss.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "kxmusic.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initPlayManager();
    initButtonClicked();
    initButtonValueChanged();
    initMusicList();
    initListWidget();
    //initDown();


    //m_manager->addMusic("C:/Users/15897/Desktop/Music/123.mp3");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_manager;
}

void MainWindow::initPlayManager()
{
    m_manager = new KxMusicManager();
    // slider 进度条， position 位置， duration 总时长， metadata 元数据（歌曲信息），state 播放器状态
    connect(ui->m_positionSlider, &QAbstractSlider::valueChanged, this, &MainWindow::setPosition);
    connect(m_manager->getPlayer(), &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(m_manager->getPlayer(), &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);
    connect(m_manager->getPlayer(), &QMediaPlayer::metaDataAvailableChanged, this, &MainWindow::updateInfo);
    connect(m_manager->getPlayer(), &QMediaPlayer::stateChanged, this, &MainWindow::updatePlayBtn);

}

void MainWindow::initButtonClicked()
{
    (void)connect(ui->m_buttonPlayer, &QPushButton::clicked, this, &MainWindow::on_ButtonPlayer_Clicked);
    (void)connect(ui->m_addButton, &QPushButton::clicked, this, &MainWindow::on_btnAdd_clicked);
    (void)connect(ui->m_requestButton, &QPushButton::clicked, this, &MainWindow::on_ButtonRequset_Clicked);

    (void)connect(ui->m_currentMusicList, &QListWidget::itemDoubleClicked, this, &MainWindow::on_current_itemDoubleClicked);
    (void)connect(ui->m_localMusicList, &QListWidget::itemDoubleClicked, this, &MainWindow::on_local_itemdoubleClicked);
    (void)connect(ui->m_internetMusicList, &QListWidget::itemDoubleClicked, this, &MainWindow::on_internet_itemdoubleClicked);
    (void)connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_listWidget_itemClicked);
    (void)connect(ui->pushButtonNext, &QPushButton::clicked, this, &MainWindow::on_ButtonNext_Clicked);
    (void)connect(ui->pushButtonPre, &QPushButton::clicked, this, &MainWindow::on_ButtonPre_Clicked);
    (void)connect(ui->m_imageButton, &QPushButton::clicked, this, &MainWindow::on_MusicButton_Clicked);
}

void MainWindow::initButtonValueChanged()
{
    (void)connect(ui->m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::on_volumeSlider_valueChanged);
    QObject::connect(&m_response, &KxResponse::songInfoReady, this, &MainWindow::handleSongInfo);
    connect(m_manager->getPlayer(), &QMediaPlayer::positionChanged,this, &MainWindow::updateTextTime);
}

void MainWindow::initMusicList()
{
    ui->m_currentMusicList->getMusicList().setName("currentMusic");
    //KxMusic music("C:/Users/15897/Desktop/Music/123.mp3");
    //KxMusic music2("C:/Users/15897/Desktop/Music/372.mp3");
    //KxMusic music4("C:/Users/15897/Desktop/Music/music.mp3");
    //ui->m_currentMusicList->getMusicList().addMusic(music);
    //ui->m_currentMusicList->getMusicList().addMusic(music2);
    //ui->m_currentMusicList->getMusicList().addMusic(music4);
    refreshCurrentMusicList();
    //ui->m_currentMusicList->getMusicList().addToPlayMusicList(m_manager->getPlayList(), m_currentMusic);

    ui->m_localMusicList->getMusicList().setName("LocalMusic");


    ui->m_internetMusicList->getMusicList().setName("internetMusic");
  //  KxMusic music3("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/%E9%99%88%E5%A5%95%E8%BF%85/music.mp3");
    //ui->m_internetMusicList->getMusicList().addMusic(music3);
    //ui->m_internetMusicList->getMusicList().showThisMusiclist(ui->m_internetMusicList);
    

}

void MainWindow::initListWidget()
{

    ui->listWidget->addItem("当前播放");
    ui->listWidget->addItem("在线音乐");
    ui->listWidget->addItem("本地音乐");

    ui->fatherStackedWidget->setCurrentIndex(0);

    setBackgroundImage(ui->m_imageButton, ":/image/musicsong.png");
    ui->m_imageButton->setStyleSheet(m_imageButtonStyle());

    setBackgroundImage(ui->m_buttonPlayer, ":/image/buttonstop.png");
    setBackgroundImage(ui->pushButtonPre, ":/image/musicpre.png");
    setBackgroundImage(ui->pushButtonNext, ":/image/musicnext.png");

    ui->m_positionSlider->setStyleSheet(m_positionSliderStyle());

    setBackgroundImage(ui->m_addButton, ":/image/addfile.png");
    setBackgroundImage(ui->m_requestButton, ":/image/up.png");

    QString lurl = ":/image/lyric.txt";
    m_lyrics.readLyricsFile(lurl);
    m_listLyricsMap = m_lyrics.getListLyricsMap();
    displayLyrics(ui->m_textEdit);
    

    
    QVBoxLayout* layout = new QVBoxLayout(ui->m_lyricWidget);
    m_lyricsLayout = layout;
    ui->m_lyricWidget->setLayout(m_lyricsLayout);

    ui->m_volumeSlider->raise();





}

void MainWindow::initDown()
{
    qDebug() << "begin down........................";

    QUrl jsonUrl("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/music.json");
    QUrl mp3Url("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/E9%99%88%E5%A5%95%E8%BF%85/music.mp3");
    QUrl txtUrl("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/E9%99%88%E5%A5%95%E8%BF%85/img.jpg");

    //MusicHttp h("music.json");
    //h.onGet("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/music.json");
    //QByteArray data = h.onreadData();
    //qDebug() << "Resources stored at:" << fetcher.getStoragePath();
}



void MainWindow::setPosition(qint64 position)
{
    // 进图条改变大于90毫秒再执行
    if (qAbs(m_manager->getPlayer()->position() - position) > 90)
    {
        m_manager->getPlayer()->setPosition(position);
    }
}

void MainWindow::updatePosition(qint64 position)
{
    ui->m_positionSlider->setValue(static_cast<int>(position));
    ui->m_positionLabel->setText(formatTime(position) + "/" + formatTime(m_manager->getPlayer()->duration()));
    //updateTextTime(position);
    //if (m_manager->getPlayList()->currentIndex() >= 0)
    //{
    //    ui->m_lyricWidget->show(position);
    //}
}

void MainWindow::updateDuration(qint64 duration)
{
    ui->m_positionSlider->setRange(0, static_cast<int>(duration));
    ui->m_positionSlider->setEnabled(static_cast<int>(duration) > 0);
    if (!(static_cast<int>(duration) > 0)) {
        //无音乐播放时，界面元素
        //ui->m_infoLabel->setText("KEEP CALM AND CARRY ON ...");
        //mySystemTray->setToolTip(u8"LightMusicPlayer · By NJU-TJL");
        //QImage image(":/image/image/image/non-music.png");
        //ui->coverLabel->setPixmap(QPixmap::fromImage(image));
        //ui->musicTitleLabel->setText("");
        //ui->musicAlbumLabel->setText("");
        //ui->musicAuthorLabel->setText("");
        //ui->lyricWidget->clear();
    }
    ui->m_positionSlider->setPageStep(static_cast<int>(duration) / 10);
}

void MainWindow::updateInfo()
{
    if (m_manager->getPlayer()->isMetaDataAvailable()) {
        //返回可用MP3元数据列表（调试时可以查看）
        //QStringList listInfo_debug = m_manager->getPlayer()->availableMetaData();
        //歌曲信息
        QString info = "";

        // 作者可能有多个，所以这里author为QStringList
        QString author = m_manager->getPlayer()->metaData(QStringLiteral("Author")).toStringList().join(",");
        info.append(author);
        QString title = m_manager->getPlayer()->metaData(QStringLiteral("Title")).toString();
        QString albumTitle = m_manager->getPlayer()->metaData(QStringLiteral("AlbumTitle")).toString();
        info.append(" - " + title);
        info.append(" [" + formatTime(m_manager->getPlayer()->duration()) + "]");
        ui->m_infoLabel->setText(info);
        
        // 图片
        QImage picImage = m_manager->getPlayer()->metaData(QStringLiteral("ThumbnailImage")).value<QImage>();
        if (picImage.isNull())
        {
            picImage = QImage(":/image/peiqi.png");
        }
        //ui->m_mainLabel->setPixmap(QPixmap::fromImage(picImage));
        //ui->m_mainLabel->setScaledContents(true);

        QPixmap pixmap = QPixmap::fromImage(picImage).scaledToWidth(ui->m_imageButton->width(), Qt::SmoothTransformation);
        ui->m_imageButton->setIcon(QIcon(pixmap));
        ui->m_imageButton->setIconSize(pixmap.size());
        ui->m_imageButton->setFlat(true);

        ui->RoLabel->setImage(picImage);

        //ui->m_lyricWidget->process(ui->playListWidget->musicList.music[index].getLyricFile());
    }
    else if (1)
    {
        qDebug() << "???";
        int n = m_manager->getPlayList()->currentIndex();
        qDebug() << n;
        if (n == -1) return;
        QUrl url = m_currentMusic[n].getUrl();
        qDebug() << url;


        QString info = "";
        QString author = m_currentMusic[n].m_musicName;
        qDebug() << author;
        info.append(author);
        info.append(" [" + formatTime(m_currentMusic[n].m_duration * 1000) + "]");
        qDebug() << m_currentMusic[n].m_duration;
        ui->m_infoLabel->setText(info);


        setBackgroundImage(ui->m_imageButton, ":/image/img.jpg");
    }
    else 
    {
        qDebug() << "error updataInfo";
    }
}

void MainWindow::updatePlayBtn()
{
    if (m_manager->getPlayer()->state() == QMediaPlayer::PlayingState)
    {
        //ui->m_buttonPlayer->setStyleSheet(KxPlayerButtonPlay());
        setBackgroundImage(ui->m_buttonPlayer, ":/image/buttonstart.png");
        //action_systemTray_play->setIcon(QIcon(":/image/image/image/pause2.png"));
        //action_systemTray_play->setText(u8"暂停");
    }
    else {
        //ui->m_buttonPlayer->setStyleSheet(KxPlayerButtonStop());
        setBackgroundImage(ui->m_buttonPlayer, ":/image/buttonstop.png");
        //action_systemTray_play->setIcon(QIcon(":/image/image/image/play2.png"));
        //action_systemTray_play->setText(u8"播放");
    }
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    m_manager->getPlayer()->setVolume(value);
}

void MainWindow::on_ButtonPlayer_Clicked()
{
    if (m_manager->getPlayer()->state() == QMediaPlayer::PlayingState)
    {
        m_manager->pauseMusic();
        ui->RoLabel->setRotationEnabled(false);

    }
    else if (m_manager->getPlayer()->state() == QMediaPlayer::PausedState) {

        m_manager->playMusic();
        ui->RoLabel->setRotationEnabled(true);
    }
    else if (!m_manager->getPlayList()->isEmpty() && (m_manager->getPlayer()->state() == QMediaPlayer::StoppedState))
    {
        m_manager->getPlayList()->setCurrentIndex(0);
        m_manager->playMusic();
        ui->RoLabel->setRotationEnabled(true);
    }
}

void MainWindow::on_btnAdd_clicked()
{
    // 文件对话框，用来打开文件，可选择多个文件F
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    fileDialog.setWindowTitle(tr("添加本地音乐（注：自动过滤，按下\"Ctrl+A\"全选添加即可；不支持添加文件夹）"));

    // 文件类型过滤器
    QStringList list; list << "application/octet-stream";
    fileDialog.setMimeTypeFilters(list);

    // 初始目录，用户music
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));

    
    if (fileDialog.exec() == QDialog::Accepted) {
        QList<QUrl> urls = fileDialog.selectedUrls();
        ui->m_localMusicList->getMusicList().addMusic(urls);
        ui->m_localMusicList->refresh();
        ui->stackedWidget->setCurrentIndex(1);
    }

    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_ButtonRequset_Clicked()
{
    QObject::connect(&m_request, &KxHttpRequest::requestFinished, &m_response, &KxResponse::onHandleResponse);
    // 发送 get 请求
    QUrl url1("https://gitee.com/MarkYangUp/music/raw/master/%E5%AD%A4%E5%8B%87%E8%80%85/music.json/");
    QUrl url2("https://gitee.com/MarkYangUp/music/raw/master/%E9%A3%8E%E9%9B%A8%E6%97%A0%E9%98%BB/music.json");
    m_request.sendGetRequest(url1);
    m_request.sendGetRequest(url2);

    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::handleImageInfo(const QImage& img)
{
    int n = m_manager->getPlayList()->currentIndex();
    m_currentMusic[n].m_image = img;
}

void MainWindow::handleSongInfo(const QString& albumName, int duration, const QString& imgPath, const QString& lyricPath, const QString& mp3Path, const QString& musicName, const QString& path)
{

    QString baseUrl = "https://gitee.com/MarkYangUp/music/raw/master";
    QString tmp = "/";
    //qDebug() << encodePath;
    QByteArray exclude = "/";
    QString encodePath = QUrl::toPercentEncoding(path, exclude);
    QString url1(baseUrl + encodePath + tmp + mp3Path);
    QString url2(baseUrl + encodePath + tmp + imgPath);
    QString url3(baseUrl + encodePath + tmp + lyricPath);
    
    // 打印结果以验证  
    qDebug() << "URL1:" << url1;
    qDebug() << "URL2:" << url2;
    qDebug() << "URL3:" << url3;

    KxMusic music(url1);
    music.m_ifLocal = false;
    music.m_albumName = albumName;
    music.m_duration = duration;
    music.m_musicName = musicName;
    music.m_imgurl = url2;
    music.m_lyricurl = url3;
    ui->m_internetMusicList->getMusicList().addMusic(music);
    //ui->m_internetMusicList->getMusicList().addToPlayMusicList(m_manager->getPlayList(), m_currentMusic);
    ui->m_internetMusicList->clear();
    ui->m_internetMusicList->getMusicList().showThisMusiclist(ui->m_internetMusicList);

}

void MainWindow::on_current_itemDoubleClicked(QListWidgetItem *item)
{
    int i = ui->m_currentMusicList->row(item);
    m_manager->getPlayList()->setCurrentIndex(i);
    m_manager->getPlayer()->play();
    ui->RoLabel->setRotationEnabled(true);
}

void MainWindow::on_local_itemdoubleClicked(QListWidgetItem* item)
{

    ui->m_currentMusicList->clear();
    m_manager->getPlayList()->clear();
    m_currentMusic.clear();
    ui->m_localMusicList->getMusicList().addToPlayMusicList(m_manager->getPlayList(), m_currentMusic);
    refreshCurrentMusicList();

    int i = ui->m_localMusicList->row(item);
    qDebug() << i << "+++";
    if (!m_manager->getPlayList()->isEmpty()) {
        // 确保索引在播放列表的有效范围内
        if (i >= 0 && i < m_manager->getPlayList()->mediaCount()) {
            m_manager->getPlayList()->setCurrentIndex(i);
            m_manager->getPlayer()->play();
            ui->RoLabel->setRotationEnabled(true);
        }
        else {
            qDebug() << "Index out of range:" << i;
        }
    }
    else {
        qDebug() << "Playlist is empty";
    }

    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_internet_itemdoubleClicked(QListWidgetItem* item)
{
    ui->m_currentMusicList->clear();
    m_manager->getPlayList()->clear();
    m_currentMusic.clear();
    ui->m_internetMusicList->getMusicList().addToPlayMusicList(m_manager->getPlayList(), m_currentMusic);
    refreshCurrentMusicList();

    int i = ui->m_internetMusicList->row(item);
    m_manager->getPlayList()->setCurrentIndex(i);
    m_manager->getPlayer()->play();
    ui->RoLabel->setRotationEnabled(true);

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem* item)
{
    int index = ui->listWidget->row(item);
    ui->stackedWidget->setCurrentIndex(index);

}

void MainWindow::on_ButtonNext_Clicked()
{
    if (m_manager->getPlayList()->currentIndex() < m_manager->getPlayList()->mediaCount() - 1)
    {
        m_manager->getPlayList()->next();
    }
    else
    {
        m_manager->getPlayList()->setCurrentIndex(0);
    }
    
}

void MainWindow::on_ButtonPre_Clicked()
{
    if (m_manager->getPlayList()->currentIndex() > 0)
    {
        m_manager->getPlayList()->previous();
    }
    else
    {
        int n = m_manager->getPlayList()->mediaCount();
        m_manager->getPlayList()->setCurrentIndex(n - 1);
    }
}

void MainWindow::on_MusicButton_Clicked()
{
    if (ui->fatherStackedWidget->currentIndex() == 0)
    {
        ui->fatherStackedWidget->setCurrentIndex(1);
        
    }
    else
    {
        ui->fatherStackedWidget->setCurrentIndex(0);
    }
}

QString MainWindow::formatTime(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds %= 60;

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0')) // 分钟，至少两位，不足补0  
        .arg(seconds, 2, 10, QChar('0')); // 秒，至少两位，不足补0 
}

void MainWindow::refreshCurrentMusicList()
{
    for (auto a : m_currentMusic)
    {
        ui->m_currentMusicList->getMusicList().addMusic(a);
    }
    ui->m_currentMusicList->getMusicList().showThisMusiclist(ui->m_currentMusicList);
}

void MainWindow::setBackgroundImage(QPushButton* button, const QString& imagePath)
{
    QImage picImage = QImage(imagePath);
    QPixmap pixmap = QPixmap::fromImage(picImage).scaledToWidth(button->width(), Qt::SmoothTransformation);
    button->setIcon(QIcon(pixmap));
    button->setIconSize(pixmap.size());
    button->setFlat(true);
}

void MainWindow::displayLyrics(QTextEdit* lyricsTextEdit)
{
    lyricsTextEdit->clear();

    // 使用 QMapIterator 遍历映射
    QMapIterator<qint64, QString> it(m_listLyricsMap);

    while (it.hasNext()) {
        it.next();
        qint64 timestamp = it.key();
        QString lyric = it.value();
        //qDebug() << timestamp << "." << lyric;

        // 将时间戳转换为分钟和秒数 // 时间戳转换为毫秒，然后转换为 QTime

        // 格式化歌词
        QString formattedLyric = QString("%2").arg(lyric);
        //qDebug() << "Formatted Lyric:" << formattedLyric;
        // 将格式化的歌词添加到 QTextEdit 中
        lyricsTextEdit->append(formattedLyric);
    }
}

void MainWindow::updateTextTime(qint64 position)
{
    qDebug() << "updataTextTime";
    auto lrcMap = m_lyrics.getListLyricsMap();
    qDebug() << "lrcMap" << lrcMap.size();
    qint64 previousTime = 0;
    qint64 currentLyricTime = 0;
    QMapIterator<qint64, QString> i(lrcMap);
    while (i.hasNext())
    {
        i.next();
        if (position < i.key())
        {
            QString currentLyric = lrcMap.value(previousTime);
            currentLyricTime = previousTime;
            break;
        }
        previousTime = i.key();
    }

    QStringList displayLyrics; // 存储将要显示的歌词列表。
    // 获取将要显示的歌词
    QMap<qint64, QString>::iterator it = lrcMap.find(currentLyricTime);
    // 显示前三句，如果it不是开头，就向前移动迭代器
    for (int i = 0; i < 3 && it != lrcMap.begin(); i++)
    {
        --it;
        displayLyrics.prepend(it.value());
    }

    // 重置迭代器
    it = lrcMap.find(currentLyricTime);
    QString currntStr = QString();
    // 显示当前句
    if (it != lrcMap.end())
    {
        currntStr = QString("<font color='red'>" + it.value() + "</font>");
        displayLyrics.append(it.value());
    }

    // 显示后三句
    for (int i = 0; i < 3 && it != lrcMap.end(); i++)
    {
        ++it;
        if (it != lrcMap.end())
        {
            displayLyrics.append(it.value());
        }
    }
    for (auto a : displayLyrics)
    {
        qDebug() << a << "====";
    }
    //更新显示
    textChanged(displayLyrics);
}

void MainWindow::textChanged(const QStringList& lyrics)
{
    qDebug() << "textChanged";

    // 更新歌词标签
    int numLyrics = lyrics.size();
    for (int i = 0; i < 6; ++i) {
        QLabel* label = nullptr;
        switch (i) {
        case 0: label = ui->label_1; break;
        case 1: label = ui->label_2; break;
        case 2: label = ui->label_3; break;
        case 3: label = ui->label_4; break;
        case 4: label = ui->label_5; break;
        case 5: label = ui->label_6; break;
        default: break; // 不应该达到这里
        }

        if (label != nullptr) {
            // 设置歌词，如果索引超出范围则设置为空字符串
            QString lyric = (i < numLyrics) ? lyrics[i] : "";
            label->setText(lyric);
        }
    }

    qDebug() << "Lyrics size: " << numLyrics;
}