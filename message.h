/**
 * @file message.h
 * @brief 消息类定义
 *
 * Message类表示系统中的消息对象，支持管理员消息和读者消息两种类型，
 * 提供消息的序列化和反序列化功能。
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <vector>

/**
 * @enum MessageStatus
 * @brief 消息状态枚举
 */
enum class MessageStatus
{
    UNREAD = 0, ///< 未读状态
    READ = 1    ///< 已读状态
};

/**
 * @class Message
 * @brief 消息类
 *
 * 表示系统中的消息对象，支持两种消息类型：
 * - 管理员消息（7字段）：包含管理员和读者双方信息
 * - 读者消息（5字段）：仅包含读者信息
 *
 * 支持消息的序列化（toFileString）和反序列化（fromFileString）。
 */
class Message
{
public:
    /**
     * @brief 默认构造函数
     *
     * 创建一个空消息对象，时间设置为当前时间，状态为未读。
     */
    Message();

    /**
     * @brief 管理员消息构造函数
     * @param adminId 管理员ID
     * @param adminName 管理员姓名
     * @param readerId 读者ID
     * @param readerName 读者姓名
     * @param content 消息内容
     * @param status 消息状态（默认未读）
     */
    Message(const QString &adminId, const QString &adminName,
            const QString &readerId, const QString &readerName,
            const QString &content, MessageStatus status = MessageStatus::UNREAD);

    /**
     * @brief 读者消息构造函数
     * @param readerId 读者ID
     * @param readerName 读者姓名
     * @param content 消息内容
     * @param status 消息状态（默认未读）
     */
    Message(const QString &readerId, const QString &readerName,
            const QString &content, MessageStatus status = MessageStatus::UNREAD);

    // ========== 类型判断 ==========

    /**
     * @brief 判断是否为管理员消息
     * @return true表示管理员消息，false表示读者消息
     */
    bool isAdminMessage() const;

    // ========== getter 方法 ==========

    /**
     * @brief 获取管理员ID
     * @return 管理员ID（仅管理员消息有效）
     */
    QString getAdminId() const;

    /**
     * @brief 获取管理员姓名
     * @return 管理员姓名（仅管理员消息有效）
     */
    QString getAdminName() const;

    /**
     * @brief 获取读者ID
     * @return 读者ID
     */
    QString getReaderId() const;

    /**
     * @brief 获取读者姓名
     * @return 读者姓名
     */
    QString getReaderName() const;

    /**
     * @brief 获取消息发送时间
     * @return 消息发送时间（格式：yyyy-MM-dd HH:mm:ss）
     */
    QString getTime() const;

    /**
     * @brief 获取消息内容
     * @return 消息内容
     */
    QString getContent() const;

    /**
     * @brief 获取消息状态
     * @return 消息状态（UNREAD 或 READ）
     */
    MessageStatus getStatus() const;

    // ========== setter 方法 ==========

    /**
     * @brief 设置消息状态
     * @param status 消息状态
     */
    void setStatus(MessageStatus status);

    /**
     * @brief 设置消息发送时间
     * @param time 消息发送时间
     *
     * 用于从文件读取时恢复原始消息时间，避免被覆盖为当前时间。
     */
    void setTime(const QString &time);

    // ========== 序列化/反序列化 ==========

    /**
     * @brief 转换为文件存储格式
     * @return 文件存储格式字符串
     *
     * 管理员消息格式（7字段）：管理员id|管理员名|读者id|读者名|消息时间|消息内容|消息状态
     * 读者消息格式（5字段）：读者id|读者名|消息时间|消息内容|消息状态
     */
    QString toFileString() const;

    /**
     * @brief 从文件字符串解析消息
     * @param line 文件中的一行字符串
     * @return 解析后的Message对象
     *
     * 根据字段数量自动识别管理员消息（7字段）或读者消息（5字段）。
     */
    static Message fromFileString(const QString &line);

    // ========== 显示字段 ==========

    /**
     * @brief 获取管理员显示字段
     * @return 显示字段列表（发送者id|发送者名|消息时间|消息内容|消息状态）
     *
     * 如果是管理员发给自己的消息，显示管理员信息；否则显示读者信息。
     */
    std::vector<QString> getAdminDisplayFields() const;

    /**
     * @brief 获取读者显示字段
     * @return 显示字段列表（消息时间|消息内容|消息状态）
     */
    std::vector<QString> getReaderDisplayFields() const;

private:
    bool m_isAdmin;         ///< 是否为管理员消息
    QString m_adminId;      ///< 管理员ID（仅管理员消息有效）
    QString m_adminName;    ///< 管理员姓名（仅管理员消息有效）
    QString m_readerId;     ///< 读者ID
    QString m_readerName;   ///< 读者姓名
    QString m_time;         ///< 消息发送时间（格式：yyyy-MM-dd HH:mm:ss）
    QString m_content;      ///< 消息内容
    MessageStatus m_status; ///< 消息状态（UNREAD/READ）
};

#endif // MESSAGE_H
