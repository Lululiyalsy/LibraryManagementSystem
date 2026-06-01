#ifndef READER_H
#define READER_H

#include "User.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"
#include <QString>
#include <QDateTime>
#include <vector>

class Reader : public User
{
public:
    // 构造函数
    Reader(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    // 实现User的纯虚函数：获取用户身份
    QString typeToIdentity() override;

    // 实现User的纯虚函数：生成验证码（读者：6位数字）
    QString generateVerificationCode() override;

    // getter和setter方法
    int getMaxBooks();
    int getCreditScore();
    int getPrevCreditScore();
    QDateTime getBanUntil();

    void setMaxBooks(int max);
    void setCreditScore(int score);
    void setPrevCreditScore(int score);
    void setBanUntil(QDateTime time);

    // 预约结果枚举
    enum class ReserveResult
    {
        SUCCESS,        // 预约成功
        BOOK_NOT_FOUND, // 图书不存在
        ALREADY_EXISTS, // 已预约或已借阅该图书
        EXCEED_LIMIT,   // 预约人数已达上限
        LOW_CREDIT      // 信用分不足，被限制操作
    };

    // 借书结果枚举
    enum class BorrowResult
    {
        SUCCESS,              // 借书成功
        BOOK_NOT_FOUND,       // 图书不存在
        NO_STOCK,             // 无库存
        HAS_OVERDUE,          // 有逾期未还图书
        EXCEED_LIMIT,         // 已达最大借阅数量
        ALREADY_BORROWED,     // 已借阅该图书且未归还
        NO_VALID_RESERVATION, // 未预约成功
        LOW_CREDIT            // 信用分不足，被限制操作
    };

    // 还书结果枚举
    enum class ReturnResult
    {
        SUCCESS,        // 还书成功
        NOT_FOUND,      // 借阅记录不存在
        HAS_UNPAID_FINE // 存在未支付罚款，需先支付
    };

    // 续借结果枚举
    enum class RenewResult
    {
        SUCCESS,         // 续借申请提交成功
        NOT_FOUND,       // 借阅记录不存在
        ALREADY_PENDING, // 已有待审核的续借申请
        HAS_OVERDUE,     // 图书已逾期
        EXCEED_LIMIT,    // 续借后借期超过90天
        LOW_CREDIT       // 信用分不足，被限制操作
    };

    // 信用分检查结果
    enum class CreditCheckResult
    {
        OK,            // 信用分正常
        LOW_CREDIT_90, // 低于90分，限制1天
        LOW_CREDIT_80, // 低于80分，限制3天
        LOW_CREDIT_70, // 低于70分，限制1周
        LOW_CREDIT_60, // 低于60分，限制2周
        LOW_CREDIT_50  // 低于50分，限制1个月
    };

    // 检查信用分是否允许操作
    CreditCheckResult checkCreditScore() const;
    // 检查是否在限制期间
    bool isBanned() const;
    // 计算信用分变化（逾期扣分或按时加分）
    void calculateCreditScore(int overdueDays, Book *book, const QString &isbn);

    // 预约图书（按ISBN），返回预约结果
    ReserveResult reserveBook(const QString &isbn);

    // 取消预约（按ISBN）
    bool cancelReservation(const QString &isbn);

    // 查看我的预约
    std::vector<Reservation> viewMyReservations();

    // 借书（按ISBN），返回借书结果
    BorrowResult borrowBook(const QString &isbn);

    // 还书（按ISBN），返回还书结果
    ReturnResult returnBook(const QString &isbn);

    // 续借（按ISBN），返回续借结果
    RenewResult renewBook(const QString &isbn);

    // 查看我的借阅记录
    std::vector<BorrowRecord> viewMyBorrowRecords();

    // 查找图书（模糊搜索）
    std::vector<const Book *> findBook(const QString &isbn, const QString &title,
                                       const QString &author, const QString &category);

    // 析构函数
    ~Reader();

    // 最大借阅书量
    int maxBooks;
    // 信用分
    int creditScore;
    // 之前的信用分（用于判断是否下跌）
    int prevCreditScore;
    // 限制到期日
    QDateTime banUntil;
};

#endif // READER_H