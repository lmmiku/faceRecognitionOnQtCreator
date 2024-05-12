#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <face.hpp>
#include <QThread>
#include <iostream>
#include <vector>
#include <string>
#include <QDebug>
#include <QPropertyAnimation>
#include <QScreen>
#include <QCloseEvent>
#include <QEvent>
#include <QTimer>
#include <QPixmap>
#include <QMouseEvent>
#include <QShowEvent>
#include <QApplication>
#include <QMessageBox>
#include <QDesktopWidget>
#include <opencv2/dnn.hpp>
#include <QGraphicsDropShadowEffect>
#include "facerecongnition.h"
#include "threadfacerecong.h"
#include "database.h"
#include "liveblink.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
class AdminLogin;
}
QT_END_NAMESPACE

class AdminLogin:public QWidget{
    Q_OBJECT
public:
    explicit AdminLogin(QWidget*parent = nullptr);
    Ui::AdminLogin* getUi();
    ~AdminLogin();
signals:
    void EntryLogin();
private:
    Ui::AdminLogin *uiLogin;
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);

    //窗口拖动
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    QPoint m_dragStartPosition;
    bool m_isDragging = false;

    //重写界面出现事件
    void showEvent(QShowEvent *event);
    //界面关闭
    void widgetOut(QWidget*target);
    //初始化
    void initial();
    //ui样式
    void setUiStyle();
    bool eventFilter(QObject *watched, QEvent *event);
    //打卡成功槽函数
    void punchSuccess(QString stuNumber);
    //迟到槽函数
    void punchFailed(QString stuNumber);
    //未注册槽函数
    void punchNull();

    ~Widget();
signals:
    void startFaceRecong();
    void startLiveBlink();
private:
    Ui::Widget *ui;
    faceRecongnition * faceRecong = nullptr;
    threadFaceRecong * threadfacerecong = nullptr;
    QThread thread_faceRecong;
    //用于最大化的变量
    QSize initSize = this->size();
    QDesktopWidget *desktop = QApplication::desktop();
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    int initX = (screenWidth-initSize.width()) / 2;
    int initY = (screenHeight-initSize.height()) /2;
    bool widgetMax = false;

    //保存最初窗口状态
    int initialX;
    int initialY;
    int initialWidth;
    int initialHeight;

    //cv::dnn::experimental_dnn_34_v14::Net net;

    //登录窗口
    AdminLogin *adminLogin = nullptr;
    QMessageBox *message;
    //12个月的缩写
    QString mouth[12] ={"Jan","Feb","Mar","Apr","May",
                         "June","July","Aug","Sept","Oct","Nov","Dec"};
};



#endif // WIDGET_H
