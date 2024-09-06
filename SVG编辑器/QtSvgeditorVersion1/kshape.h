#ifndef __K_SHAPE_H_
#define __K_SHAPE_H_

#include <QObject>
#include <QPoint>
#include <QPaintDevice>
#include <QPainter>
#include <kglobaldata.h>

enum class KShapeType
{
	None = 0,
	PenShapeType,
	LineShapeType,
	CircleShapeType,
	RectShapeType,
	PentShapeType,
	HexaShapeType,
	TextShapeType,
	FiveShapeType,
	SixShapeType,
	StarShapeType,
};

class KShape : public QObject
{
	Q_OBJECT

public:
	KShape(QObject *parent = Q_NULLPTR);
	~KShape();
	

	virtual KShape& operator = (const KShape& other);

	KShape(const KShape& other) = delete;
	KShape(KShape&& other) noexcept = delete;
	KShape& operator = (KShape&& other) noexcept = delete;


	// 在子类中重写，实现不同类型的图形的绘制
	virtual void drawShape(QPaintDevice* parent = Q_NULLPTR) = 0;

	virtual void move(QPoint offset);
	virtual void moveTop(QPoint pos);
	virtual void moveBottom(QPoint pos);
	virtual void moveLeft(QPoint pos);
	virtual void moveRight(QPoint pos);
	virtual void moveTopLeft(QPoint pos);
	virtual void moveTopRight(QPoint pos);
	virtual void moveBottomLeft(QPoint pos);
	virtual void moveBottomRight(QPoint pos);
	virtual KShapeType getShapeType();
	

	QPoint getStartPoint();
	QPoint getEndPoint();
	KGlobalData::KDrawFlag getFlag();

	void setStartPoint(const QPoint &point);
	void setEndPoint(const QPoint &point);
	

	void drawOutLine(QPaintDevice* parent = Q_NULLPTR);
	QRect getShapeRect() const;
	QLine getShapeLine() const;
	QRectF getShapeEllipse() const;
	int getLayer() const;
	bool isValid();
	virtual bool isTextEdit();
	virtual QString getText();
	virtual void setText(QString str);

	void setBrush(QBrush brush);
	void setPen(QColor pen);
	void setWidth(int width);
	void setStyle(int style);
	void setFlag(KGlobalData::KDrawFlag flag);
	void setLayer(int layer);

	void changePostion(qreal scale);

	virtual KShape* copy();

protected:
	QPoint m_startPoint;// 起始坐标
	QPoint m_endPoint; // 结束坐标

	QBrush m_brush = QBrush(Qt::NoBrush);
	QColor m_pen = QColor(Qt::black);
	int m_width = 2;
	int m_style = Qt::SolidLine;

	KGlobalData::KDrawFlag m_flag = KGlobalData::KDrawFlag::NoneDrawFlag;
	// 012 3 456
	int m_layer = 3;
};

#endif
