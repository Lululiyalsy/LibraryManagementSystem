#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStackedWidget>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include "User.h"
#include "Message.h"
#include "statisticswidget.h"

class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    // （构造函数）：显式构造函数，用于创建管理员窗口实例
    explicit AdminWindow(User *user, QWidget *parent = nullptr);
    // （析构函数）：析构函数，用于释放窗口资源
    ~AdminWindow();

private slots:
    // （用户管理）：用户管理按钮点击槽函数
    void onUserManagement();
    // （图书管理）：图书管理按钮点击槽函数
    void onBookManagement();
    // （借阅管理）：借阅管理按钮点击槽函数
    void onBorrowManagement();
    // （预约管理）：预约管理按钮点击槽函数
    void onReservationManagement();
    // （统计报表）：统计报表按钮点击槽函数
    void onStatistics();
    // （处理预约）：处理预约按钮点击槽函数
    void onProcessReservation();
    // （取消预约）：取消预约按钮点击槽函数
    void onCancelReservation();
    // （退出登录）：退出按钮点击槽函数
    void onLogout();
    // （消息管理）：消息按钮点击槽函数
    void onMessage();

signals:
    // （退出登录信号）：通知主程序退出登录
    void logout();

private:
    // （初始化工具栏）：初始化左侧工具栏
    void setupToolbar();
    // （初始化中心部件）：初始化中心堆叠部件
    void setupCentralWidget();
    // （初始化用户表格）：初始化用户管理表格
    void setupUserTable();
    // （初始化图书表格）：初始化图书管理表格
    void setupBookTable();
    // （初始化借阅表格）：初始化借阅管理表格
    void setupBorrowTable();
    // （初始化预约表格）：初始化预约管理表格
    void setupReservationTable();
    // （初始化统计表格）：初始化统计报表表格
    void setupStatisticsTable();

    // （加载用户数据）：从数据管理器加载用户数据到表格
    void loadUserData();
    // （加载图书数据）：从数据管理器加载图书数据到表格
    void loadBookData();
    // （加载借阅数据）：从数据管理器加载借阅数据到表格
    void loadBorrowData();
    // （加载预约数据）：从数据管理器加载预约数据到表格
    void loadReservationData();
    // （加载统计数据）：从数据管理器加载统计数据到表格
    void loadStatisticsData();

    // （中文输入对话框）：显示带中文按钮的输入对话框
    //  返回 QPair：first=输入内容，second=true表示取消，false表示确定
    QPair<QString, bool> showInputDialog(const QString &title, const QString &label, bool required = false);
    // （用户查找）：查找按钮点击处理
    void onUserSearch();
    // （显示用户）：将用户数据显示到表格
    void displayUsers(const std::vector<User *> &users);
    // （用户增加）：增加按钮点击处理
    void onUserAdd();
    // （用户删除）：删除按钮点击处理
    void onUserDelete();
    // （用户修改）：修改按钮点击处理
    void onUserUpdate();
    // （用户清除）：清除按钮点击处理
    void onUserClear();

    // 修改5.16
    // （图书查找）：查找按钮点击处理
    void onBookSearch();
    // （显示图书）：将图书数据显示到表格
    void displayBooks(const std::vector<const Book *> &books);
    // （图书增加）：增加按钮点击处理
    void onBookAdd();
    // （图书删除）：删除按钮点击处理
    void onBookDelete();
    // （图书修改）：修改按钮点击处理
    void onBookUpdate();
    // （图书清除）：清除按钮点击处理
    void onBookClear();
    // （图书排序）：按借阅次数排序
    void onBookSort();
    // （图书排序）：按入库时间排序
    void onBookSortByTime();

    // （借阅查找）：查找按钮点击处理
    void onBorrowSearch();
    // （显示借阅记录）：将借阅记录显示到表格
    void displayBorrowRecords(const std::vector<BorrowRecord> &records);
    // （借阅添加）：借书按钮点击处理
    void onBorrowAdd();
    // （借阅归还）：还书按钮点击处理
    void onBorrowReturn();
    // （借阅续借）：续借按钮点击处理
    void onBorrowRenew();
    // 修改结束

private:
    // （当前用户）：当前登录的管理员用户对象
    User *currentUser;
    // （工具栏）：左侧工具栏指针
    QToolBar *toolbar;
    // （堆叠部件）：中心堆叠部件指针，用于切换不同表格视图
    QStackedWidget *stackedWidget;

    // （用户管理容器）：用户管理界面的容器指针
    QWidget *userWidget;
    // （图书管理容器）：图书管理界面的容器指针
    QWidget *bookWidget;
    // （用户表格）：用户管理表格指针
    QTableWidget *userTable;
    // （图书表格）：图书管理表格指针
    QTableWidget *bookTable;
    // （借阅管理容器）：借阅管理界面的容器指针
    QWidget *borrowWidget;
    // （借阅表格）：借阅管理表格指针
    QTableWidget *borrowTable;
    // （预约管理容器）：预约管理界面的容器指针
    QWidget *reservationWidget;
    // （预约表格）：预约管理表格指针
    QTableWidget *reservationTable;
    // （统计表格）：统计报表表格指针
    QTableWidget *statisticsTable;
    // （统计报表界面）：统计报表自定义界面指针
    StatisticsWidget *statisticsWidget;

    // （用户ID查找输入框）：用户ID查找输入框指针
    QLineEdit *userIdLineEdit;
    // （用户姓名查找输入框）：用户姓名查找输入框指针
    QLineEdit *userNameLineEdit;
    // （用户查找按钮）：用户查找按钮指针
    QPushButton *userSearchBtn;
    // （用户增加按钮）：用户增加按钮指针
    QPushButton *userAddBtn;
    // （用户删除按钮）：用户删除按钮指针
    QPushButton *userDeleteBtn;
    // （用户修改按钮）：用户修改按钮指针
    QPushButton *userUpdateBtn;
    // （用户清除按钮）：用户清除按钮指针
    QPushButton *userClearBtn;

    // 修改5.16
    // （图书ISBN查找输入框）：图书ISBN查找输入框指针
    QLineEdit *bookISBNLineEdit;
    // （图书书名查找输入框）：图书书名查找输入框指针
    QLineEdit *bookTitleLineEdit;
    // （图书作者查找输入框）：图书作者查找输入框指针
    QLineEdit *bookAuthorLineEdit;
    // （图书分类查找输入框）：图书分类查找输入框指针
    QLineEdit *bookCategoryLineEdit;
    // （图书查找按钮）：图书查找按钮指针
    QPushButton *bookSearchBtn;
    // （图书增加按钮）：图书增加按钮指针
    QPushButton *bookAddBtn;
    // （图书删除按钮）：图书删除按钮指针
    QPushButton *bookDeleteBtn;
    // （图书修改按钮）：图书修改按钮指针
    QPushButton *bookUpdateBtn;
    // （图书清除按钮）：图书清除按钮指针
    QPushButton *bookClearBtn;
    // （图书排序按钮）：图书排序按钮指针
    QPushButton *bookSortBtn;
    // （图书按时间排序按钮）：图书按入库时间排序按钮指针
    QPushButton *bookSortByTimeBtn;

    // （借阅ISBN查找输入框）：借阅ISBN查找输入框指针
    QLineEdit *borrowISBNLineEdit;
    // （借阅读者ID查找输入框）：借阅读者ID查找输入框指针
    QLineEdit *borrowReaderIdLineEdit;
    // （借阅查找按钮）：借阅查找按钮指针
    QPushButton *borrowSearchBtn;
    // （借阅添加按钮）：借阅添加按钮指针
    QPushButton *borrowAddBtn;
    // （借阅归还按钮）：借阅归还按钮指针
    QPushButton *borrowReturnBtn;
    // （借阅续借按钮）：借阅续借按钮指针
    QPushButton *borrowRenewBtn;
    // 修改结束

    // （处理预约按钮）：处理预约按钮指针
    QPushButton *processReservationBtn;
    // （取消预约按钮）：取消预约按钮指针
    QPushButton *cancelReservationBtn;

    // （消息管理）：初始化消息表格
    void setupMessageWidget();
    // （显示消息）：显示消息到消息表格
    void displayMessages(const std::vector<Message> &messages);

    // （消息容器）：消息管理界面的容器指针
    QWidget *messageWidget;
    // （消息表格）：消息表格指针
    QTableWidget *messageTable;
};
#endif // ADMINWINDOW_H