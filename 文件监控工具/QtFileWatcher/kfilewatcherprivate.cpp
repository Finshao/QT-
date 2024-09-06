#include "kfilewatcherprivate.h"
#include <iostream>
#include <QString>
#include <QMutex>
#include <QDir>
#include <QDirIterator>
#include <QQueue>


KFileWatcherPrivate::KFileWatcherPrivate(const QString path, QObject* parent)
    : QObject(parent), m_path(path)
{
    initwatcher();
    //qDebug() << "KFileWatcherPrivate";
}


KFileWatcherPrivate::KFileWatcherPrivate(QObject* parent) : QObject(parent)
{
}

KFileWatcherPrivate::~KFileWatcherPrivate()
{
    clear();
    m_ifExit = true;
    qDebug() << "~KFileWatcherPrivate";
}


void KFileWatcherPrivate::setPath(const QString path)
{
    m_path = path;
    if (m_ifWatcher == true) stopWatcher();
    clear();
    initwatcher();
}

QString KFileWatcherPrivate::getPath()
{
    return m_path;
}

void KFileWatcherPrivate::handleFileChange()
{
    qDebug() << "handleFileChange";

    DWORD result = WaitForSingleObject(m_overlapped.hEvent, INFINITE);
    if (result == WAIT_OBJECT_0)
    {
        DWORD bytes;
        if (GetOverlappedResult(m_hDir, &m_overlapped, &bytes, FALSE))
        {
            qDebug() << "File changes detected.";
            solvebuffer();

            //Sleep(3000);
            ResetEvent(m_overlapped.hEvent);

            if (!ReadDirectoryChangesW(m_hDir, &m_buffer[0], m_buffer.size(), TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
                nullptr, &m_overlapped, nullptr))
            {
                qCritical() << "ReadDirectoryChangesW failed with error:" << GetLastError();
            }
            else
            {
                //qDebug() << "ReadDirectoryChangesW";
            }
        }
        else
        {
            qDebug() << "GetOverlappedResult failed with error:" << GetLastError();
        }
    }
    else
    {
        qDebug() << "WaitForSingleObject failed with error:" << GetLastError();
    }

}

void KFileWatcherPrivate::run()
{
    while (true)
    {
        if (m_ifExit)
        {
            // 退出
            break;
        }
        else if (m_ifWatcher)
        {
            handleFileChange();
            //Sleep(100);
        }
        else
        {
            Sleep(1000);
        }
        
    }
}

//FILE_ACTION_ADDED: 文件或目录被创建。
//FILE_ACTION_REMOVED : 文件或目录被删除。
//FILE_ACTION_MODIFIED : 文件的内容或属性被修改。
//FILE_ACTION_RENAMED_NEW_NAME : 文件或目录被重命名（新名称）。
//FILE_ACTION_RENAMED_OLD_NAME : 文件或目录被重命名（旧名称）。
//FILE_ACTION_OVERFLOW : 文件或目录的更改太多，无法放入通知缓冲区。
//FILE_ACTION_ACCESS : 文件的访问权限发生改变。
//FILE_ACTION_CLOSE : 文件被关闭。
//FILE_ACTION_OPEN : 文件被打开。
//FILE_ACTION_LINK : 文件的链接发生变化。

void KFileWatcherPrivate::solvebuffer()
{
    QMutexLocker locker(&mutex);
    DWORD offset = 0;
    FILE_NOTIFY_INFORMATION* pFileNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&m_buffer[0]);

    while (offset < m_buffer.size()) {
        QList<QString> action;
        // 计算当前通知信息的大小
        DWORD bytes = sizeof(FILE_NOTIFY_INFORMATION) + pFileNotify->FileNameLength + pFileNotify->NextEntryOffset;

        FILE_NOTIFY_INFORMATION* pFileNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&m_buffer[offset]);

        if (pFileNotify->Action <= 0 || pFileNotify->Action > 10) break;
        qDebug() << "Action:" << pFileNotify->Action;
        action.append(QString::number(pFileNotify->Action));
        QString fileName = QString::fromUtf16((ushort*)pFileNotify->FileName, pFileNotify->FileNameLength / 2);
        qDebug() << "File:" << fileName;
        fileName = fileName.replace("\\", "/");
        //qDebug() << "++++++++" << fileName;
        action.append(fileName);

        m_action.append(action);
        // 更新偏移量以指向下一个通知信息
        offset += bytes;

        // 检查是否超出了缓冲区的边界
        if (offset > m_buffer.size())
            break;
       
    }

    emit updataAction();
}

void KFileWatcherPrivate::initwatcher()
{
    m_buffer.resize(4096);

    m_hDir = CreateFileW(m_path.toStdWString().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
    if (m_hDir == INVALID_HANDLE_VALUE)
    {
        qCritical() << "CreateFile failed with error:" << GetLastError();
        return;
    }
    m_overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!m_overlapped.hEvent)
    {
        qCritical() << "CreateEvent failed with error:" << GetLastError();
        CloseHandle(m_hDir);
        m_hDir = INVALID_HANDLE_VALUE;
        return;
    }

    // Start the file change notification.
    if (!ReadDirectoryChangesW(m_hDir, &m_buffer[0], m_buffer.size(), TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SECURITY,
        nullptr, &m_overlapped, nullptr))
    {
        qCritical() << "ReadDirectoryChangesW failed with error:" << GetLastError();
        CloseHandle(m_hDir);
        CloseHandle(m_overlapped.hEvent);
        m_hDir = INVALID_HANDLE_VALUE;
        return;
    }
    
    m_ifWatcher = true;
}

void KFileWatcherPrivate::clear()
{
    if (m_hDir != INVALID_HANDLE_VALUE) {
        m_buffer.clear();
        m_buffer.resize(4096);
        m_currentFileInfo.clear();
        m_action.clear();
        if (m_ifWatcher)
        {
            CancelIoEx(m_hDir, &m_overlapped);
            CloseHandle(m_hDir);
            CloseHandle(m_overlapped.hEvent);
        }
    }
}

void KFileWatcherPrivate::stopWatcher()
{
    m_ifWatcher = false;
    CancelIoEx(m_hDir, &m_overlapped);
    CloseHandle(m_hDir);
    CloseHandle(m_overlapped.hEvent);
}

void KFileWatcherPrivate::beginWatcher()
{
    initwatcher();
}




//void KFileWatcherPrivate::listFilesInfo(const QString& path, int level, int maxLevel)
//{
//
//    if (level > maxLevel)
//        return;
//
//    if (level == -1)
//    {
//        // 获取关于当前目录的信息
//        QFileInfo dirInfo(path);
//        qDebug() << "Current Directory Information:";
//        qDebug() << "Directory Name:" << dirInfo.fileName();
//        qDebug() << "Full Path:" << path;
//        qDebug() << "File Type:" << "Directory";
//        qDebug() << "File Size:" << "N/A"; // 目录在传统意义上没有大小
//        qDebug() << "Creation Time:" << dirInfo.created().toString(Qt::SystemLocaleShortDate);
//        qDebug() << "Last Modified Time:" << dirInfo.lastModified().toString(Qt::SystemLocaleShortDate);
//
//        // 添加当前目录信息到列表
//        FileInfo fileInfoStruct;
//        fileInfoStruct.name = dirInfo.fileName();
//        fileInfoStruct.fullPath = path;
//        fileInfoStruct.type = "Directory";
//        fileInfoStruct.size = 0; // 表示大小不适用
//        fileInfoStruct.creationTime = dirInfo.created();
//        fileInfoStruct.lastModifiedTime = dirInfo.lastModified();
//        fileInfoStruct.ownerIconIndex = -1; // 假设没有特定的图标索引
//        fileInfoStruct.directoryLevel = level; // 设置目录层级
//        m_currentFileInfo.append(fileInfoStruct);
//
//        listFilesInfo(path, level + 1, maxLevel); // 递归调用并增加层级
//        return;
//    }
//
//    // 初始化文件系统搜索对象
//    WIN32_FIND_DATAW findData;
//    HANDLE hFind = FindFirstFileExW((path.toStdWString() + L"\\*").c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, nullptr, 0);
//
//    if (hFind != INVALID_HANDLE_VALUE) {
//        do {
//            // 跳过 "." 和 ".." 目录
//            if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
//                continue;
//
//            QString fileName = QString::fromWCharArray(findData.cFileName);
//            QString fullPath = path + "/" + fileName;
//            QFileInfo fileInfo(fullPath);
//
//            // 存储文件信息
//            FileInfo fileInfoStruct;
//            fileInfoStruct.name = fileName;
//            fileInfoStruct.fullPath = fullPath;
//            fileInfoStruct.type = fileInfo.isDir() ? "Directory" : "File";
//            fileInfoStruct.size = fileInfo.size();
//            fileInfoStruct.creationTime = fileInfo.created();
//            fileInfoStruct.lastModifiedTime = fileInfo.lastModified();
//
//            // 文件所有者在 Windows 上可能比较复杂。这里使用 SHGetFileInfo 获取图标，可能会给我们一些提示
//            SHFILEINFOW shfi = { 0 };
//            SHGetFileInfoW(fullPath.toStdWString().c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
//
//            // 为了简化问题，我们假设图标索引可以提供一些关于文件类型或所有权的线索
//            fileInfoStruct.ownerIconIndex = shfi.iIcon;
//            fileInfoStruct.directoryLevel = level; // 设置目录层级
//
//            m_currentFileInfo.append(fileInfoStruct);
//
//            // 如果是目录，则递归地遍历该目录
//            if (fileInfo.isDir()) {
//                listFilesInfo(fullPath, level + 1, maxLevel); // 递归调用并增加层级
//            }
//
//        } while (FindNextFileW(hFind, &findData) != 0);
//
//        FindClose(hFind);
//    }
//}


void KFileWatcherPrivate::listFilesInfo(const QString& path, int maxLevel) {
    //QMutexLocker locker(&mutex);

    QQueue<QPair<QString, int>> directories;
    directories.enqueue(QPair<QString, int>(path, 0)); // 初始目录和深度

    while (!directories.isEmpty()) {
        QPair<QString, int> current = directories.dequeue();
        QString currentPath = current.first;
        int currentLevel = current.second;

        if (currentLevel > maxLevel) {
            continue; // 如果当前深度超过了最大深度，则跳过
        }
        //qDebug() << 1;
        QDir dir(currentPath);
        QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        foreach(const QFileInfo & fileInfo, fileList) {
            //qDebug() << 2;
            FileInfo fileInfoStruct;
            fileInfoStruct.name = fileInfo.fileName();
            fileInfoStruct.fullPath = fileInfo.absoluteFilePath();
            fileInfoStruct.type = fileInfo.isDir() ? "Directory" : "File";
            fileInfoStruct.size = fileInfo.size();
            fileInfoStruct.creationTime = fileInfo.created();
            fileInfoStruct.lastModifiedTime = fileInfo.lastModified();
            fileInfoStruct.ownerIconIndex = 0;
            fileInfoStruct.directoryLevel = currentLevel;



            m_currentFileInfo.append(fileInfoStruct);

            // 如果是目录且当前深度未超过最大深度，则将其加入队列以便进一步遍历
            if (fileInfo.isDir() && currentLevel < maxLevel) {
                directories.enqueue(QPair<QString, int>(fileInfo.absoluteFilePath(), currentLevel + 1));
            }
        }
    }
    //qDebug() << 3;
}






QList<QList<QString>> KFileWatcherPrivate::convertFileInfoToLists() const
{
    QList<QList<QString>> result;

    // Add the header row
    //result.append(QList<QString>() << "Name" << "Full Path" << "Type" << "Size" << "Creation Time" << "Last Modified Time" << "Owner Icon Index");

    // Convert each FileInfo item to a string list
    for (const FileInfo& fileInfo : m_currentFileInfo) {
        QList<QString> infoRow;
        infoRow << fileInfo.name
            << fileInfo.fullPath
            << fileInfo.type
            << QString::number(fileInfo.size)
            << fileInfo.creationTime.toString(Qt::SystemLocaleShortDate)
            << fileInfo.lastModifiedTime.toString(Qt::SystemLocaleShortDate)
            << QString::number(0)
            << QString::number(fileInfo.directoryLevel);
        //for (auto a : infoRow)
        //{
        //    qDebug() << a;
        //}
        result.append(infoRow);
    }
    return result;
}

QList<QList<QString>> KFileWatcherPrivate::coverActionToLists()
{
    QMutexLocker locker(&mutex);

    QList<QList<QString>> actionCopy = m_action;
    m_action.clear();

   
    return actionCopy;
}
