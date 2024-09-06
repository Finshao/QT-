#ifndef __K_MAINWINDOW_H_
#define __K_MAINWINDOW_H_
#include <QtWidgets/QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QList>
#include <QFileDialog>  
#include <QSvgRenderer>  
#include <QPainter>  
#include <QKeyEvent>  

#include "ksvgmainwindow.h"
#include "ktoolbar.h"
#include "kcanvasparamsbar.h"

#include "ui_kmainwindow.h"
#include "ksvgfile.h"

class KMainWindow : public QWidget
{
    Q_OBJECT

public:
    KMainWindow(QWidget *parent = Q_NULLPTR);
    void initWindows();
    void initConnection();
    void updateCanvasScale();

    void initMenu();
    void initcontextMenu();

    QList <KShape*> getShapeList();
    void setShapeList();

    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void validateCanvasParams();
    void validateCanvasParams1();
    void validateFillShapeParams();
    void validateBorderShapeParams();
    void validateBorderWidthParams();
    void onComboBoxChanged(int index);
    void saveFile();
    void saveFileAsPng();
    void openFile();
    void on_newFile();
    void insertFileList();
    void undoShape();
    void redoShape();
    void clear();
    void handleShapeCompleted();

    // 右键菜单对应功能
    void on_actionSelectAll_triggered();
    void on_actionUndo_triggered();
    void on_actionRedo_triggered();
    void on_actionCut_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_actionDuplicate_triggered();
    void on_actionDelete_triggered();
    void on_actionSendToBack_triggered();
    void on_actionBringToFront_triggered();
    void on_actionSendToBackMost_triggered();
    void on_actionBringToFrontMost_triggered();


private:
    Ui::KMainWindowClass ui;
    QVBoxLayout *m_pVLayout;
    QHBoxLayout *m_pHLayout;
    KToolBar *m_pToolBar;
    KSvgMainWindow *m_pSvgMainWin;
    KCanvasParamsBar *m_pCanvasParamsBar;
    QMenuBar* menuBar;

    QPoint m_mousePostion;

    QList<KSVGFile> m_fileList;
    KSVGFile m_svgFile;
    int m_index = 0;

    QSvgRenderer* svgRenderer;
};

#endif
