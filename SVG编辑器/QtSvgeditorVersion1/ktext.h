#ifndef __K_TEXT_H_
#define __K_TEXT_H_
#pragma execution_character_set("utf-8")
#include "kshape.h"

class KText : public KShape
{
	Q_OBJECT

public:
	KText(QObject *parent = Q_NULLPTR);
	~KText();
	virtual void drawShape(QPaintDevice* parent = Q_NULLPTR) override;
	virtual KShapeType getShapeType();
	virtual bool isTextEdit() override;
	virtual QString getText() override;
	virtual void setText(QString str) override;
private:
	QString m_text = "«Î ‰»ÎŒƒ±æ...";
};

#endif
