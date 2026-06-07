/**
 * @file DataManager.h
 * @brief 数据管理器类定义
 *
 * DataManager类采用单例模式，负责管理图书馆管理系统的所有数据，
 * 包括用户、图书、借阅记录、预约记录和消息的读写与查询。
 */

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <vector>
#include "User.h"
#include "Book.h"
#include "BorrowRecord.h"
#include "Reservation.h"
#include "BorrowPolicy.h"

class Admin;
class Reader;
class StudentReader;
class TeacherReader;
class ExternalReader;

/**
 * @class DataManager
 * @brief 数据管理器类
 *
 * 采用单例模式，负责管理所有数据的持久化和查询操作，
 * 包括用户管理、图书管理、借阅记录管理、预约管理和消息管理。
 */
class DataManager
{
public:
    /**
     * @brief DataManager单例对象的静态指针
     */
    static DataManager *instance;

    /**
     * @brief 获取DataManager单例对象
     * @return DataManager单例指针
     */
    static DataManager *getInstance();

    /**
     * @brief 禁止拷贝构造函数（单例模式要求）
     */
    DataManager(const DataManager &) = delete;

    /**
     * @brief 禁止赋值运算符（单例模式要求）
     */
    DataManager &operator=(const DataManager &) = delete;

    // ========== 用户管理 ==========

    /**
     * @brief 初始化读取用户数据
     * 从文件加载所有用户信息到内存
     */
    void initUser();

    /**
     * @brief 重新计算所有读者信用分
     */
    void recalculateCreditScores();

    /**
     * @brief 写入用户数据到文件
     * 将内存中的用户信息保存到文件
     */
    void writeUser();

    /**
     * @brief 根据ID查找用户（模糊匹配）
     * @param id 用户ID关键字
     * @return 匹配的用户列表
     */
    std::vector<User *> findUsersById(const QString &id);

    /**
     * @brief 根据姓名查找用户（模糊匹配）
     * @param name 用户姓名关键字
     * @return 匹配的用户列表
     */
    std::vector<User *> findUsersByName(const QString &name);

    /**
     * @brief 根据ID和姓名同时查找用户（均为模糊匹配）
     * @param id 用户ID关键字
     * @param name 用户姓名关键字
     * @return 匹配的用户列表
     */
    std::vector<User *> searchUsers(const QString &id, const QString &name);

    /**
     * @brief 根据ID精确查找用户
     * @param id 用户ID
     * @return 匹配的用户指针（未找到返回nullptr）
     */
    User *findUserById(const QString &id);

    /**
     * @brief 添加用户并保存到文件
     * @param user 用户指针
     */
    void addUser(User *user);

    /**
     * @brief 根据ID和姓名同时删除用户并保存到文件
     * @param id 用户ID
     * @param name 用户姓名
     * @return 删除成功返回true，失败返回false
     */
    bool deleteUser(const QString &id, const QString &name);

    /**
     * @brief 根据ID和姓名同时修改用户信息并保存到文件
     * @param id 用户ID
     * @param name 用户姓名
     * @param newUser 新用户信息
     * @return 修改成功返回true，失败返回false
     */
    bool updateUser(const QString &id, const QString &name, User *newUser);

    /**
     * @brief 清除所有用户并保存到文件
     */
    void clearAllUsers();

    /**
     * @brief 获取所有用户
     * @return 用户列表引用
     */
    std::vector<User *> &getUsers();

    /**
     * @brief 获取用户数量
     * @return 用户总数
     */
    int getUserCount() const;

    // ========== 数据清除 ==========

    /**
     * @brief 清空预约记录
     */
    void clearAllReservations();

    /**
     * @brief 清空借书记录
     */
    void clearAllBorrowRecords();

    /**
     * @brief 清空消息记录
     */
    void clearAllMessages();

    // ========== 图书管理 ==========

    /**
     * @brief 初始化读取图书数据
     * 从文件加载所有图书信息到内存
     */
    void initBook();

    /**
     * @brief 写入图书数据到文件
     * 将内存中的图书信息保存到文件
     */
    void writeBook();

    /**
     * @brief 根据ISBN查找图书（精确匹配）
     * @param isbn 图书ISBN
     * @return 匹配的图书指针（未找到返回nullptr）
     */
    Book *findBookByISBN(const QString &isbn);

    /**
     * @brief 根据书名查找图书（模糊匹配）
     * @param title 书名关键字
     * @return 匹配的图书列表
     */
    std::vector<Book> findBooksByTitle(const QString &title);

    /**
     * @brief 根据作者查找图书（模糊匹配）
     * @param author 作者关键字
     * @return 匹配的图书列表
     */
    std::vector<Book> findBooksByAuthor(const QString &author);

    /**
     * @brief 根据分类查找图书（模糊匹配）
     * @param category 分类关键字
     * @return 匹配的图书列表
     */
    std::vector<Book> findBooksByCategory(const QString &category);

    /**
     * @brief 多条件搜索图书
     * @param isbn ISBN关键字
     * @param title 书名关键字
     * @param author 作者关键字
     * @param category 分类关键字
     * @return 匹配的图书指针列表
     */
    std::vector<const Book *> searchBooks(const QString &isbn, const QString &title,
                                          const QString &author, const QString &category);

    /**
     * @brief 添加图书并保存到文件
     * @param book 图书对象
     * @return 0=成功新增，1=库存已增加，-1=ISBN冲突
     */
    int addBook(const Book &book);

    /**
     * @brief 根据ISBN删除图书并保存到文件
     * @param isbn 图书ISBN
     * @param decreaseStock 是否只减少库存
     * @return 0=成功删除记录，1=库存已减少，-1=ISBN不存在，-2=存在预约或借出无法删除
     */
    int deleteBook(const QString &isbn, int decreaseStock);

    /**
     * @brief 根据ISBN修改图书信息并保存到文件
     * @param oldIsbn 原ISBN
     * @param newBook 新图书信息
     * @return 0=成功修改，-1=原ISBN不存在，-2=存在预约或借出无法修改，-3=新ISBN已存在
     */
    int updateBook(const QString &oldIsbn, const Book &newBook);

    /**
     * @brief 获取所有图书
     * @return 图书列表引用
     */
    std::vector<Book> &getBooks();

    /**
     * @brief 获取图书数量
     * @return 图书总数
     */
    int getBookCount() const;

    /**
     * @brief 按借阅次数排序（降序）
     * @return 排序后的图书列表
     */
    std::vector<Book> sortBooksByBorrowCount();

    /**
     * @brief 按入库时间排序（降序，最新优先）
     * @return 排序后的图书列表
     */
    std::vector<Book> sortBooksByInStockTime();

    // ========== 借阅记录管理 ==========

    /**
     * @brief 初始化读取借阅记录数据
     * 从文件加载所有借阅记录到内存
     */
    void initBorrowRecord();

    /**
     * @brief 写入借阅记录数据到文件
     * 将内存中的借阅记录保存到文件
     */
    void writeBorrowRecord();

    /**
     * @brief 添加借阅记录
     * @param record 借阅记录对象
     * @return 添加成功返回true，失败返回false
     */
    bool addBorrowRecord(const BorrowRecord &record);

    /**
     * @brief 续借图书
     * @param isbn 图书ISBN
     * @param readerId 读者ID
     * @param days 续借天数
     * @return 续借成功返回true，失败返回false
     */
    bool renewBorrowRecord(const QString &isbn, const QString &readerId, int days);

    /**
     * @brief 获取某读者的借阅记录
     * @param readerId 读者ID
     * @return 借阅记录列表
     */
    std::vector<BorrowRecord> getBorrowRecordsByReader(const QString &readerId);

    /**
     * @brief 获取读者当前借阅数量
     * @param readerId 读者ID
     * @return 当前借阅数量
     */
    int getBorrowCountByReader(const QString &readerId);

    /**
     * @brief 检查读者是否有逾期未还
     * @param readerId 读者ID
     * @return 有逾期返回true，无逾期返回false
     */
    bool hasOverdueBooks(const QString &readerId);

    /**
     * @brief 获取所有借阅记录
     * @return 借阅记录列表引用
     */
    std::vector<BorrowRecord> &getBorrowRecords();

    // ========== 预约管理 ==========

    /**
     * @brief 初始化读取预约数据
     * 从文件加载所有预约记录到内存
     */
    void initReservation();

    /**
     * @brief 写入预约数据到文件
     * 将内存中的预约记录保存到文件
     */
    void writeReservation();

    /**
     * @brief 添加预约记录
     * @param reservation 预约对象
     * @return 添加成功返回true，失败返回false
     */
    bool addReservation(const Reservation &reservation);

    /**
     * @brief 取消预约记录（软删除，标记状态为CANCELLED）
     * @param isbn 图书ISBN
     * @param readerId 读者ID
     * @return 取消成功返回true，失败返回false
     */
    bool cancelReservation(const QString &isbn, const QString &readerId);

    /**
     * @brief 删除预约记录（硬删除）
     * @param isbn 图书ISBN
     * @param readerId 读者ID
     * @return 删除成功返回true，失败返回false
     */
    bool removeReservation(const QString &isbn, const QString &readerId);

    /**
     * @brief 获取某图书的所有预约（按预约时间排序）
     * @param isbn 图书ISBN
     * @return 预约列表
     */
    std::vector<Reservation> getReservationsByISBN(const QString &isbn);

    /**
     * @brief 获取某读者的所有预约
     * @param readerId 读者ID
     * @return 预约列表
     */
    std::vector<Reservation> getReservationsByReader(const QString &readerId);

    /**
     * @brief 获取所有预约记录
     * @return 预约列表引用
     */
    std::vector<Reservation> &getReservations();

    /**
     * @brief 通知可借的预约（图书入库时）
     * @param isbn 图书ISBN
     */
    void notifyReservations(const QString &isbn);

    /**
     * @brief 获取预约数量
     * @return 预约总数
     */
    int getReservationCount() const;

    /**
     * @brief 检查读者是否已预约某图书
     * @param isbn 图书ISBN
     * @param readerId 读者ID
     * @return 已预约返回true，未预约返回false
     */
    bool hasReservation(const QString &isbn, const QString &readerId);

    // ========== 消息管理 ==========

    /**
     * @brief 初始化读取消息数据
     * 从文件加载所有消息到内存
     */
    void initMessage();

    /**
     * @brief 写入消息数据到文件
     * 将内存中的消息保存到文件
     */
    void writeMessage();

    /**
     * @brief 添加管理员消息
     * @param user 管理员用户
     * @param readerId 读者ID
     * @param readerName 读者姓名
     * @param message 消息内容
     */
    void addAdminMessage(User *user, const QString &readerId, const QString &readerName, const QString &message);

    /**
     * @brief 添加读者消息
     * @param reader 读者用户
     * @param message 消息内容
     */
    void addReaderMessage(User *reader, const QString &message);

    /**
     * @brief 向所有管理员发送消息
     * @param content 消息内容
     */
    void sendMessageToAdmin(const QString &content);

    /**
     * @brief 析构函数
     */
    ~DataManager();

private:
    /**
     * @brief 私有构造函数（单例模式要求）
     */
    DataManager();

    std::vector<User *> users;               ///< 存储所有用户（Admin和Reader）
    std::vector<Book> books;                 ///< 存储所有图书
    std::vector<BorrowRecord> borrowRecords; ///< 存储所有借阅记录
    std::vector<Reservation> reservations;   ///< 存储所有预约记录

    QString userFilePath;         ///< 用户数据文件路径
    QString bookFilePath;         ///< 图书数据文件路径
    QString borrowRecordFilePath; ///< 借阅记录数据文件路径
    QString reservationFilePath;  ///< 预约数据文件路径
    QString messageFilePath;      ///< 消息数据文件路径
};

#endif // DATAMANAGER_H