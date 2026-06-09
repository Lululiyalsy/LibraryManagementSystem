/**
 * @file Book.cpp
 * @brief 图书类实现
 *
 * 实现Book类的所有成员函数，包括构造函数、getter/setter方法和状态查询功能。
 */

#include "Book.h"

/**
 * @brief 默认构造函数
 *
 * 初始化所有成员变量为默认值：
 * - stock: 0
 * - borrowCount: 0
 * - currentBorrowed: 0
 * - reservationCount: 0
 */
Book::Book() : stock(0), borrowCount(0), currentBorrowed(0), reservationCount(0), overdueReturnCount(0) {}

/**
 * @brief 构造函数
 * @param I ISBN编号
 * @param t 书名
 * @param a 作者
 * @param c 分类
 * @param s 库存数量
 * @param in 入库时间
 * @param b 借阅次数
 * @param cb 当前借出数量
 * @param rc 预约人数（默认0）
 */
Book::Book(const QString &I, const QString &t, const QString &a, const QString &c,
           int s, const QDateTime &in, int b, int cb, int rc, int orc)
    : ISBN(I), title(t), author(a), category(c), stock(s), inStockTime(in),
      borrowCount(b), currentBorrowed(cb), reservationCount(rc), overdueReturnCount(orc) {}

// ========== getter 方法 ==========

/**
 * @brief 获取ISBN编号
 * @return ISBN编号
 */
QString Book::getISBN() const { return ISBN; }

/**
 * @brief 获取书名
 * @return 书名
 */
QString Book::getTitle() const { return title; }

/**
 * @brief 获取作者
 * @return 作者
 */
QString Book::getAuthor() const { return author; }

/**
 * @brief 获取分类
 * @return 分类
 */
QString Book::getCategory() const { return category; }

/**
 * @brief 获取库存数量
 * @return 库存数量
 */
int Book::getStock() const { return stock; }

/**
 * @brief 获取入库时间
 * @return 入库时间
 */
QDateTime Book::getInStockTime() const { return inStockTime; }

/**
 * @brief 获取借阅次数
 * @return 借阅次数
 */
int Book::getBorrowCount() const { return borrowCount; }

/**
 * @brief 获取当前借出数量
 * @return 当前借出数量
 */
int Book::getCurrentBorrowed() const { return currentBorrowed; }

/**
 * @brief 获取预约人数
 * @return 预约人数
 */
int Book::getReservationCount() const { return reservationCount; }

/**
 * @brief 获取逾期归还次数
 * @return 逾期归还次数
 */
int Book::getOverdueReturnCount() const { return overdueReturnCount; }

// ========== setter 方法 ==========

/**
 * @brief 设置ISBN编号
 * @param isbn ISBN编号
 */
void Book::setISBN(const QString &isbn) { ISBN = isbn; }

/**
 * @brief 设置书名
 * @param t 书名
 */
void Book::setTitle(const QString &t) { title = t; }

/**
 * @brief 设置作者
 * @param a 作者
 */
void Book::setAuthor(const QString &a) { author = a; }

/**
 * @brief 设置分类
 * @param c 分类
 */
void Book::setCategory(const QString &c) { category = c; }

/**
 * @brief 设置库存数量
 * @param s 库存数量
 */
void Book::setStock(int s) { stock = s; }

/**
 * @brief 设置入库时间
 * @param time 入库时间
 */
void Book::setInStockTime(const QDateTime &time) { inStockTime = time; }

/**
 * @brief 设置借阅次数
 * @param count 借阅次数
 */
void Book::setBorrowCount(int count) { borrowCount = count; }

/**
 * @brief 设置当前借出数量
 * @param count 当前借出数量
 */
void Book::setCurrentBorrowed(int count) { currentBorrowed = count; }

/**
 * @brief 设置预约人数
 * @param count 预约人数
 */
void Book::setReservationCount(int count) { reservationCount = count; }

/**
 * @brief 设置逾期归还次数
 * @param count 逾期归还次数
 */
void Book::setOverdueReturnCount(int count) { overdueReturnCount = count; }

/**
 * @brief 增加逾期归还次数
 */
void Book::incrementOverdueReturnCount() { overdueReturnCount++; }

// ========== 状态查询 ==========

/**
 * @brief 获取图书状态
 * @return "可预约" 或 "不可预约"
 *
 * 根据库存数量和当前借出数量判断图书是否可预约：
 * - 可用数量 = 库存数量 - 当前借出数量
 * - 如果可用数量 <= 0，返回 "不可预约"
 * - 否则返回 "可预约"
 */
QString Book::getStatus() const
{
    int available = stock - currentBorrowed;
    if (available <= 0)
    {
        return "不可预约";
    }
    else
    {
        return "可预约";
    }
}

/**
 * @brief 析构函数
 */
Book::~Book()
{
}