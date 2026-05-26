#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <vector>

// 消息状态枚举
enum class MessageStatus
{
    UNREAD = 0, // 未读
    READ = 1    // 已读
};

class Message
{
public:
    // 默认构造函数
    Message();

    // 管理员消息构造函数
    Message(const QString &adminId, const QString &adminName,
            const QString &readerId, const QString &readerName,
            const QString &content, MessageStatus status = MessageStatus::UNREAD);

    // 读者消息构造函数
    Message(const QString &readerId, const QString &readerName,
            const QString &content, MessageStatus status = MessageStatus::UNREAD);

    // 判断是否为管理员消息
    bool isAdminMessage() const;

    // 获取管理员ID
    QString getAdminId() const;

    // 获取管理员名
    QString getAdminName() const;

    // 获取读者ID
    QString getReaderId() const;

    // 获取读者名
    QString getReaderName() const;

    // 获取消息时间
    QString getTime() const;

    // 获取消息内容
    QString getContent() const;

    // 获取消息状态
    MessageStatus getStatus() const;

    // 设置消息状态
    void setStatus(MessageStatus status);

    // 转换为文件存储格式（管理员消息7字段，读者消息5字段）
    QString toFileString() const;

    // 从文件字符串解析（自动识别管理员/读者消息）
    static Message fromFileString(const QString &line);

    // 获取管理员显示字段（读者id|读者名|消息发送时间|消息内容）
    std::vector<QString> getAdminDisplayFields() const;

    // 获取读者显示字段（消息发送时间|消息内容|消息状态）
    std::vector<QString> getReaderDisplayFields() const;

private:
    // 是否为管理员消息
    bool m_isAdmin;
    // 管理员ID（仅管理员消息）
    QString m_adminId;
    // 管理员名（仅管理员消息）
    QString m_adminName;
    // 读者ID
    QString m_readerId;
    // 读者名
    QString m_readerName;
    // 消息发送时间
    QString m_time;
    // 消息内容
    QString m_content;
    // 消息状态
    MessageStatus m_status;
};

#endif // MESSAGE_H
