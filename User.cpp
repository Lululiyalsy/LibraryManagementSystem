#include "User.h"

User::User() {}

User::User(QString& I, QString& n, QString& pa, QString& ph, QString& e)
    : ID(I), name(n), password(pa), phone(ph), email(e) {}

// getter和setter
const QString& User::getID() const { return ID; }
int User::getType() const { return type; }
const QString& User::getName() const { return name; }
const QString& User::getPassword() const { return password; }
const QString& User::getPhone() const { return phone; }
const QString& User::getEmail() const { return email; }

void User::setID(const QString& id) { ID = id; }
void User::setType(int t) { type = t; }
void User::setName(const QString& n) { name = n; }
void User::setPassword(const QString& pa) { password = pa; }
void User::setPhone(const QString& ph) { phone = ph; }
void User::setEmail(const QString& e) { email = e; }

// 消息相关方法实现
std::vector<QString>& User::getMsg() {
    return msg;
}

void User::addMsg(const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    msg.push_back(timestamp + "||" + message);
}

void User::clearMsg() {
    msg.clear();
}

int User::getUnreadMsgCount() {
    return msg.size();
}

User::~User() {

}