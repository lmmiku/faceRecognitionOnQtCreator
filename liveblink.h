#ifndef LIVEBLINK_H
#define LIVEBLINK_H

#include <QObject>
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <QDateTime>
#include <QDebug>
#include "fonts.h"
#include "gui_core_kernel_1.h"


class LiveBlink :public QObject
{
    Q_OBJECT
public:
    LiveBlink();
    void bioassay();
    void stopLiveBlink();
signals:
    void isLiving(bool flag);
private:
    bool isStop = false;
};

#endif // LIVEBLINK_H
