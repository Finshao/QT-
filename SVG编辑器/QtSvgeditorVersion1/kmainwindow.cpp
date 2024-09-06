#pragma execution_character_set("utf-8")
#include "kmainwindow.h"
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QSvgGenerator>
#include <QMessageBox>

KMainWindow::KMainWindow(QWidget *parent)
    : QWidget(parent)
    ,m_pVLayout(Q_NULLPTR)
    ,m_pHLayout(Q_NULLPTR)
    ,m_pSvgMainWin(Q_NULLPTR)
    ,m_pToolBar(Q_NULLPTR)
{
    ui.setupUi(this);
    initWindows();
    initConnection();

    insertFileList();
}

void KMainWindow::initWindows()
{

    svgRenderer = new QSvgRenderer();

    m_pVLayout = new QVBoxLayout(this);
    m_pVLayout->setSpacing(0);
    m_pVLayout->setMargin(0);

    m_pHLayout = new QHBoxLayout(this);
    m_pHLayout->setSpacing(0);

    m_pToolBar = new KToolBar(this);
    m_pSvgMainWin = new KSvgMainWindow(this);
    m_pCanvasParamsBar = new KCanvasParamsBar("����",this);


    m_pHLayout->addWidget(m_pToolBar);
    m_pHLayout->addWidget(m_pSvgMainWin);
    m_pHLayout->addWidget(m_pCanvasParamsBar);
    initMenu();
    initcontextMenu();


    m_pVLayout->addLayout(m_pHLayout);
    

    setLayout(m_pVLayout);
 }

void KMainWindow::initConnection()
{
    (void)connect(m_pCanvasParamsBar->m_pWidthBox->m_pParamEdit, &QLineEdit::editingFinished, this, &KMainWindow::validateCanvasParams);
    (void)connect(m_pCanvasParamsBar->m_pHeightBox->m_pParamEdit, &QLineEdit::editingFinished, this, &KMainWindow::validateCanvasParams);
    (void)connect(m_pCanvasParamsBar->m_pColorBox, &KColorBox::pickedColor, this, &KMainWindow::validateCanvasParams1);
    (void)connect(m_pCanvasParamsBar->m_pScalingBox->m_pParamEdit, &QLineEdit::editingFinished, this, &KMainWindow::validateCanvasParams);

    (void)connect(m_pCanvasParamsBar->m_pFillColorBox, &KColorBox::pickedColor, this, &KMainWindow::validateFillShapeParams);
    (void)connect(m_pCanvasParamsBar->m_pBorderColorBox, &KColorBox::pickedColor, this, &KMainWindow::validateBorderShapeParams);
    (void)connect(m_pCanvasParamsBar->m_pBorderWidthBox->m_pParamEdit, &QLineEdit::editingFinished, this, &KMainWindow::validateBorderWidthParams);
    (void)connect(m_pCanvasParamsBar->m_pBorderStyleBox->m_pComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &KMainWindow::onComboBoxChanged);
//    (void)connect(m_pCanvasParamsBar->m_pBorderStyleBox->m_pComboBox, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, &KMainWindow::onComboBoxChanged);

    connect(m_pSvgMainWin, &KSvgMainWindow::canvasScaleChanged, this, &KMainWindow::updateCanvasScale);

    connect(m_pSvgMainWin->m_pCanvas, &KCanvas::addShapeCompleted, this, &KMainWindow::handleShapeCompleted);
}

void KMainWindow::updateCanvasScale()
{
    int scaleInt = static_cast<int>(KGlobalData::getGlobalDataIntance()->getCanvasScale() * 100);
    QString str = QString::number(scaleInt); 
    m_pCanvasParamsBar->m_pScalingBox->m_pParamEdit->setText(str);

    
}

void KMainWindow::validateFillShapeParams()
{
    insertFileList();

    QBrush brush;
    QString str = KGlobalData::getGlobalDataIntance()->getCanvasColor();

    quint32 rgbaValue = str.toUInt(nullptr, 16);
    QColor color;
    color.setRgba(rgbaValue);

    //qDebug() << "brush color is : " << str;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(color);
    m_pSvgMainWin->m_pCanvas->updataFillShapeColor(brush);
}

void KMainWindow::validateBorderShapeParams()
{

    insertFileList();

    QString str = KGlobalData::getGlobalDataIntance()->getCanvasColor();
    quint32 rgbaValue = str.toUInt(nullptr, 16);
    QColor color;
    color.setRgba(rgbaValue);

    m_pSvgMainWin->m_pCanvas->updataBorderShapeColor(color);
}

void KMainWindow::validateBorderWidthParams()
{

    insertFileList();

    int width = m_pCanvasParamsBar->m_pBorderWidthBox->m_pParamEdit->text().toInt();
    KGlobalData::getGlobalDataIntance()->setBorderWidth(width);

    m_pSvgMainWin->m_pCanvas->updataBorderWidth(width);
}

void KMainWindow::onComboBoxChanged(int index)
{
    insertFileList();
    int style;
    if (index == 0) style = 1;
    else if (index == 1) style = 2;
    else if (index == 2) style = 3;
    else if (index == 3) style = 4;
    else if (index == 4) style = 5;
    m_pSvgMainWin->m_pCanvas->updataBorderStyle(style);
}


void KMainWindow::validateCanvasParams()
{
    insertFileList();
    qint32 width = m_pCanvasParamsBar->m_pWidthBox->m_pParamEdit->text().toInt();
    qint32 height = m_pCanvasParamsBar->m_pHeightBox->m_pParamEdit->text().toInt();
    // ������������
    qreal scale = m_pCanvasParamsBar->m_pScalingBox->m_pParamEdit->text().toInt() * 1.0 / 100;

    qDebug() << "scale is " << scale;

    KGlobalData::getGlobalDataIntance()->setCanvasWidth(width);
    KGlobalData::getGlobalDataIntance()->setCanvasHeight(height);
    KGlobalData::getGlobalDataIntance()->setCanvaScale(scale);
    
    m_pSvgMainWin->m_pCanvas->resize(width * (scale), height * (scale));

    //m_pSvgMainWin->changePostion();


}

void KMainWindow::validateCanvasParams1()
{

    insertFileList();
    m_pSvgMainWin->m_pCanvas->setStyleSheet(QString("background-color:#%1").arg(KGlobalData::getGlobalDataIntance()->getCanvasColor()));



}


void KMainWindow::saveFile()
{
    QString saveFilePath = QFileDialog::getSaveFileName(this, QStringLiteral(""), nullptr, QStringLiteral("SVG�ļ�(*.svg)"));

    if (saveFilePath.isEmpty())
    {
        return;
    }
    QSvgGenerator generator;
    generator.setFileName(saveFilePath);
    generator.setSize(QSize(m_pSvgMainWin->m_pCanvas->width(), m_pSvgMainWin->m_pCanvas->height()));
    generator.setViewBox(QRect(0, 0, m_pSvgMainWin->m_pCanvas->width(), m_pSvgMainWin->m_pCanvas->height()));
    QPainter painter;
    painter.begin(&generator);
    m_pSvgMainWin->m_pCanvas->render(&painter);
    painter.end();
}

void KMainWindow::saveFileAsPng()
{
    QString saveFilePath = QFileDialog::getSaveFileName(this,
        tr("����PNG�ļ�"),
        nullptr,
        tr("PNG�ļ� (*.png);;�����ļ� (*.*)"));

    if (saveFilePath.isEmpty())
        return;

    // ȥ���ļ���ĩβ���ܴ��ڵĶ�����չ������������������²�̫���ܣ�  
    QFileInfo fileInfo(saveFilePath);
    QString baseName = fileInfo.baseName();
    QString extension = fileInfo.suffix();

    // �����չ���Ѿ���png������Ҫ�ٴ����  
    if (extension.compare("png", Qt::CaseInsensitive) != 0) {
        saveFilePath = fileInfo.path() + "/" + baseName + ".png";
    }

    // ����m_pCanvas�ĵ�ǰ����  
    QPixmap pixmap = m_pSvgMainWin->m_pCanvas->grab();

    // ��QPixmap����ΪPNG�ļ�  
    if (!pixmap.save(saveFilePath, "PNG")) {
        // �������ʧ�ܣ�����ʾ������Ϣ  
        QMessageBox::warning(this, tr("�������"),
            tr("�޷������ļ��� %1.").arg(saveFilePath));
    }
    else {
        // ��ѡ����ʾһ����Ϣ�����־����ʾ�ļ��ѳɹ�����  
        qDebug() << "PNG file saved to:" << saveFilePath;
    }
}


void KMainWindow::openFile()
{
    on_newFile();

    qDebug() << "openfile";
    QString openFilePath = QFileDialog::getOpenFileName(this, QStringLiteral("��SVG�ļ�"), nullptr, QStringLiteral("SVG�ļ�(*.svg)"));

    if (openFilePath.isEmpty())
    {
        return;
    }

    // ����һ��QSvgRenderer������SVG�ļ�  
    svgRenderer->load(openFilePath);
    //qDebug() << "??????????????????????";
    // ����ļ��Ƿ�ɹ�����  
    if (!svgRenderer->isValid())
    {
        qDebug() << "�޷�����SVG�ļ���" << openFilePath;
        return;
    }
    else
    {
        qDebug() << "����SVG�ļ��ɹ���" << openFilePath;
    }
    m_pSvgMainWin->m_pCanvas->setSvgRenderer(svgRenderer); // ����KCanvas��һ��setSvgRenderer������������Ⱦ��  

}

void KMainWindow::on_newFile()
{

    int reply = QMessageBox::question(this, "ȷ��", "�Ƿ񱣴浱ǰ������",
        QMessageBox::Yes | QMessageBox::No);

    // �����û��Ļش��������Ƿ񱣴��ļ�  
    if (reply == QMessageBox::Yes) {
        saveFile();
    }

    int width = 600;
    int height = 400;
    QString color = "FFFFFF";
    qreal scale = 1.0;

    KGlobalData::getGlobalDataIntance()->setCanvaScale(scale);
    KGlobalData::getGlobalDataIntance()->setCanvasColor(color);
    KGlobalData::getGlobalDataIntance()->setCanvasHeight(height);
    KGlobalData::getGlobalDataIntance()->setCanvasWidth(width);

    m_fileList.clear();
    m_index = 0;
    
    QList <KShape*> ShapeList;
    m_pSvgMainWin->m_pCanvas->setShapeList(ShapeList);

    QSvgRenderer* svg = svgRenderer;
    svgRenderer = new QSvgRenderer();
    delete svg;
    m_pSvgMainWin->m_pCanvas->setSvgRenderer(svgRenderer);

    // �ֶ�ˢ�»���״̬
    m_pCanvasParamsBar->m_pWidthBox->m_pParamEdit->setText(QString::number(width));
    m_pCanvasParamsBar->m_pHeightBox->m_pParamEdit->setText(QString::number(height));
    m_pCanvasParamsBar->m_pScalingBox->m_pParamEdit->setText(QString::number(scale * 100));
    m_pSvgMainWin->m_pCanvas->resize(width * (scale), height * (scale));
    m_pSvgMainWin->m_pCanvas->setStyleSheet(QString("background-color:#%1").arg(KGlobalData::getGlobalDataIntance()->getCanvasColor()));
}

void KMainWindow::insertFileList()
{
    if (m_index == m_fileList.size()) m_index++;
    else
    {
        m_fileList.erase(m_fileList.begin() + m_index + 1, m_fileList.end());
        m_index++;
    }
    m_svgFile.setSVGFile();
    setShapeList();
    m_svgFile.test();
    m_fileList.push_back(m_svgFile);
    qDebug() << "filelist size is:" << m_fileList.size();
}

void KMainWindow::undoShape()
{
    //�ص���һ��״̬
    clear();
    //initWindows();
    //initConnection();
    if (m_index == m_fileList.size())
    {
        insertFileList();
        m_index--;
    }


    if (m_index - 1 < 0) {
        qDebug() << "uodo error, dont have more undofile";
    }
    else
    {
        //qDebug() << "copy";
        KSVGFile file = m_fileList[m_index - 1];
        m_index--;

        qDebug() << "--------" << "size :" << m_fileList.size() << "index" << m_index;

        int width = file.m_canvasWidth;
        int height = file.m_canvasHeight;
        QString color = file.m_canvasColor;
        qreal scale = file.m_canvasSCale;

        KGlobalData::getGlobalDataIntance()->setCanvaScale(file.m_canvasSCale);
        KGlobalData::getGlobalDataIntance()->setCanvasColor(file.m_canvasColor);
        KGlobalData::getGlobalDataIntance()->setCanvasHeight(file.m_canvasHeight);
        KGlobalData::getGlobalDataIntance()->setCanvasWidth(file.m_canvasWidth);

        qDebug() << file.m_canvasWidth << "," << file.m_canvasHeight;
        qDebug() << KGlobalData::getGlobalDataIntance()->getCanvasWidth() << "," << KGlobalData::getGlobalDataIntance()->getCanvasHeight();

        QList <KShape*>ShapeList = file.m_pShapeList;
        m_pSvgMainWin->m_pCanvas->setShapeList(ShapeList);

        // �ֶ�ˢ�»���״̬
        m_pCanvasParamsBar->m_pWidthBox->m_pParamEdit->setText(QString::number(width));
        m_pCanvasParamsBar->m_pHeightBox->m_pParamEdit->setText(QString::number(height));
        m_pCanvasParamsBar->m_pScalingBox->m_pParamEdit->setText(QString::number(scale * 100));
        m_pSvgMainWin->m_pCanvas->resize(width * (scale), height * (scale));
        m_pSvgMainWin->m_pCanvas->setStyleSheet(QString("background-color:#%1").arg(KGlobalData::getGlobalDataIntance()->getCanvasColor()));
    }

    update();

}

void KMainWindow::redoShape()
{



    if (m_index + 1 < m_fileList.size())
    {
        //qDebug() << "copy";
        KSVGFile file = m_fileList[m_index + 1];
        m_index++;

        int width = file.m_canvasWidth;
        int height = file.m_canvasHeight;
        QString color = file.m_canvasColor;
        qreal scale = file.m_canvasSCale;

        KGlobalData::getGlobalDataIntance()->setCanvaScale(file.m_canvasSCale);
        KGlobalData::getGlobalDataIntance()->setCanvasColor(file.m_canvasColor);
        KGlobalData::getGlobalDataIntance()->setCanvasHeight(file.m_canvasHeight);
        KGlobalData::getGlobalDataIntance()->setCanvasWidth(file.m_canvasWidth);

        qDebug() << file.m_canvasWidth << "," << file.m_canvasHeight;
        qDebug() << KGlobalData::getGlobalDataIntance()->getCanvasWidth() << "," << KGlobalData::getGlobalDataIntance()->getCanvasHeight();

        QList <KShape*>ShapeList = file.m_pShapeList;
        m_pSvgMainWin->m_pCanvas->setShapeList(ShapeList);

        // �ֶ�ˢ�»���״̬
        m_pCanvasParamsBar->m_pWidthBox->m_pParamEdit->setText(QString::number(width));
        m_pCanvasParamsBar->m_pHeightBox->m_pParamEdit->setText(QString::number(height));
        m_pCanvasParamsBar->m_pScalingBox->m_pParamEdit->setText(QString::number(scale * 100));
        m_pSvgMainWin->m_pCanvas->resize(width * (scale), height * (scale));
        m_pSvgMainWin->m_pCanvas->setStyleSheet(QString("background-color:#%1").arg(KGlobalData::getGlobalDataIntance()->getCanvasColor()));
    }
    else
    {
        qDebug() << "redo error, dont have more redofile ";
    }
}

void KMainWindow::clear()
{

    

}

void KMainWindow::handleShapeCompleted()
{
    insertFileList();
}


void KMainWindow::initMenu()
{

    // �����˵���  
    menuBar = new QMenuBar(this);


    // �����ļ��˵�  
    QMenu* fileMenu = menuBar->addMenu(tr("&File"));  // ʹ��tr()����֧�ֹ��ʻ�  

    QString styleSheet = "QMenu { color: white; background-color: black; selection-background-color: #3399ff; }";
    fileMenu->setStyleSheet(styleSheet);

    // ����½��ļ��Ķ���  
    QAction* newFileAction = new QAction(tr("&New File"), this);
    newFileAction->setShortcut(QKeySequence::New);  // ʹ�ñ�׼�Ŀ�ݼ�Ctrl+N  
    connect(newFileAction, &QAction::triggered, this, &KMainWindow::on_newFile);
    fileMenu->addAction(newFileAction);

    // ��Ӵ�SVG�ļ��Ķ���  
    QAction* openSvgAction = new QAction(tr("&Open SVG..."), this);
    openSvgAction->setShortcut(QKeySequence::Open);
    connect(openSvgAction, &QAction::triggered, this, &KMainWindow::openFile);
    fileMenu->addAction(openSvgAction);

    // ��ӱ����ļ��Ķ���  
    QAction* saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &KMainWindow::saveFile);
    fileMenu->addAction(saveAction);

    // ��ӵ���PNG�ļ��Ķ���  
    QAction* exportPngAction = new QAction(tr("&Export PNG..."), this);
    exportPngAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    connect(exportPngAction, &QAction::triggered, this, &KMainWindow::saveFileAsPng);
    fileMenu->addAction(exportPngAction);

    //QString styleSheet = "QMenuBar { color: white; background-color: black; selection-background-color: #3399ff; }";
    menuBar->setStyleSheet(styleSheet);


    // ���˵�����ӵ������У�������������������m_pVLayout��  
    m_pVLayout->addWidget(menuBar);
}

void KMainWindow::initcontextMenu()
{
    QMenu* contextMenu = new QMenu(this);

    // ���ѡ��˵�  
    QAction* action1 = contextMenu->addAction(tr("ȫѡ\tA"));
    //action1->setShortcut(QKeySequence(Qt::Key_A));
    (void)connect(action1, &QAction::triggered, this, &KMainWindow::on_actionSelectAll_triggered);

    QAction* action2 = contextMenu->addAction(tr("����(undo)\tZ"));
    //action2->setShortcut(QKeySequence(Qt::Key_Z));
    (void)connect(action2, &QAction::triggered, this, &KMainWindow::on_actionUndo_triggered);

    QAction* action3 = contextMenu->addAction(tr("����(redo)\tY"));
    //action3->setShortcut(QKeySequence(Qt::Key_Y));
    (void)connect(action3, &QAction::triggered, this, &KMainWindow::on_actionRedo_triggered);

    QAction* action4 = contextMenu->addAction(tr("����\tX"));
    //action4->setShortcut(QKeySequence(Qt::Key_X));
    (void)connect(action4, &QAction::triggered, this, &KMainWindow::on_actionCut_triggered);

    QAction* action5 = contextMenu->addAction(tr("����\tC"));
    //action5->setShortcut(QKeySequence(Qt::Key_C));
    (void)connect(action5, &QAction::triggered, this, &KMainWindow::on_actionCopy_triggered);

    QAction* action6 = contextMenu->addAction(tr("ճ��\tV"));
    //action6->setShortcut(QKeySequence(Qt::Key_V));
    (void)connect(action6, &QAction::triggered, this, &KMainWindow::on_actionPaste_triggered);

    QAction* action7 = contextMenu->addAction(tr("��д\tD"));
    //action7->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_D)); // �Զ����ݼ�  
    (void)connect(action7, &QAction::triggered, this, &KMainWindow::on_actionDuplicate_triggered);

    QAction* action8 = contextMenu->addAction(tr("ɾ��\tDelete"));
    //action8->setShortcut(QKeySequence(Qt::Key_Delete));
    (void)connect(action8, &QAction::triggered, this, &KMainWindow::on_actionDelete_triggered);

    QAction* action9 = contextMenu->addAction(tr("����һ��\tCtrl+Up"));
    //action9->setShortcut(QKeySequence(Qt::Key_Up));
    (void)connect(action9, &QAction::triggered, this, &KMainWindow::on_actionBringToFront_triggered);

    QAction* action10 = contextMenu->addAction(tr("����һ��\tCtrl+Down"));
    //action10->setShortcut(QKeySequence(Qt::Key_Down));
    (void)connect(action10, &QAction::triggered, this, &KMainWindow::on_actionSendToBack_triggered); // ����ĺ�����  

    QAction* action11 = contextMenu->addAction(tr("����ǰ\tCtrl+Left"));
    //action11->setShortcut(QKeySequence(Qt::Key_Left));
    (void)connect(action11, &QAction::triggered, this, &KMainWindow::on_actionBringToFrontMost_triggered); // �����������Ҳ��������ǰ  

    QAction* action12 = contextMenu->addAction(tr("�����\tCtrl+Right"));
    //action12->setShortcut(QKeySequence(Qt::Key_Right));
    (void)connect(action12, &QAction::triggered, this, &KMainWindow::on_actionSendToBackMost_triggered); // ����ĺ�����

    // ���Ҽ��˵��� m_pSvgMainWin �ؼ�����  
    // ע�⣺������� m_pSvgMainWin �̳��� QWidget �������࣬��������Ҫ�� m_pSvgMainWin ����ʾ�Ҽ��˵�  
    m_pSvgMainWin->setContextMenuPolicy(Qt::CustomContextMenu);
    QString styleSheet = "QMenu { color: white; background-color: black; selection-background-color: #3399ff; }";
    contextMenu->setStyleSheet(styleSheet);

    connect(m_pSvgMainWin, &QWidget::customContextMenuRequested, [this, contextMenu](const QPoint& pos) 
        {
        m_mousePostion = m_pSvgMainWin->mapToGlobal(pos);
        contextMenu->exec(m_pSvgMainWin->mapToGlobal(pos));
        });
}

QList<KShape*> KMainWindow::getShapeList()
{
    return m_pSvgMainWin->getShapeList();
}

void KMainWindow::setShapeList()
{
    QList<KShape*> ShapeList = getShapeList();
    m_svgFile.setShapeList(ShapeList);
}

void KMainWindow::keyPressEvent(QKeyEvent* event)
{
    // ����Ƿ�����Ctrl��  
    if (event->modifiers() & Qt::ControlModifier) {
        // ���ڼ�����ĸ���ĸ��������  
        switch (event->key()) {
        case Qt::Key_N:
            on_newFile();
            break;
        case Qt::Key_O:
            openFile();
            break;
            // ... Ϊ������ĸ���ظ������ģʽ  
        case Qt::Key_S:
            saveFile();
            break;
            // �Դ����ƣ�ֱ�� Qt::Key_P  
        case Qt::Key_E:
            saveFileAsPng();
            break;
        case Qt::Key_A:
            on_actionSelectAll_triggered();
            break;
        case Qt::Key_Z:
            on_actionUndo_triggered();
            break;
        case Qt::Key_Y:
            on_actionRedo_triggered();
            break;
        case Qt::Key_X:
            on_actionCut_triggered();
            break;
        case Qt::Key_C:
            on_actionCopy_triggered();
            break;
        case Qt::Key_V:
            on_actionPaste_triggered();
            break;
        case Qt::Key_D:
            on_actionDuplicate_triggered();
            break;
        case Qt::Key_Delete:
            on_actionDelete_triggered();
            break;
        case Qt::Key_Up:
            on_actionBringToFront_triggered();
            break;
        case Qt::Key_Down:
            on_actionSendToBack_triggered();
            break;
        case Qt::Key_Left:
            on_actionBringToFrontMost_triggered();
            break;
        case Qt::Key_Right:
            on_actionSendToBackMost_triggered();
            break;
        default:
            // ����������ǹ��ĵļ�������ѡ����û����keyPressEvent  
            // ����ʲôҲ����������������ʲôҲ������  
            break;
        }
    }

    update();

    // ����㻹����������Ctrl+��ĸ�ļ���Ĭ����Ϊ��  
    // ����ȡ�������ע�������û����keyPressEvent  
    // QMainWindow::keyPressEvent(event);  
}


void KMainWindow::on_actionSelectAll_triggered()
{
    // ʵ��ȫѡ�߼� 
    m_pSvgMainWin->on_actionSelectAll_triggered();
    
}

void KMainWindow::on_actionUndo_triggered()
{
    // ʵ�ֳ����߼�  
    m_pSvgMainWin->on_actionUndo_triggered();
    undoShape();
}

void KMainWindow::on_actionRedo_triggered()
{
    // ʵ�������߼�  
    m_pSvgMainWin->on_actionRedo_triggered();
    redoShape();
}

void KMainWindow::on_actionCut_triggered()
{
    // ʵ�ּ����߼�  
    insertFileList();
    m_pSvgMainWin->on_actionCut_triggered();
}

void KMainWindow::on_actionCopy_triggered()
{
    // ʵ�ָ����߼�  
    m_pSvgMainWin->on_actionCopy_triggered();
}

void KMainWindow::on_actionPaste_triggered()
{
    // ʵ��ճ���߼�  
    insertFileList();
    m_pSvgMainWin->on_actionPaste_triggered(m_mousePostion);
}

void KMainWindow::on_actionDuplicate_triggered()
{
    // ʵ�ָ�д�߼�  
    m_pSvgMainWin->on_actionDuplicate_triggered(m_mousePostion);
}

void KMainWindow::on_actionDelete_triggered()
{
    // ʵ��ɾ���߼�  
    insertFileList();
    m_pSvgMainWin->on_actionDelete_triggered();
}

void KMainWindow::on_actionBringToFront_triggered()
{
    // ʵ�ֽ��������ڵ�ǰ�����һ���߼������ܲ������ϲ㣩  
    insertFileList();
    m_pSvgMainWin->on_actionBringToFront_triggered();
}


void KMainWindow::on_actionSendToBack_triggered()
{
    // ʵ�ֽ�����������һ����߼�  
    insertFileList();
    m_pSvgMainWin->on_actionSendToBack_triggered();
}


void KMainWindow::on_actionBringToFrontMost_triggered()
{
    // ʵ�ֽ��������������߼�  
    insertFileList();
    m_pSvgMainWin->on_actionBringToFrontMost_triggered();
}

void KMainWindow::on_actionSendToBackMost_triggered()
{
    // ʵ�ֽ�����������ײ���߼�  
    insertFileList();
    m_pSvgMainWin->on_actionSendToBackMost_triggered();
}