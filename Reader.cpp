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
        return ReserveResult::ALREADY_EXISTS;
    }

    std::vector<BorrowRecord> &allRecords = dm->getBorrowRecords();
    for (const auto &record : allRecords)
    {
        if (record.getISBN() == isbn && record.getReaderID() == ID && !record.isReturned())
        {
            return ReserveResult::ALREADY_EXISTS;
        }
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

        QString myMsgContent = QString("您已成功借阅图书《%1》(ISBN:%2)，借阅时间：%3，应还时间：%4")
                                   .arg(book->getTitle())
                                   .arg(isbn)
                                   .arg(now.toString("yyyy-MM-dd HH:mm:ss"))
                                   .arg(dueTime.toString("yyyy-MM-dd HH:mm:ss"));
        Message myMsg(ID, getName(), myMsgContent);
        addMessage(myMsg);

        dm->writeMessage();

        return BorrowResult::SUCCESS;
    }

    return BorrowResult::BOOK_NOT_FOUND;
}

// 还书（按ISBN），返回还书结果
Reader::ReturnResult Reader::returnBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    std::vector<BorrowRecord> &allRecords = dm->getBorrowRecords();
    BorrowRecord *targetRecord = nullptr;
    int recordIndex = -1;

    for (int i = 0; i < allRecords.size(); ++i)
    {
        if (allRecords[i].getISBN() == isbn && allRecords[i].getReaderID() == ID && !allRecords[i].isReturned())
        {
            targetRecord = &allRecords[i];
            recordIndex = i;
            break;
        }
    }

    if (!targetRecord)
    {
        return ReturnResult::NOT_FOUND;
    }

    double fineAmount = targetRecord->calculateFine();
    double paidFine = targetRecord->getPaidFine();

    if (fineAmount > paidFine)
    {
        return ReturnResult::HAS_UNPAID_FINE;
    }

    QDateTime now = QDateTime::currentDateTime();
    Book *book = dm->findBookByISBN(isbn);

    allRecords.erase(allRecords.begin() + recordIndex);
    dm->writeBorrowRecord();

    if (book)
    {
        book->setCurrentBorrowed(book->getCurrentBorrowed() - 1);
        dm->writeBook();

        std::vector<Reservation> &allReservations = dm->getReservations();
        for (auto &reservation : allReservations)
        {
            if (reservation.getISBN() == isbn && reservation.getStatus() == Reservation::APPROVED)
            {
                User *reserver = dm->findUserById(reservation.getReaderID());
                if (reserver)
                {
                    QString msgContent = QString("您预约的图书(ISBN:%1)有新的入库或归还，您可以尝试前往借阅。")
                                             .arg(isbn);
                    Message msg(reserver->getID(), reserver->getName(), msgContent);
                    reserver->addMessage(msg);
                }
            }
        }
    }

    QString myMsgContent = QString("您已成功归还图书《%1》(ISBN:%2)，归还时间：%3")
                               .arg(book ? book->getTitle() : "未知")
                               .arg(isbn)
                               .arg(now.toString("yyyy-MM-dd HH:mm:ss"));
    Message myMsg(ID, getName(), myMsgContent);
    addMessage(myMsg);

    QString adminMsgContent = QString("读者 %1 归还了图书《%2》(ISBN:%3)，归还时间：%4")
                                  .arg(getName())
                                  .arg(book ? book->getTitle() : "未知")
                                  .arg(isbn)
                                  .arg(now.toString("yyyy-MM-dd HH:mm:ss"));

    std::vector<User *> &users = dm->getUsers();
    for (auto user : users)
    {
        if (user->getType() == 1)
        {
            Message msg(user->getID(), user->getName(), ID, getName(), adminMsgContent);
            user->addMessage(msg);
        }
    }

    dm->writeMessage();

    return ReturnResult::SUCCESS;
}

// 续借（按ISBN），返回续借结果
Reader::RenewResult Reader::renewBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    std::vector<BorrowRecord> &allRecords = dm->getBorrowRecords();
    BorrowRecord *targetRecord = nullptr;

    for (auto &record : allRecords)
    {
        if (record.getISBN() == isbn && record.getReaderID() == ID && !record.isReturned())
        {
            targetRecord = &record;
            break;
        }
    }

    if (!targetRecord)
    {
        return RenewResult::NOT_FOUND;
    }

    if (targetRecord->calculateOverdueDays() > 0)
    {
        return RenewResult::HAS_OVERDUE;
    }

    if (targetRecord->getRenewStatus() == BorrowRecord::RenewStatus::PENDING)
    {
        return RenewResult::ALREADY_PENDING;
    }

    QDateTime newDueTime = targetRecord->getDueTime().addDays(30);
    QDateTime maxDueTime = QDateTime::currentDateTime().addDays(90);
    if (newDueTime > maxDueTime)
    {
        return RenewResult::EXCEED_LIMIT;
    }

    targetRecord->setRenewStatus(BorrowRecord::RenewStatus::PENDING);
    dm->writeBorrowRecord();

    Book *book = dm->findBookByISBN(isbn);
    QString bookTitle = book ? book->getTitle() : "未知";

    QString myMsgContent = QString("您已提交续借申请，图书《%1》(ISBN:%2)，请等待管理员审核。")
                               .arg(bookTitle)
                               .arg(isbn);
    Message myMsg(ID, getName(), myMsgContent);
    addMessage(myMsg);

    QString adminMsgContent = QString("读者 %1 (ID:%2) 申请续借图书《%3》(ISBN:%4)，请审核。")
                                  .arg(getName())
                                  .arg(ID)
                                  .arg(bookTitle)
                                  .arg(isbn);

    std::vector<User *> &users = dm->getUsers();
    for (auto user : users)
    {
        if (user->getType() == 1)
        {
            Message msg(user->getID(), user->getName(), ID, getName(), adminMsgContent);
            user->addMessage(msg);
        }
    }

    dm->writeMessage();

    return RenewResult::SUCCESS;
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