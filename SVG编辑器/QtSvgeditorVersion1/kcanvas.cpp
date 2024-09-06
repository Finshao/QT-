#include "kcanvas.h"
#include "kshapefactory.h"
#include "kpen.h"

#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

KCanvas::KCanvas(QWidget* parent)
	: QWidget(parent)
	, m_pCurrentShape(Q_NULLPTR)
	, m_TransType(KTransType::None)
	, m_isDrawing(false)
	, m_isLPress(false)
	, m_isSelected(false)
	, m_brush(Qt::NoBrush)
	, m_pen(Qt::black)
	, m_width(2)
	, m_style(1)
	, m_selectAllShape(false)
	, m_svgRenderer(Q_NULLPTR)
{
	setAttribute(Qt::WA_StyledBackground, true);

	// ���ñ�����ɫΪ��ɫ
	setStyleSheet("background-color:#FFFFFF");

	KGlobalData::getGlobalDataIntance()->setCanvasColor("FFFFFF");
	resize(KGlobalData::getGlobalDataIntance()->getCanvasWidth(),
		KGlobalData::getGlobalDataIntance()->getCanvasHeight());

	setMouseTracking(true);//�������׷��
	setFocusPolicy(Qt::ClickFocus);// ������ȡ����

	qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
	textEdit = new QLineEdit(this);
	QFont font;
	font.setPointSize(12 * scale);
	textEdit->setFont(font);
	textEdit->hide();
	connect(textEdit, &QLineEdit::returnPressed, this, &KCanvas::onTextEditReturnPressed);
}

KCanvas::~KCanvas()
{
	
	for (auto shape : m_pShapeList)
		delete shape;
	m_pShapeList.clear();
}

// ��ͼ�¼�����
void KCanvas::paintEvent(QPaintEvent* event)
{
#if 0
	if (!m_pShapeList->isEmpty())
	{
			//����ͼ�ζ����  List
		for (QList<KShape*>::iterator it = m_pShapeList->begin();
								it != m_pShapeList->end(); ++it)
			(*it)->drawShape(this);// ����
	}
#endif
	QPainter painter(this);
	if (m_svgRenderer && m_svgRenderer->isValid())  
	{  
	    m_svgRenderer->render(&painter);  
	} 
	if (!m_pShapeList.isEmpty())
	{
		
		// ����ӵ����ͼ�㿪ʼ�滭
		for (int i = 6; i >= 0; i--)
		{
			auto it = m_pShapeList.rbegin();
			while (it != m_pShapeList.rend())
			{
				if ((*it)->getLayer() == i)
				{
					//qDebug() << "paint";
					(*it)->drawShape(this);
				}
				it++;
			}
		}
		

		// ���ȫѡ���������һ��
		if (m_selectAllShape == true)
		{
			auto it = m_pShapeList.begin();
			while (it != m_pShapeList.end())
			{
				(*it)->drawOutLine(this);
				it++;
			}
		}
	}
	if (m_pCurrentShape != Q_NULLPTR)
	{
		if (m_isDrawing) // ͼ�λ��Ʊ�־Ϊ true�����ʾ���ڻ��Ƹ�ͼ�Σ���ʱ��Ҫ�������»���
			qDebug() << "paint";
			m_pCurrentShape->drawShape(this);

		if (m_isSelected) // ͼ��ѡ�У������ѡ�п�
			m_pCurrentShape->drawOutLine(this);
	}
}

void KCanvas::mousePressEvent(QMouseEvent* event)
{
	//��ȡ��ǰ��Ҫ���Ƶ�ͼ������
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();
	
	if (flag == KGlobalData::KDrawFlag::NoneDrawFlag)
		return;
	
	if (event->buttons() == Qt::LeftButton)
	{
		m_isLPress = true;// ��¼�������Ѿ����
		if (flag == KGlobalData::KDrawFlag::MouseDrawFlag)	// ����ʱ���ѡ�У���ʼ��¼���������position��Ϊ�ƶ���׼��
		{
			m_pCurrentShape = getCurrentShape(event->pos());
			if (m_pCurrentShape != Q_NULLPTR)
			{
				m_lastPos = event->pos();// ��ǰλ���ƶ������λ��(�����)
				m_isSelected = true;// ����ѡ�б�־
				m_TransType = getTransType(event->pos());// ��ȡ�ƶ�����
			}
			else
			{
				m_isSelected = false;
				m_selectAllShape = false;
			}
		}
		// ˵��Ҫ��ʼ�����µ�ͼ����
		// ���������߶����߼�,��Ϊ�����start,end�������ߵı仯���仯
		else if (flag == KGlobalData::KDrawFlag::PenDrawFlag)
		{
			m_isSelected = false;
			m_selectAllShape = false;
			m_pCurrentShape = KShapeFactory::createShape(KGlobalData::getGlobalDataIntance()->getDrawFlag());

			if (m_pCurrentShape != Q_NULLPTR)
			{
				static_cast<KPen*>(m_pCurrentShape)->addPoint(event->pos());
			}
			m_pCurrentShape->setFlag(KGlobalData::getGlobalDataIntance()->getDrawFlag());
		}
		else 
		{ 
			m_isSelected = false;
			m_selectAllShape = false;
			m_pCurrentShape = KShapeFactory::createShape(KGlobalData::getGlobalDataIntance()->getDrawFlag());
			// shape������flagΨһ��ֻ���ڴ���ʱ����һ��
			m_pCurrentShape->setFlag(KGlobalData::getGlobalDataIntance()->getDrawFlag());


			// ���������Ȱ�point��endҲ�����ˣ���������draw��bug
			if (m_pCurrentShape != Q_NULLPTR)
			{
				m_pCurrentShape->setStartPoint(event->pos());
				m_pCurrentShape->setEndPoint(event->pos());
			}
				
				
		}
	}
	update();
}

void KCanvas::mouseMoveEvent(QMouseEvent* event)
{
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();
	if (flag == KGlobalData::KDrawFlag::NoneDrawFlag)
		return;


	KTransType transType = getTransType(event->pos());
	updateCusorStyle(flag, transType);//ѡ��ʱ�������ѡ����ʽ

	if (event->buttons() != Qt::LeftButton)//bug : �ڲ����������������ѡ��״̬���ƶ������ƶ�ͼ��
			return;


	if (flag == KGlobalData::KDrawFlag::PenDrawFlag)
	{
		if (m_isLPress && !m_isDrawing)
			m_isDrawing = true;
		if (m_pCurrentShape != Q_NULLPTR)
		{
			static_cast<KPen*>(m_pCurrentShape)->addPoint(event->pos());
		}
	}
	else if (flag != KGlobalData::KDrawFlag::MouseDrawFlag)
	{
		if (m_isLPress && !m_isDrawing)
			m_isDrawing = true;
		if (m_pCurrentShape != Q_NULLPTR)
			m_pCurrentShape->setEndPoint(event->pos());
	}
	else
	{
		if (m_pCurrentShape == Q_NULLPTR)
		{
			return;
		}

		// ���������������ȫѡģʽ��ÿһ��shape��Ҫ�ƶ�
		if (m_selectAllShape == true)
		{
			
			drawMoveAllShape(event->pos());
		}
		else
		{
			
			dragMoveShape(transType, event->pos());//�ƶ�ѡ��ͼ��
		}
	}

	update();
}

void KCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();

	if (m_isLPress) // ���������º��ͷţ����ʾ�ƶ����߻�ͼ���
	{
		if (flag != KGlobalData::KDrawFlag::MouseDrawFlag)// ����ǻ�ͼ
		{
			if (m_pCurrentShape != Q_NULLPTR)
			{
				if (m_pCurrentShape->isValid())// �ж�ͼ�ξ����Ƿ���Ч
				{
					if (flag == KGlobalData::KDrawFlag::PenDrawFlag)
					{
						qobject_cast<KPen*>(m_pCurrentShape)->addPoint(event->pos());
					}
					else
					{
						m_pCurrentShape->setEndPoint(event->pos());
					}
				}

				m_pShapeList.prepend(m_pCurrentShape);
				m_pCurrentShape->setBrush(m_brush);
				m_pCurrentShape->setPen(m_pen);
				m_pCurrentShape->setWidth(m_width);
				m_pCurrentShape->setStyle(m_style);
				m_pCurrentShape = Q_NULLPTR;
				//m_isSelected = true;
				//m_isDrawing = false;
				emit addShapeCompleted();
					
			}
			else
			{
				emit addShapeCompleted();
				delete m_pCurrentShape;
				m_pCurrentShape = Q_NULLPTR;
			}
		}
		/*
		else
		{
			// ��������һ��text��ѡ��״ֱ̬���ύ�ı�������bug
			if (m_pCurrentShape != Q_NULLPTR && m_pCurrentShape->isTextEdit())
			{
				QString text = textEdit->text();
				m_pCurrentShape->setText(text);
				textEdit->hide();
			}
		}
		*/
		m_isLPress = false;
		m_isDrawing = false;
		m_TransType = KTransType::None;
	}

	update();
}

void KCanvas::mouseDoubleClickEvent(QMouseEvent* event)
{
	//��ȡ��ǰ��Ҫ���Ƶ�ͼ������
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();

	if (flag == KGlobalData::KDrawFlag::NoneDrawFlag)
		return;

	if (flag == KGlobalData::KDrawFlag::MouseDrawFlag)	// ����ʱ���ѡ��
	{
		m_pCurrentShape = getCurrentShape(event->pos());
		if (m_pCurrentShape != Q_NULLPTR && m_pCurrentShape->isTextEdit())
		{
			qDebug() << "success double";
			qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
			textEdit->setText(m_pCurrentShape->getText()); // ����getText()������״�ĵ�ǰ�ı�  
			double width = m_pCurrentShape->getEndPoint().x() - m_pCurrentShape->getStartPoint().x();
			double height = m_pCurrentShape->getEndPoint().y() - m_pCurrentShape->getStartPoint().y();
			textEdit->setGeometry(m_pCurrentShape->getStartPoint().x() * scale, m_pCurrentShape->getStartPoint().y() * scale, width * scale, height * scale);
			textEdit->show();
			textEdit->setFocus(); // �����ı��༭������  
		}
	}
		
}


KShape* KCanvas::getCurrentShape(const QPoint& pos)
{
	KShape* res = nullptr;
	long long area = LLONG_MAX;
	for (QList<KShape*>::iterator it = m_pShapeList.begin();
		it != m_pShapeList.end(); ++it)
	{
		if ((*it)->getShapeRect().contains(pos))
		{
			long long curArea = abs((long long)(*it)->getStartPoint().rx() - (long long)(*it)->getEndPoint().rx())
				* abs((long long)(*it)->getStartPoint().ry() - (long long)(*it)->getEndPoint().ry());
			if (curArea < area)
			{
				res = *it;
				area = curArea;
			}
		}
	}
	return res;
}

KTransType KCanvas::getTransType(const QPoint& pos)
{
	if (!m_pCurrentShape)
		return KTransType::None;

	// ��ȡͼ�ξ���
	QRect rect = m_pCurrentShape->getShapeRect();

	if (!rect.contains(pos))
		return KTransType::None;
	
	//qDebug() << "rect = " << rect.topLeft().x();
	//qDebug() << "rect = " << rect.topLeft().y();

	// �ж��������ƶ����ֲ���ק�ƶ�
	if (qAbs(pos.x() - rect.topLeft().x()) < 5 
			&& qAbs(pos.y() - rect.topLeft().y()) < 5)
		return KTransType::TopLeft;

	if (qAbs(rect.topRight().x() - pos.x()) < 5 
			&& qAbs(pos.y() - rect.topRight().y()) < 5)
		return KTransType::TopRight;

	if (qAbs(rect.bottomRight().x() - pos.x()) < 5 
			&& qAbs(rect.bottomRight().y() - pos.y()) < 5)
		return KTransType::BottomRight;

	if (qAbs(pos.x() - rect.bottomLeft().x()) < 5 
			&& qAbs(rect.bottomLeft().y() - pos.y()) < 5)
		return KTransType::BottomLeft;

	//bug

	if (qAbs(pos.y() - rect.top()) < 5 
			&& pos.x() > rect.topLeft().x() + 5
			&& pos.x() < rect.topRight().x() - 5)
		return KTransType::Top;

	if (qAbs(rect.right() - pos.x()) < 5
			&& pos.y() > rect.topRight().y() + 5
			&& pos.y() < rect.bottomRight().y() - 5)
		return KTransType::Right;

	if (qAbs(rect.bottom() - pos.y()) < 5 
			&& pos.x() > rect.bottomLeft().x() + 5 
			&& pos.x() < rect.bottomRight().x() - 5)
		return KTransType::Bottom;

	if (qAbs(pos.x() - rect.left()) < 5 
			&& pos.y() > rect.topLeft().y() + 5 
			&& pos.y() < rect.bottomLeft().y() - 5)
		return KTransType::Left;

	return KTransType::Contains;
}

void KCanvas::updateCusorStyle(KGlobalData::KDrawFlag flag, KTransType transType)
{
	// ����Ѿ�ѡ��
	if (m_isSelected
		&& flag == KGlobalData::KDrawFlag::MouseDrawFlag
		&& transType != KTransType::None)
	{
		//TODO���任ͼ��,���ò�ͬ�������ʽ,���ݵ��λ�ã����ò�ͬ�������ʽ
		if (transType == KTransType::TopLeft || transType == KTransType::BottomRight)
			setCursor(Qt::SizeFDiagCursor);
		else if (transType == KTransType::TopRight || transType == KTransType::BottomLeft)
			setCursor(Qt::SizeBDiagCursor);
		else if (transType == KTransType::Top || transType == KTransType::Bottom)
			setCursor(Qt::SizeVerCursor);
		else if (transType == KTransType::Left || transType == KTransType::Right)
			setCursor(Qt::SizeHorCursor);

		else if (transType == KTransType::Contains)
			setCursor(Qt::SizeAllCursor);
	}
	else
		unsetCursor();
}

void KCanvas::dragMoveShape(KTransType transType, const QPoint& pos)
{
	if (m_pCurrentShape == NULL)
		return;

	/*
		���ƶ������У���Ϊ�����ƶ���ʽ
			(1) �����ƶ�  contains
			(2) ����ĳһ���������ק�ƶ�

	*/
	switch (m_TransType)
	{
	case KTransType::None:
		return;
	case KTransType::TopLeft:
		// ��������Ͻǣ���ǰλ��Ϊ�µľ�����ʼλ��
		m_pCurrentShape->moveTopLeft(pos);
		break;
	case KTransType::Top:
		// ����������ƶ������޸���ʼλ�õ� y ����
		m_pCurrentShape->moveTop(pos);
		break;
	case KTransType::TopRight:
		m_pCurrentShape->moveTopRight(pos);
		break;
	case KTransType::Left:
		m_pCurrentShape->moveLeft(pos);
		break;
	case KTransType::Contains: // �����ƶ�
	{
		// m_lastPos Ϊѡ��ʱ���λ�ã����ƶ������в��ϸ�ֵΪǰһ�ε�λ��
		QPoint newpos = pos -  m_lastPos; // ����Ҫ�ƶ���ƫ��
		m_pCurrentShape->move(newpos);
		m_lastPos = pos;
	}
	break;
	case KTransType::Right:
		m_pCurrentShape->moveRight(pos);
		break;
	case KTransType::BottomLeft:
		m_pCurrentShape->moveBottomLeft(pos);
		break;
	case KTransType::Bottom:
		m_pCurrentShape->moveBottom(pos);
		break;
	case KTransType::BottomRight:
		m_pCurrentShape->moveBottomRight(pos);
		break;
	default:
		break;
	}
}

void KCanvas::drawMoveAllShape(const QPoint& pos)
{
	QPoint newpos = pos - m_lastPos; // ����Ҫ�ƶ���ƫ��
	auto it = m_pShapeList.begin();
	while (it != m_pShapeList.end())
	{
		(*it)->move(newpos);
		it++;
	}
	m_lastPos = pos;
}

void KCanvas::updataFillShapeColor(QBrush brush, QColor pen)
{
	if (m_selectAllShape == true)
	{
		auto it = m_pShapeList.begin();
		while (it != m_pShapeList.end())
		{
			(*it)->setBrush(brush);
			it++;
		}
	}
	// ��Ϊѡ��״̬������һ��current��color
	else if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			qDebug() << "m_pcurrentShape not empty" << m_pCurrentShape->getEndPoint().x();

				//m_brush = brush;
				m_pCurrentShape->setBrush(brush);


			//m_isDrawing = true;
		}
	}

	m_brush = brush;
	//if (m_pCurrentBrushit != m_pShapeBrush.end()) *m_pCurrentBrushit = brush;
	update();
}

void KCanvas::updataBorderShapeColor(QColor pen)
{
	if (m_selectAllShape == true)
	{
		auto it = m_pShapeList.begin();
		while (it != m_pShapeList.end())
		{
			(*it)->setPen(pen);
			it++;
		}
	}
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			qDebug() << "m_pcurrentShape not empty" << m_pCurrentShape->getEndPoint().x();
			m_pCurrentShape->setPen(pen);


			//m_isDrawing = true;
		}
	}


	m_pen = pen;
	//if (m_pCurrentPenit != m_pShapePen.end()) *m_pCurrentPenit = pen;
	//update();
}

void KCanvas::updataBorderWidth(int width)
{
	if (m_selectAllShape == true)
	{
		auto it = m_pShapeList.begin();
		while (it != m_pShapeList.end())
		{
			(*it)->setWidth(width);
			it++;
		}
	}
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			qDebug() << "m_pcurrentShape not empty" << m_pCurrentShape->getEndPoint().x();

			m_pCurrentShape->setWidth(width);


			//m_isDrawing = true;
		}
	}

	m_width = width;
	//if (m_pCurrentWidthit != m_pBorderWidth.end()) *m_pCurrentWidthit = width;
	update();
}

void KCanvas::updataBorderStyle(int index)
{
	if (m_selectAllShape == true)
	{
		auto it = m_pShapeList.begin();
		while (it != m_pShapeList.end())
		{
			(*it)->setStyle(index);
			it++;
		}
	}
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			m_pCurrentShape->setStyle(index);


			//m_isDrawing = true;
		}
	}


	m_style = index;
	//if (m_pCurrentStyleit != m_pBorderStyle.end()) *m_pCurrentStyleit = index;
	update();
}

void KCanvas::selectAllShape()
{
	m_selectAllShape = true;
}

void KCanvas::copyShape()
{
	if (!m_isSelected) {
		qDebug() << "is not selected copy shape";
		return;
	}
	else
	{
		if (m_pCurrentShape == Q_NULLPTR)
		{
			qDebug() << "m_pCurrentShape is null";
		}
		else
		{
			KShape* tmp = KShapeFactory::createShape(m_pCurrentShape->getFlag());
			*tmp = (*m_pCurrentShape);
			if (m_buff.isEmpty()) m_buff.push_back(tmp);
			else m_buff[0] = tmp;
			//qDebug() << tmp->getEndPoint().x() << "." << tmp->getEndPoint().y();
		}
	}
	//qDebug() << m_pCurrentShape->getEndPoint().x() << "." << m_pCurrentShape->getEndPoint().y();

	
}

void KCanvas::shearShape()
{
	copyShape();
	deleteShape();
}

void KCanvas::deleteShape()
{
	//qDebug() << m_pShapeList.size();

	auto it = m_pShapeList.begin();
	while (it != m_pShapeList.end())
	{
		if (*it == m_pCurrentShape)
		{
			it = m_pShapeList.erase(it);
			qDebug() << "find shape and delete";
			break;
		}
		it++;
	}

	delete m_pCurrentShape;

	m_pCurrentShape = Q_NULLPTR;

	//qDebug() << m_pShapeList.size();
}

void KCanvas::pasteShape(QPoint pos)
{

	if (m_buff.isEmpty())
	{
		qDebug() << "buff is empty, cannnot paste";
	}
	else
	{
		m_pCurrentShape = KShapeFactory::createShape(m_buff[0]->getFlag());
		*m_pCurrentShape = *m_buff[0];
		//qDebug() << m_buff[0]->getEndPoint().x() << "." << m_buff[0]->getEndPoint().y();

		QPoint pos(10, 10);
		QPoint startpos = m_pCurrentShape->getStartPoint();
		QPoint endpos = m_pCurrentShape->getEndPoint();
		m_pCurrentShape->setStartPoint(startpos + pos);
		m_pCurrentShape->setEndPoint(endpos + pos);
		*m_buff[0] = *m_pCurrentShape;

		qDebug() << m_pShapeList.size();
		m_pShapeList.prepend(m_pCurrentShape);
		qDebug() << m_pShapeList.size();

	}
}

void KCanvas::setBackLayer()
{
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			int layer = (m_pCurrentShape->getLayer() + 1) % 7;
			m_pCurrentShape->setLayer(layer);
		}
		else
		{
			qDebug() << "setBacklayer currentshape is empty";
		}
	}
}

void KCanvas::setFrontLayer()
{
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			int layer = (m_pCurrentShape->getLayer() - 1) % 7;
			m_pCurrentShape->setLayer(layer);
		}
		else
		{
			qDebug() << "setFrontLayer currentshape is empty";
		}
	}
}

void KCanvas::setBackestLayer()
{
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			if (m_pCurrentShape->getLayer() == 6)
			{
				qDebug() << "current shape is in backestlayer";
			}
			else
			{
				m_pCurrentShape->setLayer(6);
			}
		}
		else
		{
			qDebug() << "setBackestLayer currentshape is empty";
		}
	}
}

void KCanvas::setFrontestLayer()
{
	if (m_isSelected)
	{
		if (m_pCurrentShape != Q_NULLPTR)
		{
			if (m_pCurrentShape->getLayer() == 0)
			{
				qDebug() << "current shape is in frontestLayer";
			}
			else
			{
				m_pCurrentShape->setLayer(0);
			}
		}
		else
		{
			qDebug() << "setFrontestLayer currentshape is empty";
		}
	}
}



QList<KShape*> KCanvas::getShapeList()
{
	return m_pShapeList;
}

void KCanvas::setShapeList(QList<KShape*> ShapeList)
{
	qDebug() << "copy shapelist";
	m_pShapeList.clear();
	qDebug() << m_pShapeList.size();
	auto it = ShapeList.begin();
	int i = 0;
	while (it != ShapeList.end())
	{
		m_pShapeList.push_back(KShapeFactory::createShape((*it)->getFlag()));
		if (i >= m_pShapeList.size())
		{
			qDebug() << "canvas error copy shapelist over split";
		}
		else
		{
			*(m_pShapeList[i]) = *ShapeList[i];
		}
		it++;
		i++;
	}
}

void KCanvas::setSvgRenderer(QSvgRenderer* renderer)
{
	m_svgRenderer = renderer;
	update(); // �����ػ�  
}

void KCanvas::onTextEditReturnPressed()
{
	QString text = textEdit->text();
	if (m_pCurrentShape != Q_NULLPTR)
	{
		m_pCurrentShape->setText(text);
	}
	else
	{
		qDebug() << "onTextEditReturnPressed currentshape is empty";
	}
	textEdit->hide();
	//m_pCurrentShape->drawShape();
}
