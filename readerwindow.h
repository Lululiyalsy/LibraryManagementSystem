/**
 * @file readerwindow.h
 * @brief 读者窗口类定义
 *
 * ReaderWindow类是图书馆管理系统的读者主界面，提供图书查询、预约管理、
 * 借阅管理、罚款支付和消息管理等功能。
 */

#ifndef READERWINDOW_H
#define READERWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStackedWidget>
#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <vector>
#include "User.h"
#include "Message.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"

/**
 * @class ReaderWindow
 * @brief 读者窗口类
 *
 * 读者主界面窗口，提供读者端的图书馆服务功能，包括：
 * - 图书查询（多条件搜索）
 * - 图书预约（预约、取消预约）
 * - 图书借阅（借书、还书、续借）
 * - 罚款支付（支付所有罚款）
 * - 消息管理（查看、删除、标记已读）
 * - 个人信息查看
 */
class ReaderWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param user 当前登录的读者用户对象
     * @param parent 父窗口指针
     */
    explicit ReaderWindow(User *user, QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~ReaderWindow();

private slots:
    /**
     * @brief 图书查询按钮点击槽函数
     */
    void onBookSearch();

    /**
     * @brief 预约图书按钮点击槽函数
     */
    void onBookReserve();

    /**
     * @brief 取消预约按钮点击槽函数（弹出对话框输入ISBN）
     */
    void onCancelReservation();

    /**
     * @brief 查找预约按钮点击槽函数
     */
    void onSearchReservation();

    /**
     * @brief 借书按钮点击槽函数
     */
    void onBorrowBook();

    /**
     * @brief 还书按钮点击槽函数
     */
    void onReturnBook();

    /**
     * @brief 续借按钮点击槽函数
     */
    void onRenewBook();

    /**
     * @brief 查找借阅记录按钮点击槽函数
     */
    void onSearchBorrow();

    /**
     * @brief 支付所有罚款按钮点击槽函数
     */
    void onPayAllFines();

    /**
     * @brief 切换到图书查询页面
     */
    void switchToBookSearch();

    /**
     * @brief 切换到我的借阅页面
     */
    void switchToMyBorrow();

    /**
     * @brief 切换到我的预约页面
     */
    void switchToMyReservation();

    /**
     * @brief 查看消息按钮点击槽函数
     */
    void onCheckMessages();

    /**
     * @brief 退出登录按钮点击槽函数
     */
    void onLogout();

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

    /**
     * @brief 缴纳押金按钮点击槽函数
     */
    void onPayDeposit();

    /**
     * @brief 退还押金按钮点击槽函数
     */
    void onRefundDeposit();

signals:
    /**
     * @brief 退出登录信号
     * 通知主程序读者已退出登录
     */
    void logout();

private:
    /**
     * @brief 设置中心窗口
     */
    void setupCentralWidget();

    /**
     * @brief 设置图书查询页面
     */
    void setupBookSearchWidget();

    /**
     * @brief 设置我的借阅页面
     */
    void setupMyBorrowWidget();

    /**
     * @brief 设置我的预约页面
     */
    void setupMyReservationWidget();

    /**
     * @brief 设置消息管理页面
     */
    void setupMessageWidget();

    /**
     * @brief 设置个人信息页面
     */
    void setupInfoWidget();

    /**
     * @brief 显示图书列表到表格
     * @param books 图书指针列表
     */
    void displayBooks(const std::vector<const Book *> &books);

    /**
     * @brief 显示我的借阅记录
     */
    void displayMyBorrowRecords();

    /**
     * @brief 显示我的预约记录
     */
    void displayMyReservations();

    /**
     * @brief 显示消息列表到表格
     * @param messages 消息列表
     */
    void displayMessages(const std::vector<Message> &messages);

    /**
     * @brief 切换到个人信息页面
     */
    void switchToInfo();

    /**
     * @brief 更新个人信息列表
     */
    void updateInfoList();

    /**
     * @brief 显示输入对话框
     * @param title 对话框标题
     * @param label 输入标签
     * @param isPassword 是否为密码输入
     * @return QPair：first=输入内容，second=true表示取消，false表示确定
     */
    QPair<QString, bool> showInputDialog(const QString &title, const QString &label, bool isPassword = false);

    // 成员变量
    User *currentUser;             ///< 当前登录的读者用户对象
    QToolBar *toolbar;             ///< 左侧工具栏
    QStackedWidget *stackedWidget; ///< 堆叠窗口，用于管理多个页面

    // 页面容器
    QWidget *bookSearchWidget;    ///< 图书查询页面容器
    QWidget *myBorrowWidget;      ///< 我的借阅页面容器
    QWidget *myReservationWidget; ///< 我的预约页面容器
    QWidget *messageWidget;       ///< 消息管理页面容器
    QWidget *infoWidget;          ///< 个人信息页面容器

    // 表格控件
    QTableWidget *bookSearchTable;    ///< 图书查询表格
    QTableWidget *myBorrowTable;      ///< 我的借阅表格
    QTableWidget *myReservationTable; ///< 我的预约表格
    QTableWidget *messageTable;       ///< 消息表格

    // 消息管理控件
    QLineEdit *messageTimeEdit;    ///< 消息时间查找输入框
    QLineEdit *messageContentEdit; ///< 消息内容查找输入框
    QComboBox *messageStatusCombo; ///< 消息状态下拉框

    // 预约管理控件
    QLineEdit *reservationTimeEdit;    ///< 预约时间查找输入框
    QLineEdit *reservationISBNEdit;    ///< 预约ISBN查找输入框
    QLineEdit *reservationTitleEdit;   ///< 预约书名查找输入框
    QComboBox *reservationStatusCombo; ///< 预约状态下拉框

    // 图书查询控件
    QLineEdit *bookISBNLineEdit;     ///< 图书ISBN查询输入框
    QLineEdit *bookTitleLineEdit;    ///< 图书名称查询输入框
    QLineEdit *bookAuthorLineEdit;   ///< 图书作者查询输入框
    QLineEdit *bookCategoryLineEdit; ///< 图书分类查询输入框

    // 借阅管理控件
    QLineEdit *borrowISBNEdit;       ///< 借阅ISBN查询输入框
    QLineEdit *borrowTitleEdit;      ///< 借阅书名查询输入框
    QLineEdit *borrowTimeEdit;       ///< 借阅时间查询输入框
    QLineEdit *borrowDueTimeEdit;    ///< 应还时间查询输入框
    QLineEdit *borrowReturnTimeEdit; ///< 归还时间查询输入框
    QLineEdit *borrowStatusEdit;     ///< 借阅状态查询输入框
    QComboBox *borrowFineCombo;      ///< 借阅罚款状态下拉框
    QComboBox *borrowRenewCombo;     ///< 借阅续借状态下拉框

    // 操作按钮
    QPushButton *bookSearchBtn;  ///< 图书查询按钮
    QPushButton *bookReserveBtn; ///< 预约按钮
    QPushButton *borrowBtn;      ///< 借书按钮
    QPushButton *returnBtn;      ///< 还书按钮
    QPushButton *renewBtn;       ///< 续借按钮
    QPushButton *payFineBtn;     ///< 支付所有罚款按钮
    QPushButton *payDepositBtn;  ///< 缴纳押金按钮
    QPushButton *refundDepositBtn; ///< 退还押金按钮

    // 个人信息控件
    QListWidget *infoListWidget; ///< 个人信息列表
};

#endif // READERWINDOW_H