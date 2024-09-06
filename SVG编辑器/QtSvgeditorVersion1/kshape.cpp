#include "kshape.h"
#include "kshapefactory.h"

KShape::KShape(QObject *parent)
	: QObject(parent)
{
}

KShape::~KShape()
{
}

KShape& KShape::operator=(const KShape& other)
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

void KShape::drawShape(QPaintDevice* parent)
{
}

void KShape::move(QPoint offset)
{
	m_startPoint += offset;
	m_endPoint += offset;
}

void KShape::moveTop(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_startPoint.ry() = pos.y() / scale;
}

void KShape::moveBottom(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_endPoint.ry() = pos.y() / scale;
}

void KShape::moveLeft(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_startPoint.rx() = pos.x() / scale;
}

void KShape::moveRight(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_endPoint.rx() = pos.x() / scale;
}

void KShape::moveTopLeft(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_startPoint = pos / scale;
}

void KShape::moveTopRight(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_startPoint.ry() = pos.y() / scale;
	m_endPoint.rx() = pos.x() / scale;
}

void KShape::moveBottomLeft(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_startPoint.rx() = pos.x() / scale;
	m_endPoint.ry() = pos.y() / scale;
}

void KShape::moveBottomRight(QPoint pos)
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	m_endPoint = pos / scale;
}

// 绘制选中矩形框
void KShape::drawOutLine(QPaintDevice* parent)
{
	QPainter painter(parent);
	QPen pen;

	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();

	pen.setColor(QColor(79, 128, 255));
	pen.setWidth(3);
	painter.setPen(pen);

	QPoint start = getStartPoint();
	QPoint end = getEndPoint();

	painter.drawRect(QRect(getStartPoint() * scale, getEndPoint() * scale));
}

QPoint KShape::getStartPoint()
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	return m_startPoint;
}

QPoint KShape::getEndPoint()
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	return m_endPoint;
}

KGlobalData::KDrawFlag KShape::getFlag()
{
	return m_flag;
}

void KShape::setStartPoint(const QPoint & point)
{
	m_startPoint = point;
}

void KShape::setEndPoint(const QPoint& point)
{
	m_endPoint = point;
}

KShapeType KShape::getShapeType()
{
	return KShapeType();
}

QRect KShape::getShapeRect() const
{
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	return QRect(m_startPoint * scale,m_endPoint * scale);
}

QLine KShape::getShapeLine() const
{
	return QLine(m_startPoint, m_endPoint);
}

QRectF KShape::getShapeEllipse() const
{
	return QRectF(m_startPoint, m_endPoint);
}

int KShape::getLayer() const
{
	return m_layer;
}


bool KShape::isValid()
{
	return (m_startPoint.x() != m_endPoint.x() || m_startPoint.y() != m_endPoint.y());
}

bool KShape::isTextEdit()
{
	return false;
}

QString KShape::getText()
{
	return QString();
}

void KShape::setText(QString str)
{
	return;
}

void KShape::setBrush(QBrush brush)
{
	m_brush = brush;
}

void KShape::setPen(QColor pen)
{
	m_pen = pen;
}

void KShape::setWidth(int width)
{
	m_width = width;
}

void KShape::setStyle(int style)
{
	m_style = style;
}

void KShape::setFlag(KGlobalData::KDrawFlag flag)
{
	m_flag = flag;
}

void KShape::setLayer(int layer)
{
	m_layer = layer;
}

void KShape::changePostion(qreal scale)
{
	m_startPoint *= scale;
	m_endPoint *= scale;
}


KShape* KShape::copy()
{
	KShape* shape = KShapeFactory::createShape(KGlobalData::KDrawFlag(getShapeType()));

	shape->m_startPoint = m_startPoint;
	shape->m_endPoint = m_endPoint;
	shape->m_brush = m_brush;
	shape->m_pen = m_pen;
	shape->m_width = m_width;
	shape->m_style = m_style;
	shape->m_flag = m_flag;

	return shape;
}