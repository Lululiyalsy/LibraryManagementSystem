/**
 * @file BorrowRecord.cpp
 * @brief 借阅记录类实现
 *
 * 实现BorrowRecord类的所有成员函数，包括借阅记录管理、逾期计算、
 * 罚款管理和续借状态管理。
 */

#include "BorrowRecord.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QIODevice>

/**
 * @brief 构造函数
 * @param isbn 图书ISBN编号
 * @param readerID 读者ID
 * @param borrowTime 借阅时间
 * @param dueTime 应还时间
 *
 * 初始化借阅记录，默认状态：
 * - 未归还 (returned = false)
 * - 罚款金额为0 (fineAmount = 0)
 * - 已支付罚款为0 (paidFine = 0)
 * - 罚款状态未支付 (fineStatus = UNPAID)
 * - 无续借申请 (renewStatus = NONE)
 * - 续借次数为0 (renewCount = 0)
 * - 已扣信用分数为0 (deductedScore = 0)
 */
BorrowRecord::BorrowRecord(QString isbn, QString readerID, QDateTime borrowTime, QDateTime dueTime)
    : ISBN(isbn), readerID(readerID), borrowTime(borrowTime), dueTime(dueTime),
      returned(false), fineAmount(0), paidFine(0), fineStatus(FineStatus::UNPAID),
      renewStatus(RenewStatus::NONE), renewCount(0), deductedScore(0)
{
}

/**
 * @brief 计算逾期天数
 * @return 逾期天数（未逾期或已归还返回0）
 *
 * 如果图书已归还，返回0；
 * 如果当前时间超过应还时间，计算逾期天数；
 * 否则返回0。
 */
int BorrowRecord::calculateOverdueDays() const
{
    if (returned)
    {
        return 0;
    }
    QDateTime now = QDateTime::currentDateTime();
    if (now > dueTime)
    {
        return dueTime.daysTo(now);
    }
    return 0;
}

/**
 * @brief 计算罚款金额
 * @param finePerDay 每日逾期罚款金额
 * @return 罚款金额
 *
 * 罚款规则：根据逾期天数和传入的每日罚款金额计算。
 */
double BorrowRecord::calculateFine(double finePerDay) const
{
    int overdueDays = calculateOverdueDays();
    return overdueDays * finePerDay;
}

// ========== getter 方法 ==========

/**
 * @brief 获取图书ISBN
 * @return 图书ISBN编号
 */
QString BorrowRecord::getISBN() const
{
    return ISBN;
}

/**
 * @brief 获取读者ID
 * @return 读者ID
 */
QString BorrowRecord::getReaderID() const
{
    return readerID;
}

/**
 * @brief 获取借阅时间
 * @return 借阅时间
 */
QDateTime BorrowRecord::getBorrowTime() const
{
    return borrowTime;
}

/**
 * @brief 获取应还时间
 * @return 应还时间
 */
QDateTime BorrowRecord::getDueTime() const
{
    return dueTime;
}

/**
 * @brief 获取归还时间
 * @return 归还时间（未归还返回无效时间）
 */
QDateTime BorrowRecord::getReturnTime() const
{
    return returnTime;
}

/**
 * @brief 判断是否已归还
 * @return true表示已归还，false表示未归还
 */
bool BorrowRecord::isReturned() const
{
    return returned;
}

/**
 * @brief 获取罚款状态
 * @param finePerDay 每日逾期罚款金额
 * @return 罚款状态
 *
 * 特殊逻辑：如果当前状态为已支付，但已支付金额小于当前计算的罚款金额，
 * 说明有新的逾期产生，返回未支付状态。
 * 这处理了支付罚款后继续逾期的情况。
 */
BorrowRecord::FineStatus BorrowRecord::getFineStatus(double finePerDay) const
{
    if (fineStatus == FineStatus::PAID)
    {
        double currentFine = calculateFine(finePerDay);
        if (paidFine < currentFine - 0.01) // 允许微小的浮点误差
        {
            // 已支付金额小于当前罚款，说明有新的逾期产生
            return FineStatus::UNPAID;
        }
    }
    return fineStatus;
}

/**
 * @brief 获取已支付罚款金额
 * @return 已支付的罚款金额
 */
double BorrowRecord::getPaidFine() const
{
    return paidFine;
}

/**
 * @brief 获取罚款金额
 * @return 罚款金额
 */
double BorrowRecord::getFineAmount() const
{
    return fineAmount;
}

/**
 * @brief 获取续借审核状态
 * @return 续借审核状态
 */
BorrowRecord::RenewStatus BorrowRecord::getRenewStatus() const
{
    return renewStatus;
}

/**
 * @brief 获取当前续借次数
 * @return 当前续借次数
 */
int BorrowRecord::getRenewCount() const
{
    return renewCount;
}

/**
 * @brief 获取已扣信用分数
 * @return 已扣除的信用分数
 */
int BorrowRecord::getDeductedScore() const
{
    return deductedScore;
}

// ========== setter 方法 ==========

/**
 * @brief 设置应还时间
 * @param time 新的应还时间
 */
void BorrowRecord::setDueTime(QDateTime time)
{
    dueTime = time;
}

/**
 * @brief 设置归还时间
 * @param time 归还时间
 */
void BorrowRecord::setReturnTime(QDateTime time)
{
    returnTime = time;
}

/**
 * @brief 设置是否已归还
 * @param isReturned 是否已归还
 *
 * 如果设置为已归还，自动设置归还时间为当前时间。
 */
void BorrowRecord::setReturned(bool isReturned)
{
    returned = isReturned;
    if (returned)
    {
        returnTime = QDateTime::currentDateTime();
    }
}

/**
 * @brief 设置已支付罚款金额
 * @param amount 已支付金额
 */
void BorrowRecord::setPaidFine(double amount)
{
    paidFine = amount;
}

/**
 * @brief 设置罚款状态
 * @param status 罚款状态
 */
void BorrowRecord::setFineStatus(FineStatus status)
{
    fineStatus = status;
}

/**
 * @brief 设置罚款金额
 * @param amount 罚款金额
 */
void BorrowRecord::setFineAmount(double amount)
{
    fineAmount = amount;
}

/**
 * @brief 设置续借审核状态
 * @param status 续借审核状态
 */
void BorrowRecord::setRenewStatus(RenewStatus status)
{
    renewStatus = status;
}

/**
 * @brief 设置当前续借次数
 * @param count 当前续借次数
 */
void BorrowRecord::setRenewCount(int count)
{
    renewCount = count;
}

/**
 * @brief 增加续借次数
 */
void BorrowRecord::incrementRenewCount()
{
    renewCount++;
}

/**
 * @brief 设置已扣信用分数
 * @param score 已扣除的信用分数
 */
void BorrowRecord::setDeductedScore(int score)
{
    deductedScore = score;
}

/**
 * @brief 析构函数
 */
BorrowRecord::~BorrowRecord()
{
}