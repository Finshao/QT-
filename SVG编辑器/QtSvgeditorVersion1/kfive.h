#ifndef __K_FIVE_H_
#define __K_FIVE_H_

#include "kshape.h"

class KFive : public KShape
{
	Q_OBJECT

public:
	KFive(QObject *parent = Q_NULLPTR);
	~KFive();
	virtual void drawShape(QPaintDevice* parent = Q_NULLPTR) override;
	virtual KShapeType getShapeType();
};

#endif
