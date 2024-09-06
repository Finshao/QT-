#ifndef __K_MAINWINDOW_H_
#define __K_MAINWINDOW_H_
#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QFileDialog>  
#include <QSvgRenderer>  
#include <QPainter>  
#include <QKeyEvent>  
#include <QToolBar>
#include <QTableWidget>
#include <QStackedWidget>
#include <QTimer>
#include <QSystemTrayIcon>


#include "kstackedwidget.h"
#include "ui_kmainwindow.h"
#include "kfilewatcher.h"
#include "filtersettingdialog.h"



class KMainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit KMainWindow(QWidget *parent = Q_NULLPTR);
    ~KMainWindow();

    void initWindow();
    void initWatcher(QString dir);
    void initTimer();
    void initFilterSettings();
    void initTrayIcon();
    void initLoadingDialog();

    void startTrayIconBlinking();
    void stopTrayIconBlinking();
    void blinkTrayIcon();
    
    void clear();

    QList<QList<QString>> getFileInfo();
    QList<QList<QString>> getActionInfo();
    void sendPathToWatcher(const QString path);
    void setFileInfoToStackWidget();
    void setActionInfoToStackWidget();
    

    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;

    void logUserAction(const QString& action);

    void showLoadingDialog();
    void hideLoadingDialog();



private:
    
private slots:
    void onNewListener();
    void backPrePath();
    void updataActionFuction();
    void updataFileInfoTunction();
    void stopWatcher();
    void beginWatcher();
    void onFilterTypeAdded(const QString& type);
    void removeFilterType(const QString& type);
    void saveFilterConfiguration();
    void onMonitorFiles();
    void onMonitorLogs();
    void onLogInfo();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void openNewProgram();
    void initUserLog();
    void onExitTriggered();
    void onFileChangeSignal();
    void restoreWindow();

private:
    // 布局
    Ui::KMainWindowClass ui;
    QVBoxLayout* m_pVLayout;
    QHBoxLayout* m_pHLayout;
    QToolBar* m_menuBarTool;
    QToolBar* m_functionBar;
    //QTableWidget* m_tableWidget;
    KStackWidget* m_stackWidget;

    // 监视
    KFileWatcherEngine* m_watcher;
    QThread* m_thread = nullptr;
    QString m_path;

    // 定时器
    QTimer* m_timer;

    bool m_ifWatch = true;

    // 过滤器对话框
    KFilterSettingsDialog* m_filterSettingsDialog;

    // 系统托盘
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayIconMenu;
    QTimer* m_blinkTimer = nullptr;
    bool m_isBlinking;

   // 用户操作日志
    QStringList m_userLogs;

    bool m_needsUpdate = false;
    int i = 0;

    // 加载窗口
    QDialog* m_loadingDialog;
    QVBoxLayout* m_loadLayout;
    QLabel* m_loadLabel;
};

#endif
