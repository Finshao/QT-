#pragma execution_character_set("utf-8")

#include <QStandardItemModel>
#include <QList>
#include <QString>
#include <QHeaderView>
#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QLineEdit>
#include <QFileDialog>
#include <QPushButton>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "kstackedwidget.h"

KStackWidget::KStackWidget(QWidget* parent) :
	QStackedWidget(parent)
	, m_tableView(Q_NULLPTR)
	, m_tableModel(Q_NULLPTR)
	, m_treeView(Q_NULLPTR)
	, m_easytableView(Q_NULLPTR)
	, m_easytableModel(Q_NULLPTR)
	, m_logTableView(Q_NULLPTR)
	, m_logTableModel(Q_NULLPTR)
{

	m_fileFilter.loadFromFile("filter_config.json");

	m_treeView = new QTreeView(this);
	m_tableView = new QTableView(this);
	m_easytableView = new QTableView(this);
	m_logTableView = new QTableView(this);

	m_tableModel = new QStandardItemModel(0, tableItem.size() - 1, this);
	m_easytableModel = new QStandardItemModel(0, easytableItem.size(), this);
	m_logTableModel = new QStandardItemModel(0, logTableItem.size(), this);
	//for (int i = 0; i < 1000; i++)
	//{
	//	m_index.append(-1);
	//}

	updateTableModel();
	updateEasyTableModel();
	updateLogTableModel();

	initTable();
	initEasyTable();
	initLogTable();
	initContextMenu();

	setCurrentWidget(m_logTableView);
}

KStackWidget::~KStackWidget()
{
}



void KStackWidget::updateTableModel()
{
	int rowCount = m_tableModel->rowCount();
	if (rowCount > 0) {
		m_tableModel->removeRows(0, rowCount);
	}

	int row = 0;
	int indexRow = 0;
		foreach(const QList<QString> & rowData, m_showData) {
			int column = 0;
			// 条件1处理目录级数， 条件2处理文件所属目录
			if (rowData[7].toInt() != m_currentLevel || !fullPathMatch(rowData[1]))
			{
				//qDebug() << m_showData[row][1] << "," << m_showData[row][7];
				++indexRow;
				continue;
			}
			foreach(const QString & data, rowData) {
				if (column == 7 || column == 6) continue;


				QStandardItem* item = new QStandardItem(data);

				//qDebug() << "++" << m_index[column];
				if (m_index[indexRow] != -1) {
					// 设置文本颜色为蓝色
					QColor color(42, 193, 213);
					item->setData(QBrush(color), Qt::TextColorRole);
				}

				m_tableModel->setItem(row, column, item);

				++column;
			}
			++row;
			++indexRow;
		}
}

//void KStackWidget::updateEasyTableModel()
//{
//	// 清除现有的行
//	int rowCount = m_easytableModel->rowCount();
//	if (rowCount > 0) {
//		m_easytableModel->removeRows(0, rowCount);
//	}
//
//
//	// 使用 QMap 来存储文件路径及其对应的计数器
//	QMap<QString, QList<int>> fileCounts;
//	foreach(const QList<QString> & rowData, m_actionData) {
//		// 检查第一个元素，根据值更新计数器
//		int actionType = rowData[0].toInt();
//		QString filePath = rowData[1];
//
//		// 初始化文件路径的计数器，如果尚未存在
//		if (!fileCounts.contains(filePath)) {
//			fileCounts.insert(filePath, { 0, 0, 0, 0, 0 }); // 修改次数, 创建次数, 删除次数
//		}
//
//		// 根据动作类型更新计数器
//		QList<int>& counts = fileCounts[filePath];
//		switch (actionType) {
//		case 1: // 创建
//			++counts[0];
//			break;
//		case 2: // 删除
//			++counts[1];
//			break;
//		case 3: // 修改
//			++counts[2];
//			break;
//		case 4: // 重命名（新名称）
//		case 5: // 重命名（旧名称）
//			++counts[3];
//			break;
//		default:
//			// 忽略其他值
//			break;
//		}
//	}
//
//	// 将文件路径和计数器添加到模型
//	for (auto it = fileCounts.begin(); it != fileCounts.end(); ++it) {
//		m_easytableModel->insertRow(m_easytableModel->rowCount());
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 0, new QStandardItem(it.key())); // 文件名称
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 1, new QStandardItem(QString::number(it.value()[0]))); // 创建次数
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 2, new QStandardItem(QString::number(it.value()[1]))); // 删除次数
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 3, new QStandardItem(QString::number(it.value()[2]))); // 修改次数
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 4, new QStandardItem(QString::number(it.value()[3]))); // 重命名次数
//	}
//}

void KStackWidget::updateEasyTableModel()
{
	// 清除现有的行
	int rowCount = m_easytableModel->rowCount();
	if (rowCount > 0) {
		m_easytableModel->removeRows(0, rowCount);
	}

	// 使用 QMap 来存储文件路径及其对应的计数器和最新变更时间
	QMap<QString, QList<int>> fileCounts; // 存储 [创建次数, 删除次数, 修改次数, 重命名次数]
	QMap<QString, QString> fileTimes; // 存储最新变更时间

	foreach(const QList<QString> & rowData, m_actionData) {
		int actionType = rowData[0].toInt();
		QString filePath = rowData[1];
		QString changeTime = rowData[2]; // 最新变更时间

		// 初始化文件路径的计数器和时间，如果尚未存在
		if (!fileCounts.contains(filePath)) {
			fileCounts.insert(filePath, { 0, 0, 0, 0 }); // 初始化计数器
			fileTimes.insert(filePath, changeTime); // 初始化时间
		}

		// 根据动作类型更新计数器
		QList<int>& counts = fileCounts[filePath];
		switch (actionType) {
		case 1: // 创建
			++counts[0];
			break;
		case 2: // 删除
			++counts[1];
			break;
		case 3: // 修改
			++counts[2];
			break;
		case 4: // 重命名（新名称）
		case 5: // 重命名（旧名称）
			++counts[3];
			break;
		default:
			// 忽略其他值
			break;
		}

		// 更新最新变更时间（如果有新的时间）
		if (changeTime > fileTimes[filePath]) {
			fileTimes[filePath] = changeTime;
		}
	}

	// 将文件路径和计数器添加到模型
	for (auto it = fileCounts.begin(); it != fileCounts.end(); ++it) {
		m_easytableModel->insertRow(m_easytableModel->rowCount());
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 0, new QStandardItem(it.key())); // 文件名称
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 1, new QStandardItem(QString::number(it.value()[0]))); // 创建次数
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 2, new QStandardItem(QString::number(it.value()[1]))); // 删除次数
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 3, new QStandardItem(QString::number(it.value()[2]))); // 修改次数
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 4, new QStandardItem(QString::number(it.value()[3]))); // 重命名次数
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 5, new QStandardItem(fileTimes[it.key()])); // 最新变更时间
	}
}


void KStackWidget::updateLogTableModel()
{
	// 清除现有的行
	int rowCount = m_logTableModel->rowCount();
	if (rowCount > 0) {
		m_logTableModel->removeRows(0, rowCount);
	}

	// 更新表格模型
	for (const QList<QString>& action : m_actionData) {
		if (action.size() >= 3) { // 确保有足够的数据
			QString actionText;

			// 假设 action[0] 是行为代码，转换为整数
			int actionCode = action[0].toInt();

			// 根据 actionCode 显示不同的文本信息
			switch (actionCode) {
			case 1:
				actionText = "文件或目录被创建";
				break;
			case 2:
				actionText = "文件或目录被删除";
				break;
			case 3:
				actionText = "文件的内容或属性被修改";
				break;
			case 4:
				actionText = "文件或目录被重命名（新名称）";
				break;
			case 5:
				actionText = "文件或目录被重命名（旧名称）";
				break;
			case 6:
				actionText = "文件或目录的更改太多，无法放入通知缓冲区";
				break;
			case 7:
				actionText = "文件的访问权限发生改变";
				break;
			case 8:
				actionText = "文件被关闭";
				break;
			case 9:
				actionText = "文件被打开";
				break;
			case 10:
				actionText = "文件的链接发生变化";
				break;
			default:
				actionText = "未知行为";
				break;
			}

			m_logTableModel->insertRow(m_logTableModel->rowCount());
			m_logTableModel->setItem(m_logTableModel->rowCount() - 1, 0, new QStandardItem(action[1])); // 文件名
			m_logTableModel->setItem(m_logTableModel->rowCount() - 1, 1, new QStandardItem(actionText)); // 行为文本
			m_logTableModel->setItem(m_logTableModel->rowCount() - 1, 2, new QStandardItem(action[2])); // 时间
		}
	}
}


void KStackWidget::initTable()
{

	for (int i = 0; i < tableItem.size() - 1; ++i) {
		m_tableModel->setHeaderData(i, Qt::Horizontal, tableItem[i]);
	}

	m_tableView->setModel(m_tableModel);
	m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//m_tableView->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);


	// 禁止编辑
	m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 行选中
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	// 选中颜色
	QString styleSheet = "QTableView::item:selected { background-color: rgb(229,243,255); color: black; }";
	m_tableView->setStyleSheet(styleSheet);

	(void)connect(m_tableView, &QTableView::doubleClicked, this, &KStackWidget::handleDoubleClick);


	

	addWidget(m_tableView);
}

void KStackWidget::initEasyTable()
{

	qDebug() << "initEasyTable";
	for (int i = 0; i < easytableItem.size(); ++i) {
		m_easytableModel->setHeaderData(i, Qt::Horizontal, easytableItem[i]);
	}

	m_easytableView->setModel(m_easytableModel);
	m_easytableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//m_tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//m_tableView->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);


	// 禁止编辑
	m_easytableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 行选中
	m_easytableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_easytableView->setSelectionMode(QAbstractItemView::SingleSelection);
	// 选中颜色
	QString styleSheet = "QTableView::item:selected { background-color: rgb(229,243,255); color: black; }";
	m_easytableView->setStyleSheet(styleSheet);

	//(void)connect(m_tableView, &QTableView::doubleClicked, this, &KStackWidget::handleDoubleClick);

	addWidget(m_easytableView);
}

//void KStackWidget::initLogTable()
//{
//	//m_logTableView = new QTableView(this);
//	//m_logTableModel = new QStandardItemModel(0, 3, this); // 3列
//
//	// 设置表头
//	m_logTableModel->setHorizontalHeaderLabels(logTableItem);
//
//	m_logTableView->setSortingEnabled(true);
//	m_logTableView->setModel(m_logTableModel);
//	m_logTableView->horizontalHeader()->setStretchLastSection(true);
//
//	addWidget(m_logTableView);
//}

void KStackWidget::initLogTable()
{
	// 创建 QTableView 和 QStandardItemModel
	//m_logTableView = new QTableView(this);
	//m_logTableModel = new QStandardItemModel(0, 3, this); // 3 列


	
	// 设置表头
	m_logTableModel->setHorizontalHeaderLabels(logTableItem);

	// 启用排序功能
	m_logTableView->setSortingEnabled(true);


	QPushButton* saveButton = new QPushButton("保存日志", this);
	(void)connect(saveButton, &QPushButton::clicked, this, &KStackWidget::onSaveLogButtonClicked);

	// 创建 QLineEdit 作为搜索框
	QLineEdit* searchBox = new QLineEdit(this);
	searchBox->setFixedWidth(200);  // 设置固定宽度为 200 像素
	//searchBox->setStyleSheet("background: none; border: none;");

	searchBox->setPlaceholderText("输入关键词搜索");

	// 连接输入框的信号到槽函数
	(void)connect(searchBox, &QLineEdit::textChanged, this, &KStackWidget::filterLogTable);

	// 创建包含表格和搜索框的页面
	QWidget* logPage = new QWidget(this);
	//logPage->setStyleSheet("border: none;background-color:#F0F0F0;");
	//m_logTableView->setStyleSheet("border: 1px solid black;");
	//searchBox->setStyleSheet("border: 1px solid black;");
	//saveButton->setStyleSheet(
	//	"border: 1px solid black;"  // 边框为2像素宽的黑色实线
	//	"padding: 5px;"            // 设置内边距为10像素
	//);
	//logPage->setStyleSheet("background: none;");
	


	// 创建布局并添加小部件
	QVBoxLayout* layout = new QVBoxLayout(logPage);

	// 创建水平布局来放置搜索框
	QHBoxLayout* searchLayout = new QHBoxLayout();

	// 在水平布局中添加伸展器 (QSpacerItem)，以便在搜索框左侧留出空间
	searchLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	//QPushButton* Button = new QPushButton(this);
	//Button->setGeometry(50, 50, 200, 100);
	//layout->addWidget(Button);
	//Button->setStyleSheet("background-color:#00FF00;");
	// 添加搜索框到水平布局
	searchLayout->addWidget(saveButton);
	searchLayout->addWidget(searchBox);


	// 将水平布局添加到主布局
	layout->addLayout(searchLayout);

	// 添加表格到布局
	layout->addWidget(m_logTableView);

	// 设置布局到页面
	logPage->setLayout(layout);

	// 设置模型到表格视图
	m_logTableView->setModel(m_logTableModel);
	m_logTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	// 将包含搜索框和表格的页面添加到 stack widget
	addWidget(logPage);

	/*logPage->dumpObjectInfo();
	logPage->dumpObjectTree();*/
}

void KStackWidget::initContextMenu() {
	m_contextMenu = new QMenu(this);

	m_monitorAction = new QAction(tr("恢复监听"), this);
	m_unmonitorAction = new QAction(tr("暂停监听"), this);
	//m_showLogAction = new QAction(tr("显示日志"), this);

	connect(m_monitorAction, &QAction::triggered, this, &KStackWidget::onMonitorSelected);
	connect(m_unmonitorAction, &QAction::triggered, this, &KStackWidget::onUnmonitorSelected);
	//connect(m_showLogAction, &QAction::triggered, this, &KStackWidget::onShowLogSelected);

	m_contextMenu->addAction(m_monitorAction);
	m_contextMenu->addAction(m_unmonitorAction);
	//m_contextMenu->addAction(m_showLogAction);

	m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_tableView, &QTableView::customContextMenuRequested, this, &KStackWidget::showContextMenu);
}
void KStackWidget::showContextMenu(const QPoint& pos) {
	// 获取点击位置的表格项索引
	QModelIndex index = m_tableView->indexAt(pos);

	if (index.isValid()) {
		// 记录点击的索引
		m_menuIndex = index.row();

		// 在全局位置显示菜单
		m_contextMenu->exec(m_tableView->viewport()->mapToGlobal(pos));
	}
}

void KStackWidget::onMonitorSelected() {
	// 实现 Monitor 功能
	QModelIndex index = m_tableView->currentIndex();
	if (index.isValid()) {
		//QString fileName = m_tableModel->data(m_tableModel->index(index.row(), 0)).toString();
		QString filePath = m_tableModel->data(m_tableModel->index(index.row(), 1)).toString();
		//QString fileType = m_tableModel->data(m_tableModel->index(index.row(), 2)).toString();

		// 添加到过滤器
		//m_fileFilter.addFilterByName(fileName);
		m_fileFilter.removeFilterByPath(filePath);
		//m_fileFilter.addFilterByType(fileType);

	}
}

void KStackWidget::onUnmonitorSelected() {
	// 实现 Unmonitor 功能

	QModelIndex index = m_tableView->currentIndex();
	if (index.isValid()) {
		//QString fileName = m_tableModel->data(m_tableModel->index(index.row(), 0)).toString();
		QString filePath = m_tableModel->data(m_tableModel->index(index.row(), 1)).toString();
		//QString fileType = m_tableModel->data(m_tableModel->index(index.row(), 2)).toString();

		// 添加到过滤器
		//m_fileFilter.addFilterByName(fileName);
		m_fileFilter.addFilterByPath(filePath);
		//m_fileFilter.addFilterByType(fileType);

	}
}

void KStackWidget::onShowLogSelected() {
	// 实现 Show Log 功能
	qDebug() << "Show Log selected at row:" << m_menuIndex;
}



void KStackWidget::setShowData(QList<QList<QString>> showData)
{
	for (auto a : showData)
	{
		for (auto b : a)
		{
			qDebug() << b;
		}
	}
	m_showData = showData;
	m_index.resize(m_showData.size(), -1);
	updateTableModel();
	updateEasyTableModel();
	updateLogTableModel();
}

void KStackWidget::setActionData(QList<QList<QString>> actionData)
{
	m_actionMutex.lock();
	//m_actionData = actionData;
	// 这里加上action的筛选
	QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
	for (auto action : actionData) {
		//QString fileName = action[0];
		QString filePath = action[1];
		QString path = m_path + "/" + filePath;
		qDebug() << "KStackWidget::setActionData path:" << path;
		//QString fileType = action[2];

		// 获取文件后缀名，如果是文件夹则为空值
		QFileInfo fileInfo(path);
		QString type = fileInfo.isDir() ? "" : fileInfo.suffix();

		if (!m_fileFilter.isFiltered(path, type)) {
			action.append(currentTime); // 添加当前时间
			m_actionData.append(action);
		}
		
	}
	m_actionMutex.unlock();
	/*for (auto a : m_actionData)
	{
		for (auto b : a)
		{
			qDebug() << b;
		}
	}*/
	

	matchAction();
	updateTableModel();
	updateEasyTableModel();
	updateLogTableModel();
}

void KStackWidget::setPath(const QString path)
{
	QString processedPath = path;

	// 去掉末尾的斜杠
	if (processedPath.endsWith('/')) {
		processedPath.chop(1); // 去掉最后一个字符
	}

	m_fullPath = processedPath;
	m_path = processedPath;
}


void KStackWidget::matchAction()
{

	m_actionMutex.lock();
	m_index.resize(m_showData.size(), -1);

	for (int i = m_actionIndex; i < m_actionData.size(); ++i)
	{
		if (m_actionData[i][0] == "1")
		{
			creatNewFile(m_actionData[i]);
			m_index.push_back(1);
			continue;
		}
		for (int j = 0; j < m_showData.size(); ++j)
		{
			if (tailMatch(m_showData[j][1], m_actionData[i][1]))
			{
				m_index[j] = m_actionData[i][0].toInt();
				//qDebug() << "++++++++++++++++++++++++++++++";
			}
		}
		m_actionIndex++;
	}
	m_actionMutex.unlock();
}

bool KStackWidget::tailMatch(QString show, QString action)
{

	QString path = m_path + "/" + action;
	//qDebug() << m_path << "," << action << "," << show;
	if (path == show)
	{
		//qDebug() << "true" << "------------";
		return true;
	}
	else return false;
	
}

bool KStackWidget::fullPathMatch(QString path)
{
	//qDebug() << show << "," << action;
	if (path.size() < m_fullPath.size())
	{
		qDebug() << "fullPathMatch error, path < m_fullPath";
		return false;
	}
	else
	{
		for (int i = 0; i < m_fullPath.size(); i++)
		{
			if (path[i] != m_fullPath[i])
			{
				//qDebug() << path << "." << m_fullPath;
				return false;
			}
		}
		//qDebug() << "true";
		return true;
	}
}

void KStackWidget::backPrePath()
{
	if (m_currentLevel <= 0 || m_fullPath == "")
	{
		qDebug() << "backPrePath error, cannot back";
	}
	else
	{

		auto it = m_fullPath.end() - 1;
		while (*it == "/") it--;
		while (*it != "/") it--;

		if (it != m_fullPath.begin())
		{
			int index = std::distance(m_fullPath.begin(), it);
			m_fullPath.truncate(index);
		}


		//Debug() << m_fullPath;
		m_currentLevel--;
		updateTableModel();
	}
}

void KStackWidget::creatNewFile(QList<QString> action)
{
	int i = 0;
	for (auto a : action[1])
	{
		if (a == "/") i++;
	}
	QList<QString> data;
	data << action[1]
		<< m_path + "/" + action[1]
		<< ""
		<< "0"
		<< QDateTime::currentDateTime().toString(Qt::ISODate)
		<< QDateTime::currentDateTime().toString(Qt::ISODate)
		<< "0"
		<< QString::number(i);
	m_showData.append(data);
}

void KStackWidget::clear()
{
	m_actionData.clear();
	m_actionIndex = 0;
	m_index.clear();
	//m_fileFilter.clear();
}



void KStackWidget::handleDoubleClick(const QModelIndex& index)
{
	// 获取双击的列索引
	int row = index.row();
	QVariant data = index.data();

	QModelIndex columnIndex2 = index.siblingAtColumn(2);
	QVariant style = columnIndex2.data();

	if (style.toString() != "Directory") return;

	QModelIndex columnIndex1 = index.siblingAtColumn(1);
	QVariant fullPath = columnIndex1.data();

	m_fullPath = fullPath.toString();

	m_currentLevel++;

	updateTableModel();
}

void KStackWidget::filterLogTable(const QString& keyword)
{
	for (int i = 0; i < m_logTableModel->rowCount(); ++i) {
		bool match = false;
		for (int j = 0; j < m_logTableModel->columnCount(); ++j) {
			QModelIndex index = m_logTableModel->index(i, j);
			QString data = m_logTableModel->data(index).toString();
			if (data.contains(keyword, Qt::CaseInsensitive)) {
				match = true;
				break;
			}
		}
		m_logTableView->setRowHidden(i, !match);
	}
}

//void KStackWidget::saveLogToJson(const QString& filePath)
//{
//	QString path = filePath.isEmpty() ? "log.json" : filePath;
//
//	QFile file(path);
//	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
//		qWarning() << "无法打开文件用于写入: " << path;
//		return;
//	}
//
//	QJsonArray jsonArray;
//
//	for (const QList<QString>& action : m_actionData) {
//		if (action.size() >= 3) {
//			QJsonObject jsonObj;
//			jsonObj["文件名"] = action[1];
//			jsonObj["行为"] = action[0];
//			jsonObj["时间"] = action[2];
//			jsonArray.append(jsonObj);
//		}
//	}
//
//	QJsonDocument doc(jsonArray);
//	file.write(doc.toJson());
//
//	file.close();
//	qDebug() << "日志已保存到: " << path;
//}

void KStackWidget::saveLogToJson(const QString& filePath)
{
	QString path = filePath.isEmpty() ? "log.json" : filePath;

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qWarning() << "无法打开文件用于写入: " << path;
		return;
	}

	QJsonArray jsonArray;

	for (const QList<QString>& action : m_actionData) {
		if (action.size() >= 3) {
			QJsonObject jsonObj;
			jsonObj["文件名"] = action[1];

			// 将行为代码转换为行为文本
			QString actionText;
			int actionCode = action[0].toInt();

			switch (actionCode) {
			case 1:
				actionText = "文件或目录被创建";
				break;
			case 2:
				actionText = "文件或目录被删除";
				break;
			case 3:
				actionText = "文件的内容或属性被修改";
				break;
			case 4:
				actionText = "文件或目录被重命名（新名称）";
				break;
			case 5:
				actionText = "文件或目录被重命名（旧名称）";
				break;
			case 6:
				actionText = "文件或目录的更改太多，无法放入通知缓冲区";
				break;
			case 7:
				actionText = "文件的访问权限发生改变";
				break;
			case 8:
				actionText = "文件被关闭";
				break;
			case 9:
				actionText = "文件被打开";
				break;
			case 10:
				actionText = "文件的链接发生变化";
				break;
			default:
				actionText = "未知行为";
				break;
			}

			jsonObj["行为"] = actionText;
			jsonObj["时间"] = action[2];
			jsonArray.append(jsonObj);
		}
	}

	QJsonDocument doc(jsonArray);
	file.write(doc.toJson());

	file.close();
	qDebug() << "日志已保存到: " << path;
}


void KStackWidget::onSaveLogButtonClicked()
{
	QString filePath = QFileDialog::getSaveFileName(this, "保存日志", "", "JSON 文件 (*.json);;所有文件 (*.*)");

	// 如果用户取消文件对话框，使用默认路径
	if (filePath.isEmpty())
	{
		filePath = "log.json";
	}

	saveLogToJson(filePath);
}
