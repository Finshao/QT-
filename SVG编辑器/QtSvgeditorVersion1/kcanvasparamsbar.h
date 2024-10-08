#ifndef __K_CANVAS_PARAMS_BAR_H_
#define __K_CANVAS_PARAMS_BAR_H_

#include "kparamsbar.h"
#include "kcolorbox.h"
#include "kvaluebox.h"
#include "kdownbox.h"

class KCanvasParamsBar : public KParamsBar
{
	Q_OBJECT
	friend class KMainWindow;
public:
	KCanvasParamsBar(const QString &title,QWidget *parent);
	~KCanvasParamsBar();
private:
	KValueBox *m_pWidthBox;
	KValueBox *m_pHeightBox;
	KColorBox *m_pColorBox;
	KValueBox* m_pScalingBox;
	KColorBox* m_pBorderColorBox;
	KColorBox* m_pFillColorBox;
	KValueBox* m_pBorderWidthBox;
	KDownBox* m_pBorderStyleBox;
};

#endif
