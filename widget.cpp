#include "widget.h"
#include "ui_widget.h"
#include "ui_AdminLogin.h"

AdminLogin::AdminLogin(QWidget*parent):QWidget(parent),uiLogin(new Ui::AdminLogin){
    uiLogin->setupUi(this);
    connect(uiLogin->login,&QPushButton::clicked,this,[=](){
        emit EntryLogin();
    });
}

Ui::AdminLogin* AdminLogin::getUi(){
    return uiLogin;
}

AdminLogin::~AdminLogin(){
    delete uiLogin;
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //初始化ui样式
    this->setUiStyle();
    //初始化部分值
    this->initial();
    //安装事件过滤器
    ui->max->installEventFilter(this);

    //人脸识别线程
    this->threadfacerecong->moveToThread(&thread_faceRecong);
    thread_faceRecong.start();
    connect(this,&Widget::startFaceRecong,threadfacerecong,&threadFaceRecong::faceIdentification);
    connect(threadfacerecong,&threadFaceRecong::imageToRecong,this,[=](QImage image){
        ui->viewFaceImage->setPixmap(QPixmap::fromImage(image));
    });

    //打卡成功
    connect(threadfacerecong,&threadFaceRecong::PunchSuccess,this,[=](QString name){
        //                                QMessageBox::information(nullptr,"打卡成功",name+",打卡成功!\n时间："+\
//                                        QDateTime::currentDateTime().toString("hh:mm:ss"));
        if(!message->isVisible()){
            message->information(nullptr,"打卡成功",name+",打卡成功!\n时间："+\
                                        QDateTime::currentDateTime().toString("hh:mm:ss"));
            message->show();
        }
        if(message == nullptr){
            message = new QMessageBox();
        }
        QTimer::singleShot(800,[=](){
            message->hide();
            message->close();
            delete message;
            message = nullptr;
        });
    });
    //打卡失败（迟到）
    connect(threadfacerecong,&threadFaceRecong::PunchFailed,this,[=](QString name){
        if(!message->isVisible()){
            message->information(nullptr,"打卡成功","打卡成功,已迟到!\n时间："+\
                                        QDateTime::currentDateTime().toString("hh:mm:ss"));
            message->show();
            disconnect(threadfacerecong,&threadFaceRecong::PunchSuccess,nullptr,nullptr);
        }
    });
    //打卡失败，未注册
    connect(threadfacerecong,&threadFaceRecong::PunchNull,this,[=](){
        if(!message->isVisible()){
            message->information(nullptr,"打卡失败","打卡失败,未注册用户!\n时间："+\
                                        QDateTime::currentDateTime().toString("hh:mm:ss"));
            message->show();
            disconnect(threadfacerecong,&threadFaceRecong::PunchSuccess,nullptr,nullptr);
        }
    });
    //关闭按钮功能
    connect(ui->close,&QPushButton::clicked,this,[=](){
        this->widgetOut(this);
        QTimer::singleShot(325,this,[=](){
            this->close();
        });
    });

    //最小化按钮功能
    connect(ui->min,&QPushButton::clicked,this,[=](){
        this->showMinimized();
    });

    //管理员登录
    connect(ui->login,&QPushButton::clicked,this,[=](){
        if(this->adminLogin == nullptr){
            this->adminLogin = new AdminLogin();
        }
        this->adminLogin->show();
        disconnect(this->adminLogin,&AdminLogin::EntryLogin,nullptr,nullptr);
        connect(this->adminLogin,&AdminLogin::EntryLogin,this,[=](){
            if(DataBase::instance()->verifyAdmin(adminLogin->getUi()->account->text(),adminLogin->getUi()->account->text())){
                if(this->faceRecong == nullptr){
                    this->faceRecong = new faceRecongnition(adminLogin->getUi()->account->text());
                }
                adminLogin->hide();
                this->widgetOut(this);
                QTimer::singleShot(325,this,[=](){
                    this->hide();
                    faceRecong->show();
                });
                //接收返回信号
                disconnect(faceRecong,&faceRecongnition::returnWidget,nullptr,nullptr);
                connect(faceRecong,&faceRecongnition::returnWidget,this,[=](){
                    this->show();
                });

                //接收重新加载人脸识别模型信号
                connect(faceRecong,&faceRecongnition::reload,this,[=](){
                    threadfacerecong->reloadModel();
                });

            }else{
                adminLogin->hide();
                QMessageBox::warning(this,"失败","账号或密码有误！");
            }
        });
    });

    //打开摄像头
    connect(ui->startCap,&QPushButton::clicked,this,[=](){
        if(!ui->startCap->text().compare("打开摄像头")){
            ui->startCap->setIcon(QIcon(":/blue.png"));
            emit startFaceRecong();
            ui->startCap->setText("关闭摄像头");
        }else{
            if(threadfacerecong != nullptr){
                threadfacerecong->stopFaceRecong();
                ui->startCap->setIcon(QIcon(":/black.png"));
                ui->startCap->setText("打开摄像头");
                QTimer::singleShot(200,this,[=](){
                    ui->viewFaceImage->setPixmap(QPixmap(":/faceImage.jpg"));
                });

            }

        }
    });
}

void Widget::initial(){
    this->initialX = this->x();
    this->initialY = this->y();
    this->initialWidth = this->width();
    this->initialHeight = this->height();
    this->threadfacerecong = new threadFaceRecong();
    message = new QMessageBox();
//    this->net = cv::dnn::experimental_dnn_34_v14::readNetFromCaffe("C:/Users/shocker/QTpractise/face_recognition/Live/4_0_0_80x80_MiniFASNetV1SE.caffemodel",\
//                                                                   "C:/Users/shocker/QTpractise/face_recognition/Live/deploy.prototxt");
//    net.setPreferableBackend(cv::dnn::experimental_dnn_34_v14::DNN_BACKEND_OPENCV);
//    net.setPreferableTarget(cv::dnn::experimental_dnn_34_v14::DNN_TARGET_CPU);


}

void Widget::showEvent(QShowEvent *event){
    QPropertyAnimation *animation= new QPropertyAnimation(this,"geometry");
    //qDebug()<<"x:"<<this->x()<<"y:"<<this->y()<<"width:"<<this->width()<<"height:"<<this->height()<<"initialWidth:"<<this->initialWidth<<"initialHeight："<<this->initialHeight;
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

void Widget::widgetOut(QWidget*target){
    QPropertyAnimation * animation = new QPropertyAnimation(target , "geometry");
    animation->setDuration(320);
    animation->setStartValue(QRect(target->x(),target->y(),target->width(),target->height()));
    animation->setEndValue(QRect(target->x(),target->y(),target->width(),0));
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

//实现窗口拖动
void Widget::mousePressEvent(QMouseEvent* event){
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
        m_isDragging = true;
    }
    QWidget::mousePressEvent(event);
}
void Widget::mouseMoveEvent(QMouseEvent* event){
    if (m_isDragging) {
        QPoint diff = event->pos() - m_dragStartPosition;
        QPoint newTopLeft = this->geometry().topLeft() + diff;
        move(newTopLeft);
    }
    QWidget::mouseMoveEvent(event);
}
void Widget::mouseReleaseEvent(QMouseEvent* event){
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void Widget::setUiStyle(){
    //设置窗口无边框
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    //设置窗体透明,防止边距显示不同的颜色
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    //设置标题栏颜色
    ui->widget_title->setStyleSheet(QString("QWidget#widget_title{background-color:#ECF5F9;border-radius:9px 9px}"));

    //设置按键区背景图片
    ui->button_widget->setStyleSheet(QString("QWidget#button_widget{background-image:url(:/980.jpg)}"));

    ui->icon->setPixmap(QPixmap(":/faceRecongnition.png"));
    ui->icon->setScaledContents(true);
    ui->title->setText("人脸识别考勤管理系统");
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

    ui->startCap->setIcon(QIcon(":/black.png"));
    //设置边框阴影
    ui->widget->setStyleSheet(QString("QWidget#widget{background-color:rgb(243,243,243);border-radius:9px 9px;}"));
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(Qt::gray);
    shadow->setBlurRadius(15);
    ui->widget->setGraphicsEffect(shadow);

    //圆角
    ui->viewFaceImage->setStyleSheet(QString("QLabel#viewFaceImage{border-radius:7px 7px}"));
    //设置人脸识别默认图片
    ui->viewFaceImage->setPixmap(QPixmap(":/faceImage.jpg"));

    ui->viewFaceImage->setScaledContents(true);
}

//实现最大化按钮图标以及窗口大小变化
bool Widget::eventFilter(QObject *watched, QEvent *event){
    if(watched == ui->max && event->type() == QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton){
            if(widgetMax){
                this->setGeometry(initX,initY,initSize.width(),initSize.height());
                this->move(initX,initY);
                ui->max->setIcon(QIcon(":/max.png"));
                widgetMax = false;
                return QWidget::eventFilter(watched,event);
            }else{
                this->showMaximized();
                ui->max->setIcon(QIcon(":/max1.png"));
                widgetMax = true;
                return QWidget::eventFilter(watched,event);
            }
        }
    }
    return QWidget::eventFilter(watched,event);
}

Widget::~Widget()
{
    delete ui;
    threadfacerecong->stopFaceRecong();
    thread_faceRecong.exit(0);
    thread_faceRecong.wait();
    delete threadfacerecong;
}
