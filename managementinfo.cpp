#include "managementinfo.h"
#include "ui_managementinfo.h"

managementInfo::managementInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::managementInfo)
{
    ui->setupUi(this);
}

managementInfo::managementInfo(QString account,QWidget*parent):QWidget(parent),ui(new Ui::managementInfo),account(account){
    ui->setupUi(this);
    this->initial();
    this->setUiStyle();

    //关闭按钮功能
    connect(ui->close,&QPushButton::clicked,this,[=](){

        widgetOut(this);
        QTimer::singleShot(325,this,[=](){
            this->close();
        });
    });

    //最小化按钮功能
    connect(ui->min,&QPushButton::clicked,this,[=](){
        this->showMinimized();
    });

    //返回
    connect(ui->returnWidget,&QPushButton::clicked,this,[=](){
        widgetOut(this);
        QTimer::singleShot(325,this,[=](){
            this->hide();
            emit returnWidget();
        });
    });

    //删除用户
    connect(ui->deleteUser,&QPushButton::clicked,this,[=](){
        QList<QTableWidgetItem*> items = ui->infomation->selectedItems();

        for(QTableWidgetItem *item:items){
            int row = ui->infomation->row(item);
            qDebug()<<row;
            if (row != -1){
                DeleteFileOrFolder(QString("video/"+ui->infomation->item(row,0)->text()+"_"+ui->infomation->item(row,2)->text()));
                DataBase::instance()->deleteUserFromStuInfo(ui->infomation->item(row,2)->text());
                ui->infomation->removeRow(row);
            }
        }

    });
}

void managementInfo::initial(){
    this->initialWidth = this->width();
    this->initialHeight = this->height();
    int state1 = 0;int state2 = 0;int state3 = 0;
    //tabwidget数据初始化
    QVector<QString> idInfo = DataBase::instance()->getInfoFromAdminInfo(account);
    for(QString stuNumber:idInfo){
        std::tuple<int,QString,QString,QString,int,QString> info = DataBase::instance()->getInfoFromId(stuNumber);
        int rowCount = ui->infomation->rowCount();
        ui->infomation->insertRow(rowCount);
        ui->infomation->setItem(rowCount,0,new QTableWidgetItem(std::get<1>(info)));
        ui->infomation->setItem(rowCount,1,new QTableWidgetItem(std::get<2>(info)));
        ui->infomation->setItem(rowCount,2,new QTableWidgetItem(std::get<3>(info)));
        ui->infomation->setItem(rowCount,3,new QTableWidgetItem(QString::number(std::get<4>(info))));
        ui->infomation->setItem(rowCount,4,new QTableWidgetItem(std::get<5>(info)));
        QString state = DataBase::instance()->getStateFromId(std::get<0>(info));
        ui->infomation->setItem(rowCount,5,new QTableWidgetItem(state));
        if("已打卡" == state){
            state1++;
        }else if("迟到" == state){
            state2++;
        }else{
            state3++;
        }
        ui->infomation->item(rowCount,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        ui->infomation->item(rowCount,1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        ui->infomation->item(rowCount,2)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        ui->infomation->item(rowCount,3)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        ui->infomation->item(rowCount,4)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        ui->infomation->item(rowCount,5)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    }

    //pieseries数据初始化
    pie_series = new QPieSeries(ui->pie_widget);
    ui->pie_widget->setRenderHint(QPainter::Antialiasing);
    ui->pie_widget->chart()->setTheme(QChart::ChartThemeQt);
    ui->pie_widget->chart()->setTitle("考勤情况");
    pie_series->append("已打卡",state1);
    pie_series->append("迟到",state2);
    pie_series->append("未打卡",state3);

    for(QPieSlice * slice:pie_series->slices()){
        slice->setLabel(QString("%1%2%3").arg(slice->label()).arg(":").arg(slice->value()));
    }
    pie_series->setLabelsPosition(QPieSlice::LabelPosition::LabelOutside);
    pie_series->setLabelsVisible(true);
    ui->pie_widget->chart()->addSeries(pie_series);
    connect(pie_series,&QPieSeries::clicked,this,[=](QPieSlice* slice){
        slice->setExploded(!slice->isExploded());
    });
}

bool managementInfo::DeleteFileOrFolder(const QString &strPath)//要删除的文件夹或文件的路径
{
    if (strPath.isEmpty() || !QDir().exists(strPath))//是否传入了空的路径||路径是否存在
        return false;

    QFileInfo FileInfo(strPath);

    if (FileInfo.isFile())
        QFile::remove(strPath);
    else if (FileInfo.isDir()){
        QDir qDir(strPath);
        qDir.removeRecursively();
    }
    return true;
}

void managementInfo::showEvent(QShowEvent* event){
    QPropertyAnimation *animation= new QPropertyAnimation(this,"geometry");
    //QScreen* screen = QGuiApplication::primaryScreen();
    animation->setDuration(320);
    //animation->setStartValue(QRect(this->x()+screen->geometry().width()/3, this->y()+screen->geometry().height()/4, this->width(),0));
    //animation->setEndValue(QRect(this->x()+screen->geometry().width()/3, this->y()+screen->geometry().height()/4, this->width(),this->height()));
    animation->setStartValue(QRect(this->x(), this->y(), this->initialWidth,0));
    animation->setEndValue(QRect(this->x() ,this->y(), this->initialWidth,this->initialHeight));
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    QWidget::showEvent(event);
}

void managementInfo::widgetOut(QWidget *target){
    QPropertyAnimation * animation = new QPropertyAnimation(target , "geometry");
    animation->setDuration(320);
    animation->setStartValue(QRect(target->x(),target->y(),target->width(),target->height()));
    animation->setEndValue(QRect(target->x(),target->y(),target->width(),0));
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void managementInfo::setUiStyle(){
    //设置窗口无边框
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    //设置窗体透明,防止边距显示不同的颜色
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    ui->title->setText("人脸识别考勤管理系统");
    ui->icon->setPixmap(QPixmap(":/faceRecongnition.png"));
    ui->icon->setScaledContents(true);
    //设置图标
    ui->close->setIcon(QIcon(":/close.png"));
    ui->max->setIcon(QIcon(":/max.png"));
    ui->min->setIcon(QIcon(":/min.png"));
    ui->close->setStyleSheet(QString("QPushButton#close{border:none;background-color:transparent;}\
                                      QPushButton#close:hover{background-color:red;border-radius:3px 3px;}"));
    ui->close->setIcon(QIcon(":/close.png"));
    ui->min->setStyleSheet(QString("QPushButton#min{border:none;background-color:transparent;}\
                                    QPushButton#min:hover{background-color:#C0C0C0;border-radius:3px 3px;}"));
    ui->min->setIcon(QIcon(":/min.png"));
    ui->max->setStyleSheet(QString("QPushButton#max{border:none;background-color:transparent;}\
                                    QPushButton#max:hover{background-color:#C0C0C0;border-radius:3px 3px;}"));
    ui->max->setIcon(QIcon(":/max.png"));
    //设置标题栏颜色
    ui->widget_title->setStyleSheet(QString("QWidget#widget_title{background-color:#ECF5F9;border-radius:9px 9px}"));
    //设置边框阴影
    ui->widget->setStyleSheet(QString("QWidget#widget{background-color:rgb(243,243,243);border-radius:9px 9px;}"));
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(Qt::gray);
    shadow->setBlurRadius(15);
    ui->widget->setGraphicsEffect(shadow);

    //设置tabwidget样式
    ui->infomation->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);//表头字体居中
    ui->infomation->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->infomation->setColumnWidth(0,130);
    ui->infomation->setColumnWidth(1,125);
    ui->infomation->setColumnWidth(2,175);
    ui->infomation->setColumnWidth(3,100);
    ui->infomation->setColumnWidth(4,125);
    ui->infomation->setColumnWidth(5,125);
    ui->infomation->setSelectionBehavior(QAbstractItemView::SelectRows);  //选中模式：整行
    ui->infomation->verticalHeader()->setVisible(false);  //取消默认行数
}

//实现窗口拖动
void managementInfo::mousePressEvent(QMouseEvent* event){
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        m_isDragging = true;
    }
    QWidget::mousePressEvent(event);
}
void managementInfo::mouseMoveEvent(QMouseEvent* event){
    if (m_isDragging) {
        QPoint diff = event->pos() - m_dragStartPosition;
        QPoint newTopLeft = this->geometry().topLeft() + diff;
        move(newTopLeft);
    }
    QWidget::mouseMoveEvent(event);
}
void managementInfo::mouseReleaseEvent(QMouseEvent* event){
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

managementInfo::~managementInfo()
{
    delete ui;
}
