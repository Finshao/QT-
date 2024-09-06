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
    //    // 初始化过滤设置界面
    //    QVBoxLayout* layout = new QVBoxLayout(this);

    //    QLabel* label = new QLabel("忽略文件列表", this);
    //    m_typeInput = new QLineEdit(this);
    //    layout->addWidget(label);
    //    layout->addWidget(m_typeInput);

    //    QPushButton* addButton = new QPushButton("添加", this);
    //    layout->addWidget(addButton);

    //    // 添加 QListWidget 来显示忽略列表
    //    m_ignoreListWidget = new QListWidget(this);
    //    layout->addWidget(m_ignoreListWidget);

    //    QPushButton* removeButton = new QPushButton("删除", this);
    //    layout->addWidget(removeButton);

    //    QPushButton* saveButton = new QPushButton("保存配置", this);
    //    layout->addWidget(saveButton);


    //    connect(addButton, &QPushButton::clicked, this, &KFilterSettingsDialog::addFilterType);
    //    connect(removeButton, &QPushButton::clicked, this, &KFilterSettingsDialog::removeSelectedFilter);
    //    connect(saveButton, &QPushButton::clicked, this, &KFilterSettingsDialog::saveConfiguration);

    //    loadFromFile("filter_config.json");

    //}

    explicit KFilterSettingsDialog(QWidget* parent = nullptr) : QDialog(parent)
    {
        // 设置固定大小
        setFixedSize(400, 300); // Adjust size as needed

        // 设置背景颜色为浅灰色
        QPalette palette;
        palette.setColor(QPalette::Background, QColor(240, 240, 240)); // Light gray
        setPalette(palette);
        setAutoFillBackground(true);

        // 初始化过滤设置界面
        QVBoxLayout* layout = new QVBoxLayout(this);

        // 设置字体样式
        QFont font("Arial", 10); // Use your preferred font and size
        setFont(font);

        QLabel* label = new QLabel("忽略文件列表", this);
        layout->addWidget(label);

        m_typeInput = new QLineEdit(this);
        layout->addWidget(m_typeInput);

        // 创建按钮并设置样式
        QPushButton* addButton = new QPushButton("添加", this);
        QPushButton* removeButton = new QPushButton("删除", this);
        QPushButton* saveButton = new QPushButton("保存配置", this);

        // 按钮样式
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

        // 使用 QHBoxLayout 控制按钮对齐
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(removeButton);
        buttonLayout->addWidget(saveButton);

        // 按钮宽度设置为固定值（例如 100 像素）
        const int buttonWidth = 100;
        addButton->setFixedWidth(buttonWidth);
        removeButton->setFixedWidth(buttonWidth);
        saveButton->setFixedWidth(buttonWidth);

        // 将按钮布局添加到主布局
        layout->addLayout(buttonLayout);

        // 添加 QListWidget 来显示忽略列表
        m_ignoreListWidget = new QListWidget(this);
        layout->addWidget(m_ignoreListWidget);

        // 连接信号和槽
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

        // 加载文件类型过滤器
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
