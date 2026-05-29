#ifndef READER_H
#define READER_H

#include "User.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"
#include <QString>
#include <vector>

class Reader : public User {
public:
    // 构造函数
    Reader(QString& I, QString& n, QString& pa, QString& ph, QString& e);

    // 实现User的纯虚函数：获取用户身份
    QString typeToIdentity() override;

    // 实现User的纯虚函数：生成验证码（读者：6位数字）
    QString generateVerificationCode() override;

    // getter和setter方法
    int getMaxBooks();
    int getCreditScore();
    std::vector<Book>& getBooks();

    void setMaxBooks(int max);
    void setCreditScore(int score);

    // 预约图书（按ISBN）
    bool reserveBook(const QString& isbn);

    // 取消预约（按ISBN）
    bool cancelReservation(const QString& isbn);

    // 查看我的预约
    std::vector<Reservation> viewMyReservations();

    // 删除预约（已审核/已取消状态，非待审核）
    bool deleteReservation(const QString& isbn);

    // 清空所有非待审核状态的预约
    int clearAllReservationsExceptPending();

    // 借书（按ISBN）
    bool borrowBook(const QString& isbn);

    // 还书（按ISBN）
    bool returnBook(const QString& isbn);

    // 续借（按ISBN）
    bool renewBook(const QString& isbn);

    // 查看我的借阅记录
    std::vector<BorrowRecord> viewMyBorrowRecords();

    // 查找图书（模糊搜索）
    std::vector<const Book*> findBook(const QString& isbn, const QString& title, 
                                          const QString& author, const QString& category);

    // 析构函数
    ~Reader();

    // 最大借阅书量
    int maxBooks;
    // 信用分
    int creditScore;
    // 当前借阅列表
    std::vector<Book> books;
};

#endif // READER_H