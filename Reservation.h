#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>
#include <QDateTime>

class Book;
class Reader;

class Reservation {
public:
    // 预约状态枚举
    enum Status {
        PENDING,    // 待处理
        NOTIFIED,   // 已通知
        COMPLETED,  // 已完成
        CANCELLED   // 已取消
    };

    //（构造函数）：预约记录构造函数
    Reservation(const QString &isbn, const QString &readerID, const QDateTime &reserveTime, Status status = PENDING);

    //（发送通知）：发送图书入库和完成预约的通知
    void Notify();

    //（getter和setter）：获取ISBN
    QString getISBN() const;
    //（getter和setter）：获取读者ID
    QString getReaderID() const;
    //（getter和setter）：获取预约时间
    QDateTime getReserveTime() const;
    //（getter和setter）：获取预约状态
    Status getStatus() const;
    //（获取状态字符串）：获取状态的中文描述
    QString getStatusString() const;

    //（getter和setter）：设置预约状态
    void setStatus(Status status);

    //（析构函数）：预约记录析构函数
    ~Reservation();

private:
    //（图书ISBN）：关联的图书ISBN
    QString ISBN;
    //（读者ID）：关联的读者ID
    QString readerID;
    //（预约时间）：预约图书的时间
    QDateTime reserveTime;
    //（预约状态）：预约当前状态
    Status status;
};

#endif // RESERVATION_H