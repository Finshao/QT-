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
        m_timer->start(50);//���޸���ת��֡��
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

    // ��ȡ�ؼ��ĳߴ�
    const QSize widgetSize = this->size();

    // ȷ��m_image�Ѿ�����
    if (m_image.isNull()) {
        qDebug() << "img is empty";
        m_image = QImage(":/image/Eason.jpg");
    }

    // ����ͼ������Ӧ�ؼ���С
    QPixmap pixmap = QPixmap::fromImage(m_image).scaled(widgetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // ����Բ���ɰ�
    QPainterPath path;
    path.addEllipse(pixmap.rect());
    painter.setClipPath(path);

    // ������ת���ĵ�
    QPointF center(widgetSize.width() / 2, widgetSize.height() / 2);
    painter.translate(center);

    if (m_enabled) {
        painter.rotate(m_angle);
        m_angle += 1;
        m_angle %= 360;
    }

    // ����ͼ�񣬿������ĵ��ͼ��ߴ�
    painter.drawPixmap(-pixmap.width() / 2, -pixmap.height() / 2, pixmap);
}