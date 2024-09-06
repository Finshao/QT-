#ifndef __K_SHAPEFACTORY_H_
#define __K_SHAPEFACTORY_H_

#include <QObject>

#include "kglobaldata.h"
#include "krect.h"
#include "kline.h"
#include "kellipse.h"
#include "kpen.h"
#include "kfive.h"
#include "ksix.h"
#include "kstar.h"
#include "ktext.h"

class KShapeFactory : public QObject
{
	Q_OBJECT

public:
	KShapeFactory(QObject *parent);
	~KShapeFactory();
	static KShape* createShape(KGlobalData::KDrawFlag drawFlag);
};

#endif
