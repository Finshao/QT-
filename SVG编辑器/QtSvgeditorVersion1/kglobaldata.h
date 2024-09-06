#ifndef __K_GLOBAL_DATA__
#define __K_GLOBAL_DATA__

#include <QObject>

class KGlobalData : public QObject
{
	Q_OBJECT

public:
	enum class KDrawFlag
	{
		NoneDrawFlag = 0,
		MouseDrawFlag,
		PenDrawFlag,
		LineDrawFlag,
		RectDrawFlag,
		CircleDrawFlag,
		TextDrawFlag,
		StarDrawFlag,
		FiveDrawFlag,
		SixDrawFlag
	};

	KGlobalData(QObject *parent = Q_NULLPTR);
	~KGlobalData();

	static KGlobalData *getGlobalDataIntance();

	void setDrawFlag(KGlobalData::KDrawFlag drawflag);
	KDrawFlag getDrawFlag();

	void setCanvasWidth(const int width);
	void setCanvasHeight(const int height);
	void setCanvasScaling(const int scaling);
	void setBorderWidth(const int width);

	int getCanvasWidth() const;
	int getCanvasHeight() const;
	int getCanvasScaling() const;
	int getBorderWidth() const;

	void setCanvasColor(const QString & colorStr);
	void setBorderColor(const QString& borderColor);
	void setFillColor(const QString& fillColor);
	QString getCanvasColor();
	QString getBorderColor();
	QString getFillColor();

	void setCanvaScale(qreal scale);
	qreal getCanvasScale();

private:
	KGlobalData(const KGlobalData &other) = delete;
	KGlobalData(const KGlobalData &&other) = delete;
	void operator=(const KGlobalData) = delete;

	KDrawFlag m_drawFlag;
	
	int m_canvasWidth;
	int m_canvasHeight;
	int m_canvasScaling;
	int m_borderWidth;
	QString m_canvasColor;
	QString m_borderColor;
	QString m_fillColor;
	
	qreal m_scale;
};


#endif
