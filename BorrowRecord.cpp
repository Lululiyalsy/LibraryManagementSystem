#include "BorrowRecord.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QIODevice>

// （构造函数）：借阅记录构造函数
BorrowRecord::BorrowRecord(QString isbn, QString readerID, QDateTime borrowTime, QDateTime dueTime)
    : ISBN(isbn), readerID(readerID), borrowTime(borrowTime), dueTime(dueTime),
      returned(false), fineAmount(0), paidFine(0), fineStatus(FineStatus::UNPAID),
      renewStatus(RenewStatus::NONE), deductedScore(0)
{
}

// （计算逾期天数）：计算图书逾期天数
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

// （计算罚款）：根据逾期天数计算罚款金额
double BorrowRecord::calculateFine() const
{
    int overdueDays = calculateOverdueDays();
    const double finePerDay = 1.0; // 每天罚款1元
    return overdueDays * finePerDay;
}

// （减免罚款）：减免全部或部分罚款
bool BorrowRecord::waiveFine(double amount)
{
    double totalFine = calculateFine();

    if (amount <= 0)
    {
        // 全额减免
        fineStatus = FineStatus::WAIVED;
        paidFine = totalFine; // 标记为已结清

        QString logPath = QCoreApplication::applicationDirPath() + "/fine_payments.log";
        QFile logFile(logPath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] "
                << "读者ID: " << readerID << ", "
                << "ISBN: " << ISBN << ", "
                << "减免金额: " << totalFine << "元, "
                << "状态: 已减免\n";
            logFile.close();
        }
    }
    else
    {
        // 部分减免
        if (amount > totalFine - paidFine)
        {
            return false; // 减免金额超过剩余罚款
        }

        paidFine += amount;

        if (paidFine >= totalFine - 0.01)
        {
            fineStatus = FineStatus::WAIVED;
            paidFine = totalFine;
        }

        QString logPath = QCoreApplication::applicationDirPath() + "/fine_payments.log";
        QFile logFile(logPath);
        if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        {
            QTextStream out(&logFile);
            out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] "
                << "读者ID: " << readerID << ", "
                << "ISBN: " << ISBN << ", "
                << "减免金额: " << amount << "元\n";
            logFile.close();
        }
    }

    return true;
}

// ========== getter 方法 ==========
// （getter和setter）：获取ISBN
QString BorrowRecord::getISBN() const
{
    return ISBN;
}

// （getter和setter）：获取读者ID
QString BorrowRecord::getReaderID() const
{
    return readerID;
}

// （getter和setter）：获取借阅时间
QDateTime BorrowRecord::getBorrowTime() const
{
    return borrowTime;
}

// （getter和setter）：获取应还时间
QDateTime BorrowRecord::getDueTime() const
{
    return dueTime;
}

// （getter和setter）：获取归还时间
QDateTime BorrowRecord::getReturnTime() const
{
    return returnTime;
}

// （getter和setter）：获取是否已归还
bool BorrowRecord::isReturned() const
{
    return returned;
}

// （获取罚款状态）：获取当前罚款状态
BorrowRecord::FineStatus BorrowRecord::getFineStatus() const
{
    return fineStatus;
}

// （获取已支付罚款）：获取已支付的罚款金额
double BorrowRecord::getPaidFine() const
{
    return paidFine;
}

// （getter和setter）：获取罚款金额
double BorrowRecord::getFineAmount() const
{
    return fineAmount;
}

// （getter和setter）：获取续借审核状态
BorrowRecord::RenewStatus BorrowRecord::getRenewStatus() const
{
    return renewStatus;
}

// （getter和setter）：获取已扣信用分数
int BorrowRecord::getDeductedScore() const
{
    return deductedScore;
}

// ========== setter 方法 ==========
// （getter和setter）：设置应还时间
void BorrowRecord::setDueTime(QDateTime time)
{
    dueTime = time;
}

// （getter和setter）：设置归还时间
void BorrowRecord::setReturnTime(QDateTime time)
{
    returnTime = time;
}

// （getter和setter）：设置是否已归还
void BorrowRecord::setReturned(bool isReturned)
{
    returned = isReturned;
    if (returned)
    {
        returnTime = QDateTime::currentDateTime();
    }
}

// （getter和setter）：设置已支付罚款金额
void BorrowRecord::setPaidFine(double amount)
{
    paidFine = amount;
}

// （getter和setter）：设置罚款状态
void BorrowRecord::setFineStatus(FineStatus status)
{
    fineStatus = status;
}

// （getter和setter）：设置罚款金额
void BorrowRecord::setFineAmount(double amount)
{
    fineAmount = amount;
}

// （getter和setter）：设置续借审核状态
void BorrowRecord::setRenewStatus(RenewStatus status)
{
    renewStatus = status;
}

// （getter和setter）：设置已扣信用分数
void BorrowRecord::setDeductedScore(int score)
{
    deductedScore = score;
}

// （析构函数）：借阅记录析构函数
BorrowRecord::~BorrowRecord()
{
}