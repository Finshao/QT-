#ifndef __K_SIX_H_
#define __K_SIX_H_

#include "kshape.h"

class KSix : public KShape
{
	Q_OBJECT

public:
	KSix(QObject *parent = Q_NULLPTR);
	~KSix();
	virtual void drawShape(QPaintDevice* parent = Q_NULLPTR) override;
	virtual KShapeType getShapeType();
};

#endif
