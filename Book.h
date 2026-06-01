/**
 * @file Book.h
 * @brief 图书类定义
 *
 * Book类表示图书馆中的图书对象，包含图书的基本信息和状态管理。
 */

#ifndef BOOK_H
#define BOOK_H

#include <QString>
#include <QDateTime>

/**
 * @class Book
 * @brief 图书类
 *
 * 表示图书馆中的图书对象，包含图书的基本信息（ISBN、书名、作者、分类等）
 * 以及库存管理、借阅状态等功能。
 */
class Book
{
public:
    /**
     * @brief 默认构造函数
     */
    Book();

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
    Book(const QString &I, const QString &t, const QString &a, const QString &c,
         int s, const QDateTime &in, int b, int cb, int rc = 0);

    // ========== getter 方法 ==========

    /**
     * @brief 获取ISBN编号
     * @return ISBN编号
     */
    QString getISBN() const;

    /**
     * @brief 获取书名
     * @return 书名
     */
    QString getTitle() const;

    /**
     * @brief 获取作者
     * @return 作者
     */
    QString getAuthor() const;

    /**
     * @brief 获取分类
     * @return 分类
     */
    QString getCategory() const;

    /**
     * @brief 获取库存数量
     * @return 库存数量
     */
    int getStock() const;

    /**
     * @brief 获取入库时间
     * @return 入库时间
     */
    QDateTime getInStockTime() const;

    /**
     * @brief 获取借阅次数
     * @return 借阅次数
     */
    int getBorrowCount() const;

    /**
     * @brief 获取当前借出数量
     * @return 当前借出数量
     */
    int getCurrentBorrowed() const;

    /**
     * @brief 获取预约人数
     * @return 预约人数
     */
    int getReservationCount() const;

    // ========== setter 方法 ==========

    /**
     * @brief 设置ISBN编号
     * @param isbn ISBN编号
     */
    void setISBN(const QString &isbn);

    /**
     * @brief 设置书名
     * @param title 书名
     */
    void setTitle(const QString &title);

    /**
     * @brief 设置作者
     * @param author 作者
     */
    void setAuthor(const QString &author);

    /**
     * @brief 设置分类
     * @param category 分类
     */
    void setCategory(const QString &category);

    /**
     * @brief 设置库存数量
     * @param stock 库存数量
     */
    void setStock(int stock);

    /**
     * @brief 设置入库时间
     * @param time 入库时间
     */
    void setInStockTime(const QDateTime &time);

    /**
     * @brief 设置借阅次数
     * @param count 借阅次数
     */
    void setBorrowCount(int count);

    /**
     * @brief 设置当前借出数量
     * @param count 当前借出数量
     */
    void setCurrentBorrowed(int count);

    /**
     * @brief 设置预约人数
     * @param count 预约人数
     */
    void setReservationCount(int count);

    // ========== 状态查询 ==========

    /**
     * @brief 获取图书状态
     * @return "可预约" 或 "不可预约"
     */
    QString getStatus() const;

    /**
     * @brief 析构函数
     */
    ~Book();

    // ========== 成员变量 ==========

    QString ISBN;          ///< 书本ISBN编号
    QString title;         ///< 书名
    QString author;        ///< 作者
    QString category;      ///< 分类
    int stock;             ///< 库存数量
    QDateTime inStockTime; ///< 入库时间
    int borrowCount;       ///< 借阅次数（累计）
    int currentBorrowed;   ///< 当前借出数量
    int reservationCount;  ///< 预约人数
};

#endif // BOOK_H
