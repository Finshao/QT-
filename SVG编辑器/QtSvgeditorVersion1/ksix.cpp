#include "ksix.h"
#include <QPainter>

KSix::KSix(QObject *parent)
	: KShape(parent)
{
}

KSix::~KSix()
{
}

void KSix::drawShape(QPaintDevice* parent)
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

    // Áù±ßÐÎµÄ¶¥µã  
    QPolygonF hexagon;

    hexagon << QPointF((startX + middlex) / 2, startY);

    hexagon << QPointF(startX, middley);

    hexagon << QPointF((startX + middlex) / 2, endY);

    hexagon << QPointF((middlex + endX) / 2, endY);

    hexagon << QPointF(endX, middley);

    hexagon << QPointF((middlex + endX) / 2, startY);

   
    painter.drawPolygon(hexagon);
}

KShapeType KSix::getShapeType()
{
	return KShapeType::RectShapeType;
}
