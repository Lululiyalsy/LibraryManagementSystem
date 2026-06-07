/**
 * @file adminwindow.h
 * @brief 管理员窗口类定义
 *
 * AdminWindow类是图书馆管理系统的管理员主界面，提供用户管理、图书管理、
 * 借阅管理、预约管理、统计报表和消息管理等功能。
 */

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
#include <QComboBox>
#include "User.h"
#include "Message.h"
#include "statisticswidget.h"

/**
 * @class AdminWindow
 * @brief 管理员窗口类
 *
 * 管理员主界面窗口，提供完整的图书馆管理功能，包括：
 * - 用户管理（增删改查）
 * - 图书管理（增删改查、排序）
 * - 借阅管理（查询、续借审核）
 * - 预约管理（审核、查询）
 * - 统计报表（数据统计）
 * - 消息管理（查看、删除、标记已读）
 */
class AdminWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param user 当前登录的管理员用户对象
     * @param parent 父窗口指针
     */
    explicit AdminWindow(User *user, QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~AdminWindow();

private slots:
    /**
     * @brief 用户管理按钮点击槽函数
     */
    void onUserManagement();

    /**
     * @brief 图书管理按钮点击槽函数
     */
    void onBookManagement();

    /**
     * @brief 借阅管理按钮点击槽函数
     */
    void onBorrowManagement();

    /**
     * @brief 预约管理按钮点击槽函数
     */
    void onReservationManagement();

    /**
     * @brief 统计报表按钮点击槽函数
     */
    void onStatistics();

    /**
     * @brief 审核预约按钮点击槽函数
     */
    void onProcessReservation();

    /**
     * @brief 查询预约按钮点击槽函数
     */
    void onSearchReservation();

    /**
     * @brief 退出登录按钮点击槽函数
     */
    void onLogout();

    /**
     * @brief 消息管理按钮点击槽函数
     */
    void onMessage();

    /**
     * @brief 删除选中消息
     */
    void onDeleteMessage();

    /**
     * @brief 清除所有消息
     */
    void onClearAllMessages();

    /**
     * @brief 将所有未读消息设为已读
     */
    void onMarkAllRead();

    /**
     * @brief 查找消息
     */
    void onSearchMessage();

signals:
    /**
     * @brief 退出登录信号
     * 通知主程序管理员已退出登录
     */
    void logout();

private:
    /**
     * @brief 初始化左侧工具栏
     */
    void setupToolbar();

    /**
     * @brief 初始化中心堆叠部件
     */
    void setupCentralWidget();

    /**
     * @brief 初始化用户管理表格
     */
    void setupUserTable();

    /**
     * @brief 初始化图书管理表格
     */
    void setupBookTable();

    /**
     * @brief 初始化借阅管理表格
     */
    void setupBorrowTable();

    /**
     * @brief 初始化预约管理表格
     */
    void setupReservationTable();

    /**
     * @brief 初始化统计报表表格
     */
    void setupStatisticsTable();

    /**
     * @brief 从数据管理器加载用户数据到表格
     */
    void loadUserData();

    /**
     * @brief 从数据管理器加载图书数据到表格
     */
    void loadBookData();

    /**
     * @brief 从数据管理器加载借阅数据到表格
     */
    void loadBorrowData();

    /**
     * @brief 从数据管理器加载预约数据到表格
     */
    void loadReservationData();

    /**
     * @brief 从数据管理器加载统计数据到表格
     */
    void loadStatisticsData();

    /**
     * @brief 显示带中文按钮的输入对话框
     * @param title 对话框标题
     * @param label 输入标签
     * @param required 是否必填
     * @return QPair：first=输入内容，second=true表示取消，false表示确定
     */
    QPair<QString, bool> showInputDialog(const QString &title, const QString &label, bool required = false);

    /**
     * @brief 用户查找按钮点击处理
     */
    void onUserSearch();

    /**
     * @brief 角色筛选下拉框改变处理
     *
     * 当角色筛选下拉框选择改变时，重新执行搜索以应用角色过滤。
     */
    void onUserRoleFilter();

    /**
     * @brief 将用户数据显示到表格
     * @param users 用户列表
     */
    void displayUsers(const std::vector<User *> &users);

    /**
     * @brief 用户增加按钮点击处理
     */
    void onUserAdd();

    /**
     * @brief 用户删除按钮点击处理
     */
    void onUserDelete();

    /**
     * @brief 用户修改按钮点击处理
     */
    void onUserUpdate();

    /**
     * @brief 用户清除按钮点击处理
     */
    void onUserClear();

    /**
     * @brief 图书查找按钮点击处理
     */
    void onBookSearch();

    /**
     * @brief 将图书数据显示到表格
     * @param books 图书指针列表
     */
    void displayBooks(const std::vector<const Book *> &books);

    /**
     * @brief 图书增加按钮点击处理
     */
    void onBookAdd();

    /**
     * @brief 图书删除按钮点击处理
     */
    void onBookDelete();

    /**
     * @brief 图书修改按钮点击处理
     */
    void onBookUpdate();

    /**
     * @brief 图书清除按钮点击处理
     */
    void onBookClear();

    /**
     * @brief 图书按借阅次数排序按钮点击处理
     */
    void onBookSort();

    /**
     * @brief 图书按入库时间排序按钮点击处理
     */
    void onBookSortByTime();

    /**
     * @brief 借阅查找按钮点击处理
     */
    void onBorrowSearch();

    /**
     * @brief 将借阅记录显示到表格
     * @param records 借阅记录列表
     */
    void displayBorrowRecords(const std::vector<BorrowRecord> &records);

    /**
     * @brief 借阅续借审核按钮点击处理
     */
    void onBorrowRenewAudit();

    /**
     * @brief 初始化消息表格
     */
    void setupMessageWidget();

    /**
     * @brief 显示消息到消息表格
     * @param messages 消息列表
     */
    void displayMessages(const std::vector<Message> &messages);

private:
    User *currentUser;             ///< 当前登录的管理员用户对象
    QToolBar *toolbar;             ///< 左侧工具栏
    QStackedWidget *stackedWidget; ///< 中心堆叠部件，用于切换不同视图

    // 界面容器
    QWidget *userWidget;        ///< 用户管理界面容器
    QWidget *bookWidget;        ///< 图书管理界面容器
    QWidget *borrowWidget;      ///< 借阅管理界面容器
    QWidget *reservationWidget; ///< 预约管理界面容器
    QWidget *messageWidget;     ///< 消息管理界面容器

    // 表格控件
    QTableWidget *userTable;            ///< 用户管理表格
    QTableWidget *bookTable;            ///< 图书管理表格
    QTableWidget *borrowTable;          ///< 借阅管理表格
    QTableWidget *reservationTable;     ///< 预约管理表格
    QTableWidget *statisticsTable;      ///< 统计报表表格
    StatisticsWidget *statisticsWidget; ///< 统计报表自定义界面

    // 用户管理控件
    QLineEdit *userIdLineEdit;   ///< 用户ID查找输入框
    QLineEdit *userNameLineEdit; ///< 用户姓名查找输入框
    QPushButton *userSearchBtn;  ///< 用户查找按钮
    QPushButton *userAddBtn;     ///< 用户增加按钮
    QPushButton *userDeleteBtn;  ///< 用户删除按钮
    QPushButton *userUpdateBtn;  ///< 用户修改按钮
    QPushButton *userClearBtn;   ///< 用户清除按钮
    QComboBox *userRoleCombo;    ///< 用户角色选择下拉框

    // 图书管理控件
    QLineEdit *bookISBNLineEdit;     ///< 图书ISBN查找输入框
    QLineEdit *bookTitleLineEdit;    ///< 图书书名查找输入框
    QLineEdit *bookAuthorLineEdit;   ///< 图书作者查找输入框
    QLineEdit *bookCategoryLineEdit; ///< 图书分类查找输入框
    QPushButton *bookSearchBtn;      ///< 图书查找按钮
    QPushButton *bookAddBtn;         ///< 图书增加按钮
    QPushButton *bookDeleteBtn;      ///< 图书删除按钮
    QPushButton *bookUpdateBtn;      ///< 图书修改按钮
    QPushButton *bookClearBtn;       ///< 图书清除按钮
    QPushButton *bookSortBtn;        ///< 图书按借阅次数排序按钮
    QPushButton *bookSortByTimeBtn;  ///< 图书按入库时间排序按钮

    // 借阅管理控件
    QLineEdit *borrowISBNLineEdit;       ///< 借阅ISBN查找输入框
    QLineEdit *borrowTitleLineEdit;      ///< 借阅书名查找输入框
    QLineEdit *borrowReaderIdLineEdit;   ///< 借阅读者ID查找输入框
    QLineEdit *borrowTimeLineEdit;       ///< 借阅时间查找输入框
    QLineEdit *borrowDueTimeLineEdit;    ///< 借阅应还时间查找输入框
    QLineEdit *borrowReturnTimeLineEdit; ///< 借阅归还时间查找输入框
    QLineEdit *borrowStatusEdit;         ///< 借阅状态查找输入框
    QComboBox *borrowFineCombo;          ///< 借阅罚款状态下拉框
    QComboBox *borrowRenewCombo;         ///< 借阅续借状态下拉框
    QPushButton *borrowSearchBtn;        ///< 借阅查找按钮
    QPushButton *borrowRenewAuditBtn;    ///< 借阅续借审核按钮

    // 预约管理控件
    QPushButton *processReservationBtn;     ///< 审核预约按钮
    QLineEdit *reservationISBNLineEdit;     ///< 预约ISBN查找输入框
    QLineEdit *reservationReaderIdLineEdit; ///< 预约读者ID查找输入框
    QLineEdit *reservationTimeLineEdit;     ///< 预约时间查找输入框
    QComboBox *reservationStatusCombo;      ///< 预约状态下拉框

    // 消息管理控件
    QTableWidget *messageTable;       ///< 消息表格
    QLineEdit *messageReaderIdEdit;   ///< 消息读者ID查找输入框
    QLineEdit *messageReaderNameEdit; ///< 消息读者姓名查找输入框
    QLineEdit *messageTimeEdit;       ///< 消息时间查找输入框
    QLineEdit *messageContentEdit;    ///< 消息内容查找输入框
    QComboBox *messageStatusCombo;    ///< 消息状态下拉框
};

#endif // ADMINWINDOW_H