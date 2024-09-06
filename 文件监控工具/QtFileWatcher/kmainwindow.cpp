#pragma execution_character_set("utf-8")
#include "kmainwindow.h"
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QSvgGenerator>
#include <QMessageBox>
#include <QProcess>
#include <QMenuBar>
#include <QLabel>

#include "kfilewatcherprivate.h"


KMainWindow::KMainWindow(QWidget *parent)
    : QWidget(parent)
    , m_pHLayout(Q_NULLPTR)
    , m_pVLayout(Q_NULLPTR)
    , m_menuBarTool(Q_NULLPTR)
    , m_functionBar(Q_NULLPTR)
    , m_stackWidget(Q_NULLPTR)
    , m_watcher(Q_NULLPTR)
    , m_path("C:/Users/15897/Desktop/test_for_watch")
    , m_filterSettingsDialog(Q_NULLPTR)

{

    ui.setupUi(this);
    initFilterSettings();
    initWindow();

    setWindowTitle("FileWatcher");
    setWindowIcon(QIcon(":/icons/kmainwidow.png"));

    //initWatcher();
    initTimer();
    initTrayIcon();
    initLoadingDialog();
    


    m_stackWidget->clear();
}

KMainWindow::~KMainWindow()
{
    clear();
}

//void::KMainWindow::clear()
//{
//    delete m_pVLayout;
//    delete m_pHLayout;
//    delete m_menuBarTool;
//    delete m_functionBar;
//    delete m_stackWidget;
//
//    delete m_watcher;
//
//    delete m_timer;
//    delete m_trayIcon;
//}


void KMainWindow::clear()
{

    if (m_watcher) {
        delete m_watcher;  // 销毁 m_watcher
        m_watcher = nullptr; // 防止悬挂指针
    }

    // 等待线程结束
    if (m_thread) {
        m_thread->quit();      // 请求线程退出
        m_thread->wait();      // 等待线程结束
        delete m_thread;       // 释放线程对象
        m_thread = nullptr;    // 防止悬挂指针
    }

    // 释放定时器
    if (m_timer) {
        m_timer->stop();
    }

    delete m_timer;
    m_timer = nullptr;

    // 释放其他资源
    delete m_pVLayout;
    m_pVLayout = nullptr;

    delete m_pHLayout;
    m_pHLayout = nullptr;

    delete m_menuBarTool;
    m_menuBarTool = nullptr;

    delete m_functionBar;
    m_functionBar = nullptr;

    delete m_stackWidget;
    m_stackWidget = nullptr;

    delete m_trayIcon;
    m_trayIcon = nullptr;

    // 如果还有其他需要释放的资源，也在这里处理
}


QList<QList<QString>> KMainWindow::getFileInfo()
{
    QVariant resultVariant;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // 目标对象
        "listFilesInfo", // 方法名
        Qt::DirectConnection, // 连接类型
        Q_RETURN_ARG(QVariant, resultVariant) // 返回值
    );

    if (success) {
        // 如果成功调用
        QList<QList<QString>> result = resultVariant.value<QList<QList<QString>>>();
        //qDebug() << "Result from listFilesInfo:" << result;

        return result;
    }
    else {
        // 如果调用失败
        qDebug() << "getFileInfo Failed to invoke method.";
        return QList<QList<QString>>();
    }
}

QList<QList<QString>> KMainWindow::getActionInfo()
{
    QVariant resultVariant;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // 目标对象
        "listActionInfo", // 方法名
        Qt::DirectConnection, // 连接类型
        Q_RETURN_ARG(QVariant, resultVariant) // 返回值
    );

    if (success) {
        // 如果成功调用
        QList<QList<QString>> result = resultVariant.value<QList<QList<QString>>>();
        //qDebug() << "Result from listActionInfo:" << result;
        return result;
    }
    else {
        // 如果调用失败
        qDebug() << "getActionInfo Failed to invoke method.";
        return QList<QList<QString>>();
    }
}

// "D:/XboxGames/Minecraft- Java Edition/Content/StoreLogo.png"
// "D:/XboxGames/Minecraft Launcher/Content/layout_c7d69349-36c0-45a2-b72b-8de96007332e.xml"
// Successfully invoked setPath method with path: "D:/XboxGames"
// Successfully invoked setPath method with path: "D:/"

void KMainWindow::sendPathToWatcher(const QString path)
{


    bool success = QMetaObject::invokeMethod(
        m_watcher, // 目标对象
        "setPath", // 方法名
        Qt::DirectConnection, // 连接类型
        Q_ARG(QString, path) // 参数
    );

    if (success) {
        qDebug() << "Successfully invoked setPath method with path:" << path;
        m_stackWidget->setPath(path);
    }
    else {
        qDebug() << "Failed to invoke setPath method.";
    }
}

void KMainWindow::setFileInfoToStackWidget()
{
    if (m_stackWidget != Q_NULLPTR)
    {
        showLoadingDialog();
        QList<QList<QString>> result = getFileInfo();
        qDebug() << "KMainWindow::setFileInfoToStackWidget";
        //for (auto a : result)
        //{
        //    for (auto b : a)
        //    {
        //        qDebug() << b;
        //    }
        //}
        m_stackWidget->setShowData(result);
        hideLoadingDialog();

    }
    else
    {
        qDebug() << "setFileInfoToStackWidget m_stackWidget is NULL";
    }
}

void KMainWindow::setActionInfoToStackWidget()
{
    showLoadingDialog();
    if (m_stackWidget != Q_NULLPTR)
    {
        QList<QList<QString>> result = getActionInfo();
        qDebug() << "KMainWindow::setActionInfoToStackWidget";
              for (auto a : result)
              {
                  for (auto b : a)
                  {
                      qDebug() << b;
                  }
              }
        m_stackWidget->setActionData(result);
        // 如果窗口不可见，且有消息，开始闪烁
        if (!isVisible() && !result.isEmpty()) {
            startTrayIconBlinking();
        }
    }
    else
    {
        qDebug() << "setFileInfoToStackWidget m_stackWidget is NULL";
    }
    hideLoadingDialog();
}



void KMainWindow::updataActionFuction()
{

    qDebug() << "KMainWindow success signal";

    setActionInfoToStackWidget();
    

}

void KMainWindow::updataFileInfoTunction()
{
    //setFileInfoToStackWidget();
    m_stackWidget->updateEasyTableModel();
}

void KMainWindow::stopWatcher()
{
    logUserAction("暂停监听");

    if (m_ifWatch == false) return;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // 目标对象
        "stopWatcher", // 方法名
        Qt::DirectConnection // 连接类型
    );

    if (success) {
        qDebug() << "Successfully stopWatcher";
        m_ifWatch = false;
        QAction* pauseAction = m_functionBar->actions().at(1); // 获取“暂停监听”动作
        pauseAction->setIcon(QIcon(":/icons/pauseyes.svg"));

        QAction* startAction = m_functionBar->actions().at(2); // 获取“开始监听”动作
        startAction->setIcon(QIcon(":/icons/startno.svg"));
    }
    else {
        qDebug() << "Failed stopWatcher";
    }
}

void KMainWindow::beginWatcher()
{
    logUserAction("恢复监听");

    if (m_ifWatch == true) return;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // 目标对象
        "beginWatcher", // 方法名
        Qt::DirectConnection // 连接类型
    );

    if (success) {
        qDebug() << "Successfully beginWatcher";
        m_ifWatch = true;
        QAction* pauseAction = m_functionBar->actions().at(1); // 获取“暂停监听”动作
        pauseAction->setIcon(QIcon(":/icons/pauseno.svg"));

        QAction* startAction = m_functionBar->actions().at(2); // 获取“开始监听”动作
        startAction->setIcon(QIcon(":/icons/startyes.svg"));
    }
    else {
        qDebug() << "Failed beginWatcher";
    }
}

void KMainWindow::backPrePath()
{
    logUserAction("回退界面");
    m_stackWidget->backPrePath();
}



//void::KMainWindow::initWindow()
//{
//    // 创建主布局  
//    m_pVLayout = new QVBoxLayout(this);
//    m_pVLayout->setSpacing(0);
//    m_pVLayout->setMargin(0);
//
//    // 横向菜单栏（使用QToolBar模拟）  
//    m_menuBarTool = new QToolBar(this);
//    QAction* newNewAction = m_menuBarTool->addAction("新建监听");
//    QObject::connect(newNewAction, &QAction::triggered, this, &KMainWindow::openNewProgram);
//
//    QAction* action1 = m_menuBarTool->addAction("更改监听");
//    QObject::connect(action1, &QAction::triggered, this, &KMainWindow::onNewListener);
//
//    QAction* action5 = m_menuBarTool->addAction("过滤设置");
//    QObject::connect(action5, &QAction::triggered, m_filterSettingsDialog, &QDialog::exec);
//
//    // 添加“监控文件”选项
//    QAction* monitorFilesAction = m_menuBarTool->addAction("监控文件");
//    QObject::connect(monitorFilesAction, &QAction::triggered, this, &KMainWindow::onMonitorFiles);
//
//    // 添加“监控日志”选项
//    QAction* monitorLogsAction = m_menuBarTool->addAction("监控日志");
//    QObject::connect(monitorLogsAction, &QAction::triggered, this, &KMainWindow::onMonitorLogs);
//
//    QAction* logInfoAction = m_menuBarTool->addAction("日志信息");
//    QObject::connect(logInfoAction, &QAction::triggered, this, &KMainWindow::onLogInfo);
//
//    QAction* userLogAction = m_menuBarTool->addAction("用户日志");
//    QObject::connect(userLogAction, &QAction::triggered, this, &KMainWindow::initUserLog);
//
//    m_functionBar = new QToolBar(this);
//    // QIcon(":/icons/print.png"), 
//    QAction* action3 = m_functionBar->addAction("暂停监听");
//    QObject::connect(action3, &QAction::triggered, this, &KMainWindow::stopWatcher);
//    QAction* action4 = m_functionBar->addAction("开始监听");
//    QObject::connect(action4, &QAction::triggered, this, &KMainWindow::beginWatcher);
//    
//    QAction* action2 = m_functionBar->addAction("回退");
//    QObject::connect(action2, &QAction::triggered, this, &KMainWindow::backPrePath);
//
//
//    m_pVLayout->addWidget(m_menuBarTool);
//    m_pVLayout->addWidget(m_functionBar);
//
//    // 添加一个分隔符或额外的间距（可选）  
//    m_pVLayout->addSpacing(10);
//
//
//    m_stackWidget = new KStackWidget(this);
//    m_pVLayout->addWidget(m_stackWidget);
//
//    m_stackWidget->setPath(m_path);
//
//}

void KMainWindow::initWindow()
{
    // 创建主布局
    m_pVLayout = new QVBoxLayout(this);
    m_pVLayout->setSpacing(0);
    m_pVLayout->setMargin(0);

    // 创建菜单栏
    QMenuBar* menuBar = new QMenuBar(this);

    // 创建"文件"菜单
    QMenu* fileMenu = menuBar->addMenu("文件");
    QAction* newListenerAction = fileMenu->addAction("新建监听");
    QObject::connect(newListenerAction, &QAction::triggered, this, &KMainWindow::openNewProgram);

    QAction* changeListenerAction = fileMenu->addAction("更改监听");
    //changeListenerAction->setWhatsThis("请不要监听C盘，不间断的高速文件改动会导致主线程持续处理逻辑，界面无法及时渲染");
    QObject::connect(changeListenerAction, &QAction::triggered, this, &KMainWindow::onNewListener);


    // 创建"编辑"菜单
    QMenu* editMenu = menuBar->addMenu("编辑");
    QAction* filterSettingsAction = editMenu->addAction("过滤设置");
    QObject::connect(filterSettingsAction, &QAction::triggered, m_filterSettingsDialog, &QDialog::exec);

    // 创建"查看"菜单
    QMenu* viewMenu = menuBar->addMenu("查看");
    QAction* monitorFilesAction = viewMenu->addAction("监控文件");
    QObject::connect(monitorFilesAction, &QAction::triggered, this, &KMainWindow::onMonitorFiles);

    QAction* monitorLogsAction = viewMenu->addAction("监控日志");
    QObject::connect(monitorLogsAction, &QAction::triggered, this, &KMainWindow::onMonitorLogs);

    QAction* logInfoAction = viewMenu->addAction("日志信息");
    QObject::connect(logInfoAction, &QAction::triggered, this, &KMainWindow::onLogInfo);

    QAction* userLogAction = viewMenu->addAction("用户日志");
    QObject::connect(userLogAction, &QAction::triggered, this, &KMainWindow::initUserLog);

    // 创建功能栏
    m_functionBar = new QToolBar(this);

    QAction* backPrePathAction = m_functionBar->addAction(QIcon(":/icons/back.svg"), "回退");
    QObject::connect(backPrePathAction, &QAction::triggered, this, &KMainWindow::backPrePath);

    QAction* pauseListenerAction = m_functionBar->addAction(QIcon(":/icons/pauseno.svg"), "暂停监听");
    QObject::connect(pauseListenerAction, &QAction::triggered, this, &KMainWindow::stopWatcher);

    QAction* startListenerAction = m_functionBar->addAction(QIcon(":/icons/startyes.svg"), "开始监听");
    QObject::connect(startListenerAction, &QAction::triggered, this, &KMainWindow::beginWatcher);

    // 将菜单栏和功能栏添加到布局
    m_pVLayout->setMenuBar(menuBar);
    m_pVLayout->addWidget(m_functionBar);

    // 添加一个分隔符或额外的间距（可选）
    m_pVLayout->addSpacing(10);

    // 创建并添加堆栈小部件
    m_stackWidget = new KStackWidget(this);
    m_pVLayout->addWidget(m_stackWidget);

    //m_stackWidget->setPath(m_path);
}

void KMainWindow::initWatcher(QString dir)
{
    
    m_thread = new QThread();

    // Create an instance of FileWatcher and move it to the thread.
    m_watcher = new KFileWatcherEngine(dir);
    m_watcher->moveToThread(m_thread);

    // Connect signals and slots for starting and stopping the thread.
    (void)connect(m_thread, &QThread::started, m_watcher, &KFileWatcherEngine::handleFileChange);
    (void)connect(m_watcher, &KFileWatcherEngine::destroyed, m_thread, &QThread::quit);
    (void)connect(m_watcher, &KFileWatcherEngine::updataAction, this, &KMainWindow::onFileChangeSignal);

    // Start the thread.
    m_thread->start();


    //Sleep(3000);
    sendPathToWatcher(dir);
    setFileInfoToStackWidget();

}
void KMainWindow::initTimer()
{
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &KMainWindow::updataActionFuction);
}

void KMainWindow::onFileChangeSignal()
{
    qDebug() << "++" << i++;
    qDebug() << " KMainWindow::onFileChangeSignal()";
    if (m_timer) {
        m_timer->start(500); // 延时500毫秒触发
    }
}



void KMainWindow::onNewListener() {
    logUserAction("更改监听目录");

    // 显示初始提示信息
    QMessageBox::information(this, tr("注意"), tr("请不要监听C盘,持续不间断高速文件改动且很大的目录，会导致主线程持续处理逻辑，界面无法及时渲染。"));

    QString dir = QFileDialog::getExistingDirectory(this, tr("选择监听目录"), QDir::currentPath());

    if (dir.isEmpty()) {
        qDebug() << "操作已取消或对话框已关闭，退出函数";
        return;  // 用户点击取消或关闭对话框，不执行后续操作
    }


    // 检查是否选择了C盘
    if (dir == "C:\\" || dir == "C:" || dir == "C:/") {
        QMessageBox::warning(this, tr("无效操作"), tr("选择的目录是C盘，本次操作无效。"));
        return; // 跳过本次操作
    }

    qDebug() << "选中的目录:" << dir;

    if (m_watcher == Q_NULLPTR)
    {
        initWatcher(dir);   
    }
    else
    {
        sendPathToWatcher(dir);
        m_stackWidget->clear();
        setFileInfoToStackWidget();

    }

    m_ifWatch = true;
    QAction* pauseAction = m_functionBar->actions().at(1); // 获取“暂停监听”动作
    pauseAction->setIcon(QIcon(":/icons/pauseno.svg"));

    QAction* startAction = m_functionBar->actions().at(2); // 获取“开始监听”动作
    startAction->setIcon(QIcon(":/icons/startyes.svg"));

}



void KMainWindow::initFilterSettings() 
{
    m_filterSettingsDialog = new KFilterSettingsDialog(this);
    //(void)connect(m_filterSettingsDialog, &KFilterSettingsDialog::filterTypeAdded, this, &KMainWindow::onFilterTypeAdded);
    (void)connect(m_filterSettingsDialog, &KFilterSettingsDialog::filterTypeAdded,this, &KMainWindow::onFilterTypeAdded);
    (void)connect(m_filterSettingsDialog, &KFilterSettingsDialog::filterTypeRemoved,this, &KMainWindow::removeFilterType);
    (void)connect(m_filterSettingsDialog, &KFilterSettingsDialog::saveConfiguration, this, &KMainWindow::saveFilterConfiguration);
}



//void KMainWindow::initUserLog()
//{
//    logUserAction("打开用户操作日志");
//    QDialog logDialog(this);
//    logDialog.setWindowTitle("用户操作日志");
//
//    QVBoxLayout* layout = new QVBoxLayout(&logDialog);
//
//    QListWidget* logListWidget = new QListWidget(&logDialog);
//    logListWidget->addItems(m_userLogs);
//    layout->addWidget(logListWidget);
//
//    QPushButton* saveButton = new QPushButton("保存日志", &logDialog);
//    layout->addWidget(saveButton);
//
//    connect(saveButton, &QPushButton::clicked, [&]() {
//        QString filePath = QFileDialog::getSaveFileName(this, "Save Logs", "", "Text Files (*.txt);;All Files (*)");
//        if (!filePath.isEmpty()) {
//            QFile file(filePath);
//            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//                QTextStream out(&file);
//                for (const QString& log : m_userLogs) {
//                    out << log << "\n";
//                }
//                file.close();
//            }
//        }
//        });
//
//    logDialog.exec();
//}

void KMainWindow::initUserLog()
{
    logUserAction("打开用户操作日志");

    // 创建并设置对话框
    QDialog logDialog(this);
    logDialog.setWindowTitle("用户操作日志");
    logDialog.setFixedSize(400, 300); // 设置固定大小

    // 设置背景颜色为浅灰色
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(240, 240, 240)); // Light gray
    logDialog.setPalette(palette);
    logDialog.setAutoFillBackground(true);

    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(&logDialog);

    // 创建并配置 QListWidget
    QListWidget* logListWidget = new QListWidget(&logDialog);
    logListWidget->addItems(m_userLogs);
    layout->addWidget(logListWidget);

    // 创建并配置按钮
    QPushButton* saveButton = new QPushButton("保存日志", &logDialog);

    // 按钮样式
    QString buttonStyle = "QPushButton {"
        "    background-color: #e0e0e0;" // Light gray background
        "    border: 1px solid #b0b0b0;" // Gray border
        "    border-radius: 4px;"
        "    padding: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #d0d0d0;" // Slightly darker gray on hover
        "}";
    saveButton->setStyleSheet(buttonStyle);
    saveButton->setFixedWidth(100); // 设置按钮宽度

    // 添加按钮到布局
    layout->addWidget(saveButton);

    // 连接按钮的信号和槽
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString filePath = QFileDialog::getSaveFileName(&logDialog, "保存日志", "", "Text Files (*.txt);;All Files (*)");
        if (!filePath.isEmpty()) {
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                for (const QString& log : m_userLogs) {
                    out << log << "\n";
                }
                file.close();
            }
        }
        });

    // 执行对话框
    logDialog.exec();
}


void KMainWindow::onFilterTypeAdded(const QString& type) 
{
    logUserAction("添加忽略文件后缀");
    qDebug() << "KMainWindow::onFilterTypeAdded";
    if (m_stackWidget) 
    {
        m_stackWidget->m_fileFilter.addFilterByType(type);
        
    }
}

void KMainWindow::removeFilterType(const QString& type)
{
    logUserAction("删除忽略文件后缀");
    qDebug() << "KMainWindow::removeFilterType";
    if (m_stackWidget)
    {
        m_stackWidget->m_fileFilter.removeFilterByType(type);
    }
}

void KMainWindow::saveFilterConfiguration()
{
    logUserAction("保存配置文件");
    if (m_stackWidget)
    {
        m_stackWidget->m_fileFilter.saveToFile("filter_config.json");
    }
}

void KMainWindow::onMonitorFiles()
{
    logUserAction("切换监控文件界面");
    m_stackWidget->setCurrentIndex(0);
}

void KMainWindow::onMonitorLogs()
{
    logUserAction("切换监控日志界面");
    m_stackWidget->setCurrentIndex(1);
}   

void KMainWindow::onLogInfo()
{
    logUserAction("切换日志信息界面");
    m_stackWidget->setCurrentIndex(2);
}

void KMainWindow::openNewProgram()
{
    logUserAction("新建监听");
    // 获取当前程序的目录路径
    QString programDir = QCoreApplication::applicationDirPath();

    // 构造当前程序的路径
    QString programPath = QDir(programDir).filePath(QCoreApplication::applicationName());

    QStringList arguments; // 可以设置程序的参数，如果有的话

    // 启动当前程序的新实例
    bool success = QProcess::startDetached(programPath, arguments);
    if (!success) {
        qDebug() << "Failed to start the program!";
    }
}

void KMainWindow::initTrayIcon()
{
    // 创建托盘图标
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/kmainwidow.png"));
    m_trayIcon->setToolTip("FileWatcher");

    // 创建托盘菜单
    m_trayIconMenu = new QMenu(this);
    QAction* restoreAction = new QAction("显示窗口", this);
    connect(restoreAction, &QAction::triggered, this, &KMainWindow::restoreWindow);
    QAction* quitAction = new QAction("退出程序", this);
    connect(quitAction, &QAction::triggered, this, &KMainWindow::onExitTriggered);

    m_trayIconMenu->addAction(restoreAction);
    m_trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayIconMenu);

    // 连接托盘图标点击信号
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &KMainWindow::iconActivated);

    // 显示托盘图标
    m_trayIcon->show();
}

void KMainWindow::restoreWindow()
{
    this->showNormal(); // 恢复窗口
    stopTrayIconBlinking(); // 停止闪烁
}

void KMainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        this->showNormal(); // 单击托盘图标时还原窗口
        stopTrayIconBlinking(); // 停止闪烁
        break;
    default:
        break;
    }
}

void KMainWindow::startTrayIconBlinking()
{
    if (!m_blinkTimer) {
        m_blinkTimer = new QTimer(this);
        m_blinkTimer->setInterval(500); // 每500毫秒切换图标一次
        connect(m_blinkTimer, &QTimer::timeout, this, &KMainWindow::blinkTrayIcon);
    }
    m_blinkTimer->start();
}

void KMainWindow::stopTrayIconBlinking()
{
    if (m_blinkTimer) {
        m_blinkTimer->stop();
    }
    // 恢复正常图标
    m_trayIcon->setIcon(QIcon(":/icons/kmainwidow.png"));
    m_isBlinking = false;
}

void KMainWindow::blinkTrayIcon()
{
    if (m_isBlinking) {
        m_trayIcon->setIcon(QIcon(":/icons/kmainwidow.png"));
    }
    else {
        m_trayIcon->setIcon(QIcon(":/icons/trayIconBlink.svg"));
    }
    m_isBlinking = !m_isBlinking;
}




void KMainWindow::closeEvent(QCloseEvent* event)
{
    if (m_trayIcon->isVisible()) {
        hide(); // 隐藏窗口
        event->ignore(); // 忽略关闭事件，防止窗口被销毁
        //startTrayIconBlinking(); // 开始闪烁
        qDebug() << " KMainWindow::closeEvent";
    }
    else {
        event->accept();
    }
}


void KMainWindow::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            //startTrayIconBlinking(); // 开始闪烁
        }
        else if (isVisible()) {
            stopTrayIconBlinking(); // 停止闪烁
        }
    }
}

void KMainWindow::onExitTriggered() {
    logUserAction("退出程序");
    qApp->quit();  // 退出应用程序
}

void KMainWindow::logUserAction(const QString& action) {
    QString logEntry = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + " - " + action;
    m_userLogs.append(logEntry);
}

//void KMainWindow::initLoadingDialog()
//{
//    m_loadingDialog = new QDialog(this);
//    m_loadingDialog->setWindowTitle("加载中...");
//    m_loadingDialog->setModal(true); // 确保提示框阻止用户与其他窗口交互
//    QVBoxLayout* layout = new QVBoxLayout(m_loadingDialog);
//    QLabel* label = new QLabel("正在加载，请稍候...", m_loadingDialog);
//    layout->addWidget(label);
//    m_loadingDialog->setLayout(layout);
//}

void KMainWindow::initLoadingDialog()
{
    // 创建对话框并设置标题和模态属性
    m_loadingDialog = new QDialog(this);
    m_loadingDialog->setWindowTitle("正在加载，请稍候...");
    m_loadingDialog->setFixedSize(300, 15); // 设置固定大小
    m_loadingDialog->setModal(false);

    // 设置背景颜色为浅灰色
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(240, 240, 240)); // Light gray
    m_loadingDialog->setPalette(palette);
    m_loadingDialog->setAutoFillBackground(true);

    //// 初始化布局
    //m_loadLayout = new QVBoxLayout(m_loadingDialog);

    //// 创建并配置标签
    //m_loadLabel = new QLabel("正在加载，请稍候...", m_loadingDialog);
    //m_loadLabel->setAlignment(Qt::AlignCenter); // 居中对齐文本

    //// 设置字体样式
    //QFont font("Arial", 12); // 使用你喜欢的字体和大小
    //m_loadLabel->setFont(font);

    //// 添加标签到布局
    //m_loadLayout->addWidget(m_loadLabel);

    //// 确保布局设置好
    //m_loadingDialog->setLayout(m_loadLayout);

    // 禁用关闭按钮
    //m_loadingDialog->setWindowFlags(m_loadingDialog->windowFlags() & ~Qt::WindowCloseButtonHint);
}


// 显示加载提示框
void KMainWindow::showLoadingDialog()
{
    if (m_loadingDialog) {
        m_loadingDialog->show();
    }
}

// 隐藏加载提示框
void KMainWindow::hideLoadingDialog()
{
    if (m_loadingDialog) {
        m_loadingDialog->hide();
    }
}
