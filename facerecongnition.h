#ifndef FACERECONGNITION_H
#define FACERECONGNITION_H

#include <QWidget>
#include <face.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <QDesktopWidget>
#include <QApplication>
#include <QEvent>
#include <QTimer>
#include <QScreen>
#include <QMouseEvent>
#include <string>
#include <QPropertyAnimation>
#include <QThread>
#include <QGraphicsDropShadowEffect>
#include <QRgb>
#include <QImage>

#include "threadfacerecord.h"
#include "managementinfo.h"
namespace Ui {
class faceRecongnition;
class InformationEntry;
}

class InformationEntry:public QWidget{
    Q_OBJECT

public:
    explicit InformationEntry(QWidget *parent=nullptr);
    Ui::InformationEntry* getUi();
    ~InformationEntry();
signals:
    void entrySuccess();
private:
    Ui::InformationEntry *uiInf;
};

class faceRecongnition : public QWidget
{
    Q_OBJECT

public:
    faceRecongnition(QString account,QWidget *parent = nullptr);
    //将cv::Mat转化为QImage
    QImage Mat2Image(const cv::Mat mimage);
    //cv::Mat norm_0_255(InputArray _src);

    //窗口拖动
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);


    //重写界面出现事件
    void showEvent(QShowEvent *event);
    //界面关闭
    void widgetOut(QWidget *target);

    //ui样式
    void setUiStyle();

    void initial();

    bool eventFilter(QObject *watched, QEvent *event);

    ~faceRecongnition();

signals:
    void startFaceRecord(QString name,QString gender,QString stuNumber,QString age,QString c);
    void returnWidget();

private:
    Ui::faceRecongnition *ui;
    QThread thread_faceRecord;
    threadFaceRecord *threadfacerecord = nullptr;
    InformationEntry *informationEntry = nullptr;
    managementInfo *manage = nullptr;
    //用于最大化的变量
    QSize initSize = this->size();
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int initX = (screenWidth-initSize.width()) / 2;
    int initY = (screenHeight-initSize.height()) /2;
    bool widgetMax = false;
    //用于窗口拖动的变量
    QPoint m_dragStartPosition;
    bool m_isDragging = false;
    //保存初始窗口状态
    int initialWidth;
    int initialHeight;
};

#endif // FACERECONGNITION_H
