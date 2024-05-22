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

//    //删除用户
//    connect(ui->deleteUser,&QPushButton::clicked,this,[=](){
//        QList<QTableWidgetItem*> items = ui->infomation->selectedItems();

//        if(items.size() > 0){
//            int row = ui->infomation->row(items[0]);
//            qDebug()<<row;
//            if (row != -1){
//                QString name = ui->infomation->item(row,0)->text();
//                DeleteFileOrFolder(QString("video/"+ui->infomation->item(row,0)->text()+"_"+ui->infomation->item(row,2)->text()));
//                DataBase::instance()->deleteUserFromStuInfo(ui->infomation->item(row,2)->text());
//                ui->infomation->removeRow(row);
//                QMessageBox::information(nullptr,"删除用户","删除用户"+name+"成功!");
//            }
//        }
//        //this->initial();
//        emit reTrain();
//    });

    //导出考勤情况表
    connect(ui->derive,&QPushButton::clicked,this,[=](){
        this->deriveTab();
    });

    //刷新考勤表
    connect(ui->fflush,&QPushButton::clicked,this,[=](){
        this->fflushView();
    });
    //emit ui->fflush->clicked();

    //修改为已打卡
    connect(ui->update,&QPushButton::clicked,this,[=](){
        QList<QTableWidgetItem*> items = ui->infomation->selectedItems();
        if(items.size() > 0){
            int row = ui->infomation->row(items[0]);
            if (row != -1){
                DataBase::instance()->updateState_stuNumbe(ui->infomation->item(row,2)->text());
            }
        }
    });
}

void managementInfo::deriveTab(){
    QString filePath = QFileDialog::getSaveFileName(this,tr("导出报表"),".",tr("*.xlsx"));
    if(!filePath.isEmpty()){
        QAxObject *excel = new QAxObject(this);
        excel->setControl("Excel.Application");   //连接excel控件
        excel->dynamicCall("SetVisible (bool Visible)","false");  //不显示窗体
        excel->setProperty("DisplayAlerts",false); //不显示警告信息
        QAxObject *workBooks = excel->querySubObject("WorkBooks");  //获取工作簿集合
        workBooks->dynamicCall("Add");  //新建工作簿
        QAxObject *workBook = excel->querySubObject("ActiveWorkBook");  //获取当前工作簿
        QAxObject *workSheets = workBook->querySubObject("Sheets");  //获取工作表集合
        QAxObject *workSheet = workSheets->querySubObject("Item(int)",1);  //获取工作表集合的工作表1(sheet1)
        //设置表头值
        for(int i = 1 ;i<ui->infomation->columnCount()+1;i++){
            QAxObject *range = workSheet->querySubObject("Cells(int,int)",1,i);
            range->dynamicCall("SetValue(const QString &)",ui->infomation->horizontalHeaderItem(i-1)->text());
        }
        //设置表格数据
        for(int i = 1;i<ui->infomation->rowCount()+1;i++){
            for(int j = 1;j<ui->infomation->columnCount()+1;j++){
                QAxObject *range = workSheet->querySubObject("Cells(int,int)",i+1,j);
                range->dynamicCall("SetValue(const QString &)",ui->infomation->item(i-1,j-1)->data(Qt::DisplayRole).toString());
            }
        }
        workBook->dynamicCall("SaveAs(const QString &)",QDir::toNativeSeparators(filePath));  //保存值filepath
        workBook->dynamicCall("Close()");  //关闭工作簿
        excel->dynamicCall("Quit()");  //关闭excel
        delete excel;
        excel = nullptr;
    }
}

void managementInfo::initial(){
    this->initialWidth = this->width();
    this->initialHeight = this->height();
    ui->date->setDate(QDate::currentDate());
}

void managementInfo::fflushView(){
    ui->infomation->clearContents();
    ui->infomation->setRowCount(0);
    int state1 = 0;int state2 = 0;int state3 = 0;
    //tabwidget数据初始化
    QVector<QString> idInfo = DataBase::instance()->getInfoFromAdminInfo(account);
    qDebug()<<ui->date->date().toString("yyyy.MM.dd");
    for(QString stuNumber:idInfo){
        std::tuple<int,QString,QString,QString,int,QString> info = DataBase::instance()->getInfoFromId(stuNumber);
        std::tuple<QString,QString> t = DataBase::instance()->getAttendanceInfo(ui->date->date(),std::get<3>(info));
        if(t == std::tuple<QString,QString>{}){
            t = std::make_tuple("未打卡","");

        }
        int rowCount = ui->infomation->rowCount();
        ui->infomation->insertRow(rowCount);
        ui->infomation->setItem(rowCount,0,new QTableWidgetItem(std::get<1>(info)));
        ui->infomation->setItem(rowCount,1,new QTableWidgetItem(std::get<2>(info)));
        ui->infomation->setItem(rowCount,2,new QTableWidgetItem(std::get<3>(info)));
        ui->infomation->setItem(rowCount,3,new QTableWidgetItem(QString::number(std::get<4>(info))));
        ui->infomation->setItem(rowCount,4,new QTableWidgetItem(std::get<5>(info)));

        ui->infomation->setItem(rowCount,5,new QTableWidgetItem(std::get<0>(t)));
        ui->infomation->setItem(rowCount,6,new QTableWidgetItem(std::get<1>(t)));
        QString state = std::get<0>(t);
        if("已打卡" == state){
            state1++;
        }else if("已迟到" == state){
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
        ui->infomation->item(rowCount,6)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    }

    //pieseries数据初始化
    if(pie_series != nullptr){
        ui->pie_widget->chart()->removeAllSeries();
//        delete pie_series;
//        pie_series = nullptr;
    }

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
//    QPropertyAnimation *animation= new QPropertyAnimation(this,"geometry");
//    //QScreen* screen = QGuiApplication::primaryScreen();
//    animation->setDuration(320);
//    //animation->setStartValue(QRect(this->x()+screen->geometry().width()/3, this->y()+screen->geometry().height()/4, this->width(),0));
//    //animation->setEndValue(QRect(this->x()+screen->geometry().width()/3, this->y()+screen->geometry().height()/4, this->width(),this->height()));
//    animation->setStartValue(QRect(this->x(), this->y(), this->initialWidth,0));
//    animation->setEndValue(QRect(this->x() ,this->y(), this->initialWidth,this->initialHeight));
//    animation->setEasingCurve(QEasingCurve::Linear);
//    animation->start(QAbstractAnimation::DeleteWhenStopped);

    QWidget::showEvent(event);
}

void managementInfo::widgetOut(QWidget *target){
//    QPropertyAnimation * animation = new QPropertyAnimation(target , "geometry");
//    animation->setDuration(320);
//    animation->setStartValue(QRect(target->x(),target->y(),target->width(),target->height()));
//    animation->setEndValue(QRect(target->x(),target->y(),target->width(),0));
//    animation->setEasingCurve(QEasingCurve::Linear);
//    animation->start(QAbstractAnimation::DeleteWhenStopped);
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
    ui->infomation->setColumnWidth(0,100);
    ui->infomation->setColumnWidth(1,80);
    ui->infomation->setColumnWidth(2,150);
    ui->infomation->setColumnWidth(3,100);
    ui->infomation->setColumnWidth(4,125);
    ui->infomation->setColumnWidth(5,125);
    ui->infomation->horizontalHeader()->setStretchLastSection(true);  //最后一列自动充满
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
