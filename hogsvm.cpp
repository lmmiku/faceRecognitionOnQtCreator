#include "hogsvm.h"

HOGSVM::HOGSVM(){
    this->svm = cv::Algorithm::load<cv::ml::SVM>("C:/Users/shocker/QTpractise/face_recognition/svm_save.xml");
}

int HOGSVM::predict(const cv::Mat& image){
    //qDebug()<<image.type()<<image.cols<<image.rows;
    cv::Mat dst;
    cv::resize(image, dst, cv::Size(64, 64));
    cv::Mat img_gray;
    cv::cvtColor(dst, img_gray, cv::COLOR_BGR2GRAY);
    cv::Mat feature = this->Hog(img_gray);

    float respose = svm->predict(feature);
    //qDebug()<<respose;
    return respose;
}

cv::Mat HOGSVM::Hog(const cv::Mat &image){
    cv::Mat Hog;
    Hog = cv::Mat::zeros(1, N_PHOG, CV_32FC1);

    cv::Mat Ix, Iy;

    //Find orientation gradients in x and y directions
    Sobel(image, Ix, CV_16S, 1, 0, 3);
    Sobel(image, Iy, CV_16S, 0, 1, 3);

    int cellx = image.cols / N_DIVS;
    int celly = image.rows / N_DIVS;

    int img_area = image.rows * image.cols;

    for (int m = 0; m < N_DIVS; m++)
    {
        for (int n = 0; n < N_DIVS; n++)
        {
            for (int i = 0; i < cellx; i++)
            {
                for (int j = 0; j < celly; j++)
                {

                    float px, py, grad, norm_grad, angle, nth_bin;

                    //px = Ix.at(m*cellx+i, n*celly+j);
                    //py = Iy.at(m*cellx+i, n*celly+j);
                    px = static_cast<float>(Ix.at<int16_t>((m * cellx) + i, (n * celly) + j));
                    py = static_cast<float>(Iy.at<int16_t>((m * cellx) + i, (n * celly) + j));

                    grad = static_cast<float>(std::sqrt(1.0 * px * px + py * py));
                    norm_grad = grad / img_area;

                    //Orientation
                    angle = std::atan2(py, px);

                    //convert to 0 to 360 (0 to 2*pi)
                    if (angle < 0)
                        angle += 2 * CV_PI;

                    //find appropriate bin for angle
                    nth_bin = angle / BIN_RANGE;

                    //add magnitude of the edges in the hog matrix
                    Hog.at<float>(0, (m * N_DIVS + n) * N_BINS + static_cast<int>(angle)) += norm_grad;

                }
            }
        }
    }

    //Normalization
    for (int i = 0; i < N_DIVS * N_DIVS; i++)
    {
        float max = 0;
        int j;
        for (j = 0; j < N_BINS; j++)
        {
            if (Hog.at<float>(0, i * N_BINS + j) > max)
                max = Hog.at<float>(0, i * N_BINS + j);
        }
        for (j = 0; j < N_BINS; j++)
            Hog.at<float>(0, i * N_BINS + j) /= max;
    }
    return Hog;
}

HOGSVM::~HOGSVM(){

}
