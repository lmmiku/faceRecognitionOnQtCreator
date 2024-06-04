#include "threadfacerecord.h"

threadFaceRecord::threadFaceRecord(){
    cascade.load("C:/Users/shocker/QTpractise/face_recognition/haarcascade_frontalface_alt2.xml");
    this->live = new LiveBlink();
    QDir dir;
    if(!dir.exists(videoPath)){
        dir.mkpath(videoPath);
    }
    connect(live,&LiveBlink::isLiving,this,[=](bool flag){
        live->stopLiveBlink();
        this->isLiving = true;
    });
    connect(live,&LiveBlink::imageToView,this,[=](QImage image){
        emit imageToRecord(image);
    });
}

void threadFaceRecord::faceRecord(QString name,QString gender,QString number,QString age,QString c){
    QDir dir;
    if(!dir.exists(videoPath+name+"_"+number) && !DataBase::instance()->inquiryInfo(number)){
        dir.mkpath(videoPath+name+"_"+number);
    }else{
        return ;
    }
    live->bioassay();
    while(isLiving){
        if(cap.open(0)){
            qDebug()<<"faceRecord相机成功打开";
            this->isStop = false;
        }
        cv::Mat frame, myFace;
        int pic_num = 1;
        while (!isStop) {
            cap >> frame;
            if(frame.empty() || isStop){
                qDebug()<<"faceRecord获取为空"<<"摄像头状态："<<cap.isOpened()<<"isStop = "<<isStop;
                return;
            }
//            try{
            std::vector<cv::Rect> faces;//vector容器存检测到的faces
            cv::Mat frame_gray;
            cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);//转灰度化，减少运算
            cascade.detectMultiScale(frame_gray, faces, 1.1, 4, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(70, 70), cv::Size(1000, 1000));

            qDebug()<<QString("检测到人脸个数：%1").arg(faces.size());
            //识别到的脸用矩形圈出
            for (unsigned int i = 0; i < faces.size(); i++)
            {
                cv::rectangle(frame, faces[i], cv::Scalar(255, 0, 0), 2, 8, 0);
            }
            //当只有一个人脸时，开始拍照
            if (faces.size() != 0 /*&& HOG_SVM->predict(frame) == 1*/){
                cv::Mat faceROI = frame_gray(faces[0]);//在灰度图中将圈出的脸所在区域裁剪出
                cv::resize(faceROI, myFace, cv::Size(92, 112));//将兴趣域size为92*112
                putText(frame, QString::number(pic_num).toStdString(), faces[0].tl(), 3, 1.2, (0, 0, 225), 2, 0);//在 faces[0].tl()的左上角上面写序号
                QString filename = QString(videoPath+name+"_"+number+"/%1.jpg").arg(pic_num); //图片的存放位置，frmat的用法跟QString差不对
                cv::imwrite(filename.toLocal8Bit().toStdString(), myFace);//存在当前目录下
                //cv::imshow(filename.toStdString(), myFace);//显示下size后的脸
                QThread::usleep(100);//等待100us
                cv::destroyWindow(filename.toLocal8Bit().toStdString());//:销毁指定的窗口
                pic_num++;//序号加1
                if (pic_num == 21){
//                    int label = this->append_csv(name+"_"+number);
                    DataBase::instance()->addToStuInfo(-1,name,gender,number,age.toInt(),c);
                    this->create_csv();
                    this->faceTrain();
                    emit end();
                    return;//当序号为11时退出循环,一共拍10张照片
                }
            }
            emit imageToRecord(Mat2Image(frame));
            QThread::usleep(300);//等待100us
//            //qDebug()<<"faceRecord 正在运行" <<"isStop == "<<isStop;
//            }catch(cv::Exception& e){

//            }
        }

    }
}

void threadFaceRecord::faceTrain(){
    //读取你的CSV文件路径.
    //string fn_csv = string(argv[1]);
    std::string fn_csv = QString(videoPath+"at.txt").toLocal8Bit().toStdString();

    // 2个容器来存放图像数据和对应的标签
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    // 读取数据. 如果文件不合法就会出错
    // 输入的文件名已经有了.
    try{
        read_csv(fn_csv, images, labels); //从csv文件中批量读取训练数据
    }
    catch (cv::Exception& e){
        qDebug() << "Error opening file \"" << QString::fromStdString(fn_csv) << "\". Reason: " << e.msg.c_str();
        return;
    }
    // 如果没有读取到足够图片，也退出.
    if (images.size() <= 1) {
        std::string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(CV_StsError, error_message);
    }
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    try{
        model->train(images, labels);
    }catch(cv::Exception &e){

    }

    model->save("MyFaceLBPHModel1.xml");
    emit reload();

    getchar();
    //waitKey(0);
    return;
}

void threadFaceRecord::create_csv(){
    QString BASE_PATH = videoPath;
    QString SEPARATOR = ";";
    QFile outFile(BASE_PATH+"at.txt");

    outFile.open(QFile::WriteOnly|QFile::Truncate);
    outFile.close();

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        qDebug() << "Failed to open file for writing";
        return;
    }
    QTextStream fh(&outFile);
    int label = 0;

    QDirIterator it(BASE_PATH, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString subject_path = it.next();
        QDir subjectDir(subject_path);
        qDebug()<<subject_path<<"  "<<subjectDir;
        QStringList stringlist = subject_path.split("/");
        //DataBase::instance()->addToStuInfo(label,name,gender,number,age.toInt(),c);
        //DataBase::instance()->getId_stuNumber(stringlist.at(1).split("_").at(1));
        DataBase::instance()->updateId_stuNumber(stringlist.at(1).split("_").at(1),label);
        subjectDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList files = subjectDir.entryInfoList();
        foreach (QFileInfo file, files)
        {
            QString abs_path = file.absoluteFilePath();
            //qDebug() << abs_path + SEPARATOR + QString::number(label);
            fh << abs_path << SEPARATOR << label << "\n";
        }
        label++;
    }

    outFile.close();
}

int threadFaceRecord::append_csv(QString dirName){
    QString BASE_PATH = videoPath;
    QString SEPARATOR = ";";
    QFile outFile(BASE_PATH+"at.txt");
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        qDebug() << "Failed to open file for writing";
        return 0;
    }
    QTextStream fh(&outFile);
    //查询最大的id，没有查到返回1
    int label = DataBase::instance()->getLastIdFromStuInfo()+1;
    QDir subjectDir(BASE_PATH+dirName);
    subjectDir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList files = subjectDir.entryInfoList();
    foreach (QFileInfo file, files)
    {
        QString abs_path = file.absoluteFilePath();
        //qDebug() << abs_path + SEPARATOR + QString::number(label);
        fh << abs_path << SEPARATOR << label << "\n";
    }
    return label;
    outFile.close();
}

void threadFaceRecord::read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, char separator) {
    std::ifstream file(filename.c_str(), std::ifstream::in);
    if (!file)
    {
        std::string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    std::string line, path, classlabel;
    while (getline(file, line)) //从文本文件中读取一行字符，未指定限定符默认限定符为“/n”
    {
        std::stringstream liness(line);//这里采用stringstream主要作用是做字符串的分割
        getline(liness, path, separator);//读入图片文件路径以分号作为限定符
        getline(liness, classlabel);//读入图片标签，默认限定符
        if (!path.empty() && !classlabel.empty()) //如果读取成功，则将图片和对应标签压入对应容器中
        {
            images.push_back(cv::imread(path, 0));
            //qDebug()<<path.c_str();
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

void threadFaceRecord::stopFaceRecord(){
    this->isStop = true;
    this->cap.release();
}

QImage threadFaceRecord::Mat2Image(const cv::Mat mat){
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

threadFaceRecord::~threadFaceRecord(){

}
