#include "kstar.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>

KStar::KStar(QObject* parent)
    : KShape(parent)
{
}

KStar::~KStar()
{
}

/*
void KStar::drawShape(QPaintDevice* parent)
{
    QPainter painter(parent);

    // 假设中心点
    int middlex = (getStartPoint().x() + getEndPoint().x()) / 2;
    int middley = (getStartPoint().y() + getEndPoint().y()) / 2;

    // 假设半径
    int radius = qMin(abs(getStartPoint().x() - getEndPoint().x()), abs(getStartPoint().y() - getEndPoint().y())) / 2;

    // 创建一个五角星的多边形
    QPolygonF starPolygon;

    // 计算五个顶点的角度（五角星的一个顶点对应72度，因为360度/5 = 72度）
    for (int i = 0; i < 5; ++i) {
        double angle = i * 72.0 + 18.0; // 18度偏移使五角星更尖
        double rad = angle * M_PI / 180.0;

        // 外顶点
        QPointF outerPoint(middlex + radius * cos(rad), middley - radius * sin(rad));

        // 内顶点（半径减小一定比例，这里使用0.5作为示例）
        double innerRadius = radius * 0.4;
        QPointF innerPoint(middlex + innerRadius * cos(rad + M_PI / 5), middley - innerRadius * sin(rad + M_PI / 5));

        // 添加两个点以形成线段
        starPolygon << outerPoint << innerPoint;
    }

    // 闭合多边形（可选，因为五角星已经是闭合的，但添加起始点可以帮助确保线条正确连接）
    starPolygon << starPolygon.first();

    // 绘制五角星
    painter.setPen(Qt::black); // 设置画笔颜色
    painter.drawPolygon(starPolygon);
}
*/

void KStar::drawShape(QPaintDevice* parent)
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
    double rectWidth = abs(getStartPoint().x() - getEndPoint().x()) * scale;
    double rectHeight = abs(getStartPoint().y() - getEndPoint().y()) * scale;
    double startX = getStartPoint().x() * scale;
    double startY = getStartPoint().y() * scale;
    double endX = getEndPoint().x() * scale;
    double endY = getEndPoint().y() * scale;
    double middlex = (startX + endX) / 2;
    double middley = (startY + endY) / 2;
    double height = (endY - startY);
    double _width = (endX - startX);

    // 六边形的顶点  
    QPolygonF hexagon;
    //1
    hexagon << QPointF(startX + _width /2, startY);
    //2
    hexagon << QPointF(startX + _width / 10 * 6, startY + height / 100 * 38);
    //3
    hexagon << QPointF(endX, startY + height / 100 * 38);
    //4
    hexagon << QPointF(startX + _width / 100 * 68, startY + height / 100 * 60);
    //5
    hexagon << QPointF(startX + _width / 100 * 80, endY);
    //6
    hexagon << QPointF(startX + _width / 2, startY + height / 100 * 73);
    //7
    hexagon << QPointF(startX + _width / 100 * 20, endY);
    //8
    hexagon << QPointF(startX + _width / 100 * 32, startY + height / 100 * 60);
    //9
    hexagon << QPointF(startX, startY + height / 100 * 38);
    //10
    hexagon << QPointF(startX + _width / 100 * 40, startY + height / 100 * 38);



    painter.drawPolygon(hexagon);
}


KShapeType KStar::getShapeType()
{
    return KShapeType::RectShapeType;
}
