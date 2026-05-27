#ifndef READERWINDOW_H
#define READERWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStackedWidget>
#include <QWidget>
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

// 读者窗口类，提供读者用户界面功能
class ReaderWindow : public QMainWindow
{
    Q_OBJECT

public:
    // 构造函数
    explicit ReaderWindow(User *user, QWidget *parent = nullptr);
    // 析构函数
    ~ReaderWindow();

private slots:
    // 图书查询
    void onBookSearch();
    // 预约图书
    void onBookReserve();
    // 取消预约
    void onCancelReservation();
    // 借书
    void onBorrowBook();
    // 还书
    void onReturnBook();
    // 续借
    void onRenewBook();
    // 切换到图书查询页面
    void switchToBookSearch();
    // 切换到我的借阅页面
    void switchToMyBorrow();
    // 切换到我的预约页面
    void switchToMyReservation();
    // 查看消息
    void onCheckMessages();
    // 退出登录
    void onLogout();
    // 删除消息
    void onDeleteMessage();
    // 清除所有消息
    void onClearAllMessages();
    // 全部设为已读
    void onMarkAllRead();
    // 查找消息
    void onSearchMessage();

signals:
    // 退出登录信号
    void logout();

private:
    User *currentUser;             // 当前登录的用户对象
    QToolBar *toolbar;             // 左侧工具栏
    QStackedWidget *stackedWidget; // 堆叠窗口，用于管理多个页面

    QWidget *bookSearchWidget;    // 图书查询页面
    QWidget *myBorrowWidget;      // 我的借阅页面
    QWidget *myReservationWidget; // 我的预约页面
    QWidget *messageWidget;       // 消息管理页面

    QTableWidget *bookSearchTable;    // 图书查询表格
    QTableWidget *myBorrowTable;      // 我的借阅表格
    QTableWidget *myReservationTable; // 我的预约表格
    QTableWidget *messageTable;       // 消息表格

    QLineEdit *messageTimeEdit;    // 消息时间查找输入框
    QLineEdit *messageContentEdit; // 消息内容查找输入框
    QComboBox *messageStatusCombo; // 消息状态下拉框

    QLineEdit *bookISBNLineEdit;     // 图书ISBN查询输入框
    QLineEdit *bookTitleLineEdit;    // 图书名称查询输入框
    QLineEdit *bookAuthorLineEdit;   // 图书作者查询输入框
    QLineEdit *bookCategoryLineEdit; // 图书分类查询输入框

    QPushButton *bookSearchBtn;    // 图书查询按钮
    QPushButton *bookReserveBtn;   // 预约按钮
    QPushButton *borrowBtn;        // 借书按钮
    QPushButton *returnBtn;        // 还书按钮
    QPushButton *renewBtn;         // 续借按钮
    QPushButton *cancelReserveBtn; // 取消预约按钮

    // 设置中心窗口
    void setupCentralWidget();
    // 设置图书查询页面
    void setupBookSearchWidget();
    // 设置我的借阅页面
    void setupMyBorrowWidget();
    // 设置我的预约页面
    void setupMyReservationWidget();
    // 设置消息管理页面
    void setupMessageWidget();
    // 显示图书列表
    void displayBooks(const std::vector<const Book *> &books);
    // 显示我的借阅记录
    void displayMyBorrowRecords();
    // 显示我的预约记录
    void displayMyReservations();
    // 显示消息列表
    void displayMessages(const std::vector<Message> &messages);
    // 显示输入对话框
    QPair<QString, bool> showInputDialog(const QString &title, const QString &label, bool isPassword = false);
};

#endif // READERWINDOW_H