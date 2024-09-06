#include "kcanvasparamsbar.h"

KCanvasParamsBar::KCanvasParamsBar(const QString &title,QWidget *parent)
	: KParamsBar(title,parent)
	, m_pWidthBox(Q_NULLPTR)
	, m_pHeightBox(Q_NULLPTR)
	, m_pColorBox(Q_NULLPTR)
{
	m_pWidthBox = new KValueBox(QStringLiteral("宽度"), KGlobalData::getGlobalDataIntance()->getCanvasWidth(), this);
	m_pHeightBox = new KValueBox(QStringLiteral("高度"), KGlobalData::getGlobalDataIntance()->getCanvasHeight(), this);
	m_pScalingBox = new KValueBox(QStringLiteral("缩放比例"), KGlobalData::getGlobalDataIntance()->getCanvasScale() * 100, this);
	m_pBorderWidthBox = new KValueBox(QStringLiteral("边框宽度"), KGlobalData::getGlobalDataIntance()->getBorderWidth(), this);

	m_pColorBox = new KColorBox(QStringLiteral("颜色"), KGlobalData::getGlobalDataIntance()->getCanvasColor(), this);
	m_pBorderColorBox = new KColorBox(QStringLiteral("边框颜色"), KGlobalData::getGlobalDataIntance()->getBorderColor(), this);
	m_pFillColorBox = new KColorBox(QStringLiteral("填充颜色"), KGlobalData::getGlobalDataIntance()->getFillColor(), this);

	m_pBorderStyleBox = new KDownBox(QStringLiteral("边框样式"), this);
	// 添加到网格布局
	m_pGridLayout->addWidget(m_pWidthBox, 0, 0);// 第 0 行 第 0 列
	m_pGridLayout->addWidget(m_pHeightBox, 0, 1);// 第 0 行 第 1 列
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
