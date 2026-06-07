/**
 * @file Reader.h
 * @brief 读者类定义
 *
 * Reader类继承自User类，提供读者特有的功能，包括图书预约、借阅、
 * 还书、续借等操作，以及信用分管理和限制机制。
 */

#ifndef READER_H
#define READER_H

#include "User.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"
#include "BorrowPolicy.h"
#include <QString>
#include <QDateTime>
#include <vector>

/**
 * @class Reader
 * @brief 读者类
 *
 * 继承自User类，实现读者的全部功能，包括图书预约、借阅、还书、续借等操作，
 * 以及信用分管理和限制机制。
 */
class Reader : public User
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
    Reader(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    /**
     * @brief 获取用户身份标识（实现User的纯虚函数）
     * @return 返回"读者"
     *
     * 子类可覆盖此方法返回更具体的身份标识，如"学生读者"、"教师读者"等。
     */
    virtual QString typeToIdentity();

    /**
     * @brief 生成验证码（实现User的纯虚函数）
     * @return 6位数字验证码
     */
    QString generateVerificationCode() override;

    // ========== getter 和 setter 方法 ==========

    /**
     * @brief 获取最大借阅数量
     * @return 最大借阅数量
     */
    int getMaxBooks();

    /**
     * @brief 获取当前信用分
     * @return 当前信用分
     */
    int getCreditScore();

    /**
     * @brief 获取之前的信用分
     * @return 之前的信用分（用于判断信用分是否下跌）
     */
    int getPrevCreditScore();

    /**
     * @brief 获取限制到期时间
     * @return 限制到期时间
     */
    QDateTime getBanUntil();

    /**
     * @brief 设置最大借阅数量
     * @param max 最大借阅数量
     */
    void setMaxBooks(int max);

    /**
     * @brief 设置当前信用分
     * @param score 信用分数值
     */
    void setCreditScore(int score);

    /**
     * @brief 设置之前的信用分
     * @param score 之前的信用分数值
     */
    void setPrevCreditScore(int score);

    /**
     * @brief 设置限制到期时间
     * @param time 限制到期时间
     */
    void setBanUntil(QDateTime time);

    // ========== 角色和策略管理 ==========

    /**
     * @enum Role
     * @brief 读者角色枚举
     */
    enum class Role
    {
        STUDENT = 1, ///< 学生读者
        TEACHER = 2, ///< 教师读者
        EXTERNAL = 3 ///< 外部读者
    };

    /**
     * @brief 获取读者角色
     * @return 读者角色
     */
    Role getRole() const;

    /**
     * @brief 获取读者角色字符串描述
     * @return 角色中文名称（"学生读者"/"教师读者"/"外部读者"/"普通读者"）
     */
    QString getRoleString() const;

    /**
     * @brief 设置借阅策略
     * @param p 借阅策略指针（Reader对象获取所有权）
     *
     * 设置策略后会自动更新maxBooks为策略中配置的值。
     */
    void setPolicy(BorrowPolicy *p);

    /**
     * @brief 获取当前借阅策略
     * @return 借阅策略指针（可能为nullptr）
     */
    BorrowPolicy *getPolicy() const;

    // ========== 枚举类型定义 ==========

    /**
     * @enum ReserveResult
     * @brief 预约结果枚举
     */
    enum class ReserveResult
    {
        SUCCESS,          ///< 预约成功
        BOOK_NOT_FOUND,   ///< 图书不存在
        ALREADY_EXISTS,   ///< 已预约或已借阅该图书
        EXCEED_LIMIT,     ///< 预约人数已达上限
        LOW_CREDIT,       ///< 信用分不足，被限制操作
        DEPOSIT_REQUIRED  ///< 需要缴纳押金才能预约（校外读者）
    };

    /**
     * @enum BorrowResult
     * @brief 借书结果枚举
     */
    enum class BorrowResult
    {
        SUCCESS,              ///< 借书成功
        BOOK_NOT_FOUND,       ///< 图书不存在
        NO_STOCK,             ///< 无库存
        HAS_OVERDUE,          ///< 有逾期未还图书
        EXCEED_LIMIT,         ///< 已达最大借阅数量
        ALREADY_BORROWED,     ///< 已借阅该图书且未归还
        NO_VALID_RESERVATION, ///< 未预约成功
        LOW_CREDIT,           ///< 信用分不足，被限制操作
        DEPOSIT_REQUIRED      ///< 需要缴纳押金（校外读者）
    };

    /**
     * @enum ReturnResult
     * @brief 还书结果枚举
     */
    enum class ReturnResult
    {
        SUCCESS,        ///< 还书成功
        NOT_FOUND,      ///< 借阅记录不存在
        HAS_UNPAID_FINE ///< 存在未支付罚款，需先支付
    };

    /**
     * @enum RenewResult
     * @brief 续借结果枚举
     */
    enum class RenewResult
    {
        SUCCESS,         ///< 续借申请提交成功
        NOT_FOUND,       ///< 借阅记录不存在
        ALREADY_PENDING, ///< 已有待审核的续借申请
        HAS_OVERDUE,     ///< 图书已逾期
        EXCEED_LIMIT,    ///< 续借后借期超过90天
        LOW_CREDIT       ///< 信用分不足，被限制操作
    };

    /**
     * @enum CreditCheckResult
     * @brief 信用分检查结果枚举
     */
    enum class CreditCheckResult
    {
        OK,            ///< 信用分正常
        LOW_CREDIT_90, ///< 低于90分，限制1天
        LOW_CREDIT_80, ///< 低于80分，限制3天
        LOW_CREDIT_70, ///< 低于70分，限制1周
        LOW_CREDIT_60, ///< 低于60分，限制2周
        LOW_CREDIT_50  ///< 低于50分，限制1个月
    };

    // ========== 信用分管理 ==========

    /**
     * @brief 检查信用分是否允许操作
     * @return 信用分检查结果
     */
    CreditCheckResult checkCreditScore() const;

    /**
     * @brief 检查是否在限制期间
     * @return true表示在限制期间，false表示正常
     */
    bool isBanned() const;

    /**
     * @brief 计算信用分变化
     * @param overdueDays 逾期天数
     * @param book 图书对象指针
     * @param isbn 图书ISBN
     */
    void calculateCreditScore(int overdueDays, Book *book, const QString &isbn);

    // ========== 预约管理 ==========

    /**
     * @brief 预约图书（按ISBN）
     * @param isbn 图书ISBN
     * @return 预约结果
     */
    ReserveResult reserveBook(const QString &isbn);

    /**
     * @brief 取消预约（按ISBN）
     * @param isbn 图书ISBN
     * @return 取消成功返回true，失败返回false
     */
    bool cancelReservation(const QString &isbn);

    /**
     * @brief 查看我的预约
     * @return 预约记录列表
     */
    std::vector<Reservation> viewMyReservations();

    // ========== 借阅管理 ==========

    /**
     * @brief 借书（按ISBN）
     * @param isbn 图书ISBN
     * @return 借书结果
     */
    BorrowResult borrowBook(const QString &isbn);

    /**
     * @brief 还书（按ISBN）
     * @param isbn 图书ISBN
     * @return 还书结果
     */
    ReturnResult returnBook(const QString &isbn);

    /**
     * @brief 续借（按ISBN）
     * @param isbn 图书ISBN
     * @return 续借结果
     */
    RenewResult renewBook(const QString &isbn);

    /**
     * @brief 查看我的借阅记录
     * @return 借阅记录列表
     */
    std::vector<BorrowRecord> viewMyBorrowRecords();

    // ========== 图书查找 ==========

    /**
     * @brief 查找图书（模糊搜索）
     * @param isbn ISBN（可选）
     * @param title 书名（可选）
     * @param author 作者（可选）
     * @param category 分类（可选）
     * @return 匹配的图书指针列表
     */
    std::vector<const Book *> findBook(const QString &isbn, const QString &title,
                                       const QString &author, const QString &category);

    /**
     * @brief 析构函数
     */
    ~Reader();

    // ========== 成员变量 ==========

    int maxBooks;        ///< 最大借阅书量（默认10本）
    int creditScore;     ///< 当前信用分（默认100分）
    int prevCreditScore; ///< 之前的信用分（用于判断是否下跌）
    QDateTime banUntil;  ///< 限制到期日（无效表示无限制）
    Role role;           ///< 读者角色（默认STUDENT）
    BorrowPolicy *policy;///< 借阅策略指针（由子类注入，Reader拥有所有权）
};

#endif // READER_H