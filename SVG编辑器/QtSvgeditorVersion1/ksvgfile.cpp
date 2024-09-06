#include "ksvgfile.h"
#include "kglobaldata.h"
#include <QPainter>
#include <QDebug>
#include <QList>
#include "kshape.h"
#include "kshapefactory.h"
#include "kpen.h"

KSVGFile::KSVGFile()
{
}

KSVGFile::~KSVGFile()
{
}

void KSVGFile::setSVGFile()
{
	m_canvasHeight = KGlobalData::getGlobalDataIntance()->getCanvasHeight();
	m_canvasWidth = KGlobalData::getGlobalDataIntance()->getCanvasWidth();
	m_canvasSCale = KGlobalData::getGlobalDataIntance()->getCanvasScale();

}

void KSVGFile::setShapeList(QList<KShape*> ShapeList)
{
	m_pShapeList.clear();
	auto it = ShapeList.begin();
	int i = 0;
	while (it != ShapeList.end())
	{
		m_pShapeList.push_back(KShapeFactory::createShape((*it)->getFlag()));
		if (i >= m_pShapeList.size())
		{
			qDebug() << "error copy shapelist over split";
		}
		else
		{
			if (ShapeList[i]->getFlag() == KGlobalData::KDrawFlag::PenDrawFlag)
			{
				qDebug() << "find Pen, run...";
				m_pShapeList[i] = ShapeList[i]->copy();
			}
			else
			{
				(*m_pShapeList[i]) = *ShapeList[i];
			}
		}
		it++;
		i++;
	}
}



/*
void KSVGFile::getMain(KMainWindow* ui)
{
	QList<KShape*> ShapeList = ui->getShapeList();
	auto it = ShapeList.begin();
	int i = 0;
	while (it != ShapeList.end())
	{
		m_pShapeList.prepend(KShapeFactory::createShape((*it)->getFlag()));
		if (i >= m_pShapeList.size())
		{
			qDebug() << "error copy shapelist over split";
		}
		else
		{
			m_pShapeList[i] = ShapeList[i];
		}
		it++;
		i++;
	}

}
*/
void KSVGFile::test()
{
	if (m_canvasHeight == 0 || m_canvasWidth == 0)
	{
		qDebug() << "error load";
	}
	qDebug() << "canvas height" << m_canvasHeight;
	qDebug() << "canvas width" << m_canvasWidth;
	qDebug() << "canvas scale" << m_canvasSCale;
	qDebug() << "canvas color" << m_canvasColor;

	qDebug() << "shapelist size:" << m_pShapeList.size();
	for (int i = 0; i < m_pShapeList.size(); i++)
	{
		qDebug() << "(" << m_pShapeList[i]->getStartPoint().x() << "," << m_pShapeList[i]->getStartPoint().y() << ")"
			<< "(" << m_pShapeList[i]->getEndPoint().x() << "," << m_pShapeList[i]->getEndPoint().y() << ")";
	}
}
