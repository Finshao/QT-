#include "kxrotatablelabel.h"
#include <QTimer>
#include <QPainter>
#include <QDebug>
#include <QPainterPath>

KxRotatableLabel::KxRotatableLabel(QWidget* parent) : QLabel(parent)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_enabled = false;
    m_angle = 0;
}

KxRotatableLabel::~KxRotatableLabel()
{
    delete m_timer;
}

void KxRotatableLabel::setRotationEnabled(bool enabled)
{
    m_enabled = enabled;

    if (m_enabled)
        m_timer->start(50);//可修改旋转的帧率
    else
        m_timer->stop();
}

void KxRotatableLabel::setImage(QString& url)
{
    m_image = QImage(url);
    if (m_image.isNull()) qDebug() << "img locad error";
}

void KxRotatableLabel::setImage(QImage& img)
{
    m_image = img;
    if (m_image.isNull()) qDebug() << "img locad error";
}

void KxRotatableLabel::paintEvent(QPaintEvent* event)
{
    QLabel::paintEvent(event);
    QPainter painter(this);

    // 获取控件的尺寸
    const QSize widgetSize = this->size();

    // 确保m_image已经加载
    if (m_image.isNull()) {
        qDebug() << "img is empty";
        m_image = QImage(":/image/Eason.jpg");
    }

    // 缩放图像以适应控件大小
    QPixmap pixmap = QPixmap::fromImage(m_image).scaled(widgetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建圆形蒙版
    QPainterPath path;
    path.addEllipse(pixmap.rect());
    painter.setClipPath(path);

    // 计算旋转中心点
    QPointF center(widgetSize.width() / 2, widgetSize.height() / 2);
    painter.translate(center);

    if (m_enabled) {
        painter.rotate(m_angle);
        m_angle += 1;
        m_angle %= 360;
    }

    // 绘制图像，考虑中心点和图像尺寸
    painter.drawPixmap(-pixmap.width() / 2, -pixmap.height() / 2, pixmap);
}