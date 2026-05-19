#ifndef USER_H
#define USER_H

#include <QString>

//（前向声明）：前向声明需要的类，避免循环包含
class DataManager;
class Book;
class BorrowRecord;
class Reservation;

class User
{
public:

    // 默认构造函数
    User();

    // 构造函数
    User(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    // 析构函数
    virtual ~User();

    // 获取用户身份
    virtual QString typeToIdentity() = 0;

    // 生成验证码（纯虚函数，由子类实现）
    virtual QString generateVerificationCode() = 0;

    // getter和setter
    const QString &getID() const;
    int getType() const;
    const QString &getName() const;
    const QString &getPassword() const;
    const QString &getPhone() const;
    const QString &getEmail() const;

    // Setter
    void setID(const QString &I);
    void setType(int t);
    void setName(const QString &n);
    void setPassword(const QString &pa);
    void setPhone(const QString &ph);
    void setEmail(const QString &e);

    // 用户唯一标识
    QString ID;
    // 用户类型
    int type;
    // 姓名
    QString name;
    // 密码
    QString password;
    // 绑定电话
    QString phone;
    // 绑定邮箱
    QString email;
};

#endif // USER_H