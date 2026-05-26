#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"
#include <QString>
#include <vector>

class user;

class Admin : public User
{
public:
    // 构造函数
    Admin(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    // 实现User的纯虚函数：获取用户身份
    QString typeToIdentity() override;

    // 实现User的纯虚函数：生成验证码（管理员：大小写字母+数字，6位）
    QString generateVerificationCode() override;

    // 注册用户账号（包括管理员和读者）
    void registerUser(const QString &id, const QString &type, const QString &name,
                      const QString &password, const QString &phone, const QString &email);

    // 删除用户账号（包括管理员和读者）
    bool deleteUser(const QString &id, const QString &name);

    // 修改用户信息（包括管理员和读者）
    bool updateUser(const QString &oldId, const QString &oldName,
                    const QString &newId, const QString &newType, const QString &newName,
                    const QString &password, const QString &phone, const QString &email);

    // 查看全部用户信息（包括管理员和读者）
    std::vector<::User *> findAllUser();

    // 查找用户信息（支持ID、姓名、ID+姓名模糊匹配）
    std::vector<::User *> findUser(const QString &id = "", const QString &name = "");

    // 清空用户信息（包括管理员和读者）
    void clearUser();

    // 析构函数
    ~Admin();

    // 添加书本信息，返回值：0=成功新增，1=库存已增加，-1=ISBN冲突
    int addBook(const QString &isbn, const QString &title, const QString &author,
                const QString &category, int stock);

    // 删除书本信息，返回值：0=成功删除记录，1=库存已减少，-1=ISBN不存在，-2=存在预约或借出无法删除
    int deleteBook(const QString &isbn, int decreaseStock);

    // 修改书本信息，返回值：0=成功修改，-1=原ISBN不存在，-2=存在预约或借出无法修改，-3=新ISBN已存在
    int updateBook(const QString &oldIsbn, const QString &newIsbn, const QString &title, 
                   const QString &author, const QString &category, int stock, 
                   const QDateTime &inStockTime);

    // 查找书本信息（支持多条件模糊搜索）
    std::vector<const Book *> findBook(const QString &isbn, const QString &title,
                                       const QString &author, const QString &category);

    // 查找所有图书
    std::vector<const Book *> findAllBook();

    // 清空书本信息
    void clearBook();

    // 按借阅次数排序
    std::vector<const Book *> sortBookByBorrowCount();

    // 按入库时间排序（最新入库优先）
    std::vector<const Book *> sortBookByInStockTime();

    // 生成统计报表(主要是借书情况)
    void generateReport();

    // 处理预约信息
    void processReservation();
    bool processSingleReservation(const QString &isbn, const QString &readerId);

    // 预约管理：查看所有预约记录
    std::vector<Reservation> viewAllReservations();
    // 预约管理：取消预约（按ISBN和读者ID）
    bool cancelReservation(const QString &isbn, const QString &readerId);
    // 预约管理：图书归还时处理预约
    void handleBookReturned(const QString &isbn);

    // 管理员消息：获取所有消息
    std::vector<Message> getAllMessages();

    // 借阅管理：办理借书
    bool borrowBook(const QString &isbn, const QString &readerId);
    // 借阅管理：办理还书
    bool returnBook(const QString &isbn, const QString &readerId);
    // 借阅管理：办理续借
    bool renewBook(const QString &isbn, const QString &readerId);
    // 借阅管理：查看所有借阅记录
    std::vector<BorrowRecord> viewBorrowRecords();
    // 借阅管理：查看逾期记录
    std::vector<BorrowRecord> viewOverdueRecords();
};

#endif // ADMIN_H