#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <vector>
#include "User.h"
#include "Book.h"
#include "BorrowRecord.h"
#include "Reservation.h"

class Admin;
class Reader;

class DataManager
{
public:
    // （单例静态变量）：DataManager单例对象的静态指针
    static DataManager *instance;

    // （单例模式）：获取DataManager单例对象
    static DataManager *getInstance();

    // （禁止拷贝）：禁止拷贝构造函数
    DataManager(const DataManager &) = delete;
    // （禁止赋值）：禁止赋值运算符
    DataManager &operator=(const DataManager &) = delete;

    // （用户管理）：初始化读取用户数据
    void initUser();
    // （用户管理）：写入用户数据到文件
    void writeUser();
    // （用户查询）：根据ID查找用户（模糊匹配，可能多个）
    std::vector<User *> findUsersById(const QString &id);
    // （用户查询）：根据姓名查找用户（模糊匹配，可能多个）
    std::vector<User *> findUsersByName(const QString &name);
    // （用户查询）：根据ID和姓名同时查找用户（均为模糊匹配，可能多个）
    std::vector<User *> searchUsers(const QString &id, const QString &name);
    // （用户查询）：根据ID精确查找用户（返回单个）
    User *findUserById(const QString &id);
    // （用户添加）：添加用户并保存到文件
    void addUser(User *user);
    // （用户删除）：根据ID和姓名同时删除用户并保存到文件（两个关键字都需要匹配）
    bool deleteUser(const QString &id, const QString &name);
    // （用户修改）：根据ID和姓名同时修改用户信息并保存到文件（两个关键字都需要匹配）
    bool updateUser(const QString &id, const QString &name, User *newUser);
    // （用户清除）：清除所有用户并保存到文件
    void clearAllUsers();
    // （用户获取）：获取所有用户
    std::vector<User *> &getUsers();
    // （用户数量）：获取用户数量
    int getUserCount() const;
    
    // （数据清除）：清空预约记录
    void clearAllReservations();
    // （数据清除）：清空借书记录
    void clearAllBorrowRecords();
    // （数据清除）：清空消息记录
    void clearAllMessages();

    // （图书管理）：初始化读取图书数据
    void initBook();
    // （图书管理）：写入图书数据到文件
    void writeBook();

    // 修改5.16
    //  （图书查询）：根据ISBN查找图书（精确匹配）
    Book *findBookByISBN(const QString &isbn);
    // （图书查询）：根据书名查找图书（模糊匹配）
    std::vector<Book> findBooksByTitle(const QString &title);
    // （图书查询）：根据作者查找图书（模糊匹配）
    std::vector<Book> findBooksByAuthor(const QString &author);
    // （图书查询）：根据分类查找图书（模糊匹配）
    std::vector<Book> findBooksByCategory(const QString &category);
    // （图书查询）：多条件搜索图书
    std::vector<const Book *> searchBooks(const QString &isbn, const QString &title,
                                          const QString &author, const QString &category);
    // （图书添加）：添加图书并保存到文件
    // 返回值：0=成功新增，1=库存已增加，-1=ISBN冲突
    int addBook(const Book &book);
    // （图书删除）：根据ISBN删除图书并保存到文件
    // 返回值：0=成功删除记录，1=库存已减少，-1=ISBN不存在，-2=存在预约或借出无法删除
    int deleteBook(const QString &isbn, int decreaseStock);
    // （图书修改）：根据ISBN修改图书信息并保存到文件
    // 返回值：0=成功修改，-1=原ISBN不存在，-2=存在预约或借出无法修改，-3=新ISBN已存在
    int updateBook(const QString &oldIsbn, const Book &newBook);
    // （图书获取）：获取所有图书
    std::vector<Book> &getBooks();
    // （图书数量）：获取图书数量
    int getBookCount() const;
    // （图书排序）：按借阅次数排序（降序）
    std::vector<Book> sortBooksByBorrowCount();
    // （图书排序）：按入库时间排序（降序，最新优先）
    std::vector<Book> sortBooksByInStockTime();
    // 修改结束

    // （借阅记录管理）：初始化读取借阅记录数据
    void initBorrowRecord();
    // （借阅记录管理）：写入借阅记录数据到文件
    void writeBorrowRecord();
    // （借阅记录管理）：添加借阅记录
    bool addBorrowRecord(const BorrowRecord &record);
    // （借阅记录管理）：更新借阅记录（还书）
    bool updateBorrowRecord(const QString &isbn, const QString &readerId);
    // （借阅记录管理）：续借图书
    bool renewBorrowRecord(const QString &isbn, const QString &readerId, int days);
    // （借阅记录管理）：获取某读者的借阅记录
    std::vector<BorrowRecord> getBorrowRecordsByReader(const QString &readerId);
    // （借阅记录管理）：获取某图书的借阅记录
    std::vector<BorrowRecord> getBorrowRecordsByISBN(const QString &isbn);
    // （借阅记录管理）：获取所有未归还的借阅记录
    std::vector<BorrowRecord> getBorrowingRecords();
    // （借阅记录管理）：获取读者当前借阅数量
    int getBorrowCountByReader(const QString &readerId);
    // （借阅记录管理）：检查读者是否有逾期未还
    bool hasOverdueBooks(const QString &readerId);
    // （借阅记录管理）：获取所有借阅记录
    std::vector<BorrowRecord> &getBorrowRecords();

    // （预约管理）：初始化读取预约数据
    void initReservation();
    // （预约管理）：写入预约数据到文件
    void writeReservation();
    // （预约管理）：添加预约记录
    bool addReservation(const Reservation &reservation);
    // （预约管理）：取消预约记录（按ISBN和读者ID）
    bool cancelReservation(const QString &isbn, const QString &readerId);
    // （预约管理）：删除预约记录（硬删除）
    bool removeReservation(const QString &isbn, const QString &readerId);
    // （预约管理）：获取某图书的所有预约（按预约时间排序）
    std::vector<Reservation> getReservationsByISBN(const QString &isbn);
    // （预约管理）：获取某读者的所有预约
    std::vector<Reservation> getReservationsByReader(const QString &readerId);
    // （预约管理）：获取所有预约记录
    std::vector<Reservation> &getReservations();
    // （预约管理）：通知可借的预约（图书入库时）
    void notifyReservations(const QString &isbn);
    // （预约管理）：获取预约数量
    int getReservationCount() const;
    // （预约管理）：检查读者是否已预约某图书
    bool hasReservation(const QString &isbn, const QString &readerId);

    // （消息管理）：初始化读取消息数据
    void initMessage();
    // （消息管理）：写入消息数据到文件
    void writeMessage();

    // （管理员消息）：添加管理员消息
    void addAdminMessage(User *user, const QString& readerId, const QString& readerName, const QString &message);

    // （读者消息）：添加读者消息
    void addReaderMessage(User *reader, const QString &message);

    // （析构函数）：DataManager析构函数
    ~DataManager();

private:
    // （私有构造）：单例模式私有构造函数
    DataManager();

    // （用户容器）：存储所有用户（Admin和Reader）
    std::vector<User *> users;

    // （图书容器）：存储所有图书
    std::vector<Book> books;

    // （借阅记录容器）：存储所有借阅记录
    std::vector<BorrowRecord> borrowRecords;
    // （预约容器）：存储所有预约
    std::vector<Reservation> reservations;

    // （文件路径）：用户数据文件路径
    QString userFilePath;
    // （文件路径）：图书数据文件路径
    QString bookFilePath;
    // （文件路径）：借阅记录数据文件路径
    QString borrowRecordFilePath;
    // （文件路径）：预约数据文件路径
    QString reservationFilePath;
    // （文件路径）：消息数据文件路径
    QString messageFilePath;
};

#endif // DATAMANAGER_H