#include "Book.h"

Book::Book() : stock(0), borrowCount(0), currentBorrowed(0), reservationCount(0) {}

Book::Book(const QString &I, const QString &t, const QString &a, const QString &c, int s, const QDateTime &in, int b, int cb, int rc)
    : ISBN(I), title(t), author(a), category(c), stock(s), inStockTime(in), borrowCount(b), currentBorrowed(cb), reservationCount(rc) {}

QString Book::getISBN() const { return ISBN; }
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
QString Book::getCategory() const { return category; }
int Book::getStock() const { return stock; }
QDateTime Book::getInStockTime() const { return inStockTime; }
int Book::getBorrowCount() const { return borrowCount; }
int Book::getCurrentBorrowed() const { return currentBorrowed; }
int Book::getReservationCount() const { return reservationCount; }

void Book::setISBN(const QString &isbn) { ISBN = isbn; }
void Book::setTitle(const QString &t) { title = t; }
void Book::setAuthor(const QString &a) { author = a; }
void Book::setCategory(const QString &c) { category = c; }
void Book::setStock(int s) { stock = s; }
void Book::setInStockTime(const QDateTime &time) { inStockTime = time; }
void Book::setBorrowCount(int count) { borrowCount = count; }
void Book::setCurrentBorrowed(int count) { currentBorrowed = count; }
void Book::setReservationCount(int count) { reservationCount = count; }

QString Book::getStatus() const
{
    int available = stock - currentBorrowed;
    if (available <= 0)
    {
        return "借出";
    }
    else if (reservationCount > 0)
    {
        return "预约中";
    }
    else
    {
        return "可借";
    }
}

// 析构函数
Book::~Book()
{
}