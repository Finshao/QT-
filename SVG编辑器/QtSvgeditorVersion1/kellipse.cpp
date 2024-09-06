#include "kellipse.h"
#include <QPainter>

KEllipse::KEllipse(QObject *parent)
	: KShape(parent)
{
}

KEllipse::~KEllipse()
{
}

void KEllipse::drawShape(QPaintDevice* parent)
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
	painter.drawEllipse(QRectF(getStartPoint() * scale,getEndPoint() * scale));
}

KShapeType KEllipse::getShapeType()
{
	return KShapeType::CircleShapeType;
}
