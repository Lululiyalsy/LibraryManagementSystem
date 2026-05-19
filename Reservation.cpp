#include "Reservation.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QIODevice>

//（构造函数）：预约记录构造函数
Reservation::Reservation(const QString &isbn, const QString &readerID, const QDateTime &reserveTime, Status status)
    : ISBN(isbn), readerID(readerID), reserveTime(reserveTime), status(status) {
}

//（发送通知）：发送图书入库和完成预约的通知
void Reservation::Notify() {
    // 1. 更新状态为已通知
    status = NOTIFIED;

    // 2. 模拟发送邮件通知
    QString emailContent = QString("【图书馆预约通知】\n\n") +
                          QString("尊敬的读者，您预约的图书已可借阅：\n") +
                          QString("ISBN: %1\n").arg(ISBN) +
                          QString("预约时间: %1\n").arg(reserveTime.toString("yyyy-MM-dd HH:mm:ss")) +
                          QString("请尽快到图书馆办理借阅手续。\n\n") +
                          QString("图书馆管理系统");

    // 保存通知日志到文件
    QString logFileName = QString("notification_log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
    QString logPath = QCoreApplication::applicationDirPath() + "/notifications/" + logFileName;

    QDir notificationDir(QCoreApplication::applicationDirPath() + "/notifications");
    if (!notificationDir.exists()) {
        notificationDir.mkpath(".");
    }

    QFile logFile(logPath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        out << "[" << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "] "
            << "读者ID: " << readerID << ", "
            << "ISBN: " << ISBN << ", "
            << "通知类型: 邮件通知\n";
        logFile.close();
    }

    // 3. 发送到系统日志
    qDebug() << "[Reservation] 已发送通知 - 读者ID:" << readerID << ", ISBN:" << ISBN;

    // TODO: 实际项目中可以添加：
    // - 真实邮件发送（使用 QNetworkAccessManager 调用邮件API）
    // - 短信发送（调用短信服务商API）
    // - 系统内消息推送
}

//（getter和setter）：获取ISBN
QString Reservation::getISBN() const {
    return ISBN;
}

//（getter和setter）：获取读者ID
QString Reservation::getReaderID() const {
    return readerID;
}

//（getter和setter）：获取预约时间
QDateTime Reservation::getReserveTime() const {
    return reserveTime;
}

//（getter和setter）：获取预约状态
Reservation::Status Reservation::getStatus() const {
    return status;
}

//（获取状态字符串）：获取状态的中文描述
QString Reservation::getStatusString() const {
    switch (status) {
        case PENDING:    return "待处理";
        case NOTIFIED:   return "已通知";
        case COMPLETED:  return "已完成";
        case CANCELLED:  return "已取消";
        default:         return "未知状态";
    }
}

//（getter和setter）：设置预约状态
void Reservation::setStatus(Status newStatus) {
    status = newStatus;
}

//（析构函数）：预约记录析构函数
Reservation::~Reservation() {
}