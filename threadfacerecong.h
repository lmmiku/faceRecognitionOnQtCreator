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

#include "database.h"

class threadFaceRecong:public QObject{
    Q_OBJECT
public:
    threadFaceRecong();
    void faceIdentification();
    void stopFaceRecong();
    int Predict(cv::Mat src_image);
    QImage Mat2Image(const cv::Mat mat);
    ~threadFaceRecong();
signals:
    void imageToView(QImage image);
    void imageToRecong(QImage image);
private:
    cv::Ptr<cv::face::FaceRecognizer> model;
    cv::CascadeClassifier cascade;
    bool isStop = false;
    cv::VideoCapture cap;
};

#endif // THREADFACERECONG_H
