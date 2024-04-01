#include "log.h"

Log::Log()
{
    this->fileName = QString(QDir::currentPath()+"/serial"+QDateTime::currentDateTime().toString("yyyy_MM_dd").append(".log"));
    this->file.setFileName(this->fileName);
    if(!this->file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        QMessageBox::warning(nullptr,"失败","日志文件打开失败！");
}
