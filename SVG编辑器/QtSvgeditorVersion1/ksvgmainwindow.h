#ifndef __K_SVG_MAINWINDOW_H_
#define __K_SVG_MAINWINDOW_H_

#include <QScrollArea>
#include "kcanvas.h"

class KSvgMainWindow : public QScrollArea
{
	Q_OBJECT
	friend class KMainWindow;

public:
	KSvgMainWindow(QWidget *parent);
	~KSvgMainWindow();
	void setCustorStyle(KGlobalData::KDrawFlag drawFlag);

	void enterEvent(QEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

    void on_actionSelectAll_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered(QPoint pos);
    void on_actionDuplicate_triggered(QPoint pos);
    void on_actionDelete_triggered();
    void on_actionSendToBack_triggered();
    void on_actionBringToFront_triggered();
    void on_actionSendToBackMost_triggered();
    void on_actionBringToFrontMost_triggered();

    QList <KShape*> getShapeList();

    void changePostion();

signals:
	void canvasScaleChanged();
private:
	KCanvas *m_pCanvas;
};

#endif
