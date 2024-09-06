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
        delete m_watcher;  // ���� m_watcher
        m_watcher = nullptr; // ��ֹ����ָ��
    }

    // �ȴ��߳̽���
    if (m_thread) {
        m_thread->quit();      // �����߳��˳�
        m_thread->wait();      // �ȴ��߳̽���
        delete m_thread;       // �ͷ��̶߳���
        m_thread = nullptr;    // ��ֹ����ָ��
    }

    // �ͷŶ�ʱ��
    if (m_timer) {
        m_timer->stop();
    }

    delete m_timer;
    m_timer = nullptr;

    // �ͷ�������Դ
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

    // �������������Ҫ�ͷŵ���Դ��Ҳ�����ﴦ��
}


QList<QList<QString>> KMainWindow::getFileInfo()
{
    QVariant resultVariant;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // Ŀ�����
        "listFilesInfo", // ������
        Qt::DirectConnection, // ��������
        Q_RETURN_ARG(QVariant, resultVariant) // ����ֵ
    );

    if (success) {
        // ����ɹ�����
        QList<QList<QString>> result = resultVariant.value<QList<QList<QString>>>();
        //qDebug() << "Result from listFilesInfo:" << result;

        return result;
    }
    else {
        // �������ʧ��
        qDebug() << "getFileInfo Failed to invoke method.";
        return QList<QList<QString>>();
    }
}

QList<QList<QString>> KMainWindow::getActionInfo()
{
    QVariant resultVariant;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // Ŀ�����
        "listActionInfo", // ������
        Qt::DirectConnection, // ��������
        Q_RETURN_ARG(QVariant, resultVariant) // ����ֵ
    );

    if (success) {
        // ����ɹ�����
        QList<QList<QString>> result = resultVariant.value<QList<QList<QString>>>();
        //qDebug() << "Result from listActionInfo:" << result;
        return result;
    }
    else {
        // �������ʧ��
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
        m_watcher, // Ŀ�����
        "setPath", // ������
        Qt::DirectConnection, // ��������
        Q_ARG(QString, path) // ����
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
        // ������ڲ��ɼ���������Ϣ����ʼ��˸
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
    logUserAction("��ͣ����");

    if (m_ifWatch == false) return;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // Ŀ�����
        "stopWatcher", // ������
        Qt::DirectConnection // ��������
    );

    if (success) {
        qDebug() << "Successfully stopWatcher";
        m_ifWatch = false;
        QAction* pauseAction = m_functionBar->actions().at(1); // ��ȡ����ͣ����������
        pauseAction->setIcon(QIcon(":/icons/pauseyes.svg"));

        QAction* startAction = m_functionBar->actions().at(2); // ��ȡ����ʼ����������
        startAction->setIcon(QIcon(":/icons/startno.svg"));
    }
    else {
        qDebug() << "Failed stopWatcher";
    }
}

void KMainWindow::beginWatcher()
{
    logUserAction("�ָ�����");

    if (m_ifWatch == true) return;
    bool success = QMetaObject::invokeMethod(
        m_watcher, // Ŀ�����
        "beginWatcher", // ������
        Qt::DirectConnection // ��������
    );

    if (success) {
        qDebug() << "Successfully beginWatcher";
        m_ifWatch = true;
        QAction* pauseAction = m_functionBar->actions().at(1); // ��ȡ����ͣ����������
        pauseAction->setIcon(QIcon(":/icons/pauseno.svg"));

        QAction* startAction = m_functionBar->actions().at(2); // ��ȡ����ʼ����������
        startAction->setIcon(QIcon(":/icons/startyes.svg"));
    }
    else {
        qDebug() << "Failed beginWatcher";
    }
}

void KMainWindow::backPrePath()
{
    logUserAction("���˽���");
    m_stackWidget->backPrePath();
}



//void::KMainWindow::initWindow()
//{
//    // ����������  
//    m_pVLayout = new QVBoxLayout(this);
//    m_pVLayout->setSpacing(0);
//    m_pVLayout->setMargin(0);
//
//    // ����˵�����ʹ��QToolBarģ�⣩  
//    m_menuBarTool = new QToolBar(this);
//    QAction* newNewAction = m_menuBarTool->addAction("�½�����");
//    QObject::connect(newNewAction, &QAction::triggered, this, &KMainWindow::openNewProgram);
//
//    QAction* action1 = m_menuBarTool->addAction("���ļ���");
//    QObject::connect(action1, &QAction::triggered, this, &KMainWindow::onNewListener);
//
//    QAction* action5 = m_menuBarTool->addAction("��������");
//    QObject::connect(action5, &QAction::triggered, m_filterSettingsDialog, &QDialog::exec);
//
//    // ��ӡ�����ļ���ѡ��
//    QAction* monitorFilesAction = m_menuBarTool->addAction("����ļ�");
//    QObject::connect(monitorFilesAction, &QAction::triggered, this, &KMainWindow::onMonitorFiles);
//
//    // ��ӡ������־��ѡ��
//    QAction* monitorLogsAction = m_menuBarTool->addAction("�����־");
//    QObject::connect(monitorLogsAction, &QAction::triggered, this, &KMainWindow::onMonitorLogs);
//
//    QAction* logInfoAction = m_menuBarTool->addAction("��־��Ϣ");
//    QObject::connect(logInfoAction, &QAction::triggered, this, &KMainWindow::onLogInfo);
//
//    QAction* userLogAction = m_menuBarTool->addAction("�û���־");
//    QObject::connect(userLogAction, &QAction::triggered, this, &KMainWindow::initUserLog);
//
//    m_functionBar = new QToolBar(this);
//    // QIcon(":/icons/print.png"), 
//    QAction* action3 = m_functionBar->addAction("��ͣ����");
//    QObject::connect(action3, &QAction::triggered, this, &KMainWindow::stopWatcher);
//    QAction* action4 = m_functionBar->addAction("��ʼ����");
//    QObject::connect(action4, &QAction::triggered, this, &KMainWindow::beginWatcher);
//    
//    QAction* action2 = m_functionBar->addAction("����");
//    QObject::connect(action2, &QAction::triggered, this, &KMainWindow::backPrePath);
//
//
//    m_pVLayout->addWidget(m_menuBarTool);
//    m_pVLayout->addWidget(m_functionBar);
//
//    // ���һ���ָ��������ļ�ࣨ��ѡ��  
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
    // ����������
    m_pVLayout = new QVBoxLayout(this);
    m_pVLayout->setSpacing(0);
    m_pVLayout->setMargin(0);

    // �����˵���
    QMenuBar* menuBar = new QMenuBar(this);

    // ����"�ļ�"�˵�
    QMenu* fileMenu = menuBar->addMenu("�ļ�");
    QAction* newListenerAction = fileMenu->addAction("�½�����");
    QObject::connect(newListenerAction, &QAction::triggered, this, &KMainWindow::openNewProgram);

    QAction* changeListenerAction = fileMenu->addAction("���ļ���");
    //changeListenerAction->setWhatsThis("�벻Ҫ����C�̣�����ϵĸ����ļ��Ķ��ᵼ�����̳߳��������߼��������޷���ʱ��Ⱦ");
    QObject::connect(changeListenerAction, &QAction::triggered, this, &KMainWindow::onNewListener);


    // ����"�༭"�˵�
    QMenu* editMenu = menuBar->addMenu("�༭");
    QAction* filterSettingsAction = editMenu->addAction("��������");
    QObject::connect(filterSettingsAction, &QAction::triggered, m_filterSettingsDialog, &QDialog::exec);

    // ����"�鿴"�˵�
    QMenu* viewMenu = menuBar->addMenu("�鿴");
    QAction* monitorFilesAction = viewMenu->addAction("����ļ�");
    QObject::connect(monitorFilesAction, &QAction::triggered, this, &KMainWindow::onMonitorFiles);

    QAction* monitorLogsAction = viewMenu->addAction("�����־");
    QObject::connect(monitorLogsAction, &QAction::triggered, this, &KMainWindow::onMonitorLogs);

    QAction* logInfoAction = viewMenu->addAction("��־��Ϣ");
    QObject::connect(logInfoAction, &QAction::triggered, this, &KMainWindow::onLogInfo);

    QAction* userLogAction = viewMenu->addAction("�û���־");
    QObject::connect(userLogAction, &QAction::triggered, this, &KMainWindow::initUserLog);

    // ����������
    m_functionBar = new QToolBar(this);

    QAction* backPrePathAction = m_functionBar->addAction(QIcon(":/icons/back.svg"), "����");
    QObject::connect(backPrePathAction, &QAction::triggered, this, &KMainWindow::backPrePath);

    QAction* pauseListenerAction = m_functionBar->addAction(QIcon(":/icons/pauseno.svg"), "��ͣ����");
    QObject::connect(pauseListenerAction, &QAction::triggered, this, &KMainWindow::stopWatcher);

    QAction* startListenerAction = m_functionBar->addAction(QIcon(":/icons/startyes.svg"), "��ʼ����");
    QObject::connect(startListenerAction, &QAction::triggered, this, &KMainWindow::beginWatcher);

    // ���˵����͹�������ӵ�����
    m_pVLayout->setMenuBar(menuBar);
    m_pVLayout->addWidget(m_functionBar);

    // ���һ���ָ��������ļ�ࣨ��ѡ��
    m_pVLayout->addSpacing(10);

    // ��������Ӷ�ջС����
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
        m_timer->start(500); // ��ʱ500���봥��
    }
}



void KMainWindow::onNewListener() {
    logUserAction("���ļ���Ŀ¼");

    // ��ʾ��ʼ��ʾ��Ϣ
    QMessageBox::information(this, tr("ע��"), tr("�벻Ҫ����C��,��������ϸ����ļ��Ķ��Һܴ��Ŀ¼���ᵼ�����̳߳��������߼��������޷���ʱ��Ⱦ��"));

    QString dir = QFileDialog::getExistingDirectory(this, tr("ѡ�����Ŀ¼"), QDir::currentPath());

    if (dir.isEmpty()) {
        qDebug() << "������ȡ����Ի����ѹرգ��˳�����";
        return;  // �û����ȡ����رնԻ��򣬲�ִ�к�������
    }


    // ����Ƿ�ѡ����C��
    if (dir == "C:\\" || dir == "C:" || dir == "C:/") {
        QMessageBox::warning(this, tr("��Ч����"), tr("ѡ���Ŀ¼��C�̣����β�����Ч��"));
        return; // �������β���
    }

    qDebug() << "ѡ�е�Ŀ¼:" << dir;

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
    QAction* pauseAction = m_functionBar->actions().at(1); // ��ȡ����ͣ����������
    pauseAction->setIcon(QIcon(":/icons/pauseno.svg"));

    QAction* startAction = m_functionBar->actions().at(2); // ��ȡ����ʼ����������
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
//    logUserAction("���û�������־");
//    QDialog logDialog(this);
//    logDialog.setWindowTitle("�û�������־");
//
//    QVBoxLayout* layout = new QVBoxLayout(&logDialog);
//
//    QListWidget* logListWidget = new QListWidget(&logDialog);
//    logListWidget->addItems(m_userLogs);
//    layout->addWidget(logListWidget);
//
//    QPushButton* saveButton = new QPushButton("������־", &logDialog);
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
    logUserAction("���û�������־");

    // ���������öԻ���
    QDialog logDialog(this);
    logDialog.setWindowTitle("�û�������־");
    logDialog.setFixedSize(400, 300); // ���ù̶���С

    // ���ñ�����ɫΪǳ��ɫ
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(240, 240, 240)); // Light gray
    logDialog.setPalette(palette);
    logDialog.setAutoFillBackground(true);

    // ���ò���
    QVBoxLayout* layout = new QVBoxLayout(&logDialog);

    // ���������� QListWidget
    QListWidget* logListWidget = new QListWidget(&logDialog);
    logListWidget->addItems(m_userLogs);
    layout->addWidget(logListWidget);

    // ���������ð�ť
    QPushButton* saveButton = new QPushButton("������־", &logDialog);

    // ��ť��ʽ
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
    saveButton->setFixedWidth(100); // ���ð�ť���

    // ��Ӱ�ť������
    layout->addWidget(saveButton);

    // ���Ӱ�ť���źźͲ�
    connect(saveButton, &QPushButton::clicked, [&]() {
        QString filePath = QFileDialog::getSaveFileName(&logDialog, "������־", "", "Text Files (*.txt);;All Files (*)");
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

    // ִ�жԻ���
    logDialog.exec();
}


void KMainWindow::onFilterTypeAdded(const QString& type) 
{
    logUserAction("��Ӻ����ļ���׺");
    qDebug() << "KMainWindow::onFilterTypeAdded";
    if (m_stackWidget) 
    {
        m_stackWidget->m_fileFilter.addFilterByType(type);
        
    }
}

void KMainWindow::removeFilterType(const QString& type)
{
    logUserAction("ɾ�������ļ���׺");
    qDebug() << "KMainWindow::removeFilterType";
    if (m_stackWidget)
    {
        m_stackWidget->m_fileFilter.removeFilterByType(type);
    }
}

void KMainWindow::saveFilterConfiguration()
{
    logUserAction("���������ļ�");
    if (m_stackWidget)
    {
        m_stackWidget->m_fileFilter.saveToFile("filter_config.json");
    }
}

void KMainWindow::onMonitorFiles()
{
    logUserAction("�л�����ļ�����");
    m_stackWidget->setCurrentIndex(0);
}

void KMainWindow::onMonitorLogs()
{
    logUserAction("�л������־����");
    m_stackWidget->setCurrentIndex(1);
}   

void KMainWindow::onLogInfo()
{
    logUserAction("�л���־��Ϣ����");
    m_stackWidget->setCurrentIndex(2);
}

void KMainWindow::openNewProgram()
{
    logUserAction("�½�����");
    // ��ȡ��ǰ�����Ŀ¼·��
    QString programDir = QCoreApplication::applicationDirPath();

    // ���쵱ǰ�����·��
    QString programPath = QDir(programDir).filePath(QCoreApplication::applicationName());

    QStringList arguments; // �������ó���Ĳ���������еĻ�

    // ������ǰ�������ʵ��
    bool success = QProcess::startDetached(programPath, arguments);
    if (!success) {
        qDebug() << "Failed to start the program!";
    }
}

void KMainWindow::initTrayIcon()
{
    // ��������ͼ��
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/icons/kmainwidow.png"));
    m_trayIcon->setToolTip("FileWatcher");

    // �������̲˵�
    m_trayIconMenu = new QMenu(this);
    QAction* restoreAction = new QAction("��ʾ����", this);
    connect(restoreAction, &QAction::triggered, this, &KMainWindow::restoreWindow);
    QAction* quitAction = new QAction("�˳�����", this);
    connect(quitAction, &QAction::triggered, this, &KMainWindow::onExitTriggered);

    m_trayIconMenu->addAction(restoreAction);
    m_trayIconMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayIconMenu);

    // ��������ͼ�����ź�
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &KMainWindow::iconActivated);

    // ��ʾ����ͼ��
    m_trayIcon->show();
}

void KMainWindow::restoreWindow()
{
    this->showNormal(); // �ָ�����
    stopTrayIconBlinking(); // ֹͣ��˸
}

void KMainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        this->showNormal(); // ��������ͼ��ʱ��ԭ����
        stopTrayIconBlinking(); // ֹͣ��˸
        break;
    default:
        break;
    }
}

void KMainWindow::startTrayIconBlinking()
{
    if (!m_blinkTimer) {
        m_blinkTimer = new QTimer(this);
        m_blinkTimer->setInterval(500); // ÿ500�����л�ͼ��һ��
        connect(m_blinkTimer, &QTimer::timeout, this, &KMainWindow::blinkTrayIcon);
    }
    m_blinkTimer->start();
}

void KMainWindow::stopTrayIconBlinking()
{
    if (m_blinkTimer) {
        m_blinkTimer->stop();
    }
    // �ָ�����ͼ��
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
        hide(); // ���ش���
        event->ignore(); // ���Թر��¼�����ֹ���ڱ�����
        //startTrayIconBlinking(); // ��ʼ��˸
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
            //startTrayIconBlinking(); // ��ʼ��˸
        }
        else if (isVisible()) {
            stopTrayIconBlinking(); // ֹͣ��˸
        }
    }
}

void KMainWindow::onExitTriggered() {
    logUserAction("�˳�����");
    qApp->quit();  // �˳�Ӧ�ó���
}

void KMainWindow::logUserAction(const QString& action) {
    QString logEntry = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + " - " + action;
    m_userLogs.append(logEntry);
}

//void KMainWindow::initLoadingDialog()
//{
//    m_loadingDialog = new QDialog(this);
//    m_loadingDialog->setWindowTitle("������...");
//    m_loadingDialog->setModal(true); // ȷ����ʾ����ֹ�û����������ڽ���
//    QVBoxLayout* layout = new QVBoxLayout(m_loadingDialog);
//    QLabel* label = new QLabel("���ڼ��أ����Ժ�...", m_loadingDialog);
//    layout->addWidget(label);
//    m_loadingDialog->setLayout(layout);
//}

void KMainWindow::initLoadingDialog()
{
    // �����Ի������ñ����ģ̬����
    m_loadingDialog = new QDialog(this);
    m_loadingDialog->setWindowTitle("���ڼ��أ����Ժ�...");
    m_loadingDialog->setFixedSize(300, 15); // ���ù̶���С
    m_loadingDialog->setModal(false);

    // ���ñ�����ɫΪǳ��ɫ
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(240, 240, 240)); // Light gray
    m_loadingDialog->setPalette(palette);
    m_loadingDialog->setAutoFillBackground(true);

    //// ��ʼ������
    //m_loadLayout = new QVBoxLayout(m_loadingDialog);

    //// ���������ñ�ǩ
    //m_loadLabel = new QLabel("���ڼ��أ����Ժ�...", m_loadingDialog);
    //m_loadLabel->setAlignment(Qt::AlignCenter); // ���ж����ı�

    //// ����������ʽ
    //QFont font("Arial", 12); // ʹ����ϲ��������ʹ�С
    //m_loadLabel->setFont(font);

    //// ��ӱ�ǩ������
    //m_loadLayout->addWidget(m_loadLabel);

    //// ȷ���������ú�
    //m_loadingDialog->setLayout(m_loadLayout);

    // ���ùرհ�ť
    //m_loadingDialog->setWindowFlags(m_loadingDialog->windowFlags() & ~Qt::WindowCloseButtonHint);
}


// ��ʾ������ʾ��
void KMainWindow::showLoadingDialog()
{
    if (m_loadingDialog) {
        m_loadingDialog->show();
    }
}

// ���ؼ�����ʾ��
void KMainWindow::hideLoadingDialog()
{
    if (m_loadingDialog) {
        m_loadingDialog->hide();
    }
}
