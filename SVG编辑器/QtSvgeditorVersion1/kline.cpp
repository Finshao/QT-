#include "kline.h"
#include <QPainter>

KLine::KLine(QObject *parent)
	: KShape(parent)
{
}

KLine::~KLine()
{
}

void KLine::drawShape(QPaintDevice* parent)
{
	QPainter painter(parent); 
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(m_brush);

	QPen Pen;
	Pen.setColor(m_pen);
	Pen.setWidth(m_width);
	Pen.setStyle(Qt::PenStyle(m_style));

	painter.setPen(Pen);
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	painter.drawLine(QLineF(getStartPoint() * scale, getEndPoint() * scale));
}

KShapeType KLine::getShapeType()
{
	return KShapeType::LineShapeType;
}
