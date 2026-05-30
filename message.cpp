#include "Message.h"
#include <QDateTime>

// 默认构造函数
Message::Message()
    : m_isAdmin(false), m_status(MessageStatus::UNREAD)
{
    m_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

// 管理员消息构造函数
Message::Message(const QString &adminId, const QString &adminName,
                 const QString &readerId, const QString &readerName,
                 const QString &content, MessageStatus status)
    : m_isAdmin(true), m_adminId(adminId), m_adminName(adminName),
      m_readerId(readerId), m_readerName(readerName), m_content(content),
      m_status(status)
{
    m_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

// 读者消息构造函数
Message::Message(const QString &readerId, const QString &readerName,
                 const QString &content, MessageStatus status)
    : m_isAdmin(false), m_readerId(readerId), m_readerName(readerName),
      m_content(content), m_status(status)
{
    m_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

// 判断是否为管理员消息
bool Message::isAdminMessage() const
{
    return m_isAdmin;
}

// 获取管理员ID
QString Message::getAdminId() const
{
    return m_adminId;
}

// 获取管理员名
QString Message::getAdminName() const
{
    return m_adminName;
}

// 获取读者ID
QString Message::getReaderId() const
{
    return m_readerId;
}

// 获取读者名
QString Message::getReaderName() const
{
    return m_readerName;
}

// 获取消息时间
QString Message::getTime() const
{
    return m_time;
}

// 获取消息内容
QString Message::getContent() const
{
    return m_content;
}

// 获取消息状态
MessageStatus Message::getStatus() const
{
    return m_status;
}

// 设置消息状态
void Message::setStatus(MessageStatus status)
{
    m_status = status;
}

// 转换为文件存储格式
QString Message::toFileString() const
{
    if (m_isAdmin)
    {
        // 管理员消息格式：管理员id|管理员名|读者id|读者名|消息发送时间|消息内容|消息状态
        return QString("%1|%2|%3|%4|%5|%6|%7")
            .arg(m_adminId)
            .arg(m_adminName)
            .arg(m_readerId)
            .arg(m_readerName)
            .arg(m_time)
            .arg(m_content)
            .arg(static_cast<int>(m_status));
    }
    else
    {
        // 读者消息格式：读者id|读者名|消息发送时间|消息内容|消息状态
        return QString("%1|%2|%3|%4|%5")
            .arg(m_readerId)
            .arg(m_readerName)
            .arg(m_time)
            .arg(m_content)
            .arg(static_cast<int>(m_status));
    }
}

// 从文件字符串解析
Message Message::fromFileString(const QString &line)
{
    QStringList parts = line.split("|", Qt::KeepEmptyParts);

    // 管理员消息：7个字段
    if (parts.size() == 7)
    {
        return Message(parts[0], parts[1], parts[2], parts[3],
                       parts[5], static_cast<MessageStatus>(parts[6].toInt()));
    }
    // 读者消息：5个字段
    else if (parts.size() == 5)
    {
        return Message(parts[0], parts[1], parts[3],
                       static_cast<MessageStatus>(parts[4].toInt()));
    }

    return Message();
}

// 获取管理员显示字段（读者id|读者名|消息发送时间|消息内容|消息状态）
std::vector<QString> Message::getAdminDisplayFields() const
{
    std::vector<QString> fields;
    // 管理员消息和读者消息都显示读者信息（发送消息的用户）
    fields.push_back(m_readerId);
    fields.push_back(m_readerName);
    fields.push_back(m_time);
    fields.push_back(m_content);
    fields.push_back(m_status == MessageStatus::UNREAD ? "未读" : "已读");
    return fields;
}

// 获取读者显示字段（消息发送时间|消息内容|消息状态）
std::vector<QString> Message::getReaderDisplayFields() const
{
    std::vector<QString> fields;
    fields.push_back(m_time);
    fields.push_back(m_content);
    fields.push_back(m_status == MessageStatus::UNREAD ? "未读" : "已读");
    return fields;
}
