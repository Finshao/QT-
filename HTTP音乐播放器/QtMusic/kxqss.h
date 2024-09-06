#ifndef KX_QSS_H
#define KX_QSS_H

#include <QString>

inline QString KxPlayerButtonPlay()
{
	return "QPushButton"
		"{"
		"background-image:url(:/image/buttonstart.png);"
		"}";
}

inline QString KxPlayerButtonStop()
{
	return "QPushButton"
		"{"
		"background-image:url(:/image/buttonstop.png);"
		"}";
}

inline QString m_imageButtonStyle()
{
	return "QPushButton"
		"{"
		"   background-repeat: no-repeat;"
		"   background-position: center center;"
		"   background-size: contain;"
		"   border: none;"
		"}";
}
inline QString m_positionSliderStyle()
{
    return "QSlider::groove:horizontal"
        "{"
        "   border: 1px solid #999999;"
        "   height: 8px;"
        "   background: #CCCCCC;"
        "   margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal"
        "{"
        "   background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.6 white, stop:0.8 gray);"
        "   width: 16px;"
        "   margin: -4px 0;"
        "   border: 1px solid #999999;"
        "   border-radius: 8px;"
        "}"
        "QSlider::sub-page:horizontal"
        "{"
        "   background: red;"
        "   border: 1px solid #999999;"
        "   height: 8px;"
        "   margin: 2px 0;"
        "}"
        "QSlider::add-page:horizontal"
        "{"
        "   background: #CCCCCC;"
        "   border: 1px solid #999999;"
        "   height: 8px;"
        "   margin: 2px 0;"
        "}";
}

inline QString m_volumeSliderStyle()
{
    return "QSlider#positionSlider, QSlider#volumeSlider {"
        "   border: 1px solid #999999;"
        "   background: #CCCCCC;"
        "}"
        "QSlider#positionSlider::groove:horizontal, QSlider#volumeSlider::groove:vertical {"
        "   height: 8px;"
        "   margin: 2px 0;"
        "}"
        "QSlider#positionSlider::handle:horizontal, QSlider#volumeSlider::handle:vertical {"
        "   background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5, stop:0.6 white, stop:0.8 gray);"
        "   width: 16px;"
        "   margin: -4px 0;"
        "   border: 1px solid #999999;"
        "   border-radius: 8px;"
        "}"
        "QSlider#positionSlider::sub-page:horizontal, QSlider#volumeSlider::sub-page:vertical {"
        "   background: red;"
        "   border: 1px solid #999999;"
        "   height: 8px;"
        "   margin: 2px 0;"
        "}"
        "QSlider#positionSlider::add-page:horizontal, QSlider#volumeSlider::add-page:vertical {"
        "   background: #CCCCCC;"
        "   border: 1px solid #999999;"
        "   height: 8px;"
        "   margin: 2px 0;"
        "}";
}

#endif