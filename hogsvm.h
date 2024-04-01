#ifndef HOGSVM_H
#define HOGSVM_H
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include "opencv2/ml.hpp"
#include <QDebug>
class HOGSVM
{
public:
    HOGSVM();
    cv::Mat Hog(const cv::Mat& image);
    int predict(const cv::Mat& image);
    ~HOGSVM();
private:
    int N_BINS = 16;
    int N_DIVS = 3;
    int N_PHOG = N_BINS * N_DIVS * N_DIVS;
    int BIN_RANGE = (2 * CV_PI)/N_BINS;
    cv::Ptr<cv::ml::SVM> svm;
};

#endif // HOGSVM_H
