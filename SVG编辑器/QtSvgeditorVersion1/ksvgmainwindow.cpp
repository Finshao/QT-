#include "ksvgmainwindow.h"
#include <QApplication>
#include <QWheelEvent>
#include <QDebug>

KSvgMainWindow::KSvgMainWindow(QWidget *parent)
	: QScrollArea(parent)
	, m_pCanvas(Q_NULLPTR)
{
	setFrameShape(QFrame::NoFrame); // �����ޱ߿�

	// �����ݳ�����ʾ����ʱ���ֹ�����
	setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	setStyleSheet("QScrollArea{background-color:#3f3f3c;}");
	setAlignment(Qt::AlignCenter);
	
	m_pCanvas = new KCanvas;
	setWidget(m_pCanvas);// ���õ�ǰ�ؼ�Ϊ����������ӿؼ�
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
    // ʵ��ȫѡ�߼�  
    qDebug() << "action1";
	m_pCanvas->selectAllShape();
}

void KSvgMainWindow::on_actionUndo_triggered()
{
    // ʵ�ֳ����߼�  
	qDebug() << "action2";
}

void KSvgMainWindow::on_actionRedo_triggered()
{
    // ʵ�������߼�  
	qDebug() << "action3";
}

void KSvgMainWindow::on_actionCut_triggered()
{
    // ʵ�ּ����߼�  
	qDebug() << "action4";
	m_pCanvas->shearShape();
}

void KSvgMainWindow::on_actionCopy_triggered()
{
    // ʵ�ָ����߼�  
	qDebug() << "action5";
	m_pCanvas->copyShape();
}

void KSvgMainWindow::on_actionPaste_triggered(QPoint pos)
{
    // ʵ��ճ���߼�  
	qDebug() << "action6";
	m_pCanvas->pasteShape(pos);
}

void KSvgMainWindow::on_actionDuplicate_triggered(QPoint pos)
{
    // ʵ�ָ�д�߼�  
	qDebug() << "action7";
	//m_pCanvas->copyShape();
	//m_pCanvas->pasteShape(pos);
}

void KSvgMainWindow::on_actionDelete_triggered()
{
    // ʵ��ɾ���߼�  
	qDebug() << "action8";
	m_pCanvas->deleteShape();
}

void KSvgMainWindow::on_actionBringToFront_triggered()
{
	// ʵ�ֽ��������ڵ�ǰ�����һ���߼������ܲ������ϲ㣩  
	qDebug() << "action9";
	m_pCanvas->setFrontLayer();
}

void KSvgMainWindow::on_actionSendToBack_triggered()
{
    // ʵ�ֽ�����������һ����߼�
	qDebug() << "action10";
	m_pCanvas->setBackLayer();
}

void KSvgMainWindow::on_actionBringToFrontMost_triggered()
{
	// ʵ�ֽ��������������߼�  
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
    // ʵ�ֽ�����������ײ���߼�  
	qDebug() << "action12";
	m_pCanvas->setBackestLayer();
}


