#include "Admin.h"
#include "DataManager.h"
#include "Reader.h"
#include "Book.h"
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>
#include <algorithm>

// 构造函数
Admin::Admin(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : User(I, n, pa, ph, e)
{
    type = 1;
}

// 实现User的纯虚函数：获取用户身份
QString Admin::typeToIdentity()
{
    return QString("管理员");
}

// （生成验证码）：生成管理员验证码（大小写字母+数字，6位）
QString Admin::generateVerificationCode()
{
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString code;
    for (int i = 0; i < 6; ++i)
    {
        int index = QRandomGenerator::global()->bounded(chars.length());
        code.append(chars[index]);
    }
    return code;
}

// 注册用户账号（包括管理员和读者）
void Admin::registerUser(const QString &id, const QString &type, const QString &name,
                         const QString &password, const QString &phone, const QString &email)
{
    DataManager *dm = DataManager::getInstance();

    // 检查ID是否已存在（精确匹配）
    std::vector<User *> existingUsers = dm->findUsersById(id);
    bool idExists = false;
    for (auto user : existingUsers)
    {
        if (user->getID() == id)
        {
            idExists = true;
            break;
        }
    }
    if (idExists)
    {
        return;
    }

    QString idRef = id;
    QString nameRef = name;
    QString passwordRef = password;
    QString phoneRef = phone;
    QString emailRef = email;

    User *user = nullptr;
    if (type.toInt() == 1)
    {
        user = new Admin(idRef, nameRef, passwordRef, phoneRef, emailRef);
    }
    else
    {
        user = new Reader(idRef, nameRef, passwordRef, phoneRef, emailRef);
    }

    dm->addUser(user);
}

// 删除用户账号（包括管理员和读者）
bool Admin::deleteUser(const QString &id, const QString &name)
{
    DataManager *dm = DataManager::getInstance();

    // 必须同时根据ID和姓名两个关键字删除
    bool success = dm->deleteUser(id, name);

    if (success)
    {
        // 检查是否只剩0人，如果是则自动创建新管理员
        if (dm->getUserCount() == 0)
        {
            QString newId = "10000";
            QString newName = "张三";
            QString newPassword = "123";
            QString newPhone = "11111111111";
            QString newEmail = "zhangsan@qq.com";

            QString idRef = newId;
            QString nameRef = newName;
            QString passwordRef = newPassword;
            QString phoneRef = newPhone;
            QString emailRef = newEmail;

            User *newAdmin = new Admin(idRef, nameRef, passwordRef, phoneRef, emailRef);
            dm->addUser(newAdmin);
        }
    }
    else
    {
    }

    return success;
}

// 修改用户信息（包括管理员和读者）
bool Admin::updateUser(const QString &oldId, const QString &oldName,
                       const QString &newId, const QString &newType, const QString &newName,
                       const QString &password, const QString &phone, const QString &email)
{
    DataManager *dm = DataManager::getInstance();

    // 检查是否提供了两个关键字
    if (oldId.isEmpty() || oldName.isEmpty())
    {
        return false;
    }

    QString idRef = newId;
    QString nameRef = newName;
    QString passwordRef = password;
    QString phoneRef = phone;
    QString emailRef = email;

    User *newUser = nullptr;
    if (newType.toInt() == 1)
    {
        newUser = new Admin(idRef, nameRef, passwordRef, phoneRef, emailRef);
    }
    else
    {
        newUser = new Reader(idRef, nameRef, passwordRef, phoneRef, emailRef);
    }

    bool success = dm->updateUser(oldId, oldName, newUser);
    if (success)
    {
    }
    else
    {
        delete newUser;
    }

    return success;
}

// 查看全部用户信息（包括管理员和读者）
std::vector<User *> Admin::findAllUser()
{
    DataManager *dm = DataManager::getInstance();
    std::vector<User *> users = dm->getUsers();
    return users;
}

// 查找用户信息（支持ID、姓名、ID+姓名模糊匹配）
std::vector<User *> Admin::findUser(const QString &id, const QString &name)
{
    DataManager *dm = DataManager::getInstance();

    // 使用多关键字查找（均为模糊匹配，可能多个）
    std::vector<User *> users = dm->searchUsers(id, name);

    return users;
}

// 清空用户信息（包括管理员和读者），保留当前管理员，返回新的用户指针
User *Admin::clearUser(User *currentAdmin)
{
    DataManager *dm = DataManager::getInstance();

    // 保存当前管理员信息
    QString currentId, currentName, currentPassword, currentPhone, currentEmail;
    if (currentAdmin)
    {
        currentId = currentAdmin->getID();
        currentName = currentAdmin->getName();
        currentPassword = currentAdmin->getPassword();
        currentPhone = currentAdmin->getPhone();
        currentEmail = currentAdmin->getEmail();
    }

    // 清空所有用户
    dm->clearAllUsers();

    // 清空预约记录、借书记录、消息记录
    dm->clearAllReservations();
    dm->clearAllBorrowRecords();
    dm->clearAllMessages();

    // 如果有当前管理员，重新添加并返回新指针
    if (currentAdmin)
    {
        User *admin = new Admin(currentId, currentName, currentPassword, currentPhone, currentEmail);
        dm->addUser(admin);
        return admin;
    }
    return nullptr;
}

// 添加书本信息，返回值：0=成功新增，1=库存已增加，-1=ISBN冲突
int Admin::addBook(const QString &isbn, const QString &title, const QString &author,
                   const QString &category, int stock)
{
    DataManager *dm = DataManager::getInstance();

    QDateTime now = QDateTime::currentDateTime();
    Book book(isbn, title, author, category, stock, now, 0, 0, 0);

    return dm->addBook(book);
}

// 删除书本信息，返回值：0=成功删除记录，1=库存已减少，-1=ISBN不存在，-2=存在预约或借出无法删除
int Admin::deleteBook(const QString &isbn, int decreaseStock)
{
    DataManager *dm = DataManager::getInstance();
    return dm->deleteBook(isbn, decreaseStock);
}

// 修改书本信息，返回值：0=成功修改，-1=原ISBN不存在，-2=存在预约或借出无法修改，-3=新ISBN已存在
int Admin::updateBook(const QString &oldIsbn, const QString &newIsbn, const QString &title,
                      const QString &author, const QString &category, int stock,
                      const QDateTime &inStockTime)
{
    DataManager *dm = DataManager::getInstance();

    Book newBook(newIsbn, title, author, category, stock, inStockTime, 0, 0, 0);
    return dm->updateBook(oldIsbn, newBook);
}

// 查找书本信息（支持多条件模糊搜索）
std::vector<const Book *> Admin::findBook(const QString &isbn, const QString &title,
                                          const QString &author, const QString &category)
{
    DataManager *dm = DataManager::getInstance();
    return dm->searchBooks(isbn, title, author, category);
}

// 查找所有图书
std::vector<const Book *> Admin::findAllBook()
{
    DataManager *dm = DataManager::getInstance();
    std::vector<const Book *> results;
    for (const auto &book : dm->getBooks())
    {
        results.push_back(&book);
    }
    return results;
}

// 清空书本信息
void Admin::clearBook()
{
    DataManager *dm = DataManager::getInstance();
    dm->getBooks().clear();
    dm->writeBook();
}

// 按借阅次数排序
std::vector<const Book *> Admin::sortBookByBorrowCount()
{
    DataManager *dm = DataManager::getInstance();
    std::vector<const Book *> sortedBooks;
    for (const auto &book : dm->getBooks())
    {
        sortedBooks.push_back(&book);
    }
    std::sort(sortedBooks.begin(), sortedBooks.end(),
              [](const Book *a, const Book *b)
              {
                  return a->getBorrowCount() > b->getBorrowCount();
              });
    return sortedBooks;
}

// 按入库时间排序（最新入库优先）
std::vector<const Book *> Admin::sortBookByInStockTime()
{
    DataManager *dm = DataManager::getInstance();
    std::vector<const Book *> sortedBooks;
    for (const auto &book : dm->getBooks())
    {
        sortedBooks.push_back(&book);
    }
    std::sort(sortedBooks.begin(), sortedBooks.end(),
              [](const Book *a, const Book *b)
              {
                  return a->getInStockTime() < b->getInStockTime();
              });
    return sortedBooks;
}

// 生成统计报表(主要是借书情况)
void Admin::generateReport()
{
    DataManager *dm = DataManager::getInstance();

    // 获取所有借阅记录
    std::vector<BorrowRecord> allRecords = dm->getBorrowRecords();
    std::vector<BorrowRecord> overdueRecords = viewOverdueRecords();
    std::vector<Book> allBooks = dm->getBooks();
    std::vector<User *> allUsers = dm->getUsers();

    // 统计数据
    int totalBorrowCount = allRecords.size();
    int returnedCount = 0;
    int overdueCount = overdueRecords.size();
    int totalBooks = allBooks.size();
    int totalUsers = allUsers.size();
    int readerCount = 0;
    int adminCount = 0;

    // 计算读者和管理员数量
    for (auto user : allUsers)
    {
        if (user->getType() == 1)
            adminCount++;
        else
            readerCount++;
    }

    // 计算已归还数量
    for (auto &record : allRecords)
    {
        if (record.isReturned())
            returnedCount++;
    }

    // 计算借阅率和逾期率
    double returnRate = totalBorrowCount > 0 ? (double)returnedCount / totalBorrowCount * 100 : 0;
    double overdueRate = totalBorrowCount > 0 ? (double)overdueCount / totalBorrowCount * 100 : 0;

    // 按分类统计图书
    std::map<QString, int> categoryCount;
    for (auto &book : allBooks)
    {
        categoryCount[book.getCategory()]++;
    }

    // 获取热门图书（按借阅次数排序）
    std::vector<Book> hotBooks = dm->sortBooksByBorrowCount();
    if (hotBooks.size() > 5)
        hotBooks.resize(5); // 只取前5名

    // 生成报告内容
    QString report;
    report += "========================================\n";
    report += "          图书管理系统统计报表\n";
    report += "========================================\n\n";
    report += QString("统计时间: %1\n\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    // 用户统计
    report += "【用户统计】\n";
    report += QString("  总用户数: %1\n").arg(totalUsers);
    report += QString("  管理员数: %1\n").arg(adminCount);
    report += QString("  读者数: %1\n\n").arg(readerCount);

    // 图书统计
    report += "【图书统计】\n";
    report += QString("  图书总数: %1\n").arg(totalBooks);
    report += "  分类分布:\n";
    for (auto &pair : categoryCount)
    {
        report += QString("    - %1: %2 本\n").arg(pair.first).arg(pair.second);
    }
    report += "\n";

    // 借阅统计
    report += "【借阅统计】\n";
    report += QString("  总借阅次数: %1\n").arg(totalBorrowCount);
    report += QString("  已归还次数: %1\n").arg(returnedCount);
    report += QString("  借阅归还率: %1%\n").arg(QString::number(returnRate, 'f', 2));
    report += QString("  逾期数量: %1\n").arg(overdueCount);
    report += QString("  逾期率: %1%\n\n").arg(QString::number(overdueRate, 'f', 2));

    // 热门图书
    report += "【热门图书 TOP5】\n";
    for (int i = 0; i < hotBooks.size(); i++)
    {
        Book &book = hotBooks[i];
        report += QString("  %1. 《%2》 - 借阅%3次\n").arg(i + 1).arg(book.getTitle()).arg(book.getBorrowCount());
    }
    report += "\n";

    // 逾期图书列表
    report += "【逾期图书列表】\n";
    if (overdueRecords.empty())
    {
        report += "  暂无逾期图书\n";
    }
    else
    {
        for (auto &record : overdueRecords)
        {
            Book *book = dm->findBookByISBN(record.getISBN());
            QString bookTitle = book ? book->getTitle() : "未知书名";
            report += QString("  ISBN: %1, 书名: %2, 逾期%3天\n")
                          .arg(record.getISBN())
                          .arg(bookTitle)
                          .arg(record.calculateOverdueDays());
        }
    }

    report += "\n========================================\n";

    // 保存报告到文件
    QString appDir = QCoreApplication::applicationDirPath();
    QString reportFileName = QString("report_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString reportPath = appDir + "/reports/" + reportFileName;

    QDir reportDir(appDir + "/reports");
    if (!reportDir.exists())
    {
        reportDir.mkpath(".");
    }

    QFile reportFile(reportPath);
    if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&reportFile);
        out << report;
        reportFile.close();
    }
}

// 预约管理：查看所有预约记录
std::vector<Reservation> Admin::viewAllReservations()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getReservations();
}

// 预约管理：审核预约（按ISBN和读者ID，是否成功）
bool Admin::approveReservation(const QString &isbn, const QString &readerId, bool isSuccess)
{
    DataManager *dm = DataManager::getInstance();

    if (isSuccess)
    {
        // 审核成功：设置状态为 APPROVED
        std::vector<Reservation> &reservations = dm->getReservations();
        for (auto &reservation : reservations)
        {
            if (reservation.getISBN() == isbn && reservation.getReaderID() == readerId && reservation.getStatus() == Reservation::PENDING)
            {
                reservation.setStatus(Reservation::APPROVED);
                dm->writeReservation();

                // 发送消息通知读者（读者消息格式）
                QString readerMessageContent = QString("您预约的图书(ISBN:%1)已审核成功，请及时借阅。").arg(isbn);
                User *reader = dm->findUserById(readerId);
                if (reader)
                {
                    Message readerMessage(readerId, reader->getName(), readerMessageContent);
                    reader->addMessage(readerMessage);
                }

                // 发送消息给自己（管理员消息格式，以管理员为主语）
                QString adminMessageContent = QString("已通过读者 %1 对图书(ISBN:%2)的预约审核。").arg(reader ? reader->getName() : readerId).arg(isbn);
                Message adminMessage(ID, name, readerId, reader ? reader->getName() : "", adminMessageContent);
                this->addMessage(adminMessage);

                // 写入消息文件
                dm->writeMessage();

                return true;
            }
        }
    }
    else
    {
        // 审核失败：直接删除记录
        bool success = dm->removeReservation(isbn, readerId);
        if (success)
        {
            // 发送消息通知读者（读者消息格式）
            QString readerMessageContent = QString("您预约的图书(ISBN:%1)审核失败，预约已取消。").arg(isbn);
            User *reader = dm->findUserById(readerId);
            if (reader)
            {
                Message readerMessage(readerId, reader->getName(), readerMessageContent);
                reader->addMessage(readerMessage);
            }

            // 发送消息给自己（管理员消息格式，以管理员为主语）
            QString adminMessageContent = QString("已拒绝读者 %1 对图书(ISBN:%2)的预约审核。").arg(reader ? reader->getName() : readerId).arg(isbn);
            Message adminMessage(ID, name, readerId, reader ? reader->getName() : "", adminMessageContent);
            this->addMessage(adminMessage);

            // 写入消息文件
            dm->writeMessage();
        }
        return success;
    }
    return false;
}

// 借阅管理：办理借书
// 修改5.16
bool Admin::borrowBook(const QString &isbn, const QString &readerId)
{
    DataManager *dm = DataManager::getInstance();

    Book *book = dm->findBookByISBN(isbn);
    if (!book)
        return false;

    int available = book->getStock() - book->getCurrentBorrowed();

    // 检查读者是否有逾期未还图书
    if (dm->hasOverdueBooks(readerId))
        return false;

    // 检查读者借书数量是否超限
    int currentBorrowCount = dm->getBorrowCountByReader(readerId);
    if (currentBorrowCount >= 10)
        return false;

    // 检查该读者是否有该图书的有效预约（状态为 NOTIFIED）
    bool hasValidReservation = false;
    int reservationIndex = -1;
    std::vector<Reservation> &reservations = dm->getReservations();

    // 统计审核成功但未完成的预约数量
    int notifiedCount = 0;

    for (size_t i = 0; i < reservations.size(); ++i)
    {
        if (reservations[i].getISBN() == isbn &&
            reservations[i].getStatus() == Reservation::APPROVED)
        {
            notifiedCount++;

            if (reservations[i].getReaderID() == readerId)
            {
                hasValidReservation = true;
                reservationIndex = i;
            }
        }
    }
    // 情况1：有可用库存且预约人数不超过库存 → 所有人都能借
    // 情况2：有可用库存但预约人数超过库存 → 只有有预约的读者能借
    // 情况3：无可用库存 → 只有有预约的读者能借（预约队列）
    if (available <= 0)
    {
        // 无库存，必须有有效预约才能借
        if (!hasValidReservation)
        {
            return false;
        }
    }
    else if (notifiedCount > available)
    {
        // 有库存但预约人数超过库存，必须有有效预约才能借
        if (!hasValidReservation)
        {
            return false;
        }
    }
    // 其他情况：有库存且预约人数不多于库存，或无预约者，任何人都能借

    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueTime = now.addDays(30);

    BorrowRecord record(isbn, readerId, now, dueTime);
    dm->addBorrowRecord(record);

    // 如果有有效预约，直接删除预约记录
    if (reservationIndex >= 0)
    {
        dm->removeReservation(isbn, readerId);
    }

    return true;
}

// 借阅管理：办理还书
bool Admin::returnBook(const QString &isbn, const QString &readerId)
{
    DataManager *dm = DataManager::getInstance();

    bool success = dm->updateBorrowRecord(isbn, readerId);

    return success;
}

// 借阅管理：办理续借
bool Admin::renewBook(const QString &isbn, const QString &readerId)
{
    DataManager *dm = DataManager::getInstance();
    return dm->renewBorrowRecord(isbn, readerId, 30);
}

// 借阅管理：查看所有借阅记录
std::vector<BorrowRecord> Admin::viewBorrowRecords()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getBorrowRecords();
}

// 借阅管理：查看逾期记录
std::vector<BorrowRecord> Admin::viewOverdueRecords()
{
    DataManager *dm = DataManager::getInstance();
    std::vector<BorrowRecord> allRecords = dm->getBorrowRecords();
    std::vector<BorrowRecord> overdueRecords;

    QDateTime now = QDateTime::currentDateTime();
    for (auto &record : allRecords)
    {
        if (!record.isReturned() && now > record.getDueTime())
        {
            overdueRecords.push_back(record);
        }
    }

    return overdueRecords;
}

// 修改结束
//  析构函数
Admin::~Admin()
{
}
