#ifndef __K_DOWN_BOX_H_
#define __K_DOWN_BOX_H_

#include <QComboBox>

#include "kparambox.h"

class KDownBox : public KParamBox
{
	Q_OBJECT

	friend class KMainWindow;
public:
	KDownBox(const QString &title, QWidget *parent);
	~KDownBox();
private:
	QComboBox* m_pComboBox;
};

#endif
