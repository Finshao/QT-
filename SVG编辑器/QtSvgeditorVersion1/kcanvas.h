#ifndef __K_CANVAS_H_
#define __K_CANVAS_H_

#include <QWidget>
#include <QLineEdit>
#include <QFileDialog>  
#include <QSvgRenderer>  
#include "kshape.h"
#include "kglobaldata.h"

enum class KTransType
{
	None = 0,
	TopLeft, 
	Top,
	TopRight,
	Left,
	Contains,// 全部 
	Right,
	BottomLeft,
	Bottom,
	BottomRight
};


class KCanvas : public QWidget
{
	Q_OBJECT

public:
	KCanvas(QWidget *parent = Q_NULLPTR);
	~KCanvas();

	virtual void paintEvent(QPaintEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	
	KShape* getCurrentShape(const QPoint &pos);;// 获取当前位置图形
	KTransType getTransType(const QPoint &pos); // 获取移动类型
	void updateCusorStyle(KGlobalData::KDrawFlag flag, KTransType transType);
	void dragMoveShape(KTransType transType,const QPoint &pos);
	void drawMoveAllShape(const QPoint& pos);

	void updataFillShapeColor(QBrush brush = QBrush(Qt::NoBrush), QColor pen = QColor(Qt::black));
	void updataBorderShapeColor(QColor pen = QColor(Qt::black));
	void updataBorderWidth(int width);
	void updataBorderStyle(int index);

	void selectAllShape();
	void copyShape();
	void shearShape();
	void deleteShape();
	void pasteShape(QPoint pos);
	void setBackLayer();
	void setFrontLayer();
	void setBackestLayer();
	void setFrontestLayer();


	QList <KShape*> getShapeList();
	void setShapeList(QList <KShape*> ShapeList);

	void setSvgRenderer
	(QSvgRenderer* renderer);
signals:
	void addShapeCompleted();

private:
	KShape* m_pCurrentShape;
	QList <KShape*> m_pShapeList;// 存储当前画布中的图形

	QBrush m_brush;
	//QList <QBrush> m_pShapeBrush; //存储填充颜色
	//QList <QBrush>::iterator m_pCurrentBrushit;	// 当前brush的指针

	QColor m_pen;
	//QList <QColor> m_pShapePen;
	//QList <QColor>::iterator m_pCurrentPenit;

	int m_width;
	//QList<int> m_pBorderWidth;
	//QList<int>::iterator m_pCurrentWidthit;

	int m_style;
	//QList<int> m_pBorderStyle;
	//QList<int>::iterator m_pCurrentStyleit;
	
	QPoint m_lastPos;// 记录前一次的位置
	KTransType m_TransType;// 记录移动类型
	
	bool m_isLPress;// 标记鼠标左键
	bool m_isDrawing;// 是否绘图
	bool m_isSelected;// 是否为选中

	QLineEdit* textEdit;// 用来编辑text的文本
	void KCanvas::onTextEditReturnPressed();// lineedit的槽函数

	bool m_selectAllShape;

	// 复制：shape不释放，深拷贝, 重复操作buffer.clear
	// 剪切：shape释放，深拷贝, 重复操作buffer.clear
	QList <KShape*> m_buff;
	// 删除：就是shapelist去掉后，释放内存


	// 粘贴：根据buffer创建新对象，pos更改

	// 全选状态：
	// 复制：拷贝所有
	// 剪切：复制+删除
	// 删除：清空
	// 粘贴：所有shape的pos都移动一下

	QSvgRenderer* m_svgRenderer;
};

#endif
