#include "ktext.h"
#include "kcanvas.h"
#include <QPainter>
#include <QDebug>

KText::KText(QObject *parent)
	: KShape(parent)
{
}

KText::~KText()
{
}

void KText::drawShape(QPaintDevice* parent)
{
	QPainter painter(parent); 
	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(m_brush);

	QPen Pen;
	Pen.setColor(m_pen);
	Pen.setWidth(m_width);
	Pen.setStyle(Qt::PenStyle(m_style));

	painter.setPen(Pen);

	QRect rect(getStartPoint() * scale, getEndPoint() * scale);
	painter.drawRect(rect); 


	QFont font;
	font.setPointSize(12 * scale);
	painter.setFont(font);

	QFontMetrics fontMetrics(font);

	int textWidth = fontMetrics.width(m_text);
	int textHeight = fontMetrics.height();

	int textX = rect.x() + 2;
	int textY = rect.y() + (rect.height() + textHeight) / 2;
	painter.drawText(textX, textY, m_text);

}

KShapeType KText::getShapeType()
{
	return KShapeType::TextShapeType;
}

bool KText::isTextEdit()
{
	return true;
}

QString KText::getText()
{
	return m_text;
}

void KText::setText(QString str)
{
	m_text = str;
}
