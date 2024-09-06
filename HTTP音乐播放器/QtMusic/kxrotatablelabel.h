#ifndef KX_ROTATABLELABEL_H
#define KX_ROTATABLELABEL_H

#include <QLabel>

class KxRotatableLabel : public QLabel
{
    Q_OBJECT

public:
    KxRotatableLabel(QWidget* parent = 0);
    ~KxRotatableLabel();

    void setRotationEnabled(bool enabled);
    void setImage(QString& url);
    void setImage(QImage& img);

protected:
    void paintEvent(QPaintEvent* event);

private:
    QTimer* m_timer;
    bool m_enabled;
    int m_angle;
    QImage m_image;
};


#endif // !KX_ROTATABLELABEL_H

