/**
 * @file BorrowRecord.h
 * @brief 借阅记录类定义
 *
 * BorrowRecord类表示图书借阅记录，包含借阅信息、归还状态、罚款管理和续借状态。
 */

#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <QString>
#include <QDateTime>

class Book;
class Reader;

/**
 * @class BorrowRecord
 * @brief 借阅记录类
 *
 * 表示图书借阅记录，包含借阅的基本信息（ISBN、读者ID、借阅时间、应还时间）、
 * 归还状态管理、罚款计算与支付、续借申请状态等功能。
 */
class BorrowRecord
{
public:
    /**
     * @enum FineStatus
     * @brief 罚款状态枚举
     */
    enum class FineStatus
    {
        UNPAID, ///< 未支付
        PAID    ///< 已支付
    };

    /**
     * @enum RenewStatus
     * @brief 续借审核状态枚举
     */
    enum class RenewStatus
    {
        NONE,     ///< 无续借申请
        PENDING,  ///< 待审核
        APPROVED, ///< 已通过
        REJECTED  ///< 已拒绝
    };

    /**
     * @brief 构造函数
     * @param isbn 图书ISBN编号
     * @param readerID 读者ID
     * @param borrowTime 借阅时间
     * @param dueTime 应还时间
     */
    BorrowRecord(QString isbn, QString readerID, QDateTime borrowTime, QDateTime dueTime);

    /**
     * @brief 计算逾期天数
     * @return 逾期天数（未逾期返回0）
     */
    int calculateOverdueDays() const;

    /**
     * @brief 计算罚款金额
     * @param finePerDay 每日逾期罚款金额
     * @return 罚款金额（根据逾期天数和罚款标准计算）
     */
    double calculateFine(double finePerDay) const;

    // ========== getter 方法 ==========

    /**
     * @brief 获取图书ISBN
     * @return 图书ISBN编号
     */
    QString getISBN() const;

    /**
     * @brief 获取读者ID
     * @return 读者ID
     */
    QString getReaderID() const;

    /**
     * @brief 获取借阅时间
     * @return 借阅时间
     */
    QDateTime getBorrowTime() const;

    /**
     * @brief 获取应还时间
     * @return 应还时间
     */
    QDateTime getDueTime() const;

    /**
     * @brief 获取罚款状态
     * @param finePerDay 每日逾期罚款金额
     * @return 罚款状态
     *
     * 特殊逻辑：如果当前状态为已支付，但已支付金额小于当前罚款金额，
     * 说明有新的逾期产生，返回未支付状态。
     */
    FineStatus getFineStatus(double finePerDay = 1.0) const;

    /**
     * @brief 获取已支付罚款金额
     * @return 已支付的罚款金额
     */
    double getPaidFine() const;

    /**
     * @brief 获取罚款金额
     * @return 罚款金额
     */
    double getFineAmount() const;

    /**
     * @brief 获取续借审核状态
     * @return 续借审核状态
     */
    RenewStatus getRenewStatus() const;

    /**
     * @brief 获取当前续借次数
     * @return 当前续借次数
     */
    int getRenewCount() const;

    /**
     * @brief 获取已扣信用分数
     * @return 已扣除的信用分数
     */
    int getDeductedScore() const;

    // ========== setter 方法 ==========

    /**
     * @brief 设置应还时间
     * @param time 新的应还时间
     */
    void setDueTime(QDateTime time);

    /**
     * @brief 设置已支付罚款金额
     * @param amount 已支付金额
     */
    void setPaidFine(double amount);

    /**
     * @brief 设置罚款状态
     * @param status 罚款状态
     */
    void setFineStatus(FineStatus status);

    /**
     * @brief 设置罚款金额
     * @param amount 罚款金额
     */
    void setFineAmount(double amount);

    /**
     * @brief 设置续借审核状态
     * @param status 续借审核状态
     */
    void setRenewStatus(RenewStatus status);

    /**
     * @brief 设置当前续借次数
     * @param count 当前续借次数
     */
    void setRenewCount(int count);

    /**
     * @brief 增加续借次数
     */
    void incrementRenewCount();

    /**
     * @brief 设置已扣信用分数
     * @param score 已扣除的信用分数
     */
    void setDeductedScore(int score);

    /**
     * @brief 析构函数
     */
    ~BorrowRecord();

private:
    QString ISBN;            ///< 关联的图书ISBN
    QString readerID;        ///< 关联的读者ID
    QDateTime borrowTime;    ///< 借阅时间
    QDateTime dueTime;       ///< 应还时间
    double fineAmount;       ///< 罚款金额
    double paidFine;         ///< 已支付罚款金额
    FineStatus fineStatus;   ///< 罚款状态
    RenewStatus renewStatus; ///< 续借审核状态
    int renewCount;          ///< 当前续借次数
    int deductedScore;       ///< 已扣信用分数
};

#endif // BORROWRECORD_H