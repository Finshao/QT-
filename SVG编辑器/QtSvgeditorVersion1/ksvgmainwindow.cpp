#include "ksvgmainwindow.h"
#include <QApplication>
#include <QWheelEvent>
#include <QDebug>

KSvgMainWindow::KSvgMainWindow(QWidget *parent)
	: QScrollArea(parent)
	, m_pCanvas(Q_NULLPTR)
{
	setFrameShape(QFrame::NoFrame); // 设置无边框

	// 当内容超过显示区域时出现滚动条
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setStyleSheet("QScrollArea{background-color:#3f3f3c;}");
	setAlignment(Qt::AlignCenter);
	
	m_pCanvas = new KCanvas;
	setWidget(m_pCanvas);// 设置当前控件为滚动区域的子控件
}

KSvgMainWindow::~KSvgMainWindow()
{
}

void KSvgMainWindow::setCustorStyle(KGlobalData::KDrawFlag drawFlag)
{
	switch (drawFlag)
	{
	case KGlobalData::KDrawFlag::PenDrawFlag:
		setCursor(Qt::ArrowCursor);
		break;
	case KGlobalData::KDrawFlag::LineDrawFlag:
	case KGlobalData::KDrawFlag::RectDrawFlag:
	case KGlobalData::KDrawFlag::CircleDrawFlag:

		setCursor(Qt::CrossCursor);
		break;

	case KGlobalData::KDrawFlag::TextDrawFlag:
		setCursor(Qt::IBeamCursor);
		break;
	default:
		setCursor(Qt::ArrowCursor);
		break;
	}
}

void KSvgMainWindow::enterEvent(QEvent* event)
{
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();
	setCustorStyle(flag);
	QScrollArea::enterEvent(event);
}

void KSvgMainWindow::wheelEvent(QWheelEvent* event)
{
	if (QApplication::keyboardModifiers() == Qt::ControlModifier)
	{
		qreal scale = 1.0;
		if (event->delta() > 0)
			scale = KGlobalData::getGlobalDataIntance()->getCanvasScale() + 0.05;
		else
			scale = KGlobalData::getGlobalDataIntance()->getCanvasScale() - 0.05;

		qint32 width = KGlobalData::getGlobalDataIntance()->getCanvasWidth();
		qint32 height = KGlobalData::getGlobalDataIntance()->getCanvasHeight();
		m_pCanvas->resize(width * scale, height * scale);
		KGlobalData::getGlobalDataIntance()->setCanvaScale(scale);

		emit canvasScaleChanged();
	}

}

void KSvgMainWindow::on_actionSelectAll_triggered()
{
    // 实现全选逻辑  
    qDebug() << "action1";
	m_pCanvas->selectAllShape();
}

void KSvgMainWindow::on_actionUndo_triggered()
{
    // 实现撤销逻辑  
	qDebug() << "action2";
}

void KSvgMainWindow::on_actionRedo_triggered()
{
    // 实现重做逻辑  
	qDebug() << "action3";
}

void KSvgMainWindow::on_actionCut_triggered()
{
    // 实现剪切逻辑  
	qDebug() << "action4";
	m_pCanvas->shearShape();
}

void KSvgMainWindow::on_actionCopy_triggered()
{
    // 实现复制逻辑  
	qDebug() << "action5";
	m_pCanvas->copyShape();
}

void KSvgMainWindow::on_actionPaste_triggered(QPoint pos)
{
    // 实现粘贴逻辑  
	qDebug() << "action6";
	m_pCanvas->pasteShape(pos);
}

void KSvgMainWindow::on_actionDuplicate_triggered(QPoint pos)
{
    // 实现复写逻辑  
	qDebug() << "action7";
	//m_pCanvas->copyShape();
	//m_pCanvas->pasteShape(pos);
}

void KSvgMainWindow::on_actionDelete_triggered()
{
    // 实现删除逻辑  
	qDebug() << "action8";
	m_pCanvas->deleteShape();
}

void KSvgMainWindow::on_actionBringToFront_triggered()
{
	// 实现将对象置于当前层的上一层逻辑（可能不是最上层）  
	qDebug() << "action9";
	m_pCanvas->setFrontLayer();
}

void KSvgMainWindow::on_actionSendToBack_triggered()
{
    // 实现将对象置于下一层的逻辑
	qDebug() << "action10";
	m_pCanvas->setBackLayer();
}

void KSvgMainWindow::on_actionBringToFrontMost_triggered()
{
	// 实现将对象置于最顶层的逻辑  
	qDebug() << "action11";
	m_pCanvas->setFrontestLayer();
}

QList<KShape*> KSvgMainWindow::getShapeList()
{
	return m_pCanvas->getShapeList();
}

void KSvgMainWindow::changePostion()
{
	QList<KShape*> shapelist = m_pCanvas->getShapeList();
	auto it = shapelist.begin();
	while (it != shapelist.end())
	{
		(*it)->changePostion(KGlobalData::getGlobalDataIntance()->getCanvasScale());
		it++;
	}
}

void KSvgMainWindow::on_actionSendToBackMost_triggered()
{
    // 实现将对象置于最底层的逻辑  
	qDebug() << "action12";
	m_pCanvas->setBackestLayer();
}


