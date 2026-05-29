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
std::vector<Book> &Reader::getBooks() { return books; }

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
        return ReserveResult::SUCCESS;
    }
    return ReserveResult::BOOK_NOT_FOUND;
}

// 取消预约（按ISBN）- 直接删除记录
bool Reader::cancelReservation(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 先检查预约是否存在且是待审核状态
    std::vector<Reservation> reservations = dm->getReservationsByReader(ID);
    for (const auto &reservation : reservations)
    {
        if (reservation.getISBN() == isbn && reservation.getStatus() == Reservation::PENDING)
        {
            // 直接删除记录
            bool success = dm->removeReservation(isbn, ID);
            if (success)
            {
                // TODO: 发送消息给读者自己（不给管理员发，待消息系统完善后实现）
                // QString messageContent = QString("您已取消预约图书(ISBN:%1)").arg(isbn);
                // Message message(ID, "系统", messageContent);
                // dm->addMessageToReader(ID, message);
            }
            return success;
        }
    }
    return false;
}

// 查看我的预约
std::vector<Reservation> Reader::viewMyReservations()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getReservationsByReader(ID);
}

// 删除预约（已审核/已取消状态，非待审核）
bool Reader::deleteReservation(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    std::vector<Reservation> reservations = dm->getReservationsByReader(ID);
    for (const auto &reservation : reservations)
    {
        if (reservation.getISBN() == isbn)
        {
            // 只允许删除非待审核状态的预约
            if (reservation.getStatus() != Reservation::PENDING)
            {
                return dm->removeReservation(isbn, ID);
            }
            break;
        }
    }
    return false;
}

// 清空所有非待审核状态的预约
int Reader::clearAllReservationsExceptPending()
{
    DataManager *dm = DataManager::getInstance();

    std::vector<Reservation> reservations = dm->getReservationsByReader(ID);
    int deletedCount = 0;

    for (const auto &reservation : reservations)
    {
        if (reservation.getStatus() != Reservation::PENDING)
        {
            if (dm->removeReservation(reservation.getISBN(), ID))
            {
                deletedCount++;
            }
        }
    }
    return deletedCount;
}

// 借书（按ISBN）
// 修改5.16
bool Reader::borrowBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    Book *book = dm->findBookByISBN(isbn);
    if (!book)
        return false;

    int available = book->getStock() - book->getCurrentBorrowed();
    if (available <= 0)
        return false;

    if (dm->hasOverdueBooks(ID))
        return false;

    int currentBorrowCount = dm->getBorrowCountByReader(ID);
    if (currentBorrowCount >= maxBooks)
        return false;

    std::vector<Reservation> &allReservations = dm->getReservations();
    bool hasValidReservation = false;
    int notifiedCount = 0;

    for (const auto &r : allReservations)
    {
        if (r.getISBN() == isbn && r.getStatus() == Reservation::APPROVED)
        {
            notifiedCount++;
            if (r.getReaderID() == ID)
            {
                hasValidReservation = true;
            }
        }
    }

    if (notifiedCount >= available && !hasValidReservation)
    {
        return false;
    }

    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueTime = now.addDays(30);

    BorrowRecord record(isbn, ID, now, dueTime);
    bool success = dm->addBorrowRecord(record);

    if (success && hasValidReservation)
    {
        for (auto &r : allReservations)
        {
            if (r.getISBN() == isbn && r.getReaderID() == ID &&
                r.getStatus() == Reservation::APPROVED)
            {
                r.setStatus(Reservation::CANCELLED);
                if (book && book->getReservationCount() > 0)
                {
                    book->setReservationCount(book->getReservationCount() - 1);
                }
                dm->writeBook();
                dm->writeReservation();
                break;
            }
        }
    }

    return success;
}

// 还书（按ISBN）
bool Reader::returnBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();
    bool success = dm->updateBorrowRecord(isbn, ID);
    if (success)
    {
        dm->notifyReservations(isbn);
    }
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
// 修改结束

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