#pragma execution_character_set("utf-8")
#include "kdownbox.h"
#include <QRegExpValidator>

KDownBox::KDownBox(const QString &title, QWidget *parent)
	: KParamBox(title,parent)
	, m_pComboBox(Q_NULLPTR)
{
	m_pComboBox = new QComboBox(this);
	m_pComboBox->addItem(QIcon(":/icons/1.svg"), "ʵ��");
	//m_pComboBox->addItem("ʵ��");
	m_pComboBox->addItem(QIcon(":/icons/2.svg"), "����");
	m_pComboBox->addItem(QIcon(":/icons/3.svg"), "����");
	m_pComboBox->addItem(QIcon(":/icons/2.svg"), "�㻮��");


	m_pComboBox->setStyleSheet("QComboBox { font-size: 8pt; } QComboBox QAbstractItemView { font-size: 8pt; }");

	m_pVLayout->addWidget(m_pComboBox);// ���뵽������ m_pVLayout �����ڸ�����


}

KDownBox::~KDownBox()
{
}

