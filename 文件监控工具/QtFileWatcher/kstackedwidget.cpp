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
			// ����1����Ŀ¼������ ����2�����ļ�����Ŀ¼
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
					// �����ı���ɫΪ��ɫ
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
//	// ������е���
//	int rowCount = m_easytableModel->rowCount();
//	if (rowCount > 0) {
//		m_easytableModel->removeRows(0, rowCount);
//	}
//
//
//	// ʹ�� QMap ���洢�ļ�·�������Ӧ�ļ�����
//	QMap<QString, QList<int>> fileCounts;
//	foreach(const QList<QString> & rowData, m_actionData) {
//		// ����һ��Ԫ�أ�����ֵ���¼�����
//		int actionType = rowData[0].toInt();
//		QString filePath = rowData[1];
//
//		// ��ʼ���ļ�·���ļ������������δ����
//		if (!fileCounts.contains(filePath)) {
//			fileCounts.insert(filePath, { 0, 0, 0, 0, 0 }); // �޸Ĵ���, ��������, ɾ������
//		}
//
//		// ���ݶ������͸��¼�����
//		QList<int>& counts = fileCounts[filePath];
//		switch (actionType) {
//		case 1: // ����
//			++counts[0];
//			break;
//		case 2: // ɾ��
//			++counts[1];
//			break;
//		case 3: // �޸�
//			++counts[2];
//			break;
//		case 4: // �������������ƣ�
//		case 5: // �������������ƣ�
//			++counts[3];
//			break;
//		default:
//			// ��������ֵ
//			break;
//		}
//	}
//
//	// ���ļ�·���ͼ�������ӵ�ģ��
//	for (auto it = fileCounts.begin(); it != fileCounts.end(); ++it) {
//		m_easytableModel->insertRow(m_easytableModel->rowCount());
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 0, new QStandardItem(it.key())); // �ļ�����
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 1, new QStandardItem(QString::number(it.value()[0]))); // ��������
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 2, new QStandardItem(QString::number(it.value()[1]))); // ɾ������
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 3, new QStandardItem(QString::number(it.value()[2]))); // �޸Ĵ���
//		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 4, new QStandardItem(QString::number(it.value()[3]))); // ����������
//	}
//}

void KStackWidget::updateEasyTableModel()
{
	// ������е���
	int rowCount = m_easytableModel->rowCount();
	if (rowCount > 0) {
		m_easytableModel->removeRows(0, rowCount);
	}

	// ʹ�� QMap ���洢�ļ�·�������Ӧ�ļ����������±��ʱ��
	QMap<QString, QList<int>> fileCounts; // �洢 [��������, ɾ������, �޸Ĵ���, ����������]
	QMap<QString, QString> fileTimes; // �洢���±��ʱ��

	foreach(const QList<QString> & rowData, m_actionData) {
		int actionType = rowData[0].toInt();
		QString filePath = rowData[1];
		QString changeTime = rowData[2]; // ���±��ʱ��

		// ��ʼ���ļ�·���ļ�������ʱ�䣬�����δ����
		if (!fileCounts.contains(filePath)) {
			fileCounts.insert(filePath, { 0, 0, 0, 0 }); // ��ʼ��������
			fileTimes.insert(filePath, changeTime); // ��ʼ��ʱ��
		}

		// ���ݶ������͸��¼�����
		QList<int>& counts = fileCounts[filePath];
		switch (actionType) {
		case 1: // ����
			++counts[0];
			break;
		case 2: // ɾ��
			++counts[1];
			break;
		case 3: // �޸�
			++counts[2];
			break;
		case 4: // �������������ƣ�
		case 5: // �������������ƣ�
			++counts[3];
			break;
		default:
			// ��������ֵ
			break;
		}

		// �������±��ʱ�䣨������µ�ʱ�䣩
		if (changeTime > fileTimes[filePath]) {
			fileTimes[filePath] = changeTime;
		}
	}

	// ���ļ�·���ͼ�������ӵ�ģ��
	for (auto it = fileCounts.begin(); it != fileCounts.end(); ++it) {
		m_easytableModel->insertRow(m_easytableModel->rowCount());
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 0, new QStandardItem(it.key())); // �ļ�����
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 1, new QStandardItem(QString::number(it.value()[0]))); // ��������
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 2, new QStandardItem(QString::number(it.value()[1]))); // ɾ������
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 3, new QStandardItem(QString::number(it.value()[2]))); // �޸Ĵ���
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 4, new QStandardItem(QString::number(it.value()[3]))); // ����������
		m_easytableModel->setItem(m_easytableModel->rowCount() - 1, 5, new QStandardItem(fileTimes[it.key()])); // ���±��ʱ��
	}
}


void KStackWidget::updateLogTableModel()
{
	// ������е���
	int rowCount = m_logTableModel->rowCount();
	if (rowCount > 0) {
		m_logTableModel->removeRows(0, rowCount);
	}

	// ���±��ģ��
	for (const QList<QString>& action : m_actionData) {
		if (action.size() >= 3) { // ȷ�����㹻������
			QString actionText;

			// ���� action[0] ����Ϊ���룬ת��Ϊ����
			int actionCode = action[0].toInt();

			// ���� actionCode ��ʾ��ͬ���ı���Ϣ
			switch (actionCode) {
			case 1:
				actionText = "�ļ���Ŀ¼������";
				break;
			case 2:
				actionText = "�ļ���Ŀ¼��ɾ��";
				break;
			case 3:
				actionText = "�ļ������ݻ����Ա��޸�";
				break;
			case 4:
				actionText = "�ļ���Ŀ¼���������������ƣ�";
				break;
			case 5:
				actionText = "�ļ���Ŀ¼���������������ƣ�";
				break;
			case 6:
				actionText = "�ļ���Ŀ¼�ĸ���̫�࣬�޷�����֪ͨ������";
				break;
			case 7:
				actionText = "�ļ��ķ���Ȩ�޷����ı�";
				break;
			case 8:
				actionText = "�ļ����ر�";
				break;
			case 9:
				actionText = "�ļ�����";
				break;
			case 10:
				actionText = "�ļ������ӷ����仯";
				break;
			default:
				actionText = "δ֪��Ϊ";
				break;
			}

			m_logTableModel->insertRow(m_logTableModel->rowCount());
			m_logTableModel->setItem(m_logTableModel->rowCount() - 1, 0, new QStandardItem(action[1])); // �ļ���
			m_logTableModel->setItem(m_logTableModel->rowCount() - 1, 1, new QStandardItem(actionText)); // ��Ϊ�ı�
			m_logTableModel->setItem(m_logTableModel->rowCount() - 1, 2, new QStandardItem(action[2])); // ʱ��
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


	// ��ֹ�༭
	m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// ��ѡ��
	m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	// ѡ����ɫ
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


	// ��ֹ�༭
	m_easytableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// ��ѡ��
	m_easytableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_easytableView->setSelectionMode(QAbstractItemView::SingleSelection);
	// ѡ����ɫ
	QString styleSheet = "QTableView::item:selected { background-color: rgb(229,243,255); color: black; }";
	m_easytableView->setStyleSheet(styleSheet);

	//(void)connect(m_tableView, &QTableView::doubleClicked, this, &KStackWidget::handleDoubleClick);

	addWidget(m_easytableView);
}

//void KStackWidget::initLogTable()
//{
//	//m_logTableView = new QTableView(this);
//	//m_logTableModel = new QStandardItemModel(0, 3, this); // 3��
//
//	// ���ñ�ͷ
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
	// ���� QTableView �� QStandardItemModel
	//m_logTableView = new QTableView(this);
	//m_logTableModel = new QStandardItemModel(0, 3, this); // 3 ��


	
	// ���ñ�ͷ
	m_logTableModel->setHorizontalHeaderLabels(logTableItem);

	// ����������
	m_logTableView->setSortingEnabled(true);


	QPushButton* saveButton = new QPushButton("������־", this);
	(void)connect(saveButton, &QPushButton::clicked, this, &KStackWidget::onSaveLogButtonClicked);

	// ���� QLineEdit ��Ϊ������
	QLineEdit* searchBox = new QLineEdit(this);
	searchBox->setFixedWidth(200);  // ���ù̶����Ϊ 200 ����
	//searchBox->setStyleSheet("background: none; border: none;");

	searchBox->setPlaceholderText("����ؼ�������");

	// �����������źŵ��ۺ���
	(void)connect(searchBox, &QLineEdit::textChanged, this, &KStackWidget::filterLogTable);

	// �������������������ҳ��
	QWidget* logPage = new QWidget(this);
	//logPage->setStyleSheet("border: none;background-color:#F0F0F0;");
	//m_logTableView->setStyleSheet("border: 1px solid black;");
	//searchBox->setStyleSheet("border: 1px solid black;");
	//saveButton->setStyleSheet(
	//	"border: 1px solid black;"  // �߿�Ϊ2���ؿ�ĺ�ɫʵ��
	//	"padding: 5px;"            // �����ڱ߾�Ϊ10����
	//);
	//logPage->setStyleSheet("background: none;");
	


	// �������ֲ����С����
	QVBoxLayout* layout = new QVBoxLayout(logPage);

	// ����ˮƽ����������������
	QHBoxLayout* searchLayout = new QHBoxLayout();

	// ��ˮƽ�����������չ�� (QSpacerItem)���Ա�����������������ռ�
	searchLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

	//QPushButton* Button = new QPushButton(this);
	//Button->setGeometry(50, 50, 200, 100);
	//layout->addWidget(Button);
	//Button->setStyleSheet("background-color:#00FF00;");
	// ���������ˮƽ����
	searchLayout->addWidget(saveButton);
	searchLayout->addWidget(searchBox);


	// ��ˮƽ������ӵ�������
	layout->addLayout(searchLayout);

	// ��ӱ�񵽲���
	layout->addWidget(m_logTableView);

	// ���ò��ֵ�ҳ��
	logPage->setLayout(layout);

	// ����ģ�͵������ͼ
	m_logTableView->setModel(m_logTableModel);
	m_logTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	// ������������ͱ���ҳ����ӵ� stack widget
	addWidget(logPage);

	/*logPage->dumpObjectInfo();
	logPage->dumpObjectTree();*/
}

void KStackWidget::initContextMenu() {
	m_contextMenu = new QMenu(this);

	m_monitorAction = new QAction(tr("�ָ�����"), this);
	m_unmonitorAction = new QAction(tr("��ͣ����"), this);
	//m_showLogAction = new QAction(tr("��ʾ��־"), this);

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
	// ��ȡ���λ�õı��������
	QModelIndex index = m_tableView->indexAt(pos);

	if (index.isValid()) {
		// ��¼���������
		m_menuIndex = index.row();

		// ��ȫ��λ����ʾ�˵�
		m_contextMenu->exec(m_tableView->viewport()->mapToGlobal(pos));
	}
}

void KStackWidget::onMonitorSelected() {
	// ʵ�� Monitor ����
	QModelIndex index = m_tableView->currentIndex();
	if (index.isValid()) {
		//QString fileName = m_tableModel->data(m_tableModel->index(index.row(), 0)).toString();
		QString filePath = m_tableModel->data(m_tableModel->index(index.row(), 1)).toString();
		//QString fileType = m_tableModel->data(m_tableModel->index(index.row(), 2)).toString();

		// ��ӵ�������
		//m_fileFilter.addFilterByName(fileName);
		m_fileFilter.removeFilterByPath(filePath);
		//m_fileFilter.addFilterByType(fileType);

	}
}

void KStackWidget::onUnmonitorSelected() {
	// ʵ�� Unmonitor ����

	QModelIndex index = m_tableView->currentIndex();
	if (index.isValid()) {
		//QString fileName = m_tableModel->data(m_tableModel->index(index.row(), 0)).toString();
		QString filePath = m_tableModel->data(m_tableModel->index(index.row(), 1)).toString();
		//QString fileType = m_tableModel->data(m_tableModel->index(index.row(), 2)).toString();

		// ��ӵ�������
		//m_fileFilter.addFilterByName(fileName);
		m_fileFilter.addFilterByPath(filePath);
		//m_fileFilter.addFilterByType(fileType);

	}
}

void KStackWidget::onShowLogSelected() {
	// ʵ�� Show Log ����
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
	// �������action��ɸѡ
	QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
	for (auto action : actionData) {
		//QString fileName = action[0];
		QString filePath = action[1];
		QString path = m_path + "/" + filePath;
		qDebug() << "KStackWidget::setActionData path:" << path;
		//QString fileType = action[2];

		// ��ȡ�ļ���׺����������ļ�����Ϊ��ֵ
		QFileInfo fileInfo(path);
		QString type = fileInfo.isDir() ? "" : fileInfo.suffix();

		if (!m_fileFilter.isFiltered(path, type)) {
			action.append(currentTime); // ��ӵ�ǰʱ��
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

	// ȥ��ĩβ��б��
	if (processedPath.endsWith('/')) {
		processedPath.chop(1); // ȥ�����һ���ַ�
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
	// ��ȡ˫����������
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
//		qWarning() << "�޷����ļ�����д��: " << path;
//		return;
//	}
//
//	QJsonArray jsonArray;
//
//	for (const QList<QString>& action : m_actionData) {
//		if (action.size() >= 3) {
//			QJsonObject jsonObj;
//			jsonObj["�ļ���"] = action[1];
//			jsonObj["��Ϊ"] = action[0];
//			jsonObj["ʱ��"] = action[2];
//			jsonArray.append(jsonObj);
//		}
//	}
//
//	QJsonDocument doc(jsonArray);
//	file.write(doc.toJson());
//
//	file.close();
//	qDebug() << "��־�ѱ��浽: " << path;
//}

void KStackWidget::saveLogToJson(const QString& filePath)
{
	QString path = filePath.isEmpty() ? "log.json" : filePath;

	QFile file(path);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qWarning() << "�޷����ļ�����д��: " << path;
		return;
	}

	QJsonArray jsonArray;

	for (const QList<QString>& action : m_actionData) {
		if (action.size() >= 3) {
			QJsonObject jsonObj;
			jsonObj["�ļ���"] = action[1];

			// ����Ϊ����ת��Ϊ��Ϊ�ı�
			QString actionText;
			int actionCode = action[0].toInt();

			switch (actionCode) {
			case 1:
				actionText = "�ļ���Ŀ¼������";
				break;
			case 2:
				actionText = "�ļ���Ŀ¼��ɾ��";
				break;
			case 3:
				actionText = "�ļ������ݻ����Ա��޸�";
				break;
			case 4:
				actionText = "�ļ���Ŀ¼���������������ƣ�";
				break;
			case 5:
				actionText = "�ļ���Ŀ¼���������������ƣ�";
				break;
			case 6:
				actionText = "�ļ���Ŀ¼�ĸ���̫�࣬�޷�����֪ͨ������";
				break;
			case 7:
				actionText = "�ļ��ķ���Ȩ�޷����ı�";
				break;
			case 8:
				actionText = "�ļ����ر�";
				break;
			case 9:
				actionText = "�ļ�����";
				break;
			case 10:
				actionText = "�ļ������ӷ����仯";
				break;
			default:
				actionText = "δ֪��Ϊ";
				break;
			}

			jsonObj["��Ϊ"] = actionText;
			jsonObj["ʱ��"] = action[2];
			jsonArray.append(jsonObj);
		}
	}

	QJsonDocument doc(jsonArray);
	file.write(doc.toJson());

	file.close();
	qDebug() << "��־�ѱ��浽: " << path;
}


void KStackWidget::onSaveLogButtonClicked()
{
	QString filePath = QFileDialog::getSaveFileName(this, "������־", "", "JSON �ļ� (*.json);;�����ļ� (*.*)");

	// ����û�ȡ���ļ��Ի���ʹ��Ĭ��·��
	if (filePath.isEmpty())
	{
		filePath = "log.json";
	}

	saveLogToJson(filePath);
}
