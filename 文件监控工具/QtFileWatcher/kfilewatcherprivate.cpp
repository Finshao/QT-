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
            // �˳�
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

//FILE_ACTION_ADDED: �ļ���Ŀ¼��������
//FILE_ACTION_REMOVED : �ļ���Ŀ¼��ɾ����
//FILE_ACTION_MODIFIED : �ļ������ݻ����Ա��޸ġ�
//FILE_ACTION_RENAMED_NEW_NAME : �ļ���Ŀ¼���������������ƣ���
//FILE_ACTION_RENAMED_OLD_NAME : �ļ���Ŀ¼���������������ƣ���
//FILE_ACTION_OVERFLOW : �ļ���Ŀ¼�ĸ���̫�࣬�޷�����֪ͨ��������
//FILE_ACTION_ACCESS : �ļ��ķ���Ȩ�޷����ı䡣
//FILE_ACTION_CLOSE : �ļ����رա�
//FILE_ACTION_OPEN : �ļ����򿪡�
//FILE_ACTION_LINK : �ļ������ӷ����仯��

void KFileWatcherPrivate::solvebuffer()
{
    QMutexLocker locker(&mutex);
    DWORD offset = 0;
    FILE_NOTIFY_INFORMATION* pFileNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(&m_buffer[0]);

    while (offset < m_buffer.size()) {
        QList<QString> action;
        // ���㵱ǰ֪ͨ��Ϣ�Ĵ�С
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
        // ����ƫ������ָ����һ��֪ͨ��Ϣ
        offset += bytes;

        // ����Ƿ񳬳��˻������ı߽�
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
//        // ��ȡ���ڵ�ǰĿ¼����Ϣ
//        QFileInfo dirInfo(path);
//        qDebug() << "Current Directory Information:";
//        qDebug() << "Directory Name:" << dirInfo.fileName();
//        qDebug() << "Full Path:" << path;
//        qDebug() << "File Type:" << "Directory";
//        qDebug() << "File Size:" << "N/A"; // Ŀ¼�ڴ�ͳ������û�д�С
//        qDebug() << "Creation Time:" << dirInfo.created().toString(Qt::SystemLocaleShortDate);
//        qDebug() << "Last Modified Time:" << dirInfo.lastModified().toString(Qt::SystemLocaleShortDate);
//
//        // ��ӵ�ǰĿ¼��Ϣ���б�
//        FileInfo fileInfoStruct;
//        fileInfoStruct.name = dirInfo.fileName();
//        fileInfoStruct.fullPath = path;
//        fileInfoStruct.type = "Directory";
//        fileInfoStruct.size = 0; // ��ʾ��С������
//        fileInfoStruct.creationTime = dirInfo.created();
//        fileInfoStruct.lastModifiedTime = dirInfo.lastModified();
//        fileInfoStruct.ownerIconIndex = -1; // ����û���ض���ͼ������
//        fileInfoStruct.directoryLevel = level; // ����Ŀ¼�㼶
//        m_currentFileInfo.append(fileInfoStruct);
//
//        listFilesInfo(path, level + 1, maxLevel); // �ݹ���ò����Ӳ㼶
//        return;
//    }
//
//    // ��ʼ���ļ�ϵͳ��������
//    WIN32_FIND_DATAW findData;
//    HANDLE hFind = FindFirstFileExW((path.toStdWString() + L"\\*").c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, nullptr, 0);
//
//    if (hFind != INVALID_HANDLE_VALUE) {
//        do {
//            // ���� "." �� ".." Ŀ¼
//            if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
//                continue;
//
//            QString fileName = QString::fromWCharArray(findData.cFileName);
//            QString fullPath = path + "/" + fileName;
//            QFileInfo fileInfo(fullPath);
//
//            // �洢�ļ���Ϣ
//            FileInfo fileInfoStruct;
//            fileInfoStruct.name = fileName;
//            fileInfoStruct.fullPath = fullPath;
//            fileInfoStruct.type = fileInfo.isDir() ? "Directory" : "File";
//            fileInfoStruct.size = fileInfo.size();
//            fileInfoStruct.creationTime = fileInfo.created();
//            fileInfoStruct.lastModifiedTime = fileInfo.lastModified();
//
//            // �ļ��������� Windows �Ͽ��ܱȽϸ��ӡ�����ʹ�� SHGetFileInfo ��ȡͼ�꣬���ܻ������һЩ��ʾ
//            SHFILEINFOW shfi = { 0 };
//            SHGetFileInfoW(fullPath.toStdWString().c_str(), FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
//
//            // Ϊ�˼����⣬���Ǽ���ͼ�����������ṩһЩ�����ļ����ͻ�����Ȩ������
//            fileInfoStruct.ownerIconIndex = shfi.iIcon;
//            fileInfoStruct.directoryLevel = level; // ����Ŀ¼�㼶
//
//            m_currentFileInfo.append(fileInfoStruct);
//
//            // �����Ŀ¼����ݹ�ر�����Ŀ¼
//            if (fileInfo.isDir()) {
//                listFilesInfo(fullPath, level + 1, maxLevel); // �ݹ���ò����Ӳ㼶
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
    directories.enqueue(QPair<QString, int>(path, 0)); // ��ʼĿ¼�����

    while (!directories.isEmpty()) {
        QPair<QString, int> current = directories.dequeue();
        QString currentPath = current.first;
        int currentLevel = current.second;

        if (currentLevel > maxLevel) {
            continue; // �����ǰ��ȳ����������ȣ�������
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

            // �����Ŀ¼�ҵ�ǰ���δ���������ȣ������������Ա��һ������
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
