/**
 * @file User.cpp
 * @brief 用户基类实现
 * 
 * 实现 User 类的成员函数，包括用户属性的访问器和消息管理功能。
 */

#include "User.h"

/**
 * @brief 默认构造函数
 */
User::User() {}

/**
 * @brief 有参构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 */
User::User(QString& I, QString& n, QString& pa, QString& ph, QString& e)
    : ID(I), name(n), password(pa), phone(ph), email(e) {}

// ========== getter 方法实现 ==========

/**
 * @brief 获取用户ID
 * @return 用户ID
 */
const QString& User::getID() const { return ID; }

/**
 * @brief 获取用户类型
 * @return 用户类型（1=管理员，2=读者）
 */
int User::getType() const { return type; }

/**
 * @brief 获取用户姓名
 * @return 用户姓名
 */
const QString& User::getName() const { return name; }

/**
 * @brief 获取用户密码
 * @return 用户密码
 */
const QString& User::getPassword() const { return password; }

/**
 * @brief 获取联系电话
 * @return 联系电话
 */
const QString& User::getPhone() const { return phone; }

/**
 * @brief 获取电子邮箱
 * @return 电子邮箱
 */
const QString& User::getEmail() const { return email; }

/**
 * @brief 获取消息列表
 * @return 消息列表引用
 */
std::vector<Message>& User::getMessages() { return messages; }

// ========== setter 方法实现 ==========

/**
 * @brief 设置用户ID
 * @param id 用户ID
 */
void User::setID(const QString& id) { ID = id; }

/**
 * @brief 设置用户类型
 * @param t 用户类型（1=管理员，2=读者）
 */
void User::setType(int t) { type = t; }

/**
 * @brief 设置用户姓名
 * @param n 用户姓名
 */
void User::setName(const QString& n) { name = n; }

/**
 * @brief 设置用户密码
 * @param pa 用户密码
 */
void User::setPassword(const QString& pa) { password = pa; }

/**
 * @brief 设置联系电话
 * @param ph 联系电话
 */
void User::setPhone(const QString& ph) { phone = ph; }

/**
 * @brief 设置电子邮箱
 * @param e 电子邮箱
 */
void User::setEmail(const QString& e) { email = e; }

// ========== 消息管理方法实现 ==========

/**
 * @brief 添加消息到用户消息列表
 * @param message 要添加的消息对象
 */
void User::addMessage(const Message& message) {
    messages.push_back(message);
}

/**
 * @brief 删除指定索引的消息
 * @param index 消息索引
 */
void User::deleteMessage(int index) {
    if (index >= 0 && index < static_cast<int>(messages.size())) {
        messages.erase(messages.begin() + index);
    }
}

/**
 * @brief 清除所有消息
 */
void User::clearMessages() {
    messages.clear();
}

/**
 * @brief 获取未读消息数量
 * @return 未读消息的数量
 */
int User::getUnreadMessageCount() {
    int count = 0;
    for (const auto& msg : messages) {
        if (msg.getStatus() == MessageStatus::UNREAD) {
            count++;
        }
    }
    return count;
}

/**
 * @brief 析构函数
 */
User::~User() {
}