#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QWidget>
#include <QListWidget>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class KFilterSettingsDialog : public QDialog 
{
    Q_OBJECT

public:
    //KFilterSettingsDialog(QWidget* parent = nullptr) : QDialog(parent) 
    // {
    //    // ��ʼ���������ý���
    //    QVBoxLayout* layout = new QVBoxLayout(this);

    //    QLabel* label = new QLabel("�����ļ��б�", this);
    //    m_typeInput = new QLineEdit(this);
    //    layout->addWidget(label);
    //    layout->addWidget(m_typeInput);

    //    QPushButton* addButton = new QPushButton("���", this);
    //    layout->addWidget(addButton);

    //    // ��� QListWidget ����ʾ�����б�
    //    m_ignoreListWidget = new QListWidget(this);
    //    layout->addWidget(m_ignoreListWidget);

    //    QPushButton* removeButton = new QPushButton("ɾ��", this);
    //    layout->addWidget(removeButton);

    //    QPushButton* saveButton = new QPushButton("��������", this);
    //    layout->addWidget(saveButton);


    //    connect(addButton, &QPushButton::clicked, this, &KFilterSettingsDialog::addFilterType);
    //    connect(removeButton, &QPushButton::clicked, this, &KFilterSettingsDialog::removeSelectedFilter);
    //    connect(saveButton, &QPushButton::clicked, this, &KFilterSettingsDialog::saveConfiguration);

    //    loadFromFile("filter_config.json");

    //}

    explicit KFilterSettingsDialog(QWidget* parent = nullptr) : QDialog(parent)
    {
        // ���ù̶���С
        setFixedSize(400, 300); // Adjust size as needed

        // ���ñ�����ɫΪǳ��ɫ
        QPalette palette;
        palette.setColor(QPalette::Background, QColor(240, 240, 240)); // Light gray
        setPalette(palette);
        setAutoFillBackground(true);

        // ��ʼ���������ý���
        QVBoxLayout* layout = new QVBoxLayout(this);

        // ����������ʽ
        QFont font("Arial", 10); // Use your preferred font and size
        setFont(font);

        QLabel* label = new QLabel("�����ļ��б�", this);
        layout->addWidget(label);

        m_typeInput = new QLineEdit(this);
        layout->addWidget(m_typeInput);

        // ������ť��������ʽ
        QPushButton* addButton = new QPushButton("���", this);
        QPushButton* removeButton = new QPushButton("ɾ��", this);
        QPushButton* saveButton = new QPushButton("��������", this);

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
        addButton->setStyleSheet(buttonStyle);
        removeButton->setStyleSheet(buttonStyle);
        saveButton->setStyleSheet(buttonStyle);

        // ʹ�� QHBoxLayout ���ư�ť����
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(removeButton);
        buttonLayout->addWidget(saveButton);

        // ��ť�������Ϊ�̶�ֵ������ 100 ���أ�
        const int buttonWidth = 100;
        addButton->setFixedWidth(buttonWidth);
        removeButton->setFixedWidth(buttonWidth);
        saveButton->setFixedWidth(buttonWidth);

        // ����ť������ӵ�������
        layout->addLayout(buttonLayout);

        // ��� QListWidget ����ʾ�����б�
        m_ignoreListWidget = new QListWidget(this);
        layout->addWidget(m_ignoreListWidget);

        // �����źźͲ�
        connect(addButton, &QPushButton::clicked, this, &KFilterSettingsDialog::addFilterType);
        connect(removeButton, &QPushButton::clicked, this, &KFilterSettingsDialog::removeSelectedFilter);
        connect(saveButton, &QPushButton::clicked, this, &KFilterSettingsDialog::saveConfiguration);

        loadFromFile("filter_config.json");
    }


    void setFilterTypes(const QStringList& types) 
    {
        m_ignoreListWidget->clear();
        m_ignoreListWidget->addItems(types);
    }

    QStringList getFilterTypes() const 
    {
        QStringList types;
        for (int i = 0; i < m_ignoreListWidget->count(); ++i) 
        {
            types << m_ignoreListWidget->item(i)->text();
        }
        return types;
    }

    void KFilterSettingsDialog::loadFromFile(const QString& filePath) 
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
        QJsonArray typesArray = jsonObject["fileTypes"].toArray();
        for (const QJsonValue& value : typesArray) 
        {
            QString type = value.toString().trimmed();
            if (!type.isEmpty()) 
            {
                QList<QListWidgetItem*> foundItems = m_ignoreListWidget->findItems(type, Qt::MatchExactly);
                if (foundItems.isEmpty()) 
                {
                    m_ignoreListWidget->addItem(type);
                }
            }
        }
    }


signals:
    void filterTypeAdded(const QString& type);
    void filterTypeRemoved(const QString& type);
    void saveConfiguration();



private slots:
    void addFilterType() 
    {
        QString type = m_typeInput->text().trimmed();
        qDebug() << "Input type:" << type;

        QList<QListWidgetItem*> foundItems = m_ignoreListWidget->findItems(type, Qt::MatchExactly);
        if (foundItems.isEmpty()) 
        {
            qDebug() << "No items found for type:" << type;
        }
        else 
        {
            qDebug() << "Found items for type:" << type;
        }

        if (!type.isEmpty() && foundItems.isEmpty()) 
        {
            m_ignoreListWidget->addItem(type);
            m_typeInput->clear();
            emit filterTypeAdded(type);
        }
    }

    void removeSelectedFilter() 
    {
        QList<QListWidgetItem*> selectedItems = m_ignoreListWidget->selectedItems();
        for (QListWidgetItem* item : selectedItems) 
        {
            QString type = item->text();
            delete m_ignoreListWidget->takeItem(m_ignoreListWidget->row(item));
            emit filterTypeRemoved(type);
        }
    }

private:
    QLineEdit* m_typeInput;
    QListWidget* m_ignoreListWidget;
};
