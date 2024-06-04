#include "threadfacerecong.h"

threadFaceRecong::threadFaceRecong(){
    this->model = cv::face::LBPHFaceRecognizer::create();
    model->read("MyFaceLBPHModel.xml");
    cascade.load("C:/Users/shocker//QTpractise/face_recognition/haarcascade_frontalface_alt2.xml");
    live = new LiveBlink();
    connect(live,&LiveBlink::imageToView,this,[=](QImage image){
        emit imageToRecong(image);
    });
//    connect(live,&LiveBlink::isLiving,this,[=](bool flag){
//        this->isLiving = flag;
//    });
    connect(live,&LiveBlink::faceImage,this,[=](cv::Mat frame){
        qDebug()<<"接收到活体信号";
        facerecong(frame);
    });
}

void threadFaceRecong::stopFaceRecong(){
    this->isStop = true;
    this->cap.release();
    live->stopLiveBlink();
}

void threadFaceRecong::faceIdentification(){

//    cv::Mat frame;
//    cv::Mat gray;
//    cv::RNG g_rng(12345);

//    QPen pen;
//    pen.setColor(QColor(40,44,52));
//    QFont font;
//    font.setPixelSize(20);

    //开始活体检测标志
//    Continue:
    live->startLiveBlink();
    live->bioassay();
//    while(isLiving){
//        qDebug()<<"ceui1";
//        if(cap.open(0)){
//            qDebug()<<"faceRecong相机成功打开";
//            this->isStop = false;
//        }
//        else{
//            qDebug()<<"faceRecong相机打开失败";
//            return;
//        }
//        while(!isStop){
//            int count = 1;
//            cap>>frame;
//            if(frame.empty() || isStop){
//                qDebug()<<"faceRecong获取为空"<<"摄像头状态："<<cap.isOpened()<<"isStop = "<<isStop;;
//                return;
//            }
//            std::vector<cv::Rect> faces(0);//建立用于存放人脸的向量容器
//            cvtColor(frame, gray, CV_RGB2GRAY);//测试图像必须为灰度图
//            equalizeHist(gray, gray); //变换后的图像进行直方图均值化处理
//            //检测人脸
//            cascade.detectMultiScale(gray, faces,1.1, 4, 0|CV_HAAR_DO_ROUGH_SEARCH,cv::Size(30, 30), cv::Size(500, 500));
//            cv::Mat* pImage_roi = new cv::Mat[faces.size()];
//            cv::Mat face;
//            cv::Point text_lb;//文本写在的位置
//            //框出人脸
//            std::string str;
//            if(faces.size() == 1){
//                pImage_roi[0] = gray(faces[0]); //将所有的脸部保存起来
//                text_lb = cv::Point(faces[0].x, faces[0].y);
//                if (pImage_roi[0].empty())
//                    continue;
//                str = DataBase::instance()->getInfoFromStuInfo(Predict(pImage_roi[0])).toLocal8Bit().toStdString();
//                QString stuNumber = DataBase::instance()->getStuNumber(Predict(pImage_roi[0]));
//                if(str == ""){
//                    str = "ERROR";
//                    emit PunchNull();
//                    //break;
//                    if(count++ == 100){
//                        isStop = true;
//                        isLiving = false;
//                        goto Continue;  //跳转，继续进行活体检测
//                    }
//                }else{
//                    QDateTime startTime = QDateTime::currentDateTime();
//                    if(startTime.secsTo(setTime) < 0){ // 计算时间差（秒）,负数说明超过早上八点
//                        //emit PunchFailed(stuNumber);
//                    }else{
//                        //emit PunchSuccess(stuNumber);
//                    }
//                    //break;
//                    if(count++ == 20){
//                        isStop = true;
//                        isLiving = false;
//                        goto Continue;  //跳转，继续进行活体检测
//                    }

//                }
//                cv::Scalar color = cv::Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255), g_rng.uniform(0, 255));//所取的颜色任意值
//                rectangle(frame, cv::Point(faces[0].x, faces[0].y), cv::Point(faces[0].x + faces[0].width, faces[0].y + faces[0].height), color, 1, 8);//放入缓存
//                putText(frame, stuNumber.toStdString(), text_lb, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));//添加文字
//            }
//            delete[] pImage_roi;
//            emit imageToRecong(Mat2Image(frame));
//            QThread::usleep(100);
//        }
//    }
}

int threadFaceRecong::Predict(cv::Mat src_image){
    cv::Mat face_test;
    int predict = 0;
    //截取的ROI人脸尺寸调整
    if (src_image.rows >= 120)
    {
        //改变图像大小，使用双线性差值
        cv::resize(src_image, face_test, cv::Size(92, 112));

    }
    //判断是否正确检测ROI
    if (!face_test.empty())
    {
        //测试图像应该是灰度图
        predict = model->predict(face_test);
    }
    qDebug() <<"预测标签值:" <<predict;
        return predict;
}

void threadFaceRecong::reloadModel(){
    model->read("MyFaceLBPHModel1.xml");
}

int threadFaceRecong::putString(cv::Mat &img, QString text, QPoint org, QFont font, QPen pen){
        QImage::Format imgFormat;

        switch (img.channels()) {
        case 1:
        imgFormat = QImage::Format_Grayscale8;
        break;
        case 3:
        imgFormat = QImage::Format_RGB888;
        break;
        default:
        return -1;
        break;
        }

        QImage tmpImg(img.data, img.cols, img.rows, img.step, imgFormat);

        QPainter painter(&tmpImg);
        painter.setPen(pen);
        painter.setFont(font);
        painter.drawText(org, text);

        return 0;
}

void threadFaceRecong::facerecong(cv::Mat frame){
        if(frame.empty()){
            qDebug()<<"图像为空";
            return;
        }
        cv::Mat gray;
        cv::RNG g_rng(12345);

        QPen pen;
        pen.setColor(QColor(40,44,52));
        QFont font;
        font.setPixelSize(20);
        std::vector<cv::Rect> faces(0);//建立用于存放人脸的向量容器
        cvtColor(frame, gray, CV_RGB2GRAY);//测试图像必须为灰度图
        equalizeHist(gray, gray); //变换后的图像进行直方图均值化处理
        //检测人脸
        cascade.detectMultiScale(gray, faces,1.1, 4, 0|CV_HAAR_DO_ROUGH_SEARCH,cv::Size(30, 30), cv::Size(500, 500));
        //if(faces.size() != 1){qDebug()<<"人脸图像不为1";return;}
        cv::Mat* pImage_roi = new cv::Mat[faces.size()];
        cv::Mat face;
        cv::Point text_lb;//文本写在的位置
        //框出人脸
        std::string str;
        pImage_roi[0] = gray(faces[0]); //将所有的脸部保存起来
        text_lb = cv::Point(faces[0].x, faces[0].y);

        str = DataBase::instance()->getInfoFromStuInfo(Predict(pImage_roi[0])).toLocal8Bit().toStdString();
        QString stuNumber = DataBase::instance()->getStuNumber(Predict(pImage_roi[0]));
        qDebug()<<QString::fromStdString(str)<<"  "<<stuNumber;
        if(str == ""){
            str = "ERROR";
            qDebug()<<"发送错误信号";
            emit PunchNull();
            //break;
        }else{
            QDateTime startTime = QDateTime::currentDateTime();
            for(std::tuple<QDateTime,QDateTime,QDateTime> t:attendanceTime){
                if(startTime.toString("hh").toInt() >= std::get<0>(t).toString("hh").toInt() && startTime.toString("hh").toInt() <= std::get<2>(t).toString("hh").toInt()){
                    qDebug()<<"进入时间";
                    if(startTime.toString("mm").toInt() >= std::get<0>(t).toString("mm").toInt() && startTime.toString("mm").toInt() <= std::get<1>(t).toString("mm").toInt()){
                        qDebug()<<"发送打卡成功信号";
                        emit PunchSuccess(stuNumber);
                    }else{
                        qDebug()<<"发送迟到信号";
                        emit PunchFailed(stuNumber);
                    }
                }
            }
//            if(startTime.secsTo(setTime) < 0){ // 计算时间差（秒）,负数说明超过早上八点
//                emit PunchFailed(stuNumber);
//            }else{
//                emit PunchSuccess(stuNumber);
//            }
            //break;
        }
        //cv::Scalar color = cv::Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255), g_rng.uniform(0, 255));//所取的颜色任意值
        //rectangle(frame, cv::Point(faces[0].x, faces[0].y), cv::Point(faces[0].x + faces[0].width, faces[0].y + faces[0].height), color, 1, 8);//放入缓存
        //putText(frame, stuNumber.toStdString(), text_lb, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));//添加文字
        delete[] pImage_roi;
        //emit imageToRecong(Mat2Image(frame));
        //QThread::usleep(100);
}

void threadFaceRecong::setTime(QString account){
    QVector<std::tuple<QString,QString>> data = DataBase::instance()->getclassScheduleInfo(account);
        attendanceTime.erase(attendanceTime.begin(),attendanceTime.end());
    for(std::tuple<QString,QString> t :data){
        QString time = QDateTime::fromString(std::get<0>(t),"HH.mm.ss").addSecs(-600).toString("HH.mm.ss");
        std::tuple<QDateTime,QDateTime,QDateTime> t_ = std::make_tuple(QDateTime::fromString(time,"HH.mm.ss"),QDateTime::fromString(std::get<0>(t),"HH.mm.ss"),QDateTime::fromString(std::get<1>(t),"HH.mm.ss"));
        attendanceTime.append(t_);
    }
}

QImage threadFaceRecong::Mat2Image(const cv::Mat mat){
    if(mat.type()==CV_8UC1)
    {
        QVector<QRgb>colorTable;
        for(int i=0;i<256;i++)
            colorTable.push_back(qRgb(i,i,i));
        const uchar*qImageBuffer=(const uchar*)mat.data;
        QImage img(qImageBuffer,mat.cols,mat.rows,mat.step,QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    //8-bitsunsigned,NO.OFCHANNELS=3
    if(mat.type()==CV_8UC3)
    {
        const uchar*qImageBuffer=(const uchar*)mat.data;
        QImage img(qImageBuffer,mat.cols,mat.rows,mat.step,QImage::Format_RGB888);
        return  img.rgbSwapped();
    }
    else
    {
        qDebug()<<"ERROR:MatcouldnotbeconvertedtoQImage.";
        return QImage();
    }
}

threadFaceRecong::~threadFaceRecong(){
    cap.release();
}
