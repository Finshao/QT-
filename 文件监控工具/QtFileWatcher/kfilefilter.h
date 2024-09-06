#pragma once
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

class KFileFilter 
{
public:
    void addFilterByType(const QString& type) 
    {
        m_fileTypes.append(type);
    }

    void addFilterByName(const QString& name) 
    {
        m_fileNames.append(name);
    }

    void addFilterByPath(const QString& path) 
    {
        m_filePaths.append(path);
    }

    void removeFilterByType(const QString& type) 
    {
        m_fileTypes.removeAll(type);
    }

    void removeFilterByName(const QString& name) 
    {
        m_fileNames.removeAll(name);
    }

    void removeFilterByPath(const QString& path) 
    {
        m_filePaths.removeAll(path);
    }

    bool isFiltered(const QString& fileName, const QString& filePath, const QString& fileType) 
    {
        if (m_fileTypes.contains(fileType) || m_fileNames.contains(fileName) || m_filePaths.contains(filePath)) 
        {
            return true;
        }
        return false;
    }
    bool isFiltered(const QString& filePath, const QString& fileType) 
    {
        if (m_filePaths.contains(filePath) || m_fileTypes.contains(fileType)) 
        {
            return true;
        }
        return false;
    }

    void saveToFile(const QString& filePath) 
    {
        QJsonObject jsonObject;

        // �����ļ����͹�����
        QJsonArray typesArray;
        for (const QString& type : m_fileTypes) 
        {
            typesArray.append(type);
        }
        jsonObject["fileTypes"] = typesArray;

        //// �����ļ���������
        //QJsonArray namesArray;
        //for (const QString& name : m_fileNames) {
        //    namesArray.append(name);
        //}
        //jsonObject["fileNames"] = namesArray;

        //// �����ļ�·��������
        //QJsonArray pathsArray;
        //for (const QString& path : m_filePaths) {
        //    pathsArray.append(path);
        //}
        //jsonObject["filePaths"] = pathsArray;

        // �� JSON ����д���ļ�
        QJsonDocument jsonDoc(jsonObject);
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly)) 
        {
            file.write(jsonDoc.toJson());
            file.close();
        }
    }

    void KFileFilter::loadFromFile(const QString& filePath ) 
    {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) 
        {
            return;
        }

        QByteArray data = file.readAll();
        file.close();

        QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
        QJsonObject jsonObject = jsonDoc.object();

        // �����ļ����͹�����
        m_fileTypes.clear();
        QJsonArray typesArray = jsonObject["fileTypes"].toArray();
        for (const QJsonValue& value : typesArray) 
        {
            m_fileTypes.append(value.toString());
        }

        //// �����ļ���������
        //m_fileNames.clear();
        //QJsonArray namesArray = jsonObject["fileNames"].toArray();
        //for (const QJsonValue& value : namesArray) {
        //    m_fileNames.append(value.toString());
        //}

        //// �����ļ�·��������
        //m_filePaths.clear();
        //QJsonArray pathsArray = jsonObject["filePaths"].toArray();
        //for (const QJsonValue& value : pathsArray) {
        //    m_filePaths.append(value.toString());
        //}
    }


    void clear();
private:
    QStringList m_fileTypes;
    QStringList m_fileNames;
    QStringList m_filePaths;
};
