#pragma execution_character_set("utf-8")
#include "kdownbox.h"
#include <QRegExpValidator>

KDownBox::KDownBox(const QString &title, QWidget *parent)
	: KParamBox(title,parent)
	, m_pComboBox(Q_NULLPTR)
{
	m_pComboBox = new QComboBox(this);
	m_pComboBox->addItem(QIcon(":/icons/1.svg"), "实线");
	//m_pComboBox->addItem("实线");
	m_pComboBox->addItem(QIcon(":/icons/2.svg"), "虚线");
	m_pComboBox->addItem(QIcon(":/icons/3.svg"), "点线");
	m_pComboBox->addItem(QIcon(":/icons/2.svg"), "点划线");


	m_pComboBox->setStyleSheet("QComboBox { font-size: 8pt; } QComboBox QAbstractItemView { font-size: 8pt; }");

	m_pVLayout->addWidget(m_pComboBox);// 加入到布局中 m_pVLayout 设置在父类中


}

KDownBox::~KDownBox()
{
}

