#include "kfive.h"
#include <QPainter>

KFive::KFive(QObject *parent)
	: KShape(parent)
{
}

KFive::~KFive()
{
}

void KFive::drawShape(QPaintDevice* parent)
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
	int rectWidth = abs(getStartPoint().x() - getEndPoint().x()) * scale;
	int rectHeight = abs(getStartPoint().y() - getEndPoint().y()) * scale;
	int startX = getStartPoint().x() * scale;
	int startY = getStartPoint().y() * scale;
    int endX = getEndPoint().x() * scale;
    int endY = getEndPoint().y() * scale;
    int middlex = (startX + endX) / 2;
    int middley = (startY + endY) / 2;

    // 五边形的顶点  
    QPolygonF polygon;

    polygon << QPointF(middlex, startY);

    polygon << QPointF(startX, startY + (endY - startY) / 7 * 3);


    polygon << QPointF(startX + (endX - startX) / 4, endY);

    polygon << QPointF(startX + (endX - startX) / 4 * 3, endY);

    polygon << QPointF(endX, startY + (endY - startY) / 7 * 3);


    // 绘制矩形和五边形  
    //painter.drawRect(rectX, rectY, rectWidth, rectHeight);
    painter.drawPolygon(polygon);
}

KShapeType KFive::getShapeType()
{
	return KShapeType::FiveShapeType;
}
