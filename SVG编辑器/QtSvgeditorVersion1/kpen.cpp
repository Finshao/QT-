#include "kpen.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QDebug>
#include "kshapefactory.h"

KPen::KPen(QObject *parent)
	: KShape(parent)
{
}

KPen::~KPen()
{
}

KPen& KPen::operator=(const KPen& other)
{
	m_posVector.clear();
	for (int i = 0; i < other.m_posVector.size(); i++)
	{
		m_posVector.push_back(other.m_posVector[i]);
	}
	//m_startPoint = other.m_startPoint;
	//m_endPoint = other.m_endPoint;
	m_brush = other.m_brush;
	m_pen = other.m_pen;
	m_width = other.m_width;
	m_style = other.m_style;
	m_flag = other.m_flag;
	return *this;
}

KPen* KPen::copy()
{
	KPen pen = KShapeFactory::createShape(KGlobalData::KDrawFlag::PenDrawFlag);
	for (int i = 0; i < m_posVector.size(); i++)
	{
		pen.m_posVector.push_back(m_posVector[i]);
	}
	pen.m_startPoint = m_startPoint;
	pen.m_endPoint = m_endPoint;
	pen.m_brush = m_brush;
	pen.m_pen = m_pen;
	pen.m_width = m_width;
	pen.m_style = m_style;
	pen.m_flag = m_flag;

	return &pen;
}

void KPen::addPoint(const QPoint& pos)
{	
	if (m_posVector.isEmpty())
	{
		m_endPoint = pos;
		m_startPoint = pos;
	}
	else
	{
		if (pos.x() < m_startPoint.x())
		{
			m_startPoint.rx() = pos.x();
		}
		if (pos.y() < m_startPoint.y())
		{
			m_startPoint.ry() = pos.y();
		}
		if (pos.y() > m_endPoint.y())
		{
			m_endPoint.ry() = pos.y();
		}
		if (pos.x() > m_endPoint.x())
		{
			m_endPoint.rx() = pos.x();
		}
	}

	m_posVector.append(pos);
}

void KPen::move(QPoint offset)
{
	qDebug() << "Pen is move";

	for (int i = 0; i < m_posVector.size(); i++)
	{
		m_posVector[i] += offset;
	}

	KShape::move(offset);
}

void KPen::drawShape(QPaintDevice* parent)
{
	//qDebug() << "pen is draw";
	QPainter painter(parent); 
	painter.setRenderHint(QPainter::Antialiasing);
	QPainterPath path;

	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	QPoint* pos = m_posVector.begin();
	path.moveTo((*pos) * scale);

	while (pos != m_posVector.end())
	{
		path.lineTo((*pos) * scale);
		pos++;
	}

	// 自由线不填充颜色
	//painter.setBrush(brush);


	QPen Pen;
	Pen.setColor(m_pen);
	Pen.setWidth(m_width);
	Pen.setStyle(Qt::PenStyle(m_style));
	painter.setPen(Pen);
	painter.drawPath(path);
}

KShapeType KPen::getShapeType()
{
	return KShapeType::PenShapeType;
}
