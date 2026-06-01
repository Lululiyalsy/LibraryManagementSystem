/**
 * @file Reservation.h
 * @brief 预约类定义
 *
 * Reservation类表示图书预约记录，包含预约的基本信息和状态管理。
 */

#ifndef RESERVATION_H
#define RESERVATION_H

#include <QString>
#include <QDateTime>

class Book;
class Reader;

/**
 * @class Reservation
 * @brief 预约类
 *
 * 表示图书预约记录，包含预约的基本信息（ISBN、读者ID、预约时间）和状态管理。
 */
class Reservation
{
public:
    /**
     * @enum Status
     * @brief 预约状态枚举
     */
    enum Status
    {
        PENDING,  ///< 待审核
        APPROVED, ///< 审核成功
        REJECTED, ///< 审核失败
        CANCELLED ///< 已取消
    };

    /**
     * @brief 构造函数
     * @param isbn 图书ISBN编号
     * @param readerID 读者ID
     * @param reserveTime 预约时间
     * @param status 预约状态（默认待审核）
     */
    Reservation(const QString &isbn, const QString &readerID,
                const QDateTime &reserveTime, Status status = PENDING);

    /**
     * @brief 发送预约通知
     *
     * 更新预约状态为审核成功，并保存通知日志到文件。
     * 实际项目中可扩展为邮件、短信或系统消息推送。
     */
    void Notify();

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
     * @brief 获取预约时间
     * @return 预约时间
     */
    QDateTime getReserveTime() const;

    /**
     * @brief 获取预约状态
     * @return 预约状态
     */
    Status getStatus() const;

    /**
     * @brief 获取状态的中文描述
     * @return 状态字符串（待审核/审核成功/审核失败/已取消）
     */
    QString getStatusString() const;

    // ========== setter 方法 ==========

    /**
     * @brief 设置预约状态
     * @param status 新的预约状态
     */
    void setStatus(Status status);

    /**
     * @brief 析构函数
     */
    ~Reservation();

private:
    QString ISBN;          ///< 关联的图书ISBN
    QString readerID;      ///< 关联的读者ID
    QDateTime reserveTime; ///< 预约时间
    Status status;         ///< 预约状态
};

#endif // RESERVATION_H