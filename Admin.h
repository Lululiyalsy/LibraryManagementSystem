/**
 * @file Admin.h
 * @brief 管理员类定义
 * 
 * Admin类继承自User类，提供管理员特有的功能，包括用户管理、图书管理、
 * 预约管理和借阅管理等功能。
 */

#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"
#include <QString>
#include <vector>

/**
 * @class Admin
 * @brief 管理员类
 * 
 * 继承自User类，实现管理员的全部功能，包括用户管理、图书管理、
 * 预约管理、借阅管理和统计报表生成等。
 */
class Admin : public User
{
public:
    /**
     * @brief 构造函数
     * @param I 用户ID
     * @param n 用户姓名
     * @param pa 用户密码
     * @param ph 联系电话
     * @param e 电子邮箱
     */
    Admin(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    /**
     * @brief 获取用户身份标识（实现User的纯虚函数）
     * @return 返回"管理员"
     */
    QString typeToIdentity() override;

    /**
     * @brief 生成验证码（实现User的纯虚函数）
     * @return 6位验证码（大小写字母+数字）
     */
    QString generateVerificationCode() override;

    // ========== 用户管理 ==========

    /**
     * @brief 注册用户账号（包括管理员和读者）
     * @param id 用户ID
     * @param type 用户类型（1=管理员，2=读者）
     * @param name 用户姓名
     * @param password 用户密码
     * @param phone 联系电话
     * @param email 电子邮箱
     */
    void registerUser(const QString &id, const QString &type, const QString &name,
                      const QString &password, const QString &phone, const QString &email);

    /**
     * @brief 删除用户账号（包括管理员和读者）
     * @param id 用户ID
     * @param name 用户姓名
     * @return 删除成功返回true，失败返回false
     */
    bool deleteUser(const QString &id, const QString &name);

    /**
     * @brief 修改用户信息（包括管理员和读者）
     * @param oldId 原用户ID
     * @param oldName 原用户姓名
     * @param newId 新用户ID
     * @param newType 新用户类型
     * @param newName 新用户姓名
     * @param password 新密码
     * @param phone 新联系电话
     * @param email 新电子邮箱
     * @return 修改成功返回true，失败返回false
     */
    bool updateUser(const QString &oldId, const QString &oldName,
                    const QString &newId, const QString &newType, const QString &newName,
                    const QString &password, const QString &phone, const QString &email);

    /**
     * @brief 查看全部用户信息（包括管理员和读者）
     * @return 用户指针列表
     */
    std::vector<::User *> findAllUser();

    /**
     * @brief 查找用户信息（支持ID、姓名模糊匹配）
     * @param id 用户ID（可选）
     * @param name 用户姓名（可选）
     * @return 匹配的用户指针列表
     */
    std::vector<::User *> findUser(const QString &id = "", const QString &name = "");

    /**
     * @brief 清空用户信息（包括管理员和读者）
     * @param currentAdmin 当前管理员指针（用于保留当前管理员）
     * @return 新的当前管理员指针
     */
    User *clearUser(User *currentAdmin = nullptr);

    // ========== 图书管理 ==========

    /**
     * @brief 添加书本信息
     * @param isbn 图书ISBN
     * @param title 书名
     * @param author 作者
     * @param category 分类
     * @param stock 库存数量
     * @return 0=成功新增，1=库存已增加，-1=ISBN冲突
     */
    int addBook(const QString &isbn, const QString &title, const QString &author,
                const QString &category, int stock);

    /**
     * @brief 删除书本信息
     * @param isbn 图书ISBN
     * @param decreaseStock 减少的库存数量
     * @return 0=成功删除记录，1=库存已减少，-1=ISBN不存在，-2=存在预约或借出无法删除
     */
    int deleteBook(const QString &isbn, int decreaseStock);

    /**
     * @brief 修改书本信息
     * @param oldIsbn 原ISBN
     * @param newIsbn 新ISBN
     * @param title 书名
     * @param author 作者
     * @param category 分类
     * @param stock 库存数量
     * @param inStockTime 入库时间
     * @return 0=成功修改，-1=原ISBN不存在，-2=存在预约或借出无法修改，-3=新ISBN已存在
     */
    int updateBook(const QString &oldIsbn, const QString &newIsbn, const QString &title,
                   const QString &author, const QString &category, int stock,
                   const QDateTime &inStockTime);

    /**
     * @brief 查找书本信息（支持多条件模糊搜索）
     * @param isbn ISBN（可选）
     * @param title 书名（可选）
     * @param author 作者（可选）
     * @param category 分类（可选）
     * @return 匹配的图书指针列表
     */
    std::vector<const Book *> findBook(const QString &isbn, const QString &title,
                                       const QString &author, const QString &category);

    /**
     * @brief 查找所有图书
     * @return 所有图书指针列表
     */
    std::vector<const Book *> findAllBook();

    /**
     * @brief 清空书本信息
     */
    void clearBook();

    /**
     * @brief 按借阅次数排序
     * @return 按借阅次数降序排列的图书列表
     */
    std::vector<const Book *> sortBookByBorrowCount();

    /**
     * @brief 按入库时间排序（最新入库优先）
     * @return 按入库时间排序的图书列表
     */
    std::vector<const Book *> sortBookByInStockTime();

    /**
     * @brief 生成统计报表（主要是借书情况）
     */
    void generateReport();

    // ========== 预约管理 ==========

    /**
     * @brief 查看所有预约记录
     * @return 预约记录列表
     */
    std::vector<Reservation> viewAllReservations();

    /**
     * @brief 审核预约
     * @param isbn 图书ISBN
     * @param readerId 读者ID
     * @param isSuccess 是否通过审核
     * @return 审核成功返回true，失败返回false
     */
    bool approveReservation(const QString &isbn, const QString &readerId, bool isSuccess);

    // ========== 借阅管理 ==========

    /**
     * @brief 办理续借审核
     * @param isbn 图书ISBN
     * @param readerId 读者ID
     * @param approved 是否批准续借
     * @return 审核成功返回true，失败返回false
     */
    bool renewBook(const QString &isbn, const QString &readerId, bool approved);

    /**
     * @brief 查看所有借阅记录
     * @return 借阅记录列表
     */
    std::vector<BorrowRecord> viewBorrowRecords();

    /**
     * @brief 查看逾期记录
     * @return 逾期借阅记录列表
     */
    std::vector<BorrowRecord> viewOverdueRecords();

    /**
     * @brief 析构函数
     */
    ~Admin();
};

#endif // ADMIN_H