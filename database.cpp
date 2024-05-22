#include "database.h"
DataBase* DataBase::myInstance = new DataBase();
DataBase::DataBase()
{
    QSqlDatabase database;

    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else{
        // 建立和SQlite数据库的连接
        database = QSqlDatabase::addDatabase("QSQLITE");
        // 设置数据库文件的名字
        database.setDatabaseName("database/facerecongInfor.db");
    }
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        query = QSqlQuery(database);
    }
}

DataBase* DataBase::instance(){
    return myInstance;
}

void DataBase::deleteInstance(){
    delete myInstance;
}

bool DataBase::addToStuInfo(int id,QString name,QString gender,QString number,int age,QString c){
    QString insert = QString("INSERT INTO stuInfo VALUES(%1,'%2','%3','%4',%5,'%6')")\
                         .arg(id).arg(name).arg(gender).arg(number).arg(age).arg(c);
    qDebug()<<insert;
    if(!query.exec(insert)){
        qDebug() << "Error:addToStuInfo->Fail to insert data. " << query.lastError();
    }else{
        //查询一次，确保数据已经写入
        return query.exec(QString("SELECT *FROM stuInfo WHERE id = %1").arg(id));
    }
    return false;
}

int DataBase::getLastIdFromStuInfo(){
    QString select = QString("SELECT id FROM stuInfo  ORDER BY id DESC LIMIT 1");
    if(!query.exec(select)){
        qDebug() << "Error:getLastIdFromStuInfo->Fail to select data. " << query.lastError();
    }else{
        if(query.next()){
            return query.value(0).toInt();
        }
    }
    return 0;
}

QString DataBase::getInfoFromStuInfo(int id){
    qDebug()<<id;
    QString select = QString("SELECT stuName FROM stuInfo WHERE id = %1").arg(id);
    if(!query.exec(select)){
        qDebug() << "Error:getInfoFromStuInfo->Fail to select data. " << query.lastError();
    }else{
        if(query.next()){
            return query.value(0).toString();
        }
    }
    return "";
}

bool DataBase::inquiryInfo(QString number){
    QString select = QString("SELECT id FROM stuInfo WHERE stuNumber = '%1'").arg(number);
    if(!query.exec(select)){
        qDebug() << "Error:inquiryInfo->Fail to select data. " << query.lastError();
    }else{
        if(query.next()){
            return true;
        }
    }
    return false;
}

bool DataBase::verifyAdmin(QString adminAccount,QString adminPassword){
    QString select = QString("SELECT id FROM adminInfo WHERE adminAccount = '%1' AND adminPassword = '%2'").arg(adminAccount).arg(adminPassword);
    if(!query.exec(select)){
        qDebug() << "Error:verifyAdmin->Fail to select data. " << query.lastError();
    }else{
        if(query.next()){
            return true;
        }
    }
    return false;
}

std::tuple<int,QString,QString,QString,int,QString> DataBase::getInfoFromId(QString stuNumber){
    std::tuple<int,QString,QString,QString,int,QString> info;
    QString select = QString("SELECT * FROM stuInfo WHERE stuNumber = '%1'").arg(stuNumber);
    if(!query.exec(select)){
        qDebug()<<"Error:getInfoFromId->Fail to select data. " << query.lastError();
        return info;
    }
    if(query.next()){
        info = std::make_tuple(query.value("id").toInt(),query.value("stuName").toString(),query.value("stuGender").toString()\
                               ,query.value("stuNumber").toString(),query.value("stuAge").toInt(),query.value("class").toString());
    }
    return info;
}

QString DataBase::getStateFromId(QString stuNumber){
    QString select = QString("SELECT state FROM attendanceInfo WHERE stuNumber = '%1'").arg(stuNumber);
    if(!query.exec(select)){
        qDebug()<<"Error:getStateFromId->Fail to select data. " << query.lastError();
    }else{
        if(query.next()){
            return query.value(0).toString();
        }
    }
    return "未打卡";
}

QVector<QString> DataBase::getInfoFromAdminInfo(QString account){
    QVector<QString> classInfo = this->getClassFromAdminInfo(account);
    QVector<QString> idInfo;
    for(QString c:classInfo){
        QString select = QString("SELECT stuNumber FROM stuInfo WHERE class = '%1'").arg(c);
        query.exec(select);
        while(query.next()){
            idInfo.append(query.value(0).toString());
        }
    }
    return idInfo;
}

bool DataBase::deleteUserFromStuInfo(QString stuNumber){
    QString del = QString("DELETE FROM stuInfo WHERE stuNumber = '%1'").arg(stuNumber);
    if(!query.exec(del)){
        qDebug()<<"Error:deleteUserFromStuinfo->Fail to delete data. " << query.lastError();
    }else{
        return true;
    }
    return false;
}

QVector<QString> DataBase::getClassFromAdminInfo(QString account){
    QString select = QString("SELECT class FROM adminInfo WHERE adminAccount = '%1'").arg(account);
    QVector<QString> classInfo;
    query.exec(select);
    while(query.next()){
        classInfo.append(query.value(0).toString());
    }
    return classInfo;
}

bool DataBase::updataStateForAttendanceInfo(QString stuNumber,QString state){
    QString updata = QString("UPDATE attendanceInfo SET state = '%1' WHERE stuId = '%2'").arg(state).arg(stuNumber);
    if(!query.exec(updata)){
        qDebug()<<"Error:deleteUserFromStuinfo->Fail to delete data. " << query.lastError();
    }else{
        return true;
    }
    return false;
}

int DataBase::getId_stuNumber(QString stuNumber){
    QString select = QString("SELECT id FROM stuInfo WHERE stuNumber = '%1'").arg(stuNumber);
    query.exec(select);
    if(query.next()){
        return query.value(0).toInt();
    }
    return -1;
}

bool DataBase::insertToAttendanceInfo(QString stuNumber,QString state,QString time){
    QDateTime dateTime = QDateTime::currentDateTime();
    QString string;
//    if(getExist(stuNumber)){
//        string = QString("UPDATE attendanceInfo SET state = '%1' WHERE stuNumber = '%2'").arg(state).arg(stuNumber);
//    }else{
//        string = QString("INSERT INTO attendanceInfo (stuId,stuNumber,state,time) VALUES(%1,'%2','%3','%4')").arg(getId_stuNumber(stuNumber)).arg(stuNumber).arg(state).arg(time);
//    }
    string = QString("INSERT INTO attendanceInfo (stuId,stuNumber,state,time,DATE_,year,mouth,date) VALUES(%1,'%2','%3','%4','%5',%6,%7,%8)")
                 .arg(getId_stuNumber(stuNumber)).arg(stuNumber).arg(state).arg(time).arg(dateTime.toString("yyyy.MM.dd")).arg(dateTime.date().year()).
             arg(dateTime.date().month()).arg(dateTime.date().day());
    if(!query.exec(string)){
        qDebug()<<"Error:insertToAttendanceInfo->Fail to insert data. " << query.lastError();
    }else{
        return true;
    }
    return false;
}

QString DataBase::getStuNumber(int id){
    QString select = QString("SELECT stuNumber FROM stuInfo WHERE id = %1").arg(id);
    if(!query.exec(select)){
        qDebug() << "Error:getInfoFromStuInfo->Fail to select data. " << query.lastError();
    }else{
        if(query.next()){
            return query.value(0).toString();
        }
    }
    return "";
}

QString DataBase::getName(QString stuNumber){
    QString select = QString("SELECT stuName FROM stuInfo WHERE stuNumber = '%1'").arg(stuNumber);
    if(!query.exec(select)){
        qDebug() << "Error:getInfoFromStuInfo->Fail to select data. " << query.lastError();
    }
    if(query.next()){
        return query.value(0).toString();
    }
    return "";
}

QDateTime DataBase::getTime_stuNumber(QString stuNumber){
    QString select = QString("SELECT time FROM attendanceInfo WHERE stuNumber = '%1'").arg(stuNumber);
    query.exec(select);
    if(query.next()){
        return query.value(0).toDateTime();
    }
    return QDateTime();
}

bool DataBase::getExist(QString stuNumber){
    QString select = QString("SELECT * FROM attendanceInfo WHERE stuNumber = '%1'").arg(stuNumber);
    query.exec(select);
    if(query.next()){
        return true;
    }
    return false;
}

std::tuple<QString,QString> DataBase::getAttendanceInfo(QDate date,QString stuNumber){
    QString Data = date.toString("yyyy.MM.dd");
    //QString select = QString("SELECT state,time FROM attendanceInfo WHERE stuNumber = '%1' AND year = %2 AND mouth = %3 AND date = %4").arg(stuNumber).arg(year).arg(mouth).arg(day);
    QString select = QString("SELECT state,time FROM attendanceInfo WHERE stuNumber = '%1' AND DATE_ = '%2'").arg(stuNumber).arg(Data);
    qDebug()<<select;
    if(!query.exec(select)){
        qDebug() << "Error:getAttendanceInfo->Fail to select data. " << query.lastError();
    }
    if(query.next()){
        return std::make_tuple(query.value(0).toString(),query.value(1).toString());
    }
    return std::tuple<QString,QString>{};
}

QVector<std::tuple<QString,QString>> DataBase::getclassScheduleInfo(QString account){
    QVector<std::tuple<QString,QString>> data;
    QString select = QString("SELECT startSignTime,endSignTime FROM classSchedule WHERE account = '%1'").arg(account);
    qDebug()<<select;
    if(!query.exec(select)){
        qDebug() << "Error:getclassScheduleInfo->Fail to select data. " << query.lastError();
        return data;
    }
    while(query.next()){
        std::tuple<QString,QString> t = std::make_tuple(query.value(0).toString(),query.value(1).toString());
        data.append(t);
    }
    return data;
}

bool DataBase::insertclassScheduleInfo(QString account,QVector<std::tuple<QString,QString>> data){
    QString delete_ = QString("DELETE FROM classSchedule WHERE account = '%1'").arg(account);
    if(!query.exec(delete_)){
        qDebug() << "Error:getclassScheduleInfo->Fail to select data. " << query.lastError();
        return false;
    }
    for(std::tuple<QString,QString> t:data){
        QString insert = QString("INSERT INTO classSchedule (account,startSignTime,endSignTime) VALUES('%1','%2','%3')").arg(account).arg(std::get<0>(t)).arg(std::get<1>(t));
        if(!query.exec(insert)){
            qDebug() << "Error:insertclassScheduleInfo->Fail to insert data. " << query.lastError();
            return false;
        }
    }
    return true;
}

bool DataBase::updateId_stuNumber(QString stuNumber,int id){
    QString update = QString("UPDATE stuInfo SET id = %1 WHERE stuNumber = '%2'").arg(id).arg(stuNumber);
    if(!query.exec(update)){
        qDebug() << "Error:updateId_stuNumber->Fail to update data. " << query.lastError();
        return false;
    }
    return true;
}

bool DataBase::updateState_stuNumbe(QString stuNumber){
    QString update = QString("UPDATE attendanceInfo SET state = '%1' WHERE stuNumber = '%2'").arg("已打卡").arg(stuNumber);
    if(!query.exec(update)){
        qDebug() << "Error:updateState_stuNumbe->Fail to update data. " << query.lastError();
        return false;
    }
    return true;
}

DataBase::~DataBase(){

}
