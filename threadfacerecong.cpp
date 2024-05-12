#include "threadfacerecong.h"

threadFaceRecong::threadFaceRecong(){
    this->model = cv::face::LBPHFaceRecognizer::create();
    model->read("MyFaceLBPHModel.xml");
    cascade.load("C:/Users/shocker//QTpractise/face_recognition/haarcascade_frontalface_alt2.xml");
}

void threadFaceRecong::stopFaceRecong(){
    this->isStop = true;
    this->cap.release();
}

void threadFaceRecong::faceIdentification(){
    if(cap.open(0)){
        qDebug()<<"faceRecong相机成功打开";
        this->isStop = false;
    }else{
        qDebug()<<"faceRecong相机打开失败";
        return;
    }
    cv::Mat frame;
    cv::Mat gray;
    cv::RNG g_rng(12345);

    QPen pen;
    pen.setColor(QColor(40,44,52));
    QFont font;
    font.setPixelSize(20);

    while(!isStop){
        cap>>frame;
        if(frame.empty() || isStop){
            qDebug()<<"faceRecong获取为空"<<"摄像头状态："<<cap.isOpened()<<"isStop = "<<isStop;;
            return;
        }
        std::vector<cv::Rect> faces(0);//建立用于存放人脸的向量容器
        cvtColor(frame, gray, CV_RGB2GRAY);//测试图像必须为灰度图
        equalizeHist(gray, gray); //变换后的图像进行直方图均值化处理
        //检测人脸
        cascade.detectMultiScale(gray, faces,1.1, 4, 0|CV_HAAR_DO_ROUGH_SEARCH,cv::Size(30, 30), cv::Size(500, 500));
        cv::Mat* pImage_roi = new cv::Mat[faces.size()];
        cv::Mat face;
        cv::Point text_lb;//文本写在的位置
        //框出人脸
        std::string str;
        if(faces.size() == 1){
            pImage_roi[0] = gray(faces[0]); //将所有的脸部保存起来
            text_lb = cv::Point(faces[0].x, faces[0].y);
            if (pImage_roi[0].empty())
                continue;
            str = DataBase::instance()->getInfoFromStuInfo(Predict(pImage_roi[0])).toLocal8Bit().toStdString();
            QString name = DataBase::instance()->getInfoFromStuInfo(Predict(pImage_roi[0]));
            if(str == ""){
                str = "ERROR";
                //emit PunchNull();
            }else{

            }
            cv::Scalar color = cv::Scalar(g_rng.uniform(0, 255), g_rng.uniform(0, 255), g_rng.uniform(0, 255));//所取的颜色任意值
            rectangle(frame, cv::Point(faces[0].x, faces[0].y), cv::Point(faces[0].x + faces[0].width, faces[0].y + faces[0].height), color, 1, 8);//放入缓存
            putText(frame, str, text_lb, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));//添加文字
        }
        delete[] pImage_roi;
        emit imageToRecong(Mat2Image(frame));
        QThread::usleep(100);
        //qDebug()<<"faceRecong 正在运行" <<"isStop == "<<isStop;
    }

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
    model->read("MyFaceLBPHModel.xml");
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
