#ifndef KFILEWATCHERENGINE_H
#define KFILEWATCHERENGINE_H

#include <windows.h>
#include <qt_windows.h>
#include <QCoreApplication>
#include <QObject>
#include <QString>
#include <QThread>
#include <memory>
#include "kfilewatcherprivate.h"

class KFileWatcherEngine : public QObject 
{
    Q_OBJECT

public:
    KFileWatcherEngine(const QString directoryPath, QObject* parent = nullptr);

    ~KFileWatcherEngine();

    void handleFileChange();

    Q_INVOKABLE QVariant listFilesInfo();
    Q_INVOKABLE QVariant listActionInfo();
    Q_INVOKABLE void setPath(QString path);
    Q_INVOKABLE void stopWatcher();
    Q_INVOKABLE void beginWatcher();
    void updataActionFuction();

signals:
    void updataAction();
private:
    std::unique_ptr<KFileWatcherPrivate> m_dptr;
};

#endif // KFILEWATCHERENGINE_H