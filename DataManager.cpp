#include "DataManager.h"
#include "Admin.h"
#include "Reader.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <algorithm>

// （单例静态变量）：DataManager单例对象的静态指针
DataManager *DataManager::instance = nullptr;

// （单例获取）：获取DataManager单例对象
DataManager *DataManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new DataManager();
    }
    return instance;
}

// （私有构造）：单例模式私有构造函数
DataManager::DataManager()
{
    // （初始化文件路径）：设置数据文件保存路径
    // 优先使用应用程序目录下的data子目录
    QString appDir = QCoreApplication::applicationDirPath();
    qDebug() << "[DataManager] 应用程序目录:" << appDir;
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

    qDebug() << "[DataManager] 数据目录:" << dataDir;
    qDebug() << "[DataManager] 用户文件路径:" << userFilePath;

    userFilePath = dataDir + "/users.txt";
    bookFilePath = dataDir + "/books.txt";
    borrowRecordFilePath = dataDir + "/borrow_records.txt";
    reservationFilePath = dataDir + "/reservations.txt";

    qDebug() << "[DataManager] 用户文件路径:" << userFilePath;
    qDebug() << "[DataManager] 文件是否存在:" << QFile::exists(userFilePath);

    // 检查并创建数据目录
    QDir dir(dataDir);
    if (!dir.exists())
    {
        dir.mkpath(dataDir);
    }

    // （初始化数据）：读取所有数据文件
    initUser();
    qDebug() << "[DataManager] 加载用户数量:" << users.size();
    initBook();
    initBorrowRecord();
    initReservation();
}

// （析构函数）：DataManager析构函数，自动保存数据
DataManager::~DataManager()
{
    // （自动保存）：析构时自动保存所有数据
    writeUser();
    writeBook();
    writeBorrowRecord();
    writeReservation();

    // （清理资源）：释放users中动态分配的内存
    for (auto user : users)
    {
        delete user;
    }
}

// （用户查询）：根据ID查找用户（模糊匹配，可能多个）
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

// （用户查询）：根据姓名查找用户（模糊匹配，可能多个）
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

// （用户查询）：根据ID精确查找用户（返回单个）
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

// （用户查询）：根据ID和姓名同时查找用户（均为模糊匹配，可能多个）
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

// （用户添加）：添加用户并保存到文件
void DataManager::addUser(::User *user)
{
    users.push_back(user);
    writeUser();
}

// （用户删除）：根据ID和姓名同时删除用户并保存到文件（两个关键字都需要匹配）
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

// （用户修改）：根据ID和姓名同时修改用户信息并保存到文件（两个关键字都需要匹配）
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

// （用户清除）：清除所有用户并保存到文件
void DataManager::clearAllUsers()
{
    for (auto user : users)
    {
        delete user;
    }
    users.clear();
    writeUser();
}

// （用户获取）：获取所有用户
std::vector<::User *> &DataManager::getUsers()
{
    return users;
}

// （用户数量）：获取用户数量
int DataManager::getUserCount() const
{
    return users.size();
}

// （用户管理）：初始化读取用户数据
void DataManager::initUser()
{
    // 处理用户自身属性users.txt
    QFile file(userFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // qDebug() << "无法打开用户数据文件: " << userFilePath;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList fields = line.split(" ", Qt::SkipEmptyParts);
        if (fields.size() < 6)
        {
            // qDebug() << "用户数据格式错误: " << line;
            continue;
        }

        QString id = fields[0];
        int type = fields[1].toInt();
        QString name = fields[2];
        QString password = fields[3];
        QString phone = fields[4];
        QString email = fields[5];

        ::User *user = nullptr;
        if (type == 1)
        {
            user = new ::Admin(id, name, password, phone, email);
        }
        else
        {
            user = new ::Reader(id, name, password, phone, email);
        }

        users.push_back(user);
    }

    file.close();

    // 处理消息messages.txt
    QFileInfo userFileInfo(userFilePath);
    QString msgFilePath = userFileInfo.absolutePath() + "/messages.txt";
    QFile msgFile(msgFilePath);
    if (msgFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream msgIn(&msgFile);
        while (!msgIn.atEnd())
        {
            QString line = msgIn.readLine().trimmed();
            if (line.isEmpty())
                continue;
            QStringList parts = line.split("|", Qt::KeepEmptyParts);
            if (parts.size() < 2)
                continue;
            QString readerId = parts[0];
            ::User *u = findUserById(readerId);
            if (u && u->getType() == 2)
            {
                ::Reader *reader = dynamic_cast<::Reader *>(u);
                if (reader)
                {
                    for (int i = 1; i < parts.size(); i++)
                    {
                        reader->addMsg(parts[i]);
                    }
                }
            }
        }
        msgFile.close();
    }
}

// （用户管理）：写入用户数据到文件
void DataManager::writeUser()
{
    QFile file(userFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        // qDebug() << "无法打开用户数据文件: " << userFilePath;
        return;
    }

    QTextStream out(&file);
    for (auto user : users)
    {
        QString line = QString("%1 %2 %3 %4 %5 %6")
                           .arg(user->getID())
                           .arg(user->getType())
                           .arg(user->getName())
                           .arg(user->getPassword())
                           .arg(user->getPhone())
                           .arg(user->getEmail());
        out << line << "\n";
    }

    file.close();

    QFileInfo userFileInfo2(userFilePath);
    QString msgFilePath2 = userFileInfo2.absolutePath() + "/messages.txt";
    QFile msgFile(msgFilePath2);
    if (msgFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QTextStream msgOut(&msgFile);
        for (auto user : users)
        {
            if (user->getType() == 2)
            {
                ::Reader *reader = dynamic_cast<::Reader *>(user);
                if (reader)
                {
                    std::vector<QString> &msgs = reader->getMsg();
                    if (!msgs.empty())
                    {
                        msgOut << reader->getID();
                        for (const auto &m : msgs)
                        {
                            msgOut << "|" << m;
                        }
                        msgOut << "\n";
                    }
                }
            }
        }
        msgFile.close();
    }
}

// 修改5.16
//  （图书管理）：初始化读取图书数据
void DataManager::initBook()
{
    QFile file(bookFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // qDebug() << "无法打开图书数据文件: " << bookFilePath;
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
            // qDebug() << "图书数据格式错误: " << line;
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
    // qDebug() << "图书数据初始化完成，共 " << books.size() << " 本图书";
}

// （图书管理）：写入图书数据到文件
void DataManager::writeBook()
{
    QFile file(bookFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        // qDebug() << "无法打开图书数据文件: " << bookFilePath;
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
    // qDebug() << "图书数据保存完成，共 " << books.size() << " 本图书";
}

// （图书查询）：根据ISBN查找图书（精确匹配）
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

// （图书查询）：根据书名查找图书（模糊匹配）
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

// （图书查询）：根据作者查找图书（模糊匹配）
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

// （图书查询）：根据分类查找图书（模糊匹配）
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

// （图书查询）：多条件搜索图书
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

// （图书添加）：添加图书并保存到文件
// 返回值：0=成功新增，1=库存已增加，-1=ISBN冲突（其他信息不匹配）
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
            existingBook->setStock(existingBook->getStock() + book.getStock());
            existingBook->setInStockTime(QDateTime::currentDateTime());
            writeBook();
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

// （图书删除）：根据ISBN删除图书并保存到文件
bool DataManager::deleteBook(const QString &isbn)
{
    for (auto it = books.begin(); it != books.end(); ++it)
    {
        if (it->getISBN() == isbn)
        {
            books.erase(it);
            writeBook();
            return true;
        }
    }
    return false;
}

// （图书修改）：根据ISBN修改图书信息并保存到文件
bool DataManager::updateBook(const QString &isbn, const Book &newBook)
{
    for (auto &book : books)
    {
        if (book.getISBN() == isbn)
        {
            book.setTitle(newBook.getTitle());
            book.setAuthor(newBook.getAuthor());
            book.setCategory(newBook.getCategory());
            book.setStock(newBook.getStock());
            book.setInStockTime(newBook.getInStockTime());
            book.setBorrowCount(newBook.getBorrowCount());
            book.setCurrentBorrowed(newBook.getCurrentBorrowed());
            writeBook();
            return true;
        }
    }
    return false;
}

// （图书获取）：获取所有图书
std::vector<Book> &DataManager::getBooks()
{
    return books;
}
// 修改结束

// （图书数量）：获取图书数量
int DataManager::getBookCount() const
{
    return books.size();
}

// （图书排序）：按借阅次数排序（降序）
std::vector<Book> DataManager::sortBooksByBorrowCount()
{
    std::vector<Book> sorted = books;
    std::sort(sorted.begin(), sorted.end(), [](const Book &a, const Book &b)
              { return a.getBorrowCount() > b.getBorrowCount(); });
    return sorted;
}

// （图书排序）：按入库时间排序（降序，最新优先）
std::vector<Book> DataManager::sortBooksByInStockTime()
{
    std::vector<Book> sorted = books;
    std::sort(sorted.begin(), sorted.end(), [](const Book &a, const Book &b)
              { return a.getInStockTime() > b.getInStockTime(); });
    return sorted;
}

// （借阅记录管理）：初始化读取借阅记录数据
void DataManager::initBorrowRecord()
{
    QFile file(borrowRecordFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // qDebug() << "无法打开借阅记录文件: " << borrowRecordFilePath;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        QStringList fields = line.split("|", Qt::SkipEmptyParts);
        if (fields.size() < 4)
        {
            // qDebug() << "借阅记录格式错误: " << line;
            continue;
        }

        QString isbn = fields[0];
        QString readerId = fields[1];
        QDateTime borrowTime = QDateTime::fromString(fields[2], "yyyy-MM-dd HH:mm:ss");
        QDateTime dueTime = QDateTime::fromString(fields[3], "yyyy-MM-dd HH:mm:ss");

        BorrowRecord record(isbn, readerId, borrowTime, dueTime);

        if (fields.size() >= 5 && !fields[4].isEmpty())
        {
            QDateTime returnTime = QDateTime::fromString(fields[4], "yyyy-MM-dd HH:mm:ss");
            record.setReturnTime(returnTime);
        }

        if (fields.size() >= 6)
        {
            record.setReturned(fields[5] == "true");
        }

        borrowRecords.push_back(record);
    }

    file.close();
    // qDebug() << "借阅记录初始化完成，共 " << borrowRecords.size() << " 条记录";
}

// （借阅记录管理）：写入借阅记录数据到文件
void DataManager::writeBorrowRecord()
{
    // 修改5.16
    QFile file(borrowRecordFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        // qDebug() << "无法打开借阅记录文件: " << borrowRecordFilePath;
        return;
    }

    QTextStream out(&file);
    for (auto &record : borrowRecords)
    {
        QString returnTimeStr = record.getReturnTime().isValid() ? record.getReturnTime().toString("yyyy-MM-dd HH:mm:ss") : "";
        QString line = QString("%1|%2|%3|%4|%5|%6")
                           .arg(record.getISBN())
                           .arg(record.getReaderID())
                           .arg(record.getBorrowTime().toString("yyyy-MM-dd HH:mm:ss"))
                           .arg(record.getDueTime().toString("yyyy-MM-dd HH:mm:ss"))
                           .arg(returnTimeStr)
                           .arg(record.isReturned() ? "true" : "false");
        out << line << "\n";
    }

    file.close();
    // qDebug() << "借阅记录保存完成，共 " << borrowRecords.size() << " 条记录";
}

// （借阅记录管理）：添加借阅记录
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

// （借阅记录管理）：更新借阅记录（还书）
bool DataManager::updateBorrowRecord(const QString &isbn, const QString &readerId)
{
    for (auto &record : borrowRecords)
    {
        if (record.getISBN() == isbn &&
            record.getReaderID() == readerId &&
            !record.isReturned())
        {
            record.setReturned(true);

            Book *book = findBookByISBN(isbn);

            // 检查是否逾期，逾期扣减信用分
            int overdueDays = record.calculateOverdueDays();
            if (overdueDays > 0)
            {
                ::User *user = findUserById(readerId);
                if (user && user->getType() == 2) // 读者类型
                {
                    ::Reader *reader = dynamic_cast<::Reader *>(user);
                    if (reader)
                    {
                        // 逾期1天扣1分，最多扣10分
                        int scoreDeduction = qMin(overdueDays, 10);
                        int newScore = reader->getCreditScore() - scoreDeduction;
                        reader->setCreditScore(qMax(newScore, 0)); // 最低0分

                        // 发送消息通知
                        QString bookTitle = book ? book->getTitle() : "未知书名";
                        QString msg = QString("图书《%1》(ISBN:%2)逾期%3天归还，信用分扣减%4分，当前信用分：%5")
                                          .arg(bookTitle)
                                          .arg(isbn)
                                          .arg(overdueDays)
                                          .arg(scoreDeduction)
                                          .arg(reader->getCreditScore());
                        reader->addMsg(msg);

                        writeUser();
                    }
                }
            }
            if (book)
            {
                book->setCurrentBorrowed(book->getCurrentBorrowed() - 1);
                writeBook();
            }

            writeBorrowRecord();
            return true;
        }
    }
    return false;
}

// （借阅记录管理）：续借图书
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

// （借阅记录管理）：获取某读者的借阅记录
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

// （借阅记录管理）：获取某图书的借阅记录
std::vector<BorrowRecord> DataManager::getBorrowRecordsByISBN(const QString &isbn)
{
    std::vector<BorrowRecord> result;
    for (auto &record : borrowRecords)
    {
        if (record.getISBN() == isbn)
        {
            result.push_back(record);
        }
    }
    return result;
}

// （借阅记录管理）：获取所有未归还的借阅记录
std::vector<BorrowRecord> DataManager::getBorrowingRecords()
{
    std::vector<BorrowRecord> result;
    for (auto &record : borrowRecords)
    {
        if (!record.isReturned())
        {
            result.push_back(record);
        }
    }
    return result;
}

// （借阅记录管理）：获取读者当前借阅数量
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

// （借阅记录管理）：检查读者是否有逾期未还
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

// （借阅记录管理）：获取所有借阅记录
std::vector<BorrowRecord> &DataManager::getBorrowRecords()
{
    return borrowRecords;
}
// 修改结束

// （预约管理）：初始化读取预约数据
void DataManager::initReservation()
{
    // 修改5.16
    QFile file(reservationFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // qDebug() << "无法打开预约数据文件: " << reservationFilePath;
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
            // qDebug() << "预约数据格式错误: " << line;
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
            reservation.getStatus() == Reservation::NOTIFIED)
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

void DataManager::writeReservation()
{
    QFile file(reservationFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        // qDebug() << "无法打开预约数据文件: " << reservationFilePath;
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
    // qDebug() << "预约数据保存完成，共 " << reservations.size() << " 条预约";
}

// （预约管理）：添加预约记录
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

// （预约管理）：取消预约记录（按ISBN和读者ID）
bool DataManager::cancelReservation(const QString &isbn, const QString &readerId)
{
    for (auto it = reservations.begin(); it != reservations.end(); ++it)
    {
        if (it->getISBN() == isbn && it->getReaderID() == readerId && it->getStatus() != Reservation::CANCELLED && it->getStatus() != Reservation::COMPLETED)
        {
            it->setStatus(Reservation::CANCELLED);

            Book *book = findBookByISBN(isbn);
            if (book && book->getReservationCount() > 0)
            {
                book->setReservationCount(book->getReservationCount() - 1);
                writeBook();
            }

            writeReservation();
            return true;
        }
    }
    return false;
}

// （预约管理）：获取某图书的所有预约（按预约时间排序）
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

// （预约管理）：获取某读者的所有预约
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

// （预约管理）：获取所有预约记录
std::vector<Reservation> &DataManager::getReservations()
{
    return reservations;
}

// （预约管理）：通知可借的预约（图书入库时）
void DataManager::notifyReservations(const QString &isbn)
{
    Book *book = findBookByISBN(isbn);
    if (!book)
        return;

    std::vector<Reservation> pendingReservations = getReservationsByISBN(isbn);
    if (pendingReservations.empty())
        return;

    for (auto &reservation : pendingReservations)
    {
        if (book->getStock() - book->getCurrentBorrowed() > 0)
        {
            for (auto &r : reservations)
            {
                if (r.getISBN() == isbn && r.getReaderID() == reservation.getReaderID())
                {
                    r.setStatus(Reservation::NOTIFIED);
                    break;
                }
            }

            ::User *user = findUserById(reservation.getReaderID());
            if (user && user->getType() == 2)
            {
                ::Reader *reader = dynamic_cast<::Reader *>(user);
                if (reader)
                {
                    QString msg = QString("您预约的图书《%1》(ISBN:%2)现已可借，请尽快前往借阅。")
                                      .arg(book->getTitle())
                                      .arg(isbn);
                    reader->addMsg(msg);
                }
            }
        }
        else
        {
            break;
        }
    }

    writeReservation();
    writeBook();
    writeUser();
}
int DataManager::getReservationCount() const
{
    return reservations.size();
}

// （预约管理）：检查读者是否已预约某图书
bool DataManager::hasReservation(const QString &isbn, const QString &readerId)
{
    for (auto &reservation : reservations)
    {
        if (reservation.getISBN() == isbn &&
            reservation.getReaderID() == readerId &&
            reservation.getStatus() != Reservation::CANCELLED &&
            reservation.getStatus() != Reservation::COMPLETED)
        {
            return true;
        }
    }
    return false;
}