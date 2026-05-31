#ifndef BORROWRECORD_H
#define BORROWRECORD_H

#include <QString>
#include <QDateTime>

class Book;
class Reader;

class BorrowRecord
{
public:
    // 罚款状态枚举
    enum class FineStatus
    {
        UNPAID, // 未支付
        PAID,   // 已支付
        WAIVED  // 已减免
    };

    // 续借审核状态枚举
    enum class RenewStatus
    {
        NONE,     // 无续借申请
        PENDING,  // 待审核
        APPROVED, // 已通过
        REJECTED  // 已拒绝
    };

    // （构造函数）：借阅记录构造函数
    BorrowRecord(QString isbn, QString readerID, QDateTime borrowTime, QDateTime dueTime);

    // （计算逾期天数）：计算图书逾期天数
    int calculateOverdueDays() const;

    // （计算罚款）：根据逾期天数计算罚款金额
    double calculateFine() const;

    // （减免罚款）：减免全部或部分罚款
    bool waiveFine(double amount = 0);

    // （获取罚款状态）：获取当前罚款状态
    FineStatus getFineStatus() const;

    // （获取已支付罚款）：获取已支付的罚款金额
    double getPaidFine() const;

    // （getter和setter）：获取ISBN
    QString getISBN() const;
    // （getter和setter）：获取读者ID
    QString getReaderID() const;
    // （getter和setter）：获取借阅时间
    QDateTime getBorrowTime() const;
    // （getter和setter）：获取应还时间
    QDateTime getDueTime() const;
    // （getter和setter）：获取归还时间
    QDateTime getReturnTime() const;
    // （getter和setter）：获取是否已归还
    bool isReturned() const;
    // （getter和setter）：设置应还时间
    void setDueTime(QDateTime time);
    // （getter和setter）：设置归还时间
    void setReturnTime(QDateTime time);
    // （getter和setter）：设置是否已归还
    void setReturned(bool returned);
    // （getter和setter）：设置已支付罚款金额
    void setPaidFine(double amount);
    // （getter和setter）：设置罚款状态
    void setFineStatus(FineStatus status);
    // （getter和setter）：获取续借审核状态
    RenewStatus getRenewStatus() const;
    // （getter和setter）：设置续借审核状态
    void setRenewStatus(RenewStatus status);
    // （getter和setter）：获取已扣信用分数
    int getDeductedScore() const;
    // （getter和setter）：设置已扣信用分数
    void setDeductedScore(int score);

    // （析构函数）：借阅记录析构函数
    ~BorrowRecord();

private:
    // （图书ISBN）：关联的图书ISBN
    QString ISBN;
    // （读者ID）：关联的读者ID
    QString readerID;
    // （借阅时间）：图书借出时间
    QDateTime borrowTime;
    // （应还时间）：图书应还时间
    QDateTime dueTime;
    // （归还时间）：图书实际归还时间
    QDateTime returnTime;
    // （是否已归还）：标记图书是否已归还
    bool returned;
    // （罚款金额）：计算出的罚款金额
    mutable double fineAmount;
    // （已支付罚款）：已支付的罚款金额
    double paidFine;
    // （罚款状态）：罚款支付状态
    FineStatus fineStatus;
    // （续借审核状态）：续借审核状态
    RenewStatus renewStatus;
    // （已扣信用分数）：该记录已扣除的信用分数
    int deductedScore;
};

#endif // BORROWRECORD_H