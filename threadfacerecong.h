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
#include "liveblink.h"

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
    void facerecong(cv::Mat frame);
    void setTime(QString account);
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

    LiveBlink * live;
    bool isLiving = false;
    //设定考勤时间
    QVector<std::tuple<QDateTime,QDateTime,QDateTime>> attendanceTime{std::make_tuple(QDateTime(QDate::currentDate(), QTime(8,00,00)),QDateTime(QDate::currentDate(), QTime(8, 00, 00)),QDateTime(QDate::currentDate(), QTime(8, 10, 00))),
                                                                        std::make_tuple(QDateTime(QDate::currentDate(), QTime(10,00,00)),QDateTime(QDate::currentDate(), QTime(10, 00, 00)),QDateTime(QDate::currentDate(), QTime(10, 10, 00))),
                                                                        std::make_tuple(QDateTime(QDate::currentDate(), QTime(14,00,00)),QDateTime(QDate::currentDate(), QTime(14, 00, 00)),QDateTime(QDate::currentDate(), QTime(14, 10, 00))),
                                                                        std::make_tuple(QDateTime(QDate::currentDate(), QTime(16,00,00)),QDateTime(QDate::currentDate(), QTime(16, 00, 00)),QDateTime(QDate::currentDate(), QTime(16, 10, 00))),
                                                                        std::make_tuple(QDateTime(QDate::currentDate(), QTime(19,00,00)),QDateTime(QDate::currentDate(), QTime(19, 00, 00)),QDateTime(QDate::currentDate(), QTime(19, 10, 00))),
                                                                        std::make_tuple(QDateTime(QDate::currentDate(), QTime(2,00,00)),QDateTime(QDate::currentDate(), QTime(2, 00, 00)),QDateTime(QDate::currentDate(), QTime(2, 50, 00)))
                                                                        };
//    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
//    dlib::shape_predictor pose_model;
//    dlib::deserialize("C:/Users/shocker/QTpractise/face_recognition/shape_predictor_68_face_landmarks.dat") >> pose_model;
};

#endif // THREADFACERECONG_H
