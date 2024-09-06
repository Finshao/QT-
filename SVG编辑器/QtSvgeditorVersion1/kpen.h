#ifndef __K_PEN_H_
#define __K_PEN_H_

#include <QVector>
#include "kshape.h"


class KPen : public KShape
{
	Q_OBJECT

public:
	KPen(QObject *parent = Q_NULLPTR);
	~KPen();
	KPen& operator = (const KPen& other);

	virtual KPen* copy() override;

	void addPoint(const QPoint& pos);
	virtual void move(QPoint offset) override;

	virtual void drawShape(QPaintDevice* parent = Q_NULLPTR) override;
	virtual KShapeType getShapeType();
private:
	QVector<QPoint> m_posVector;
};

#endif
