#define K_SVGFILE_H
#ifndef K_SVFFILE_H


#include <QList>
#include <QPainter>
//#include "kmainwindow.h"

class KShape;
class KGlobalData;
class KSVGFile
{
public:
	KSVGFile();
	~KSVGFile();
	void setSVGFile();
	//void getMain(KMainWindow* ui);
	void setShapeList(QList<KShape*> ShapeList);

	void test();
	int m_canvasWidth;
	int m_canvasHeight;
	QString m_canvasColor = "FFFFFF";
	int m_canvasSCale;

	QList <KShape*> m_pShapeList;
};



#endif // !K_SVFFILE_H
