#include "kcanvasparamsbar.h"

KCanvasParamsBar::KCanvasParamsBar(const QString &title,QWidget *parent)
	: KParamsBar(title,parent)
	, m_pWidthBox(Q_NULLPTR)
	, m_pHeightBox(Q_NULLPTR)
	, m_pColorBox(Q_NULLPTR)
{
	m_pWidthBox = new KValueBox(QStringLiteral("���"), KGlobalData::getGlobalDataIntance()->getCanvasWidth(), this);
	m_pHeightBox = new KValueBox(QStringLiteral("�߶�"), KGlobalData::getGlobalDataIntance()->getCanvasHeight(), this);
	m_pScalingBox = new KValueBox(QStringLiteral("���ű���"), KGlobalData::getGlobalDataIntance()->getCanvasScale() * 100, this);
	m_pBorderWidthBox = new KValueBox(QStringLiteral("�߿���"), KGlobalData::getGlobalDataIntance()->getBorderWidth(), this);

	m_pColorBox = new KColorBox(QStringLiteral("��ɫ"), KGlobalData::getGlobalDataIntance()->getCanvasColor(), this);
	m_pBorderColorBox = new KColorBox(QStringLiteral("�߿���ɫ"), KGlobalData::getGlobalDataIntance()->getBorderColor(), this);
	m_pFillColorBox = new KColorBox(QStringLiteral("�����ɫ"), KGlobalData::getGlobalDataIntance()->getFillColor(), this);

	m_pBorderStyleBox = new KDownBox(QStringLiteral("�߿���ʽ"), this);
	// ��ӵ����񲼾�
	m_pGridLayout->addWidget(m_pWidthBox, 0, 0);// �� 0 �� �� 0 ��
	m_pGridLayout->addWidget(m_pHeightBox, 0, 1);// �� 0 �� �� 1 ��
	m_pGridLayout->addWidget(m_pScalingBox, 1, 0);
	m_pGridLayout->addWidget(m_pColorBox, 1, 1);
	m_pGridLayout->addWidget(m_pBorderColorBox, 2, 0);
	m_pGridLayout->addWidget(m_pFillColorBox, 2, 1);
	m_pGridLayout->addWidget(m_pBorderWidthBox, 3, 0);
	m_pGridLayout->addWidget(m_pBorderStyleBox, 3, 1);

}

KCanvasParamsBar::~KCanvasParamsBar()
{
}
