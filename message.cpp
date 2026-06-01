/**
 * @file message.cpp
 * @brief 消息类实现
 * 
 * 实现Message类的所有成员函数，包括构造函数、getter/setter方法、
 * 序列化/反序列化功能以及显示字段获取。
 */

#include "Message.h"
#include <QDateTime>

/**
 * @brief 默认构造函数
 * 
 * 创建一个空消息对象，时间设置为当前时间，状态为未读。
 */
Message::Message()
    : m_isAdmin(false), m_status(MessageStatus::UNREAD)
{
    m_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

/**
 * @brief 管理员消息构造函数
 * @param adminId 管理员ID
 * @param adminName 管理员姓名
 * @param readerId 读者ID
 * @param readerName 读者姓名
 * @param content 消息内容
 * @param status 消息状态（默认未读）
 */
Message::Message(const QString &adminId, const QString &adminName,
                 const QString &readerId, const QString &readerName,
                 const QString &content, MessageStatus status)
    : m_isAdmin(true), m_adminId(adminId), m_adminName(adminName),
      m_readerId(readerId), m_readerName(readerName), m_content(content),
      m_status(status)
{
    m_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

/**
 * @brief 读者消息构造函数
 * @param readerId 读者ID
 * @param readerName 读者姓名
 * @param content 消息内容
 * @param status 消息状态（默认未读）
 */
Message::Message(const QString &readerId, const QString &readerName,
                 const QString &content, MessageStatus status)
    : m_isAdmin(false), m_readerId(readerId), m_readerName(readerName),
      m_content(content), m_status(status)
{
    m_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
}

// ========== 类型判断 ==========

/**
 * @brief 判断是否为管理员消息
 * @return true表示管理员消息，false表示读者消息
 */
bool Message::isAdminMessage() const
{
    return m_isAdmin;
}

// ========== getter 方法 ==========

/**
 * @brief 获取管理员ID
 * @return 管理员ID（仅管理员消息有效）
 */
QString Message::getAdminId() const
{
    return m_adminId;
}

/**
 * @brief 获取管理员姓名
 * @return 管理员姓名（仅管理员消息有效）
 */
QString Message::getAdminName() const
{
    return m_adminName;
}

/**
 * @brief 获取读者ID
 * @return 读者ID
 */
QString Message::getReaderId() const
{
    return m_readerId;
}

/**
 * @brief 获取读者姓名
 * @return 读者姓名
 */
QString Message::getReaderName() const
{
    return m_readerName;
}

/**
 * @brief 获取消息发送时间
 * @return 消息发送时间（格式：yyyy-MM-dd HH:mm:ss）
 */
QString Message::getTime() const
{
    return m_time;
}

/**
 * @brief 获取消息内容
 * @return 消息内容
 */
QString Message::getContent() const
{
    return m_content;
}

/**
 * @brief 获取消息状态
 * @return 消息状态（UNREAD 或 READ）
 */
MessageStatus Message::getStatus() const
{
    return m_status;
}

// ========== setter 方法 ==========

/**
 * @brief 设置消息状态
 * @param status 消息状态
 */
void Message::setStatus(MessageStatus status)
{
    m_status = status;
}

/**
 * @brief 设置消息发送时间
 * @param time 消息发送时间
 * 
 * 用于从文件读取时恢复原始消息时间，避免被覆盖为当前时间。
 */
void Message::setTime(const QString &time)
{
    m_time = time;
}

// ========== 序列化/反序列化 ==========

/**
 * @brief 转换为文件存储格式
 * @return 文件存储格式字符串
 * 
 * 管理员消息格式（7字段）：管理员id|管理员名|读者id|读者名|消息时间|消息内容|消息状态
 * 读者消息格式（5字段）：读者id|读者名|消息时间|消息内容|消息状态
 */
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

/**
 * @brief 从文件字符串解析消息
 * @param line 文件中的一行字符串
 * @return 解析后的Message对象
 * 
 * 根据字段数量自动识别消息类型：
 * - 7字段：管理员消息
 * - 5字段：读者消息
 * 
 * 解析后调用setTime()设置原始消息时间，避免构造函数覆盖为当前时间。
 */
Message Message::fromFileString(const QString &line)
{
    QStringList parts = line.split("|", Qt::KeepEmptyParts);

    // 管理员消息：7个字段
    if (parts.size() == 7)
    {
        Message msg(parts[0], parts[1], parts[2], parts[3],
                    parts[5], static_cast<MessageStatus>(parts[6].toInt()));
        msg.setTime(parts[4]); // 设置原始消息发送时间
        return msg;
    }
    // 读者消息：5个字段
    else if (parts.size() == 5)
    {
        Message msg(parts[0], parts[1], parts[3],
                    static_cast<MessageStatus>(parts[4].toInt()));
        msg.setTime(parts[2]); // 设置原始消息发送时间
        return msg;
    }

    // 返回默认空消息
    return Message();
}

// ========== 显示字段 ==========

/**
 * @brief 获取管理员显示字段
 * @return 显示字段列表（发送者id|发送者名|消息时间|消息内容|消息状态）
 * 
 * 如果是管理员发给自己的消息（管理员id == 读者id），显示管理员信息；
 * 否则显示读者信息（发送消息的用户）。
 */
std::vector<QString> Message::getAdminDisplayFields() const
{
    std::vector<QString> fields;
    // 判断是否为管理员发给自己的消息
    if (m_isAdmin && m_adminId == m_readerId)
    {
        // 管理员发给自己的消息，显示管理员信息
        fields.push_back(m_adminId);
        fields.push_back(m_adminName);
    }
    else
    {
        // 其他消息，显示读者信息（发送消息的用户）
        fields.push_back(m_readerId);
        fields.push_back(m_readerName);
    }
    fields.push_back(m_time);
    fields.push_back(m_content);
    fields.push_back(m_status == MessageStatus::UNREAD ? "未读" : "已读");
    return fields;
}

/**
 * @brief 获取读者显示字段
 * @return 显示字段列表（消息时间|消息内容|消息状态）
 */
std::vector<QString> Message::getReaderDisplayFields() const
{
    std::vector<QString> fields;
    fields.push_back(m_time);
    fields.push_back(m_content);
    fields.push_back(m_status == MessageStatus::UNREAD ? "未读" : "已读");
    return fields;
}
