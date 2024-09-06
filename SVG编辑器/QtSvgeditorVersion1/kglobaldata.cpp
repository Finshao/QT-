#include "kglobaldata.h"
#include <Windows.h>


KGlobalData::KGlobalData(QObject *parent)
	: QObject(parent)
	,m_drawFlag(KDrawFlag::NoneDrawFlag)
    ,m_canvasColor("FFFFFF")
    ,m_canvasWidth(600)
    ,m_canvasHeight(400)
    ,m_scale(1.0)
    ,m_canvasScaling(100)
    , m_borderWidth(2)
{
}



KGlobalData *KGlobalData::getGlobalDataIntance()
{
    static KGlobalData s_instance;
    static KGlobalData* pinstance = nullptr;
    if (pinstance == nullptr)
        pinstance = &s_instance;
    return pinstance;
}

void KGlobalData::setDrawFlag(KGlobalData::KDrawFlag drawflag)
{
    this->m_drawFlag = drawflag;
}

KGlobalData::KDrawFlag KGlobalData::getDrawFlag()
{
    return m_drawFlag;
}

void KGlobalData::setCanvasWidth(const int width)
{
    this->m_canvasWidth = width;
}

void KGlobalData::setCanvasHeight(const int height)
{
    this->m_canvasHeight = height;
}

void KGlobalData::setCanvasScaling(const int scaling)
{
    m_canvasScaling = scaling;
}

void KGlobalData::setBorderWidth(const int width)
{
    m_borderWidth = width;
}

int KGlobalData::getCanvasWidth() const
{
    return this->m_canvasWidth;
}

int KGlobalData::getCanvasHeight() const
{
    return m_canvasHeight;
}

int KGlobalData::getCanvasScaling() const
{
	return m_canvasScaling;
}

int KGlobalData::getBorderWidth() const
{
    return m_borderWidth;
}

void KGlobalData::setCanvasColor(const QString& colorStr)
{
    this->m_canvasColor = colorStr;
}

void KGlobalData::setBorderColor(const QString& borderColor)
{
    m_borderColor = borderColor;
}

void KGlobalData::setFillColor(const QString& fillColor)
{
    m_fillColor = fillColor;
}

QString KGlobalData::getCanvasColor()
{
    return m_canvasColor;
}

QString KGlobalData::getBorderColor()
{
    return m_borderColor;
}

QString KGlobalData::getFillColor()
{
    return m_fillColor;
}

void KGlobalData::setCanvaScale(qreal scale)
{
    m_scale = scale;
    
}

qreal KGlobalData::getCanvasScale()
{
    return m_scale;
}

KGlobalData::~KGlobalData()
{
}
