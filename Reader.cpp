#include "Reader.h"
#include "DataManager.h"
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>

// 构造函数
Reader::Reader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : User(I, n, pa, ph, e)
{
    type = 2;
    maxBooks = 10;
    creditScore = 100;
}

// 实现User的纯虚函数：获取用户身份
QString Reader::typeToIdentity()
{
    return QString("读者");
}

// （生成验证码）：生成读者验证码（6位数字）
QString Reader::generateVerificationCode()
{
    QString code;
    for (int i = 0; i < 6; ++i)
    {
        int digit = QRandomGenerator::global()->bounded(10);
        code.append(QString::number(digit));
    }
    return code;
}

// getter和setter方法
int Reader::getMaxBooks() { return maxBooks; }
int Reader::getCreditScore() { return creditScore; }

void Reader::setMaxBooks(int max) { maxBooks = max; }
void Reader::setCreditScore(int score) { creditScore = score; }

// 预约图书（按ISBN）
Reader::ReserveResult Reader::reserveBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    Book *book = dm->findBookByISBN(isbn);
    if (!book)
    {
        return ReserveResult::BOOK_NOT_FOUND;
    }

    if (dm->hasReservation(isbn, ID))
    {
        return ReserveResult::ALREADY_RESERVED;
    }

    int available = book->getStock() - book->getCurrentBorrowed();
    int currentReservationCount = dm->getReservationsByISBN(isbn).size();

    // 有库存时，预约量不超过剩余借书量的2倍
    if (available > 0 && currentReservationCount >= available * 2)
    {
        return ReserveResult::EXCEED_LIMIT;
    }

    QDateTime now = QDateTime::currentDateTime();
    Reservation reservation(isbn, ID, now, Reservation::PENDING);
    if (dm->addReservation(reservation))
    {
        // 发送消息给读者自己
        QString messageContent = QString("您已成功预约图书(ISBN:%1)，请等待管理员审核。").arg(isbn);
        Message message(ID, name, messageContent);
        this->addMessage(message);
        dm->writeMessage();

        return ReserveResult::SUCCESS;
    }
    return ReserveResult::BOOK_NOT_FOUND;
}

// 取消预约（按ISBN）
bool Reader::cancelReservation(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 调用 DataManager 的取消预约方法（直接删除记录）
    bool success = dm->cancelReservation(isbn, ID);
    if (success)
    {
        // 发送消息给读者自己（不给管理员发）
        QString messageContent = QString("您已取消预约图书(ISBN:%1)").arg(isbn);
        Message message(ID, name, messageContent);
        this->addMessage(message);
        dm->writeMessage();
    }
    return success;
}

// 查看我的预约
std::vector<Reservation> Reader::viewMyReservations()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getReservationsByReader(ID);
}

// 借书（按ISBN），返回借书结果
Reader::BorrowResult Reader::borrowBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    Book *book = dm->findBookByISBN(isbn);
    if (!book)
        return BorrowResult::BOOK_NOT_FOUND;

    int available = book->getStock() - book->getCurrentBorrowed();
    if (available <= 0)
        return BorrowResult::NO_STOCK;

    std::vector<BorrowRecord> myRecords = dm->getBorrowRecordsByReader(ID);
    for (const auto &record : myRecords)
    {
        if (record.getISBN() == isbn && !record.isReturned())
        {
            return BorrowResult::ALREADY_BORROWED;
        }
    }

    if (dm->hasOverdueBooks(ID))
        return BorrowResult::HAS_OVERDUE;

    int currentBorrowCount = dm->getBorrowCountByReader(ID);
    if (currentBorrowCount >= maxBooks)
        return BorrowResult::EXCEED_LIMIT;

    std::vector<Reservation> &allReservations = dm->getReservations();
    bool hasValidReservation = false;
    int reservationIndex = -1;

    for (int i = 0; i < allReservations.size(); ++i)
    {
        const auto &r = allReservations[i];
        if (r.getISBN() == isbn && r.getReaderID() == ID &&
            r.getStatus() == Reservation::APPROVED)
        {
            hasValidReservation = true;
            reservationIndex = i;
            break;
        }
    }

    if (!hasValidReservation)
    {
        return BorrowResult::NO_VALID_RESERVATION;
    }

    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueTime = now.addDays(30);

    BorrowRecord record(isbn, ID, now, dueTime);
    bool success = dm->addBorrowRecord(record);

    if (success)
    {
        if (reservationIndex >= 0)
        {
            allReservations.erase(allReservations.begin() + reservationIndex);
            if (book->getReservationCount() > 0)
            {
                book->setReservationCount(book->getReservationCount() - 1);
            }
            dm->writeBook();
            dm->writeReservation();
        }

        QString messageContent = QString("读者 %1 借阅了图书《%2》(ISBN:%3)，借阅时间：%4，应还时间：%5")
                                     .arg(ID)
                                     .arg(book->getTitle())
                                     .arg(isbn)
                                     .arg(now.toString("yyyy-MM-dd HH:mm:ss"))
                                     .arg(dueTime.toString("yyyy-MM-dd HH:mm:ss"));

        std::vector<User *> &users = dm->getUsers();
        for (auto user : users)
        {
            if (user->getType() == 1)
            {
                Message msg(ID, getName(), messageContent);
                user->addMessage(msg);
            }
        }

        QString myMsgContent = QString("您已成功借阅图书《%1》(ISBN:%2)，借阅时间：%3，应还时间：%4")
                                   .arg(book->getTitle())
                                   .arg(isbn)
                                   .arg(now.toString("yyyy-MM-dd HH:mm:ss"))
                                   .arg(dueTime.toString("yyyy-MM-dd HH:mm:ss"));
        Message myMsg("系统", "系统管理员", myMsgContent);
        addMessage(myMsg);

        dm->writeMessage();

        return BorrowResult::SUCCESS;
    }

    return BorrowResult::BOOK_NOT_FOUND;
}

// 还书（按ISBN）
bool Reader::returnBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();
    bool success = dm->updateBorrowRecord(isbn, ID);
    return success;
}

// 续借（按ISBN）
bool Reader::renewBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();
    return dm->renewBorrowRecord(isbn, ID, 30);
}

// 查看我的借阅记录
std::vector<BorrowRecord> Reader::viewMyBorrowRecords()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getBorrowRecordsByReader(ID);
}

// 查找图书（模糊搜索）
std::vector<const Book *> Reader::findBook(const QString &isbn, const QString &title,
                                           const QString &author, const QString &category)
{
    DataManager *dm = DataManager::getInstance();
    return dm->searchBooks(isbn, title, author, category);
}

// 析构函数
Reader::~Reader()
{
}