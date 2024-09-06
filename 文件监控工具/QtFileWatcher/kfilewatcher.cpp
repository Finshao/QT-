#include "kfilewatcher.h"

KFileWatcherEngine::KFileWatcherEngine(const QString directoryPath, QObject* parent)
    : QObject(parent)
    , m_dptr(std::make_unique<KFileWatcherPrivate>(directoryPath))
{
    (void)connect(m_dptr.get(), &KFileWatcherPrivate::updataAction, this, &KFileWatcherEngine::updataActionFuction);
}

KFileWatcherEngine::~KFileWatcherEngine()
{
    // The destructor of KFileWatcherPrivate will automatically close any open handles.
}

void KFileWatcherEngine::handleFileChange()
{
    //m_dptr->listFilesInfo();
    //QList<QList<QString>> result = m_dptr->convertFileInfoToLists();
    
    m_dptr->run();
    
}

QVariant KFileWatcherEngine::listFilesInfo()
{

    QString path = m_dptr->getPath();
    m_dptr->listFilesInfo(path, 5);
    QList<QList<QString>> result = m_dptr->convertFileInfoToLists();

    qDebug() << "KFileWatcherEngine::listFilesInfo()";
  /*  for (auto a : result)
    {
        for (auto b : a)
        {
            qDebug() << b;
        }
    }*/
    QVariant resultVariant = QVariant::fromValue(result);
    return resultVariant;
}

QVariant KFileWatcherEngine::listActionInfo()
{
    QList<QList<QString>> result = m_dptr->coverActionToLists();

    qDebug() << "KFileWatcherEngine::listActionInfo()";
    //for (auto a : result)
    //{
    //    for (auto b : a)
    //    {
    //        qDebug() << b;
    //    }
    //}
    QVariant resultVariant = QVariant::fromValue(result);
    return resultVariant;
}

void KFileWatcherEngine::setPath(QString path)
{
    m_dptr->setPath(path);
}

void KFileWatcherEngine::stopWatcher()
{
    return m_dptr->stopWatcher();
}

void KFileWatcherEngine::beginWatcher()
{
    return m_dptr->beginWatcher();
}

void KFileWatcherEngine::updataActionFuction()
{
    emit updataAction();
    qDebug() << "KFileWatcherEngine emit";
}
