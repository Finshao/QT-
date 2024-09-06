// FileWatcherPrivate.h  
#ifndef FILEWATCHERPRIVATE_H  
#define FILEWATCHERPRIVATE_H  

#include <windows.h>       
#include <qt_windows.h> 
#include <lmcons.h> 
#include <sddl.h> 
#include <aclapi.h> 
#include <shlobj.h>
#include <shlwapi.h>
#include <iostream>
#include <vector>

#include <QCoreApplication>
#include <QObject>  
#include <QThread>  
#include <QWaitCondition>  
#include <QDebug>  
#include <windows.h>  
#include <QDir>
#include <QDateTime>
#include <QList>
#include <QMutex>

// Necessary to link with shlwapi.lib
// #pragma comment(lib, "shlwapi.lib")

class KFileWatcherPrivate : public QObject 
{
    Q_OBJECT

public:
    KFileWatcherPrivate(const QString path, QObject* parent = nullptr);
    explicit KFileWatcherPrivate(QObject* parent = nullptr);
    ~KFileWatcherPrivate();

    void setPath(const QString path);
    QString getPath();
    void handleFileChange();

    void run();
    void solvebuffer();
    void initwatcher();
    void clear();
    void stopWatcher();
    void beginWatcher();
    //void KFileWatcherPrivate::listFilesInfo(const QString& path, int level, int maxLevel);
    void listFilesInfo(const QString& path, int maxLevel);
    QList<QList<QString>> KFileWatcherPrivate::convertFileInfoToLists() const;
    QList<QList<QString>> KFileWatcherPrivate::coverActionToLists();
public:
    struct FileInfo 
    {
        QString name;
        QString fullPath;
        QString type;
        qint64 size;
        QDateTime creationTime;
        QDateTime lastModifiedTime;
        int ownerIconIndex;
        int directoryLevel;
    };
    FileInfo m_currentDirInfo;
    QList<FileInfo> m_currentFileInfo;

    QList<QList<QString>> m_action;

signals:
    void updataAction();
private:
    HANDLE m_hDir;
    OVERLAPPED m_overlapped;
    std::vector<WCHAR> m_buffer; // Buffer for file changes.
    QString m_path;

    bool m_ifWatcher = true;
    bool m_ifExit = false;

    QMutex mutex;
};

#endif // FILEWATCHERPRIVATE_H 