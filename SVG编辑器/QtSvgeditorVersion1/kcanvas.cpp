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

	// 设置背景颜色为白色
	setStyleSheet("background-color:#FFFFFF");

	KGlobalData::getGlobalDataIntance()->setCanvasColor("FFFFFF");
	resize(KGlobalData::getGlobalDataIntance()->getCanvasWidth(),
		KGlobalData::getGlobalDataIntance()->getCanvasHeight());

	setMouseTracking(true);//设置鼠标追踪
	setFocusPolicy(Qt::ClickFocus);// 单击获取焦点

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

// 绘图事件处理
void KCanvas::paintEvent(QPaintEvent* event)
{
#if 0
	if (!m_pShapeList->isEmpty())
	{
			//遍历图形对象的  List
		for (QList<KShape*>::iterator it = m_pShapeList->begin();
								it != m_pShapeList->end(); ++it)
			(*it)->drawShape(this);// 绘制
	}
#endif
	QPainter painter(this);
	if (m_svgRenderer && m_svgRenderer->isValid())  
	{  
	    m_svgRenderer->render(&painter);  
	} 
	if (!m_pShapeList.isEmpty())
	{
		
		// 这里从第五个图层开始绘画
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
		

		// 如果全选，这里高亮一下
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
		if (m_isDrawing) // 图形绘制标志为 true，则表示正在绘制该图形，此时需要不断重新绘制
			qDebug() << "paint";
			m_pCurrentShape->drawShape(this);

		if (m_isSelected) // 图形选中，则绘制选中框
			m_pCurrentShape->drawOutLine(this);
	}
}

void KCanvas::mousePressEvent(QMouseEvent* event)
{
	//获取当前需要绘制的图形类型
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();
	
	if (flag == KGlobalData::KDrawFlag::NoneDrawFlag)
		return;
	
	if (event->buttons() == Qt::LeftButton)
	{
		m_isLPress = true;// 记录鼠标左键已经点击
		if (flag == KGlobalData::KDrawFlag::MouseDrawFlag)	// 这里时鼠标选中，开始记录鼠标左键后的position，为移动做准备
		{
			m_pCurrentShape = getCurrentShape(event->pos());
			if (m_pCurrentShape != Q_NULLPTR)
			{
				m_lastPos = event->pos();// 当前位置移动的最后位置(会更新)
				m_isSelected = true;// 设置选中标志
				m_TransType = getTransType(event->pos());// 获取移动类型
			}
			else
			{
				m_isSelected = false;
				m_selectAllShape = false;
			}
		}
		// 说明要开始创建新的图形了
		// 这里自由线独立逻辑,因为其矩形start,end会随着线的变化而变化
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
			// shape的类型flag唯一，只有在创建时定义一次
			m_pCurrentShape->setFlag(KGlobalData::getGlobalDataIntance()->getDrawFlag());


			// 这里点击是先把point的end也设置了，避免点击就draw的bug
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
	updateCusorStyle(flag, transType);//选中时设置鼠标选中样式

	if (event->buttons() != Qt::LeftButton)//bug : 在不点击鼠标左键并在在选中状态，移动鼠标会移动图形
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

		// 这里做处理，如果是全选模式，每一个shape都要移动
		if (m_selectAllShape == true)
		{
			
			drawMoveAllShape(event->pos());
		}
		else
		{
			
			dragMoveShape(transType, event->pos());//移动选中图形
		}
	}

	update();
}

void KCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();

	if (m_isLPress) // 鼠标左键按下后释放，则表示移动或者绘图完毕
	{
		if (flag != KGlobalData::KDrawFlag::MouseDrawFlag)// 如果是绘图
		{
			if (m_pCurrentShape != Q_NULLPTR)
			{
				if (m_pCurrentShape->isValid())// 判断图形矩形是否有效
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
			// 这里设置一下text非选中状态直接提交文本，避免bug
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
	//获取当前需要绘制的图形类型
	KGlobalData::KDrawFlag flag = KGlobalData::getGlobalDataIntance()->getDrawFlag();

	if (flag == KGlobalData::KDrawFlag::NoneDrawFlag)
		return;

	if (flag == KGlobalData::KDrawFlag::MouseDrawFlag)	// 这里时鼠标选中
	{
		m_pCurrentShape = getCurrentShape(event->pos());
		if (m_pCurrentShape != Q_NULLPTR && m_pCurrentShape->isTextEdit())
		{
			qDebug() << "success double";
			qreal scale = KGlobalData::getGlobalDataIntance()->getCanvasScale();
			textEdit->setText(m_pCurrentShape->getText()); // 假设getText()返回形状的当前文本  
			double width = m_pCurrentShape->getEndPoint().x() - m_pCurrentShape->getStartPoint().x();
			double height = m_pCurrentShape->getEndPoint().y() - m_pCurrentShape->getStartPoint().y();
			textEdit->setGeometry(m_pCurrentShape->getStartPoint().x() * scale, m_pCurrentShape->getStartPoint().y() * scale, width * scale, height * scale);
			textEdit->show();
			textEdit->setFocus(); // 给予文本编辑器焦点  
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

	// 获取图形矩形
	QRect rect = m_pCurrentShape->getShapeRect();

	if (!rect.contains(pos))
		return KTransType::None;
	
	//qDebug() << "rect = " << rect.topLeft().x();
	//qDebug() << "rect = " << rect.topLeft().y();

	// 判断是整体移动，局部拖拽移动
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
	// 如果已经选中
	if (m_isSelected
		&& flag == KGlobalData::KDrawFlag::MouseDrawFlag
		&& transType != KTransType::None)
	{
		//TODO：变换图标,设置不同的鼠标样式,根据点击位置，设置不同的鼠标样式
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
		在移动过程中，分为两种移动方式
			(1) 整体移动  contains
			(2) 基于某一个方向的拖拽移动

	*/
	switch (m_TransType)
	{
	case KTransType::None:
		return;
	case KTransType::TopLeft:
		// 如果是左上角，则当前位置为新的矩形起始位置
		m_pCurrentShape->moveTopLeft(pos);
		break;
	case KTransType::Top:
		// 如果是向上移动，则修改起始位置的 y 坐标
		m_pCurrentShape->moveTop(pos);
		break;
	case KTransType::TopRight:
		m_pCurrentShape->moveTopRight(pos);
		break;
	case KTransType::Left:
		m_pCurrentShape->moveLeft(pos);
		break;
	case KTransType::Contains: // 整体移动
	{
		// m_lastPos 为选中时光标位置，在移动过程中不断赋值为前一次的位置
		QPoint newpos = pos -  m_lastPos; // 计算要移动的偏移
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
	QPoint newpos = pos - m_lastPos; // 计算要移动的偏移
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
	// 当为选中状态，设置一个current的color
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
	update(); // 触发重绘  
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
