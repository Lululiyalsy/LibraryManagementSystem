/**
 * @file Reservation.cpp
 * @brief 预约类实现
 * 
 * 实现Reservation类的所有成员函数，包括构造函数、通知发送功能、
 * getter/setter方法和状态转换。
 */

#include "Reservation.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QIODevice>

/**
 * @brief 构造函数
 * @param isbn 图书ISBN编号
 * @param readerID 读者ID
 * @param reserveTime 预约时间
 * @param status 预约状态（默认待审核）
 */
Reservation::Reservation(const QString &isbn, const QString &readerID, 
                         const QDateTime &reserveTime, Status status)
    : ISBN(isbn), readerID(readerID), reserveTime(reserveTime), status(status) {
}

/**
 * @brief 发送预约通知
 * 
 * 更新预约状态为审核成功，并保存通知日志到文件。
 * 通知日志保存在应用程序目录下的 /notifications 文件夹中。
 * 
 * 扩展建议：
 * - 使用 QNetworkAccessManager 调用邮件API发送真实邮件
 * - 调用短信服务商API发送短信通知
 * - 通过系统消息机制推送站内消息
 */
void Reservation::Notify() {
    // 更新状态为审核成功
    status = APPROVED;

    // 模拟发送邮件通知内容
    QString emailContent = QString("【图书馆预约通知】\n\n") +
                          QString("尊敬的读者，您预约的图书已可借阅：\n") +
                          QString("ISBN: %1\n").arg(ISBN) +
                          QString("预约时间: %1\n").arg(reserveTime.toString("yyyy-MM-dd HH:mm:ss")) +
                          QString("请尽快到图书馆办理借阅手续。\n\n") +
                          QString("图书馆管理系统");

    // 保存通知日志到文件
    QString logFileName = QString("notification_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
    QString logPath = QCoreApplication::applicationDirPath() + "/notifications/" + logFileName;

    // 确保通知目录存在
    QDir notificationDir(QCoreApplication::applicationDirPath() + "/notifications");
    if (!notificationDir.exists()) {
        notificationDir.mkpath(".");
    }

    // 写入日志文件
    QFile logFile(logPath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] "
            << "读者ID: " << readerID << ", "
            << "ISBN: " << ISBN << ", "
            << "通知类型: 邮件通知\n";
        logFile.close();
    }

    // TODO: 实际项目中可以添加：
    // - 真实邮件发送（使用 QNetworkAccessManager 调用邮件API）
    // - 短信发送（调用短信服务商API）
    // - 系统内消息推送
}

// ========== getter 方法 ==========

/**
 * @brief 获取图书ISBN
 * @return 图书ISBN编号
 */
QString Reservation::getISBN() const {
    return ISBN;
}

/**
 * @brief 获取读者ID
 * @return 读者ID
 */
QString Reservation::getReaderID() const {
    return readerID;
}

/**
 * @brief 获取预约时间
 * @return 预约时间
 */
QDateTime Reservation::getReserveTime() const {
    return reserveTime;
}

/**
 * @brief 获取预约状态
 * @return 预约状态
 */
Reservation::Status Reservation::getStatus() const {
    return status;
}

/**
 * @brief 获取状态的中文描述
 * @return 状态字符串（待审核/审核成功/审核失败/已取消）
 */
QString Reservation::getStatusString() const {
    switch (status) {
        case PENDING:    return "待审核";
        case APPROVED:   return "审核成功";
        case REJECTED:   return "审核失败";
        case CANCELLED:  return "已取消";
        default:         return "未知状态";
    }
}

// ========== setter 方法 ==========

/**
 * @brief 设置预约状态
 * @param newStatus 新的预约状态
 */
void Reservation::setStatus(Status newStatus) {
    status = newStatus;
}

/**
 * @brief 析构函数
 */
Reservation::~Reservation() {
}
