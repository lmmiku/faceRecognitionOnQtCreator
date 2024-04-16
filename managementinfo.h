#ifndef MANAGEMENTINFO_H
#define MANAGEMENTINFO_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QDesktopWidget>
#include <QApplication>
#include <QMouseEvent>
#include <tuple>
#include <QtCharts>
#include <QGraphicsDropShadowEffect>
#include <QTableWidget>
#include "QFileDialog"
#include <ActiveQt/QAxObject>
#include "database.h"
namespace Ui {
class managementInfo;
}

class managementInfo : public QWidget
{
    Q_OBJECT

public:
    explicit managementInfo(QWidget *parent = nullptr);
    managementInfo(QString account,QWidget*parent = nullptr);
    ~managementInfo();

    bool DeleteFileOrFolder(const QString &strPath);

    void updataPieSeries();

    void initial();
    //窗口拖动
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    //重写界面出现事件
    void showEvent(QShowEvent *event);
    //界面关闭
    void widgetOut(QWidget *target);
    //设置ui样式
    void setUiStyle();
    //导出数据为excel表格
    void deriveTab();
signals:
    void returnWidget();
private:
    Ui::managementInfo *ui;

    //饼状图
    QPieSeries *pie_series;

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
    //管理员账号
    QString account;
};

#endif // MANAGEMENTINFO_H
