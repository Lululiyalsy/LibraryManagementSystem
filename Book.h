#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QDateTime>

class Book
{
public:
    // 默认构造函数
    Book();

    // 构造函数
    Book(const QString &I, const QString &t, const QString &a, const QString &c, int s, const QDateTime &in, int b, int cb, int rc = 0);

    // getter和setter
    QString getISBN() const;
    QString getTitle() const;
    QString getAuthor() const;
    QString getCategory() const;
    int getStock() const;
    QDateTime getInStockTime() const;
    int getBorrowCount() const;
    int getCurrentBorrowed() const;
    int getReservationCount() const;

    void setISBN(const QString &isbn);
    void setTitle(const QString &title);
    void setAuthor(const QString &author);
    void setCategory(const QString &category);
    void setStock(int stock);
    void setInStockTime(const QDateTime &time);
    void setBorrowCount(int count);
    void setCurrentBorrowed(int count);
    void setReservationCount(int count);

    // 获取图书状态：可借/借出/预约中
    QString getStatus() const;

    // 析构函数
    ~Book();

    // 书本ISBN
    QString ISBN;
    // 书名
    QString title;
    // 作者
    QString author;
    // 分类
    QString category;
    // 库存数量
    int stock;
    // 入库时间
    QDateTime inStockTime;
    // 借阅次数
    int borrowCount;
    // 当前借出数量
    int currentBorrowed;
    // 预约人数
    int reservationCount;
};

#endif // BOOK_H
