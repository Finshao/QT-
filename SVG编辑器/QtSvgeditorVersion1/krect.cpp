#include "krect.h"
#include <QPainter>
#include <QDebug>

KRect::KRect(QObject *parent)
	: KShape(parent)
{
}

KRect::~KRect()
{
}

KRect& KRect::operator = (const KRect& other)
{
	m_startPoint = other.m_startPoint;
	m_endPoint = other.m_endPoint;
	m_brush = other.m_brush;
	m_pen = other.m_pen;
	m_width = other.m_width;
	m_style = other.m_style;
	m_flag = other.m_flag;
	return *this;
}

void KRect::drawShape(QPaintDevice* parent)
{
	//qDebug() << "rect........................";
	QPainter painter(parent); 
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(m_brush);

	QPen Pen;
	Pen.setColor(m_pen);
	Pen.setWidth(m_width);
	Pen.setStyle(Qt::PenStyle(m_style));

	painter.setPen(Pen);
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	painter.drawRect(QRect(getStartPoint() * scale,getEndPoint()*scale));
}

KShapeType KRect::getShapeType()
{
	return KShapeType::RectShapeType;
}
