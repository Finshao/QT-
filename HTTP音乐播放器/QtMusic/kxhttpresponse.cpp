#include "kxhttpresponse.h"

#include <QJsonDocument>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

KxResponse::KxResponse(QObject* parent)
	: QObject(parent)
{
}

KxResponse::~KxResponse()
{
}

void KxResponse::onHandleResponse(QNetworkReply* reply)
{

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray responseData = reply->readAll(); // 读取应答数据  
        QString type = reply->rawHeader("X-Data-Type");
        // type == "application/json"
        if (1)
        {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);

            if (!doc.isNull() && doc.isObject())
            {
                QJsonObject jsonObject = doc.object();

                // 检查是否存在"list"键，并且其值是一个数组  
                if (jsonObject.contains("list") && jsonObject["list"].isArray())
                {
                    QJsonArray jsonArray = jsonObject["list"].toArray();

                    // 遍历数组中的每个歌曲对象  
                    for (const QJsonValue& value : jsonArray)
                    {
                        if (value.isObject())
                        {
                            QJsonObject songObject = value.toObject();

                            // 提取歌曲信息  
                            QString albumName = songObject["albumName"].toString();
                            int duration = songObject["duration"].toInt();
                            QString imgPath = songObject["img"].toString();
                            QString lyricPath = songObject["lyric"].toString();
                            QString mp3Path = songObject["mp3"].toString(); // 注意：这里可能需要根据实际情况处理路径冲突  
                            QString musicName = songObject["musicName"].toString();
                            QString path = songObject["path"].toString();

                            // 打印或处理这些信息  
                            qDebug() << "Album Name:" << albumName;
                            qDebug() << "Duration:" << duration << "seconds";
                            qDebug() << "Image Path:" << imgPath;
                            qDebug() << "Lyric Path:" << lyricPath;
                            qDebug() << "MP3 Path:" << mp3Path; // 注意：可能需要确保每个MP3路径都是唯一的  
                            qDebug() << "Music Name:" << musicName;
                            qDebug() << "Path:" << path;

                            emit songInfoReady(albumName, duration, imgPath, lyricPath, mp3Path, musicName, path);
                        }
                    }
                }
                else
                {
                    qDebug() << "Invalid JSON data or not an object";
                }
            }
        }
        else if (type == "image/jpeg")
        {
            if (reply->error()) {
                qDebug() << "Error:" << reply->errorString();
                return;
            }

            QByteArray responseData = reply->readAll(); // 读取应答数据  

            QImage image;
            if (!image.loadFromData(responseData, "IMG")) {
                qDebug() << "Failed to load image";
                return;
            }
            qDebug() << "Image loaded successfully";
            emit imgInfoReady(image);
        }
        else
        {
            qDebug() << "error Header is empty";
        }
        
        
    }
    else
    {
        qDebug() << "Error:" << reply->errorString();
    }

    // 不要忘记删除reply对象，以避免内存泄漏  
    reply->deleteLater();
}

void KxResponse::onHandleImg(QNetworkReply* reply)
{

}
