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
	Contains,// ȫ�� 
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
	
	KShape* getCurrentShape(const QPoint &pos);;// ��ȡ��ǰλ��ͼ��
	KTransType getTransType(const QPoint &pos); // ��ȡ�ƶ�����
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
	QList <KShape*> m_pShapeList;// �洢��ǰ�����е�ͼ��

	QBrush m_brush;
	//QList <QBrush> m_pShapeBrush; //�洢�����ɫ
	//QList <QBrush>::iterator m_pCurrentBrushit;	// ��ǰbrush��ָ��

	QColor m_pen;
	//QList <QColor> m_pShapePen;
	//QList <QColor>::iterator m_pCurrentPenit;

	int m_width;
	//QList<int> m_pBorderWidth;
	//QList<int>::iterator m_pCurrentWidthit;

	int m_style;
	//QList<int> m_pBorderStyle;
	//QList<int>::iterator m_pCurrentStyleit;
	
	QPoint m_lastPos;// ��¼ǰһ�ε�λ��
	KTransType m_TransType;// ��¼�ƶ�����
	
	bool m_isLPress;// ���������
	bool m_isDrawing;// �Ƿ��ͼ
	bool m_isSelected;// �Ƿ�Ϊѡ��

	QLineEdit* textEdit;// �����༭text���ı�
	void KCanvas::onTextEditReturnPressed();// lineedit�Ĳۺ���

	bool m_selectAllShape;

	// ���ƣ�shape���ͷţ����, �ظ�����buffer.clear
	// ���У�shape�ͷţ����, �ظ�����buffer.clear
	QList <KShape*> m_buff;
	// ɾ��������shapelistȥ�����ͷ��ڴ�


	// ճ��������buffer�����¶���pos����

	// ȫѡ״̬��
	// ���ƣ���������
	// ���У�����+ɾ��
	// ɾ�������
	// ճ��������shape��pos���ƶ�һ��

	QSvgRenderer* m_svgRenderer;
};

#endif
