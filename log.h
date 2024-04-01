#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
class Log
{
public:
    Log();
private:
    QString fileName;
    QFile file;
};

#endif // LOG_H
