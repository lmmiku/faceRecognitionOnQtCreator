#ifndef THREADFACERECORD_H
#define THREADFACERECORD_H

#include <QObject>
#include <face.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/ml.hpp"
#include <QImage>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QThread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <vector>
#include <QFile>
#include "database.h"
#include "hogsvm.h"
#include "liveblink.h"

class threadFaceRecord:public QObject{
    Q_OBJECT
public:
    threadFaceRecord();
    void faceRecord(QString name,QString gender,QString number,QString age,QString c);      //获取人脸照片用于训练
    void stopFaceRecord();
    void faceTrain();       //训练生成xml文件
    void create_csv();      //生成用于训练的csv文件
    int append_csv(QString dirName);      //向csv文件添加数据
    void read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator = ';');    //读取人脸数据和标签
    QImage Mat2Image(const cv::Mat mat);
    ~threadFaceRecord();
signals:
    void imageToRecord(QImage image);
    void reload();
private:
    cv::CascadeClassifier cascade;
    cv::VideoCapture cap;
    bool isStop = false;
    HOGSVM *HOG_SVM = new HOGSVM();
    LiveBlink * live;
    bool isLiving = false;
    QString videoPath = "video/";
};

#endif // THREADFACERECORD_H
