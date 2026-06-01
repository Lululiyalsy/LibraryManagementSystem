/**
 * @file User.h
 * @brief 用户基类定义
 *
 * User类是管理员(Admin)和读者(Reader)的基类，提供用户的基本属性和操作。
 * 包括用户身份信息管理和消息管理功能。
 */

#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <vector>
#include "Message.h"

// （前向声明）：前向声明需要的类，避免循环包含
class DataManager;
class Book;
class BorrowRecord;
class Reservation;

/**
 * @class User
 * @brief 用户基类
 *
 * 定义用户的基本属性和行为，包括身份信息、联系方式和消息管理。
 * 作为 Admin 和 Reader 类的基类，提供统一的接口。
 */
class User
{
public:
    /**
     * @brief 默认构造函数
     */
    User();

    /**
     * @brief 构造函数
     * @param I 用户ID
     * @param n 用户姓名
     * @param pa 用户密码
     * @param ph 联系电话
     * @param e 电子邮箱
     */
    User(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    /**
     * @brief 析构函数
     */
    virtual ~User();

    /**
     * @brief 获取用户身份标识（纯虚函数）
     * @return 用户身份字符串（如"管理员"或"读者"）
     */
    virtual QString typeToIdentity() = 0;

    /**
     * @brief 生成验证码（纯虚函数，由子类实现）
     * @return 验证码字符串
     */
    virtual QString generateVerificationCode() = 0;

    /**
     * @brief 添加消息到用户消息列表
     * @param message 要添加的消息对象
     */
    void addMessage(const Message &message);

    /**
     * @brief 删除指定索引的消息
     * @param index 消息索引
     */
    void deleteMessage(int index);

    /**
     * @brief 清除所有消息
     */
    void clearMessages();

    /**
     * @brief 获取未读消息数量
     * @return 未读消息的数量
     */
    int getUnreadMessageCount();

    // ========== getter 方法 ==========

    /**
     * @brief 获取用户ID
     * @return 用户ID
     */
    const QString &getID() const;

    /**
     * @brief 获取用户类型
     * @return 用户类型（1=管理员，2=读者）
     */
    int getType() const;

    /**
     * @brief 获取用户姓名
     * @return 用户姓名
     */
    const QString &getName() const;

    /**
     * @brief 获取用户密码
     * @return 用户密码
     */
    const QString &getPassword() const;

    /**
     * @brief 获取联系电话
     * @return 联系电话
     */
    const QString &getPhone() const;

    /**
     * @brief 获取电子邮箱
     * @return 电子邮箱
     */
    const QString &getEmail() const;

    /**
     * @brief 获取消息列表
     * @return 消息列表引用
     */
    std::vector<Message> &getMessages();

    // ========== setter 方法 ==========

    /**
     * @brief 设置用户ID
     * @param I 用户ID
     */
    void setID(const QString &I);

    /**
     * @brief 设置用户类型
     * @param t 用户类型（1=管理员，2=读者）
     */
    void setType(int t);

    /**
     * @brief 设置用户姓名
     * @param n 用户姓名
     */
    void setName(const QString &n);

    /**
     * @brief 设置用户密码
     * @param pa 用户密码
     */
    void setPassword(const QString &pa);

    /**
     * @brief 设置联系电话
     * @param ph 联系电话
     */
    void setPhone(const QString &ph);

    /**
     * @brief 设置电子邮箱
     * @param e 电子邮箱
     */
    void setEmail(const QString &e);

    // ========== 成员变量 ==========

    QString ID;                    ///< 用户唯一标识
    int type;                      ///< 用户类型（1=管理员，2=读者）
    QString name;                  ///< 用户姓名
    QString password;              ///< 用户密码
    QString phone;                 ///< 绑定电话
    QString email;                 ///< 绑定邮箱
    std::vector<Message> messages; ///< 消息容器
};

#endif // USER_H