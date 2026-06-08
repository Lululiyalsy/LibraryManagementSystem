/**
 * @file DataManager.cpp
 * @brief 数据管理器类实现
 *
 * 实现DataManager类的所有成员函数，包括数据初始化、读写操作和查询功能。
 */

#include "DataManager.h"
#include "Admin.h"
#include "Reader.h"
#include "StudentReader.h"
#include "TeacherReader.h"
#include "ExternalReader.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <algorithm>

/**
 * @brief DataManager单例对象的静态指针
 */
DataManager *DataManager::instance = nullptr;

/**
 * @brief 获取DataManager单例对象
 * @return DataManager单例指针
 *
 * 采用懒汉式单例模式，第一次调用时创建实例。
 */
DataManager *DataManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new DataManager();
    }
    return instance;
}

/**
 * @brief 私有构造函数
 *
 * 初始化数据文件路径，创建数据目录（如果不存在），
 * 并加载所有数据文件。
 */
DataManager::DataManager()
{
    // 设置数据文件保存路径
    // 优先使用应用程序目录下的data子目录
    QString appDir = QCoreApplication::applicationDirPath();
    QString dataDir = appDir + "/data";

    // 检查应用程序目录下是否存在data目录，如果不存在，尝试使用项目根目录
    QDir appDataDir(dataDir);
    if (!appDataDir.exists())
    {
        // 尝试使用应用程序目录的上一级目录（项目根目录）
        QDir parentDir(appDir);
        if (parentDir.cdUp())
        {
            dataDir = parentDir.path() + "/data";
        }
    }

    // 设置各数据文件路径
    userFilePath = dataDir + "/users.txt";
    bookFilePath = dataDir + "/books.txt";
    borrowRecordFilePath = dataDir + "/borrow_records.txt";
    reservationFilePath = dataDir + "/reservations.txt";
    messageFilePath = dataDir + "/messages.txt";

    // 检查并创建数据目录
    QDir dir(dataDir);
    if (!dir.exists())
    {
        dir.mkpath(dataDir);
    }

    // 读取所有数据文件
    initUser();
    initBook();
    initBorrowRecord();
    initReservation();
    initMessage();

    // 根据借阅记录重新计算所有读者信用分（必须在initMessage之后调用）
    recalculateCreditScores();
}

/**
 * @brief 析构函数
 *
 * 析构时自动保存所有数据到文件，并释放用户对象的内存。
 */
DataManager::~DataManager()
{
    // 保存所有数据
    writeUser();
    writeBook();
    writeBorrowRecord();
    writeReservation();

    // 释放users中动态分配的内存
    for (auto user : users)
    {
        delete user;
    }
}

// ========== 用户管理 ==========

/**
 * @brief 根据ID查找用户（模糊匹配）
 * @param id 用户ID关键字
 * @return 匹配的用户指针列表
 */
std::vector<::User *> DataManager::findUsersById(const QString &id)
{
    std::vector<::User *> results;
    for (::User *user : users)
    {
        if (user->getID().contains(id, Qt::CaseInsensitive))
        {
            results.push_back(user);
        }
    }
    return results;
}

/**
 * @brief 根据姓名查找用户（模糊匹配）
 * @param name 用户姓名关键字
 * @return 匹配的用户指针列表
 */
std::vector<::User *> DataManager::findUsersByName(const QString &name)
{
    std::vector<::User *> results;
    for (::User *user : users)
    {
        if (user->getName().contains(name, Qt::CaseInsensitive))
        {
            results.push_back(user);
        }
    }
    return results;
}

/**
 * @brief 根据ID精确查找用户
 * @param id 用户ID
 * @return 匹配的用户指针（未找到返回nullptr）
 */
::User *DataManager::findUserById(const QString &id)
{
    for (::User *user : users)
    {
        if (user->getID() == id)
        {
            return user;
        }
    }
    return nullptr;
}

/**
 * @brief 根据ID和姓名同时查找用户（均为模糊匹配）
 * @param id 用户ID关键字
 * @param name 用户姓名关键字
 * @return 匹配的用户指针列表
 */
std::vector<::User *> DataManager::searchUsers(const QString &id, const QString &name)
{
    std::vector<::User *> results;

    for (::User *user : users)
    {
        bool match = true;

        // ID模糊匹配
        if (!id.isEmpty())
        {
            if (!user->getID().contains(id, Qt::CaseInsensitive))
            {
                match = false;
            }
        }

        // 姓名模糊匹配
        if (!name.isEmpty())
        {
            if (!user->getName().contains(name, Qt::CaseInsensitive))
            {
                match = false;
            }
        }

        if (match)
        {
            results.push_back(user);
        }
    }

    return results;
}

/**
 * @brief 添加用户并保存到文件
 * @param user 用户指针
 */
void DataManager::addUser(::User *user)
{
    users.push_back(user);
    writeUser();
}

/**
 * @brief 根据ID和姓名同时删除用户并保存到文件
 * @param id 用户ID
 * @param name 用户姓名
 * @return 删除成功返回true，失败返回false
 */
bool DataManager::deleteUser(const QString &id, const QString &name)
{
    for (auto it = users.begin(); it != users.end(); ++it)
    {
        // ID和姓名都需要匹配
        if ((*it)->getID() == id && (*it)->getName() == name)
        {
            delete *it;
            users.erase(it);
            writeUser();
            return true;
        }
    }
    return false;
}

/**
 * @brief 根据ID和姓名同时修改用户信息并保存到文件
 * @param id 用户ID
 * @param name 用户姓名
 * @param newUser 新用户信息
 * @return 修改成功返回true，失败返回false
 */
bool DataManager::updateUser(const QString &id, const QString &name, ::User *newUser)
{
    for (auto it = users.begin(); it != users.end(); ++it)
    {
        // ID和姓名都需要匹配
        if ((*it)->getID() == id && (*it)->getName() == name)
        {
            delete *it;
            *it = newUser;
            writeUser();
            return true;
        }
    }
    return false;
}

/**
 * @brief 清除所有用户并保存到文件
 */
void DataManager::clearAllUsers()
{
    for (auto user : users)
    {
        delete user;
    }
    users.clear();
    writeUser();
}

// ========== 数据清除 ==========

/**
 * @brief 清空预约记录
 */
void DataManager::clearAllReservations()
{
    reservations.clear();
    writeReservation();
}

/**
 * @brief 清空借书记录
 */
void DataManager::clearAllBorrowRecords()
{
    borrowRecords.clear();
    writeBorrowRecord();
}

/**
 * @brief 清空消息记录
 */
void DataManager::clearAllMessages()
{
    for (auto user : users)
    {
        user->clearMessages();
    }
    writeMessage();
}

/**
 * @brief 获取所有用户
 * @return 用户指针列表引用
 */
std::vector<::User *> &DataManager::getUsers()
{
    return users;
}

/**
 * @brief 获取用户数量
 * @return 用户总数
 */
int DataManager::getUserCount() const
{
    return users.size();
}

/**
 * @brief 初始化读取用户数据
 *
 * 从users.txt文件中读取用户信息，包括ID、类型、角色、姓名、密码、电话、邮箱、
 * 信用分、之前信用分和限制终止日期。
 * 对于读者类型用户，根据role字段创建对应的子类（StudentReader/TeacherReader/ExternalReader），
 * 并额外初始化信用分和借阅策略相关信息。
 * 兼容旧数据格式：缺少role字段时默认创建StudentReader。
 */
void DataManager::initUser()
{
    // 处理用户自身属性users.txt
    QFile file(userFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // 使用竖线作为分隔符，避免字段内容包含空格时错位
        QStringList fields = line.split("|");
        if (fields.size() < 6)
        {
            continue;
        }

        QString id = fields[0];
        int type = fields[1].toInt();
        int role = 1; // 默认角色为学生读者（兼容旧数据）
        QString name;
        QString password;
        QString phone;
        QString email;
        int creditScore = 100;     // 默认信用分100
        int prevCreditScore = 100; // 默认之前的信用分等于当前信用分
        QDateTime banUntil;
        bool depositPaid = false;  // 押金缴纳状态（仅校外读者）

        // 新格式：ID|type|role|name|password|phone|email|creditScore|prevCreditScore|banUntil|depositPaid
        // 旧格式：ID|type|name|password|phone|email|creditScore|prevCreditScore|banUntil
        // 通过判断fields[2]是否为数字来区分新旧格式
        if (fields.size() >= 10)
        {
            // 新格式，包含role字段
            role = fields[2].toInt();
            name = fields[3];
            password = fields[4];
            phone = fields[5];
            email = fields[6];
            if (!fields[7].isEmpty())
            {
                creditScore = fields[7].toInt();
            }
            if (!fields[8].isEmpty())
            {
                prevCreditScore = fields[8].toInt();
            }
            if (!fields[9].isEmpty())
            {
                banUntil = QDateTime::fromString(fields[9], "yyyy-MM-dd HH:mm:ss");
            }
            // （读取押金状态）：第11个字段为depositPaid（兼容旧数据默认false）
            if (fields.size() >= 11 && !fields[10].isEmpty())
            {
                depositPaid = (fields[10].toInt() == 1);
            }
        }
        else
        {
            // 旧格式，不含role字段
            name = fields[2];
            password = fields[3];
            phone = fields[4];
            email = fields[5];
            if (fields.size() >= 7 && !fields[6].isEmpty())
            {
                creditScore = fields[6].toInt();
            }
            if (fields.size() >= 8 && !fields[7].isEmpty())
            {
                prevCreditScore = fields[7].toInt();
            }
            if (fields.size() >= 9 && !fields[8].isEmpty())
            {
                banUntil = QDateTime::fromString(fields[8], "yyyy-MM-dd HH:mm:ss");
            }
        }

        ::User *user = nullptr;
        if (type == 1)
        {
            user = new ::Admin(id, name, password, phone, email);
        }
        else
        {
            // 根据role创建对应的读者子类
            if (role == 2)
            {
                user = new ::TeacherReader(id, name, password, phone, email);
            }
            else if (role == 3)
            {
                user = new ::ExternalReader(id, name, password, phone, email);
            }
            else
            {
                // role == 1 或旧数据默认创建学生读者
                user = new ::StudentReader(id, name, password, phone, email);
            }

            if (user)
            {
                ::Reader *reader = dynamic_cast<::Reader *>(user);
                if (reader)
                {
                    reader->setCreditScore(creditScore);
                    reader->setPrevCreditScore(prevCreditScore);
                    reader->setBanUntil(banUntil);
                }
                // （设置押金状态）：校外读者从文件读取押金缴纳状态
                ::ExternalReader *extReader = dynamic_cast<::ExternalReader *>(user);
                if (extReader)
                {
                    extReader->setDepositPaid(depositPaid);
                }
            }
        }

        users.push_back(user);
    }

    file.close();
}

/**
 * @brief 获取信用分所在的区间
 * @param score 信用分
 * @return 区间编号（0-5）
 *
 * 信用分区间划分：
 * - 90-100分：返回0
 * - 80-89分：返回1
 * - 70-79分：返回2
 * - 60-69分：返回3
 * - 50-59分：返回4
 * - 0-49分：返回5
 */
int getCreditTier(int score)
{
    if (score < 50)
        return 5;
    if (score < 60)
        return 4;
    if (score < 70)
        return 3;
    if (score < 80)
        return 2;
    if (score < 90)
        return 1;
    return 0;
}

/**
 * @brief 重新计算所有读者信用分
 *
 * 执行两步操作：
 * 1. 处理逾期扣分：遍历所有借阅记录，对未归还且逾期的记录扣除信用分，
 *    并发送扣分消息给读者。限制期间不扣分，但继续记录已扣分数。
 * 2. 检查信用分变化：检查读者信用分是否从高区间跌落到低区间，
 *    如果是则触发限制，并发送限制消息给读者。
 *
 * 信用分限制规则：
 * - 90-100分：无限制
 * - 80-89分：限制1天
 * - 70-79分：限制3天
 * - 60-69分：限制1周
 * - 50-59分：限制2周
 * - 0-49分：限制1个月
 */
void DataManager::recalculateCreditScores()
{
    QDateTime now = QDateTime::currentDateTime();

    // 第一步：处理逾期扣分
    for (auto &record : borrowRecords)
    {
        QString readerId = record.getReaderID();
        int overdueDays = record.calculateOverdueDays();
        int deductedScore = record.getDeductedScore();

        int needDeduct = overdueDays - deductedScore;

        ::User *user = findUserById(readerId);
        if (!user || user->getType() != 2)
            continue;

        ::Reader *reader = dynamic_cast<::Reader *>(user);
        if (!reader)
            continue;

        record.setFineAmount(record.calculateFine());

        bool isBanned = reader->isBanned();

        // 先更新 deductedScore（不管是否在限制期间，都要记录已扣分数）
        if (!record.isReturned() && overdueDays > deductedScore)
        {
            record.setDeductedScore(overdueDays);
        }

        // 只有"不在限制期间、未还且逾期、需要扣分"的记录才扣分
        if (!isBanned && !record.isReturned() && overdueDays > 0 && needDeduct > 0)
        {
            // 根据读者策略获取每日信用分扣减
            int deductPerDay = reader->getCreditDeductPerDay();
            int actualDeduct = needDeduct * deductPerDay;
            int currentScore = reader->getCreditScore();
            int newScore = qMax(currentScore - actualDeduct, 0);
            reader->setCreditScore(newScore);

            // 发送扣分消息给读者
            QString bookTitle = "未知";
            Book *book = findBookByISBN(record.getISBN());
            if (book)
            {
                bookTitle = book->getTitle();
            }

            QString msgContent = QString("您借阅的图书《%1》(ISBN:%2)已逾期%3天，扣除%4分信用分，当前信用分：%5")
                                     .arg(bookTitle)
                                     .arg(record.getISBN())
                                     .arg(overdueDays)
                                     .arg(actualDeduct)
                                     .arg(newScore);
            Message msg(readerId, reader->getName(), msgContent);
            reader->addMessage(msg);
        }
    }

    // 第二步：检查信用分变化，触发限制
    for (auto user : users)
    {
        if (user->getType() != 2)
            continue;

        ::Reader *reader = dynamic_cast<::Reader *>(user);
        if (!reader)
            continue;

        int creditScore = reader->getCreditScore();
        int prevCreditScore = reader->getPrevCreditScore();
        bool isBanned = reader->isBanned();

        int currentTier = getCreditTier(creditScore);
        int prevTier = getCreditTier(prevCreditScore);

        // 只有从更高区间下跌到更低区间时才触发限制
        if (currentTier > prevTier)
        {
            QDateTime banUntil;
            if (creditScore < 50)
            {
                banUntil = now.addDays(30);
            }
            else if (creditScore < 60)
            {
                banUntil = now.addDays(14);
            }
            else if (creditScore < 70)
            {
                banUntil = now.addDays(7);
            }
            else if (creditScore < 80)
            {
                banUntil = now.addDays(3);
            }
            else if (creditScore < 90)
            {
                banUntil = now.addDays(1);
            }

            if (banUntil.isValid())
            {
                reader->setBanUntil(banUntil);

                // 发送限制消息给读者
                QString limitDays;
                if (creditScore < 50)
                {
                    limitDays = "1个月";
                }
                else if (creditScore < 60)
                {
                    limitDays = "2周";
                }
                else if (creditScore < 70)
                {
                    limitDays = "1周";
                }
                else if (creditScore < 80)
                {
                    limitDays = "3天";
                }
                else if (creditScore < 90)
                {
                    limitDays = "1天";
                }

                QString msgContent = QString("由于您的信用分降至%1分，已被限制预约、借书和续借%2。限制期间您仍可正常还书。")
                                         .arg(creditScore)
                                         .arg(limitDays);
                Message msg(reader->getID(), reader->getName(), msgContent);
                reader->addMessage(msg);
            }
        }

        // 不管怎样，更新 prevCreditScore 为当前 creditScore
        reader->setPrevCreditScore(creditScore);
    }

    writeUser();
    writeBorrowRecord();
    writeMessage(); // 写入消息，包含扣分通知
}

/**
 * @brief 写入用户数据到文件
 *
 * 将所有用户信息写入users.txt文件，包括ID、类型、角色、姓名、密码、电话、邮箱、
 * 信用分、之前信用分和限制终止日期。
 * 新格式：ID|type|role|name|password|phone|email|creditScore|prevCreditScore|banUntil
 * 管理员角色字段固定为0。
 */
void DataManager::writeUser()
{
    QFile file(userFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }

    QTextStream out(&file);
    for (auto user : users)
    {
        int creditScore = 100;
        int prevCreditScore = 100;
        int role = 0; // 管理员角色为0
        QString banUntilStr = "";
        int depositPaid = 0; // 押金缴纳状态（0=未缴纳，1=已缴纳，仅校外读者）
        if (user->getType() == 2) // 读者
        {
            ::Reader *reader = dynamic_cast<::Reader *>(user);
            if (reader)
            {
                creditScore = reader->getCreditScore();
                prevCreditScore = reader->getPrevCreditScore();
                role = static_cast<int>(reader->getRole());
                if (reader->getBanUntil().isValid())
                {
                    banUntilStr = reader->getBanUntil().toString("yyyy-MM-dd HH:mm:ss");
                }
                // （校外读者押金状态）：写入depositPaid字段
                ::ExternalReader *extReader = dynamic_cast<::ExternalReader *>(reader);
                if (extReader && extReader->isDepositPaid())
                {
                    depositPaid = 1;
                }
            }
        }
        QString line = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9|%10|%11")
                           .arg(user->getID())
                           .arg(user->getType())
                           .arg(role)
                           .arg(user->getName())
                           .arg(user->getPassword())
                           .arg(user->getPhone())
                           .arg(user->getEmail())
                           .arg(creditScore)
                           .arg(prevCreditScore)
                           .arg(banUntilStr)
                           .arg(depositPaid);
        out << line << "\n";
    }

    file.close();
}

/**
 * @brief 初始化读取图书数据
 *
 * 从books.txt文件中读取图书信息，包括ISBN、书名、作者、分类、
 * 库存、入库时间、借阅次数、当前借出数量和预约人数。
 */
void DataManager::initBook()
{
    QFile file(bookFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList fields = line.split("|", Qt::SkipEmptyParts);
        if (fields.size() < 8)
        {
            continue;
        }

        QString isbn = fields[0];
        QString title = fields[1];
        QString author = fields[2];
        QString category = fields[3];
        int stock = fields[4].toInt();
        QDateTime inStockTime = QDateTime::fromString(fields[5], "yyyy-MM-dd HH:mm:ss");
        int borrowCount = fields[6].toInt();
        int currentBorrowed = fields[7].toInt();
        int reservationCount = (fields.size() >= 9) ? fields[8].toInt() : 0;

        books.push_back(Book(isbn, title, author, category, stock, inStockTime, borrowCount, currentBorrowed, reservationCount));
    }

    file.close();
}

/**
 * @brief 写入图书数据到文件
 *
 * 将所有图书信息写入books.txt文件，包括ISBN、书名、作者、分类、
 * 库存、入库时间、借阅次数、当前借出数量和预约人数。
 */
void DataManager::writeBook()
{
    QFile file(bookFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }

    QTextStream out(&file);
    for (auto &book : books)
    {
        QString line = QString("%1|%2|%3|%4|%5|%6|%7|%8|%9")
                           .arg(book.getISBN())
                           .arg(book.getTitle())
                           .arg(book.getAuthor())
                           .arg(book.getCategory())
                           .arg(book.getStock())
                           .arg(book.getInStockTime().toString("yyyy-MM-dd HH:mm:ss"))
                           .arg(book.getBorrowCount())
                           .arg(book.getCurrentBorrowed())
                           .arg(book.getReservationCount());
        out << line << "\n";
    }

    file.close();
}

/**
 * @brief 根据ISBN查找图书（精确匹配）
 * @param isbn 图书ISBN编号
 * @return 匹配的图书指针（未找到返回nullptr）
 */
Book *DataManager::findBookByISBN(const QString &isbn)
{
    for (auto &book : books)
    {
        if (book.getISBN() == isbn)
        {
            return &book;
        }
    }
    return nullptr;
}

/**
 * @brief 根据书名查找图书（模糊匹配）
 * @param title 书名关键字
 * @return 匹配的图书列表
 */
std::vector<Book> DataManager::findBooksByTitle(const QString &title)
{
    std::vector<Book> results;
    for (auto &book : books)
    {
        if (book.getTitle().contains(title, Qt::CaseInsensitive))
        {
            results.push_back(book);
        }
    }
    return results;
}

/**
 * @brief 根据作者查找图书（模糊匹配）
 * @param author 作者关键字
 * @return 匹配的图书列表
 */
std::vector<Book> DataManager::findBooksByAuthor(const QString &author)
{
    std::vector<Book> results;
    for (auto &book : books)
    {
        if (book.getAuthor().contains(author, Qt::CaseInsensitive))
        {
            results.push_back(book);
        }
    }
    return results;
}

/**
 * @brief 根据分类查找图书（模糊匹配）
 * @param category 分类关键字
 * @return 匹配的图书列表
 */
std::vector<Book> DataManager::findBooksByCategory(const QString &category)
{
    std::vector<Book> results;
    for (auto &book : books)
    {
        if (book.getCategory().contains(category, Qt::CaseInsensitive))
        {
            results.push_back(book);
        }
    }
    return results;
}

/**
 * @brief 多条件搜索图书
 * @param isbn ISBN关键字（模糊匹配）
 * @param title 书名关键字（模糊匹配）
 * @param author 作者关键字（模糊匹配）
 * @param category 分类关键字（模糊匹配）
 * @return 匹配的图书指针列表
 *
 * 支持按ISBN、书名、作者、分类的组合条件搜索，
 * 所有条件都是模糊匹配，只有同时满足所有条件的图书才会被返回。
 */
std::vector<const Book *> DataManager::searchBooks(const QString &isbn, const QString &title,
                                                   const QString &author, const QString &category)
{
    std::vector<const Book *> results;

    for (const auto &book : books)
    {
        bool match = true;

        if (!isbn.isEmpty() && !book.getISBN().contains(isbn, Qt::CaseInsensitive))
        {
            match = false;
        }

        if (!title.isEmpty() && !book.getTitle().contains(title, Qt::CaseInsensitive))
        {
            match = false;
        }

        if (!author.isEmpty() && !book.getAuthor().contains(author, Qt::CaseInsensitive))
        {
            match = false;
        }

        if (!category.isEmpty() && !book.getCategory().contains(category, Qt::CaseInsensitive))
        {
            match = false;
        }

        if (match)
        {
            results.push_back(&book); // 返回指针，指向原对象
        }
    }

    return results;
}

/**
 * @brief 添加图书并保存到文件
 * @param book 要添加的图书
 * @return 返回值：0=成功新增，1=库存已增加，-1=ISBN冲突（其他信息不匹配）
 *
 * 如果ISBN已存在且其他信息（书名、作者、分类）也相同，则增加库存；
 * 如果ISBN已存在但其他信息不匹配，则返回错误；
 * 如果ISBN不存在，则新增图书。
 */
int DataManager::addBook(const Book &book)
{
    // 先按ISBN精确查找
    Book *existingBook = findBookByISBN(book.getISBN());

    if (existingBook != nullptr)
    {
        // ISBN已存在，检查其他三个条件是否也相同
        if (existingBook->getTitle() == book.getTitle() &&
            existingBook->getAuthor() == book.getAuthor() &&
            existingBook->getCategory() == book.getCategory())
        {
            // 四个条件都相同，更新库存量和入库时间
            int oldStock = existingBook->getStock();
            existingBook->setStock(existingBook->getStock() + book.getStock());
            existingBook->setInStockTime(QDateTime::currentDateTime());
            writeBook();

            // 如果库存增加了，通知预约成功的读者
            if (book.getStock() > 0)
            {
                notifyReservations(book.getISBN());
            }

            return 1; // 库存已增加
        }
        else
        {
            // ISBN相同但其他条件不同，冲突
            return -1; // ISBN冲突
        }
    }

    // ISBN不存在，直接添加新书
    books.push_back(book);
    writeBook();
    return 0; // 成功新增
}

/**
 * @brief 根据ISBN删除图书并保存到文件
 * @param isbn 图书ISBN编号
 * @param decreaseStock 要减少的库存数量
 * @return 返回值：0=成功删除记录，1=库存已减少，-1=ISBN不存在，-2=存在预约或借出无法删除
 *
 * 如果decreaseStock >= 当前库存，则删除整条记录；
 * 如果decreaseStock < 当前库存，则减少库存；
 * 如果存在预约或借出，则无法删除。
 */
int DataManager::deleteBook(const QString &isbn, int decreaseStock)
{
    for (auto it = books.begin(); it != books.end(); ++it)
    {
        if (it->getISBN() == isbn)
        {
            // 检查是否有预约或借出
            if (it->getReservationCount() > 0 || it->getCurrentBorrowed() > 0)
            {
                return -2; // 存在预约或借出，无法删除
            }

            int currentStock = it->getStock();
            if (decreaseStock >= currentStock)
            {
                // 删除整条记录
                books.erase(it);
                writeBook();
                return 0; // 成功删除记录
            }
            else
            {
                // 减少库存
                it->setStock(currentStock - decreaseStock);
                writeBook();
                return 1; // 库存已减少
            }
        }
    }
    return -1; // ISBN不存在
}

/**
 * @brief 根据ISBN修改图书信息并保存到文件
 * @param oldIsbn 原ISBN编号
 * @param newBook 新的图书信息
 * @return 返回值：0=成功修改，-1=原ISBN不存在，-2=存在预约或借出无法修改，-3=新ISBN已存在
 *
 * 如果存在预约或借出，则无法修改；
 * 如果新ISBN与原ISBN不同且新ISBN已存在，则返回错误；
 * 修改后借阅次数、当前借出、预约人数都重置为0。
 */
int DataManager::updateBook(const QString &oldIsbn, const Book &newBook)
{
    for (auto it = books.begin(); it != books.end(); ++it)
    {
        if (it->getISBN() == oldIsbn)
        {
            // 检查是否有预约或借出
            if (it->getReservationCount() > 0 || it->getCurrentBorrowed() > 0)
            {
                return -2; // 存在预约或借出，无法修改
            }

            // 检查新ISBN是否已存在（如果新ISBN与原ISBN不同）
            if (newBook.getISBN() != oldIsbn)
            {
                Book *existingBook = findBookByISBN(newBook.getISBN());
                if (existingBook != nullptr)
                {
                    return -3; // 新ISBN已存在
                }
            }

            // 删除原记录
            books.erase(it);

            // 添加新记录（借阅次数、当前借出、预约人数都为0）
            Book bookToAdd(newBook.getISBN(), newBook.getTitle(), newBook.getAuthor(),
                           newBook.getCategory(), newBook.getStock(),
                           newBook.getInStockTime(), 0, 0, 0);
            books.push_back(bookToAdd);

            writeBook();
            return 0; // 成功修改
        }
    }
    return -1; // 原ISBN不存在
}

/**
 * @brief 获取所有图书
 * @return 图书列表引用
 */
std::vector<Book> &DataManager::getBooks()
{
    return books;
}

/**
 * @brief 获取图书数量
 * @return 图书总数
 */
int DataManager::getBookCount() const
{
    return books.size();
}

/**
 * @brief 按借阅次数排序（降序）
 * @return 排序后的图书列表
 *
 * 借阅次数多的图书排在前面。
 */
std::vector<Book> DataManager::sortBooksByBorrowCount()
{
    std::vector<Book> sorted = books;
    std::sort(sorted.begin(), sorted.end(), [](const Book &a, const Book &b)
              { return a.getBorrowCount() > b.getBorrowCount(); });
    return sorted;
}

/**
 * @brief 按入库时间排序（降序，最新优先）
 * @return 排序后的图书列表
 *
 * 最新入库的图书排在前面。
 */
std::vector<Book> DataManager::sortBooksByInStockTime()
{
    std::vector<Book> sorted = books;
    std::sort(sorted.begin(), sorted.end(), [](const Book &a, const Book &b)
              { return a.getInStockTime() > b.getInStockTime(); });
    return sorted;
}

/**
 * @brief 初始化读取借阅记录数据
 *
 * 从borrow_records.txt文件中读取借阅记录，包括ISBN、读者ID、借阅时间、
 * 应还时间、归还时间、归还状态、罚款金额、已支付罚款、罚款状态、
 * 续借状态、已扣信用分数和每日罚款金额。
 * 兼容旧数据格式：缺少finePerDay字段时默认1.0元/天。
 */
void DataManager::initBorrowRecord()
{
    QFile file(borrowRecordFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList fields = line.split("|");
        if (fields.size() < 4)
        {
            continue;
        }

        QString isbn = fields[0];
        QString readerId = fields[1];
        QDateTime borrowTime = QDateTime::fromString(fields[2], "yyyy-MM-dd HH:mm:ss");
        QDateTime dueTime = QDateTime::fromString(fields[3], "yyyy-MM-dd HH:mm:ss");

        // 读取每日罚款金额（最后一个字段，兼容旧数据默认1.0）
        double finePerDay = 1.0;
        if (fields.size() >= 12 && !fields[11].isEmpty())
        {
            finePerDay = fields[11].toDouble();
        }

        BorrowRecord record(isbn, readerId, borrowTime, dueTime, finePerDay);

        if (fields.size() >= 5 && !fields[4].isEmpty())
        {
            QDateTime returnTime = QDateTime::fromString(fields[4], "yyyy-MM-dd HH:mm:ss");
            record.setReturnTime(returnTime);
        }

        if (fields.size() >= 6)
        {
            record.setReturned(fields[5] == "true");
        }

        if (fields.size() >= 7)
        {
            record.setRenewStatus(static_cast<BorrowRecord::RenewStatus>(fields[6].toInt()));
        }

        if (fields.size() >= 8)
        {
            record.setFineAmount(fields[7].toDouble());
        }

        if (fields.size() >= 9)
        {
            record.setPaidFine(fields[8].toDouble());
        }

        if (fields.size() >= 10)
        {
            record.setFineStatus(static_cast<BorrowRecord::FineStatus>(fields[9].toInt()));
        }

        if (fields.size() >= 11)
        {
            record.setDeductedScore(fields[10].toInt());
        }

        borrowRecords.push_back(record);
    }

    file.close();
}

/**
 * @brief 写入借阅记录数据到文件
 *
 * 将所有借阅记录写入borrow_records.txt文件，包括ISBN、读者ID、借阅时间、
 * 应还时间、归还时间、归还状态、续借状态、罚款金额、已支付罚款、
 * 罚款状态、已扣信用分数和每日罚款金额。
 */
void DataManager::writeBorrowRecord()
{
    QFile file(borrowRecordFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }

    QTextStream out(&file);
    for (auto &record : borrowRecords)
    {
        QString returnTimeStr = record.getReturnTime().isValid() ? record.getReturnTime().toString("yyyy-MM-dd HH:mm:ss") : "";

        QString line = record.getISBN() + "|" +
                       record.getReaderID() + "|" +
                       record.getBorrowTime().toString("yyyy-MM-dd HH:mm:ss") + "|" +
                       record.getDueTime().toString("yyyy-MM-dd HH:mm:ss") + "|" +
                       returnTimeStr + "|" +
                       (record.isReturned() ? "true" : "false") + "|" +
                       QString::number(static_cast<int>(record.getRenewStatus())) + "|" +
                       QString::number(record.getFineAmount(), 'f', 2) + "|" +
                       QString::number(record.getPaidFine(), 'f', 2) + "|" +
                       QString::number(static_cast<int>(record.getFineStatus())) + "|" +
                       QString::number(record.getDeductedScore()) + "|" +
                       QString::number(record.getFinePerDay(), 'f', 2);
        out << line << "\n";
    }

    file.close();
}

/**
 * @brief 添加借阅记录
 * @param record 借阅记录
 * @return 成功返回true
 *
 * 添加借阅记录后，自动更新图书的当前借出数量和借阅次数。
 */
bool DataManager::addBorrowRecord(const BorrowRecord &record)
{
    borrowRecords.push_back(record);

    Book *book = findBookByISBN(record.getISBN());
    if (book)
    {
        book->setCurrentBorrowed(book->getCurrentBorrowed() + 1);
        book->setBorrowCount(book->getBorrowCount() + 1);
        writeBook();
    }

    writeBorrowRecord();
    return true;
}

/**
 * @brief 续借图书
 * @param isbn 图书ISBN编号
 * @param readerId 读者ID
 * @param days 续借天数
 * @return 成功返回true，失败返回false
 *
 * 找到匹配的未归还借阅记录，延长应还时间。
 */
bool DataManager::renewBorrowRecord(const QString &isbn, const QString &readerId, int days)
{
    for (auto &record : borrowRecords)
    {
        if (record.getISBN() == isbn &&
            record.getReaderID() == readerId &&
            !record.isReturned())
        {
            QDateTime newDueTime = record.getDueTime().addDays(days);
            record.setDueTime(newDueTime);

            writeBorrowRecord();
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取某读者的借阅记录
 * @param readerId 读者ID
 * @return 该读者的所有借阅记录
 */
std::vector<BorrowRecord> DataManager::getBorrowRecordsByReader(const QString &readerId)
{
    std::vector<BorrowRecord> result;
    for (auto &record : borrowRecords)
    {
        if (record.getReaderID() == readerId)
        {
            result.push_back(record);
        }
    }
    return result;
}

/**
 * @brief 获取读者当前借阅数量
 * @param readerId 读者ID
 * @return 当前未归还的借阅数量
 */
int DataManager::getBorrowCountByReader(const QString &readerId)
{
    int count = 0;
    for (auto &record : borrowRecords)
    {
        if (record.getReaderID() == readerId && !record.isReturned())
        {
            count++;
        }
    }
    return count;
}

/**
 * @brief 检查读者是否有逾期未还
 * @param readerId 读者ID
 * @return 有逾期返回true，否则返回false
 */
bool DataManager::hasOverdueBooks(const QString &readerId)
{
    QDateTime now = QDateTime::currentDateTime();
    for (auto &record : borrowRecords)
    {
        if (record.getReaderID() == readerId &&
            !record.isReturned() &&
            now > record.getDueTime())
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取所有借阅记录
 * @return 借阅记录列表引用
 */
std::vector<BorrowRecord> &DataManager::getBorrowRecords()
{
    return borrowRecords;
}

/**
 * @brief 初始化读取预约数据
 *
 * 从reservations.txt文件中读取预约记录，包括ISBN、读者ID、预约时间和状态。
 * 读取完成后，重新计算每本书的预约人数。
 */
void DataManager::initReservation()
{
    // 修改5.16
    QFile file(reservationFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList fields = line.split("|", Qt::SkipEmptyParts);
        if (fields.size() < 3)
        {
            continue;
        }

        QString isbn = fields[0];
        QString readerId = fields[1];
        QDateTime reserveTime = QDateTime::fromString(fields[2], "yyyy-MM-dd HH:mm:ss");
        Reservation::Status status = (fields.size() >= 4) ? static_cast<Reservation::Status>(fields[3].toInt()) : Reservation::PENDING;

        reservations.push_back(Reservation(isbn, readerId, reserveTime, status));
    }

    file.close();

    for (auto &book : books)
    {
        book.setReservationCount(0);
    }

    for (const auto &reservation : reservations)
    {
        if (reservation.getStatus() == Reservation::PENDING ||
            reservation.getStatus() == Reservation::APPROVED ||
            reservation.getStatus() == Reservation::REJECTED)
        {
            Book *book = findBookByISBN(reservation.getISBN());
            if (book)
            {
                book->setReservationCount(book->getReservationCount() + 1);
            }
        }
    }
    writeBook();
}

/**
 * @brief 写入预约数据到文件
 *
 * 将所有预约记录写入reservations.txt文件，包括ISBN、读者ID、预约时间和状态。
 */
void DataManager::writeReservation()
{
    QFile file(reservationFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        return;
    }

    QTextStream out(&file);
    for (auto &reservation : reservations)
    {
        QString line = QString("%1|%2|%3|%4")
                           .arg(reservation.getISBN())
                           .arg(reservation.getReaderID())
                           .arg(reservation.getReserveTime().toString("yyyy-MM-dd HH:mm:ss"))
                           .arg(reservation.getStatus());
        out << line << "\n";
    }

    file.close();
}

/**
 * @brief 添加预约记录
 * @param reservation 预约记录
 * @return 成功返回true，已存在返回false
 *
 * 添加预约记录后，自动更新图书的预约人数。
 */
bool DataManager::addReservation(const Reservation &reservation)
{
    if (hasReservation(reservation.getISBN(), reservation.getReaderID()))
    {
        return false;
    }

    reservations.push_back(reservation);

    Book *book = findBookByISBN(reservation.getISBN());
    if (book)
    {
        book->setReservationCount(book->getReservationCount() + 1);
        writeBook();
    }

    writeReservation();
    return true;
}

/**
 * @brief 取消预约记录（按ISBN和读者ID，直接删除记录）
 * @param isbn 图书ISBN编号
 * @param readerId 读者ID
 * @return 成功返回true，失败返回false
 *
 * 只能取消待审核状态的预约，取消后自动更新图书的预约人数。
 */
bool DataManager::cancelReservation(const QString &isbn, const QString &readerId)
{
    for (auto it = reservations.begin(); it != reservations.end(); ++it)
    {
        if (it->getISBN() == isbn && it->getReaderID() == readerId && it->getStatus() == Reservation::PENDING)
        {
            Book *book = findBookByISBN(isbn);
            if (book && book->getReservationCount() > 0)
            {
                book->setReservationCount(book->getReservationCount() - 1);
                writeBook();
            }

            reservations.erase(it);
            writeReservation();
            return true;
        }
    }
    return false;
}

/**
 * @brief 删除预约记录（硬删除，从容器中移除）
 * @param isbn 图书ISBN编号
 * @param readerId 读者ID
 * @return 成功返回true，失败返回false
 *
 * 删除任意状态的预约记录，删除后自动更新图书的预约人数。
 */
bool DataManager::removeReservation(const QString &isbn, const QString &readerId)
{
    for (auto it = reservations.begin(); it != reservations.end(); ++it)
    {
        if (it->getISBN() == isbn && it->getReaderID() == readerId)
        {
            Book *book = findBookByISBN(isbn);
            if (book && book->getReservationCount() > 0)
            {
                book->setReservationCount(book->getReservationCount() - 1);
                writeBook();
            }

            reservations.erase(it);
            writeReservation();
            return true;
        }
    }
    return false;
}

/**
 * @brief 获取某图书的所有预约（按预约时间排序）
 * @param isbn 图书ISBN编号
 * @return 该图书的待审核预约列表，按预约时间升序排列
 */
std::vector<Reservation> DataManager::getReservationsByISBN(const QString &isbn)
{
    std::vector<Reservation> result;
    for (auto &reservation : reservations)
    {
        if (reservation.getISBN() == isbn &&
            reservation.getStatus() == Reservation::PENDING)
        {
            result.push_back(reservation);
        }
    }

    std::sort(result.begin(), result.end(), [](const Reservation &a, const Reservation &b)
              { return a.getReserveTime() < b.getReserveTime(); });

    return result;
}

/**
 * @brief 获取某读者的所有预约
 * @param readerId 读者ID
 * @return 该读者的所有预约记录
 */
std::vector<Reservation> DataManager::getReservationsByReader(const QString &readerId)
{
    std::vector<Reservation> result;
    for (auto &reservation : reservations)
    {
        if (reservation.getReaderID() == readerId)
        {
            result.push_back(reservation);
        }
    }

    std::sort(result.begin(), result.end(), [](const Reservation &a, const Reservation &b)
              { return a.getReserveTime() < b.getReserveTime(); });

    return result;
}

/**
 * @brief 获取所有预约记录
 * @return 预约记录列表引用
 */
std::vector<Reservation> &DataManager::getReservations()
{
    return reservations;
}

/**
 * @brief 通知可借的预约（图书入库时）
 * @param isbn 图书ISBN编号
 *
 * 只通知审核成功的预约，发送消息给相关读者。
 */
void DataManager::notifyReservations(const QString &isbn)
{
    Book *book = findBookByISBN(isbn);
    if (!book)
        return;

    // 只获取审核成功的预约，不是待审核
    std::vector<Reservation> approvedReservations;
    std::vector<Reservation> allReservations = getReservations();
    for (const auto &r : allReservations)
    {
        if (r.getISBN() == isbn && r.getStatus() == Reservation::APPROVED)
        {
            approvedReservations.push_back(r);
        }
    }

    if (approvedReservations.empty())
        return;

    for (const auto &reservation : approvedReservations)
    {
        ::User *user = findUserById(reservation.getReaderID());
        if (user && user->getType() == 2)
        {
            ::Reader *reader = dynamic_cast<::Reader *>(user);
            if (reader)
            {
                QString msgContent = QString("您预约的图书(ISBN:%1)有新的入库或归还，您可以尝试前往借阅。")
                                         .arg(isbn);
                Message msg(reader->getID(), reader->getName(), msgContent);
                reader->addMessage(msg);
                writeUser();
            }
        }
    }

    writeMessage();
}
/**
 * @brief 获取预约数量
 * @return 预约记录总数
 */
int DataManager::getReservationCount() const
{
    return reservations.size();
}

/**
 * @brief 检查读者是否已预约某图书
 * @param isbn 图书ISBN编号
 * @param readerId 读者ID
 * @return 已预约返回true，否则返回false
 *
 * 不包括已取消的预约。
 */
bool DataManager::hasReservation(const QString &isbn, const QString &readerId)
{
    for (auto &reservation : reservations)
    {
        if (reservation.getISBN() == isbn &&
            reservation.getReaderID() == readerId &&
            reservation.getStatus() != Reservation::CANCELLED)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief 初始化读取消息数据
 *
 * 从messages.txt文件中读取消息，根据消息类型（管理员消息/读者消息）
 * 分发给对应的用户。
 */
void DataManager::initMessage()
{
    QFile msgFile(messageFilePath);
    if (msgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream msgIn(&msgFile);
        while (!msgIn.atEnd())
        {
            QString line = msgIn.readLine().trimmed();
            if (line.isEmpty())
                continue;

            Message msg = Message::fromFileString(line);

            if (msg.isAdminMessage())
            {
                // 管理员消息：根据管理员ID和姓名找到管理员
                for (auto user : users)
                {
                    if (user->getType() == 1 && // 管理员类型
                        user->getID() == msg.getAdminId() &&
                        user->getName() == msg.getAdminName())
                    {
                        user->addMessage(msg);
                        break;
                    }
                }
            }
            else
            {
                // 读者消息：根据读者ID和姓名找到读者
                for (auto user : users)
                {
                    if (user->getType() == 2 && // 读者类型
                        user->getID() == msg.getReaderId() &&
                        user->getName() == msg.getReaderName())
                    {
                        user->addMessage(msg);
                        break;
                    }
                }
            }
        }
        msgFile.close();
    }
}

/**
 * @brief 写入消息数据到文件
 *
 * 将所有用户的消息写入messages.txt文件。
 */
void DataManager::writeMessage()
{
    QFile msgFile(messageFilePath);
    if (msgFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream msgOut(&msgFile);
        for (auto user : users)
        {
            std::vector<Message> &messages = user->getMessages();
            for (const auto &msg : messages)
            {
                msgOut << msg.toFileString() << "\n";
            }
        }
        msgFile.close();
    }
}

/**
 * @brief 添加管理员消息
 * @param user 管理员用户对象
 * @param readerId 读者ID
 * @param readerName 读者姓名
 * @param message 消息内容
 *
 * 创建管理员消息并添加到管理员的消息列表中。
 */
void DataManager::addAdminMessage(User *user, const QString &readerId, const QString &readerName, const QString &message)
{
    if (user)
    {
        // 创建管理员消息
        Message msg(user->getID(), user->getName(), readerId, readerName, message);
        user->addMessage(msg);
        writeMessage();
    }
}

/**
 * @brief 添加读者消息
 * @param reader 读者用户对象
 * @param message 消息内容
 *
 * 创建读者消息并添加到读者的消息列表中。
 */
void DataManager::addReaderMessage(User *reader, const QString &message)
{
    if (reader)
    {
        // 创建读者消息
        Message msg(reader->getID(), reader->getName(), message);
        reader->addMessage(msg);
        writeMessage();
    }
}

// （消息发送）：向所有管理员发送消息
void DataManager::sendMessageToAdmin(const QString &content)
{
    std::vector<User *> users = getUsers();
    for (auto user : users)
    {
        if (user->getType() == 1) // 管理员类型
        {
            // 创建消息（系统发给管理员）
            Message msg(user->getID(), user->getName(), "系统", "系统管理员", content);
            user->addMessage(msg);
        }
    }
    writeMessage();
}
