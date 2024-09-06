#ifndef __K_STACKWIDGET_H_
#define __K_STACKWIDGET_H_

#include <QWidget>
#include <QStackedWidget>
#include <QTableView>
#include <QTreeView>
#include <QList>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <vector>
#include <QMutex>
#include <QMenu>
#include <QEvent>  
#include <QMouseEvent>  

#include "kfilefilter.h"


class KStackWidget : public QStackedWidget
{
    Q_OBJECT
    friend class KMainWindow;
public:
    KStackWidget(QWidget *parent = nullptr);
    ~KStackWidget();

    void updateTableModel();
    void updateEasyTableModel();
    void updateLogTableModel();
    void initTable();
    void initEasyTable();
    void initLogTable();
    void initContextMenu();
    void setShowData(QList<QList<QString>> showData);
    void setActionData(QList<QList<QString>> actionData);
    void setPath(const QString path);

    void matchAction();
    bool tailMatch(QString show, QString action);
    bool fullPathMatch(QString path);

    void backPrePath();

    void creatNewFile(QList<QString> action);

    void clear();
    void saveLogToJson(const QString& filePath = "log.json");

private slots:
    void handleDoubleClick(const QModelIndex& index);
    void showContextMenu(const QPoint& pos);
    void onMonitorSelected();
    void onUnmonitorSelected();
    void onShowLogSelected();
    void filterLogTable(const QString& keyword);
    void onSaveLogButtonClicked();

private:
    QTableView* m_tableView;
    QTableView* m_easytableView;
    QTableView* m_logTableView;
    QStandardItemModel* m_tableModel;
    QStandardItemModel* m_easytableModel;
    QStandardItemModel* m_logTableModel;
    QList<QString> tableItem{
    "文件名称",
    "完整路径",
    "文件类型",
    "文件大小",
    "创建时间",
    "修改时间",
    "文件所有者"
    };

    QList<QString> easytableItem{
        "文件名称",
        "创建次数",
        "删除次数",
        "内容修改次数",
        "重命名次数",
        "最新时间",
    };
    QList<QString> logTableItem{
        "文件名",
        "行为",
        "时间",
    };


    QTreeView* m_treeView;

    QList<QList<QString>> m_showData;
    QList<QList<QString>> m_actionData;
    std::vector<int> m_index;
    
    int m_currentLevel = 0;

    QVBoxLayout* m_wLayout;

    QString m_fullPath = "";
    QString m_path = "";

    QMutex m_actionMutex;
    int m_actionIndex = 0;

    QMenu* m_contextMenu = nullptr;
    QAction* m_monitorAction = nullptr;
    QAction* m_unmonitorAction = nullptr;
    QAction* m_showLogAction = nullptr;
    int m_menuIndex;

    KFileFilter m_fileFilter;


};



#endif
