#include "kshapefactory.h"

KShapeFactory::KShapeFactory(QObject *parent)
	: QObject(parent)
{
}

KShapeFactory::~KShapeFactory()
{
}

KShape* KShapeFactory::createShape(KGlobalData::KDrawFlag drawFlag)
{
	switch(drawFlag)
	{
	case KGlobalData::KDrawFlag::RectDrawFlag:
		return new KRect;
	case KGlobalData::KDrawFlag::LineDrawFlag:
		return new KLine;
	case KGlobalData::KDrawFlag::CircleDrawFlag:
		return new KEllipse;
	case KGlobalData::KDrawFlag::PenDrawFlag:
		return new KPen;
	case KGlobalData::KDrawFlag::FiveDrawFlag:
		return new KFive;
	case KGlobalData::KDrawFlag::SixDrawFlag:
		return new KSix;
	case KGlobalData::KDrawFlag::StarDrawFlag:
		return new KStar;
	case KGlobalData::KDrawFlag::TextDrawFlag:
		return new KText;
	default:
		break;
	}

}
