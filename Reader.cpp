#include "Reader.h"
#include "DataManager.h"
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>

// 构造函数
Reader::Reader(QString& I, QString& n, QString& pa, QString& ph, QString& e)
    : User(I, n, pa, ph, e) {
    type = 2;
    maxBooks = 10;
    creditScore = 100;
}

// 实现User的纯虚函数：获取用户身份
QString Reader::typeToIdentity() {
    return QString("读者");
}

//（生成验证码）：生成读者验证码（6位数字）
QString Reader::generateVerificationCode() {
    QString code;
    for (int i = 0; i < 6; ++i) {
        int digit = QRandomGenerator::global()->bounded(10);
        code.append(QString::number(digit));
    }
    return code;
}

// getter和setter方法
int Reader::getMaxBooks() { return maxBooks; }
int Reader::getCreditScore() { return creditScore; }
std::vector<Book>& Reader::getBooks() { return books; }
std::vector<QString>& Reader::getMsg() { return msg; }

void Reader::setMaxBooks(int max) { maxBooks = max; }
void Reader::setCreditScore(int score) { creditScore = score; }

// 预约图书（按ISBN）
bool Reader::reserveBook(const QString& isbn) {
    DataManager* dm = DataManager::getInstance();
    
    Book* book = dm->findBookByISBN(isbn);
    if (!book) {
        return false;
    }
    
    if (dm->hasReservation(isbn, ID)) {
        return false;
    }
    
    int available = book->getStock() - book->getCurrentBorrowed();
    if (available > 0) {
        return false;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    Reservation reservation(isbn, ID, now, Reservation::PENDING);
    return dm->addReservation(reservation);
}

// 取消预约（按ISBN）
bool Reader::cancelReservation(const QString& isbn) {
    DataManager* dm = DataManager::getInstance();
    return dm->cancelReservation(isbn, ID);
}

// 查看我的预约
std::vector<Reservation> Reader::viewMyReservations() {
    DataManager* dm = DataManager::getInstance();
    return dm->getReservationsByReader(ID);
}

// 借书（按ISBN）
//修改5.16
bool Reader::borrowBook(const QString& isbn) {
    DataManager* dm = DataManager::getInstance();
    
    Book* book = dm->findBookByISBN(isbn);
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
    
    std::vector<Reservation>& allReservations = dm->getReservations();
    bool hasValidReservation = false;
    int notifiedCount = 0;
    
    for (const auto& r : allReservations) {
        if (r.getISBN() == isbn && r.getStatus() == Reservation::NOTIFIED) {
            notifiedCount++;
            if (r.getReaderID() == ID) {
                hasValidReservation = true;
            }
        }
    }
    
    if (notifiedCount >= available && !hasValidReservation) {
        return false;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueTime = now.addDays(30);
    
    BorrowRecord record(isbn, ID, now, dueTime);
    bool success = dm->addBorrowRecord(record);
    
    if (success && hasValidReservation) {
        for (auto& r : allReservations) {
            if (r.getISBN() == isbn && r.getReaderID() == ID && 
                r.getStatus() == Reservation::NOTIFIED) {
                r.setStatus(Reservation::COMPLETED);
                if (book && book->getReservationCount() > 0) {
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
bool Reader::returnBook(const QString& isbn) {
    DataManager* dm = DataManager::getInstance();
    bool success = dm->updateBorrowRecord(isbn, ID);
    if (success) {
        dm->notifyReservations(isbn);
    }
    return success;
}

// 续借（按ISBN）
bool Reader::renewBook(const QString& isbn) {
    DataManager* dm = DataManager::getInstance();
    return dm->renewBorrowRecord(isbn, ID, 30);
}

// 查看我的借阅记录
std::vector<BorrowRecord> Reader::viewMyBorrowRecords() {
    DataManager* dm = DataManager::getInstance();
    return dm->getBorrowRecordsByReader(ID);
}
// 修改结束

// 查找图书（模糊搜索）
std::vector<const Book*> Reader::findBook(const QString& isbn, const QString& title, 
                                          const QString& author, const QString& category) {
    DataManager* dm = DataManager::getInstance();
    return dm->searchBooks(isbn, title, author, category);
}

// 查看消息
void Reader::checkMsg() {
    // 消息已查看，可以在这里添加标记已读的逻辑
}

// 添加消息
void Reader::addMsg(const QString& message) {
    msg.push_back(message);
}

// 清空消息
void Reader::clearMsg() {
    msg.clear();
}

// 获取未读消息数量
int Reader::getUnreadMsgCount() {
    return msg.size();
}

// 析构函数
Reader::~Reader() {

}