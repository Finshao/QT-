#ifndef __K_ELLIPSE_H_
#define __K_ELLIPSE_H_

#include "kshape.h"

class KEllipse : public KShape
{
	Q_OBJECT

public:
	KEllipse(QObject *parent = Q_NULLPTR);
	~KEllipse();
	virtual void drawShape(QPaintDevice* parent = Q_NULLPTR) override;
	virtual KShapeType getShapeType();
};

#endif
