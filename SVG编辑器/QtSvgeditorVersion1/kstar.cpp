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

    // �������ĵ�
    int middlex = (getStartPoint().x() + getEndPoint().x()) / 2;
    int middley = (getStartPoint().y() + getEndPoint().y()) / 2;

    // ����뾶
    int radius = qMin(abs(getStartPoint().x() - getEndPoint().x()), abs(getStartPoint().y() - getEndPoint().y())) / 2;

    // ����һ������ǵĶ����
    QPolygonF starPolygon;

    // �����������ĽǶȣ�����ǵ�һ�������Ӧ72�ȣ���Ϊ360��/5 = 72�ȣ�
    for (int i = 0; i < 5; ++i) {
        double angle = i * 72.0 + 18.0; // 18��ƫ��ʹ����Ǹ���
        double rad = angle * M_PI / 180.0;

        // �ⶥ��
        QPointF outerPoint(middlex + radius * cos(rad), middley - radius * sin(rad));

        // �ڶ��㣨�뾶��Сһ������������ʹ��0.5��Ϊʾ����
        double innerRadius = radius * 0.4;
        QPointF innerPoint(middlex + innerRadius * cos(rad + M_PI / 5), middley - innerRadius * sin(rad + M_PI / 5));

        // ������������γ��߶�
        starPolygon << outerPoint << innerPoint;
    }

    // �պ϶���Σ���ѡ����Ϊ������Ѿ��Ǳպϵģ��������ʼ����԰���ȷ��������ȷ���ӣ�
    starPolygon << starPolygon.first();

    // ���������
    painter.setPen(Qt::black); // ���û�����ɫ
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

    // �����εĶ���  
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
