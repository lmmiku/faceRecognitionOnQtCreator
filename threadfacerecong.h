#ifndef THREADFACERECONG_H
#define THREADFACERECONG_H

#include <QObject>
#include <face.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <QImage>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QPainter>
#include <QDateTime>
#include <QTimer>

#include "database.h"

class threadFaceRecong:public QObject{
    Q_OBJECT
public:
    threadFaceRecong();
    void faceIdentification();
    void stopFaceRecong();
    int Predict(cv::Mat src_image);
    QImage Mat2Image(const cv::Mat mat);
    int putString(cv::Mat &img, QString text, QPoint org, QFont font, QPen pen);
    void reloadModel();        //重新加载模型
    ~threadFaceRecong();
signals:
    void imageToView(QImage image);
    void imageToRecong(QImage image);
    void PunchSuccess(QString stuName);
    void PunchFailed(QString stuName);
    void PunchNull();
private:
    cv::Ptr<cv::face::FaceRecognizer> model;
    cv::CascadeClassifier cascade;
    bool isStop = false;
    cv::VideoCapture cap;
    //设定考勤时间
    QDateTime setTime = QDateTime(QDate::currentDate(), QTime(8, 00, 00));
};

#endif // THREADFACERECONG_H
