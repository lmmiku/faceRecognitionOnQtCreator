#include "liveblink.h"

LiveBlink::LiveBlink(){

}

void LiveBlink::bioassay(){
    // 打开摄像头
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        qDebug()<<"open VideoCapture failed";
        //return 1;
    }
    //加载人脸识别模型
    dlib::frontal_face_detector detector = dlib::get_frontal_face_detector();
    dlib::shape_predictor pose_model;
    dlib::deserialize("C:/Users/shocker/QTpractise/face_recognition/shape_predictor_68_face_landmarks.dat") >> pose_model;

    //int mouthIndex = 0;
    int leyeIndex = 0;
    int reyeIndex = 0;

    //int counter_mouth = 0;
    int counter_leye = 0;
    int counter_reye = 0;

    // Grab and process frames until the main window is closed by the user.
    while(!isStop)
    {
        // Grab a frame
        cv::Mat frame;
        if (!cap.read(frame))
        {
            break;
        }
        //cv::Mat frame_gray;
        //cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);//转灰度化，减少运算
        dlib::cv_image<dlib::bgr_pixel> cimg(frame);
        //dlib::array2d<unsigned char> cimg;
        //dlib::assign_image(cimg, dlib::cv_image<unsigned char>(frame));
        // Detect faces
        std::vector<dlib::rectangle> faces = detector(cimg);
        // Find the pose of each face.
        std::vector<dlib::full_object_detection> shapes;

        if (1 == faces.size()) {
            dlib::full_object_detection face = pose_model(cimg,faces[0]);
            //计算左眼纵横比
            dlib::point leyeLeft = face.part(37);
            dlib::point leyeRight = face.part(40);

            dlib::point leyeLeftUp = face.part(38);
            dlib::point leyeLeftDowm = face.part(42);

            dlib::point leyeRightUp = face.part(39);
            dlib::point leyeRightDowm = face.part(41);

            float leyeA = sqrt(pow(leyeLeftUp.x() - leyeLeftDowm.x(), 2) + pow(leyeLeftUp.y() - leyeLeftDowm.y(), 2));
            float leyeB = sqrt(pow(leyeRightUp.x() - leyeRightDowm.x(), 2) + pow(leyeRightUp.y() - leyeRightDowm.y(), 2));
            float leyeC = sqrt(pow(leyeLeft.x() - leyeRight.x(), 2) + pow(leyeLeft.y() - leyeRight.y(), 2));

            float leyeEVR = (leyeA + leyeB) / (2 * leyeC);

            //计算右眼纵横比
            dlib::point reyeLeft = face.part(43);
            dlib::point reyeRight = face.part(46);

            dlib::point reyeLeftUp = face.part(44);
            dlib::point reyeLeftDowm = face.part(48);

            dlib::point reyeRightUp = face.part(45);
            dlib::point reyeRightDowm = face.part(47);

            float reyeA = sqrt(pow(reyeLeftUp.x() - reyeLeftDowm.x(), 2) + pow(reyeLeftUp.y() - reyeLeftDowm.y(), 2));
            float reyeB = sqrt(pow(reyeRightUp.x() - reyeRightDowm.x(), 2) + pow(reyeRightUp.y() - reyeRightDowm.y(), 2));
            float reyeC = sqrt(pow(reyeLeft.x() - reyeRight.x(), 2) + pow(reyeLeft.y() - reyeRight.y(), 2));

            float reyeEVR = (reyeA + reyeB) / (2 * reyeC);

//            //计算人嘴纵横比
//            dlib::point mouth_left = face.part(49);
//            dlib::point mouth_right = face.part(55);

//            dlib::point mouth_leftUp = face.part(51);
//            dlib::point mouth_leftDown = face.part(59);

//            dlib::point mouth_rightUp = face.part(53);
//            dlib::point mouth_rightDown = face.part(57);

//            float mouthA = sqrt(pow(mouth_leftUp.x() - mouth_leftDown.x(), 2) + pow(mouth_leftUp.y() - mouth_leftDown.y(), 2));
//            float mouthB = sqrt(pow(mouth_rightUp.x() - mouth_rightDown.x(), 2) + pow(mouth_rightUp.y() - mouth_rightDown.y(), 2));
//            float mouthC = sqrt(pow(mouth_left.x() - mouth_right.x(), 2) + pow(mouth_left.y() - mouth_right.y(), 2));

//            float mouthEVR = (mouthA + mouthB) / (2 * mouthC);


//            //摇头
//            //左脸边缘
//            dlib::point face_left = face.part(2);
//            //右脸边缘
//            dlib::point face_right = face.part(16);
//            //鼻子中心
//            dlib::point face_nose = face.part(31);

//            //鼻子到左脸边缘距离
//            float lfaceLength = sqrt(pow(face_nose.x() - face_left.x(), 2) + pow(face_nose.y() - face_left.y(), 2));
//            //鼻子到右脸边缘距离
//            float rfaceLength = sqrt(pow(face_nose.x() - face_right.x(), 2) + pow(face_nose.y() - face_right.y(), 2));


//            //记录张嘴次数
//            if (mouthEVR < 0.62){
//                //闭嘴
//                counter_mouth += 1;
//            }else if(mouthEVR > 0.70){
//                //张嘴
//                if (counter_mouth >= 1) {
//                    mouthIndex += 1;
//                }
//                counter_mouth = 0;
//            }
//            else {
//                //此区间处于临界区，不稳定，不做检测
//            }


//            // 显示张嘴次数
//            char mouthBuf[100] = { 0 };
//            sprintf(mouthBuf, "mouth couent : %d", mouthIndex);
//            cv::putText(frame, mouthBuf, cv::Point(0, 20), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);


            //记录左眼眨眼次数
            if (leyeEVR > 3.0) {
                //闭眼
                counter_leye += 1;
            }
            else {
                //睁眼
                if (counter_leye >= 1) {
                    leyeIndex += 1;
                }
                counter_leye = 0;
            }

            //记录右眼眨眼次数
            if (reyeEVR > 6.0) {
                //闭眼
                counter_reye += 1;
            }
            else {
                //睁眼
                if (counter_reye >= 1) {
                    reyeIndex += 1;
                }
                counter_reye = 0;
            }

            //取最小值
            if (reyeIndex > leyeIndex) {
                reyeIndex = leyeIndex;
            }

            //显示眨眼次数
            char eyeBuf[100] = { 0 };
            sprintf(eyeBuf, "eye count: %d", reyeIndex);
            cv::putText(frame, eyeBuf, cv::Point(0, 45), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
            emit imageToView(Mat2Image(frame));
        }else{
            emit imageToView(Mat2Image(frame));
        }
        if(reyeIndex > 0){
//            qDebug()<<reyeIndex<<"发送信号";
//            cap.release();
            emit isLiving(true);

            emit faceImage(frame);
            reyeIndex = 0;
//            return ;
        }
//        cv::imshow("frame",frame);
//        cv::waitKey(50);
    }
    //cv::destroyAllWindows();
}

QImage LiveBlink::Mat2Image(const cv::Mat mat){
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

void LiveBlink::stopLiveBlink(){
    this->isStop = true;
}

void LiveBlink::startLiveBlink(){
    this->isStop = false;
}
