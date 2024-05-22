#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <tuple>
class DataBase
{
public:
    static DataBase* instance();
    //插入员工（学生）信息表->id,name,gender,Number,age
    bool addToStuInfo(int id,QString name,QString gender,QString Number,int age,QString c);
    //取出最大的id号，用于新人脸的预测值
    int getLastIdFromStuInfo();
    //通过id值查找信息
    QString getInfoFromStuInfo(int id);
    //通过id查找学号
    QString getStuNumber(int id);
    //查询用户信息是否已添加
    bool inquiryInfo(QString number);
    //验证管理员账号密码是否正确
    bool verifyAdmin(QString adminAccount,QString adminPassword);
    //通过账号查找管理的班级
    QVector<QString> getClassFromAdminInfo(QString account);
    //通过管理员账号查找用户
    void extracted(QVector<QString> &classInfo, QVector<QString> &idInfo);
    QVector<QString> getInfoFromAdminInfo(QString account);
    //通过学号查找用户所有信息
    std::tuple<int,QString,QString,QString,int,QString> getInfoFromId(QString stuNumber);
    //通过id获取考勤状态
    QString getStateFromId(QString stuNumber);
    //通过学号删除用户
    bool deleteUserFromStuInfo(QString stuNumber);
    //修改打卡状态
    bool updataStateForAttendanceInfo(QString stuNumber,QString state);
    //插入打卡记录
    bool insertToAttendanceInfo(QString stuNumber,QString state,QString time);
    //查看考勤表中是否存信息
    bool getExist(QString stuNumber);
    //通过学号获取id
    int getId_stuNumber(QString stuNumber);
    //通过学号获取打卡时间
    QDateTime getTime_stuNumber(QString stuNumber);
    //通过学号查找姓名
    QString getName(QString stuNumber);
    //根据年月日查找对应天数考勤情况
    std::tuple<QString,QString> getAttendanceInfo(QDate date,QString stuNumber);
    //通过account查找课表信息
    QVector<std::tuple<QString,QString>> getclassScheduleInfo(QString account);
    //插入account课表信息
    bool insertclassScheduleInfo(QString account,QVector<std::tuple<QString,QString>> data);
    //通过学号更新id
    bool updateId_stuNumber(QString stuNumber,int id);
    //通过学号修改考勤状态
    bool updateState_stuNumbe(QString stuNumber);

private:
    DataBase();
    DataBase(const DataBase&) = delete;
    DataBase& operator=(const DataBase&) = delete;
    ~DataBase();
    static DataBase* myInstance;
    static void deleteInstance();
    QSqlQuery query;
};

#endif // DATABASE_H
