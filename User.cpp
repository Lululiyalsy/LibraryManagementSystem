#include "User.h"

// 默认构造函数
User::User() {}

// 有参构造函数
User::User(QString& I, QString& n, QString& pa, QString& ph, QString& e)
    : ID(I), name(n), password(pa), phone(ph), email(e) {}

// getter和setter
const QString& User::getID() const { return ID; }
int User::getType() const { return type; }
const QString& User::getName() const { return name; }
const QString& User::getPassword() const { return password; }
const QString& User::getPhone() const { return phone; }
const QString& User::getEmail() const { return email; }
std::vector<Message>& User::getMessages() { return messages; }

void User::setID(const QString& id) { ID = id; }
void User::setType(int t) { type = t; }
void User::setName(const QString& n) { name = n; }
void User::setPassword(const QString& pa) { password = pa; }
void User::setPhone(const QString& ph) { phone = ph; }
void User::setEmail(const QString& e) { email = e; }

// 发送消息
void User::addMessage(const Message& message) {
    messages.push_back(message);
}

// 删除消息
void User::deleteMessage(int index) {
    if (index >= 0 && index < static_cast<int>(messages.size())) {
        messages.erase(messages.begin() + index);
    }
}

// 清除所有消息
void User::clearMessages() {
    messages.clear();
}

// 获取未读消息的数量
int User::getUnreadMessageCount() {
    int count = 0;
    for (const auto& msg : messages) {
        if (msg.getStatus() == MessageStatus::UNREAD) {
            count++;
        }
    }
    return count;
}

// 析构函数
User::~User() {

}