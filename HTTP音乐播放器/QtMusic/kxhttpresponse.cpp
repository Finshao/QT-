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
        QByteArray responseData = reply->readAll(); // ��ȡӦ������  
        QString type = reply->rawHeader("X-Data-Type");
        // type == "application/json"
        if (1)
        {
            QJsonDocument doc = QJsonDocument::fromJson(responseData);

            if (!doc.isNull() && doc.isObject())
            {
                QJsonObject jsonObject = doc.object();

                // ����Ƿ����"list"����������ֵ��һ������  
                if (jsonObject.contains("list") && jsonObject["list"].isArray())
                {
                    QJsonArray jsonArray = jsonObject["list"].toArray();

                    // ���������е�ÿ����������  
                    for (const QJsonValue& value : jsonArray)
                    {
                        if (value.isObject())
                        {
                            QJsonObject songObject = value.toObject();

                            // ��ȡ������Ϣ  
                            QString albumName = songObject["albumName"].toString();
                            int duration = songObject["duration"].toInt();
                            QString imgPath = songObject["img"].toString();
                            QString lyricPath = songObject["lyric"].toString();
                            QString mp3Path = songObject["mp3"].toString(); // ע�⣺���������Ҫ����ʵ���������·����ͻ  
                            QString musicName = songObject["musicName"].toString();
                            QString path = songObject["path"].toString();

                            // ��ӡ������Щ��Ϣ  
                            qDebug() << "Album Name:" << albumName;
                            qDebug() << "Duration:" << duration << "seconds";
                            qDebug() << "Image Path:" << imgPath;
                            qDebug() << "Lyric Path:" << lyricPath;
                            qDebug() << "MP3 Path:" << mp3Path; // ע�⣺������Ҫȷ��ÿ��MP3·������Ψһ��  
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

            QByteArray responseData = reply->readAll(); // ��ȡӦ������  

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

    // ��Ҫ����ɾ��reply�����Ա����ڴ�й©  
    reply->deleteLater();
}

void KxResponse::onHandleImg(QNetworkReply* reply)
{

}
