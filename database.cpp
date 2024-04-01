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
    }else{
        if(query.next()){
            info = std::make_tuple(query.value(0).toInt(),query.value(1).toString(),query.value(2).toString()\
                                   ,query.value(3).toString(),query.value(4).toInt(),query.value(5).toString());
        }
    }
    return info;
}

QString DataBase::getStateFromId(int id){
    QString select = QString("SELECT state FROM attendanceInfo WHERE id = '%1'").arg(id);
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

bool DataBase::deleteUserFromStuinfo(QString stuNumber){
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

DataBase::~DataBase(){

}
