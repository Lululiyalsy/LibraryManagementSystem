#include "adminwindow.h"
#include "statisticswidget.h"
#include "DataManager.h"
#include "Admin.h"
#include "User.h"
#include "Book.h"
#include "BorrowRecord.h"
#include "Reservation.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QStackedWidget>
#include <QToolBar>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QDialog>
#include <QIcon>
#include <QHeaderView>
#include <QAction>
#include <QActionGroup>
#include <QMessageBox>
#include <QInputDialog>
#include <QLabel>
#include <QDateTime>
#include <vector>
#include <algorithm>

// （构造函数）：创建管理员窗口实例，初始化窗口属性和部件
AdminWindow::AdminWindow(::User *user, QWidget *parent)
    : QMainWindow(parent), currentUser(user), toolbar(nullptr), stackedWidget(nullptr), userWidget(nullptr), bookWidget(nullptr), borrowWidget(nullptr), reservationWidget(nullptr), userTable(nullptr), bookTable(nullptr), borrowTable(nullptr), reservationTable(nullptr), statisticsTable(nullptr), statisticsWidget(nullptr), userIdLineEdit(nullptr), userNameLineEdit(nullptr), userSearchBtn(nullptr), userAddBtn(nullptr), userDeleteBtn(nullptr), userUpdateBtn(nullptr), userClearBtn(nullptr), bookISBNLineEdit(nullptr), bookTitleLineEdit(nullptr), bookAuthorLineEdit(nullptr), bookCategoryLineEdit(nullptr), bookSearchBtn(nullptr), bookAddBtn(nullptr), bookDeleteBtn(nullptr), bookUpdateBtn(nullptr), bookClearBtn(nullptr), bookSortBtn(nullptr), bookSortByTimeBtn(nullptr), borrowISBNLineEdit(nullptr), borrowReaderIdLineEdit(nullptr), borrowSearchBtn(nullptr), borrowAddBtn(nullptr), borrowReturnBtn(nullptr), borrowRenewBtn(nullptr), processReservationBtn(nullptr), cancelReservationBtn(nullptr), messageWidget(nullptr), messageTable(nullptr)
{
    // （设置窗口大小）：设置初始窗口大小为800x800
    resize(800, 800);
    // （设置窗口图标）：设置窗口图标为图书图标
    setWindowIcon(QIcon(":/image/book.png"));
    // （禁用关闭按钮）：移除窗口关闭按钮
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    // （设置窗口标题）：设置窗口标题为用户身份和姓名
    if (currentUser)
    {
        setWindowTitle(QString("%1 - %2").arg(currentUser->typeToIdentity()).arg(currentUser->getName()));
    }

    // （初始化工具栏）：初始化左侧工具栏
    setupToolbar();
    // （初始化中心部件）：初始化中心堆叠部件
    setupCentralWidget();

    // （默认显示）：默认显示用户管理界面
    onUserManagement();
}

// （析构函数）：释放窗口资源
AdminWindow::~AdminWindow()
{
}

// （初始化工具栏）：创建左侧工具栏，添加功能按钮
void AdminWindow::setupToolbar()
{
    // （创建工具栏）：创建工具栏实例
    toolbar = new QToolBar(this);
    // （设置工具栏属性）：设置工具栏不可移动
    toolbar->setMovable(false);
    // （设置按钮样式）：设置按钮图标在上，文字在下
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    // （添加工具栏到窗口）：将工具栏添加到左侧
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // （创建动作）：创建用户管理动作
    QAction *userAct = new QAction(QIcon(":/image/user.png"), "用户管理", this);
    // （创建动作）：创建图书管理动作
    QAction *bookAct = new QAction(QIcon(":/image/book2.png"), "图书管理", this);
    // （创建动作）：创建借阅管理动作
    QAction *borrowAct = new QAction(QIcon(":/image/borrow.png"), "借阅管理", this);
    // （创建动作）：创建预约管理动作
    QAction *reservationAct = new QAction(QIcon(":/image/reservation.png"), "预约管理", this);
    // （创建动作）：创建统计报表动作
    QAction *statisticsAct = new QAction(QIcon(":/image/report.png"), "统计报表", this);
    // （创建动作）：创建消息管理动作
    QAction *messageAct = new QAction(QIcon(":/image/message.png"), "消息管理", this);
    // （创建动作）：创建退出登录动作
    QAction *logoutAct = new QAction(QIcon(":/image/logout.png"), "退出登录", this);

    // （连接信号槽）：连接用户管理动作到槽函数
    connect(userAct, &QAction::triggered, this, &AdminWindow::onUserManagement);
    // （连接信号槽）：连接图书管理动作到槽函数
    connect(bookAct, &QAction::triggered, this, &AdminWindow::onBookManagement);
    // （连接信号槽）：连接借阅管理动作到槽函数
    connect(borrowAct, &QAction::triggered, this, &AdminWindow::onBorrowManagement);
    // （连接信号槽）：连接预约管理动作到槽函数
    connect(reservationAct, &QAction::triggered, this, &AdminWindow::onReservationManagement);
    // （连接信号槽）：连接统计报表动作到槽函数
    connect(statisticsAct, &QAction::triggered, this, &AdminWindow::onStatistics);
    // （连接信号槽）：连接消息管理动作到槽函数
    connect(messageAct, &QAction::triggered, this, &AdminWindow::onMessage);
    // （连接信号槽）：连接退出登录动作到槽函数
    connect(logoutAct, &QAction::triggered, this, &AdminWindow::onLogout);

    // （添加动作）：将动作添加到工具栏
    toolbar->addAction(userAct);
    toolbar->addAction(bookAct);
    toolbar->addAction(borrowAct);
    toolbar->addAction(reservationAct);
    toolbar->addAction(statisticsAct);
    toolbar->addAction(messageAct);
    toolbar->addAction(logoutAct);
}

// （初始化中心部件）：创建堆叠部件，管理多个表格视图
void AdminWindow::setupCentralWidget()
{
    // （创建堆叠部件）：创建堆叠部件实例
    stackedWidget = new QStackedWidget(this);
    // （设置中心部件）：将堆叠部件设置为中心部件
    setCentralWidget(stackedWidget);

    // （初始化表格）：初始化各个功能表格
    setupUserTable();
    setupBookTable();
    setupBorrowTable();
    setupReservationTable();
    setupStatisticsTable(); // 内部已经完成了 new
    setupMessageWidget();

    stackedWidget->addWidget(userWidget);
    stackedWidget->addWidget(bookWidget);
    stackedWidget->addWidget(borrowWidget);
    stackedWidget->addWidget(reservationWidget);
    stackedWidget->addWidget(statisticsWidget);
    stackedWidget->addWidget(messageWidget);
}

// （初始化用户表格）：创建用户管理表格，设置列和属性
void AdminWindow::setupUserTable()
{
    // （创建用户管理容器）：创建用户管理界面的容器
    userWidget = new QWidget(this);
    QVBoxLayout *userMainLayout = new QVBoxLayout(userWidget);

    // （创建操作区容器）：创建操作区的容器
    QWidget *operationWidget = new QWidget(userWidget);
    QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);

    // （创建ID输入框）：创建用户ID查找输入框，设置提示文字
    userIdLineEdit = new QLineEdit(operationWidget);
    userIdLineEdit->setPlaceholderText("输入用户ID");
    userIdLineEdit->setFixedWidth(100);

    // （创建姓名输入框）：创建用户姓名查找输入框，设置提示文字
    userNameLineEdit = new QLineEdit(operationWidget);
    userNameLineEdit->setPlaceholderText("输入用户姓名");
    userNameLineEdit->setFixedWidth(120);

    // （创建按钮）：创建查找按钮
    userSearchBtn = new QPushButton("查找", operationWidget);
    // （创建按钮）：创建增加按钮
    userAddBtn = new QPushButton("增加", operationWidget);
    // （创建按钮）：创建删除按钮
    userDeleteBtn = new QPushButton("删除", operationWidget);
    // （创建按钮）：创建修改按钮
    userUpdateBtn = new QPushButton("修改", operationWidget);
    // （创建按钮）：创建清除按钮
    userClearBtn = new QPushButton("清除所有用户", operationWidget);

    // （设置布局）：将控件添加到操作区布局
    operationLayout->addWidget(userIdLineEdit);
    operationLayout->addWidget(userNameLineEdit);
    operationLayout->addWidget(userSearchBtn);
    operationLayout->addWidget(userAddBtn);
    operationLayout->addWidget(userDeleteBtn);
    operationLayout->addWidget(userUpdateBtn);
    operationLayout->addWidget(userClearBtn);
    operationLayout->addStretch();

    // （创建表格）：创建用户表格实例
    userTable = new QTableWidget(userWidget);
    // （设置列数）：设置用户表格列数为6
    userTable->setColumnCount(6);
    // （设置表头）：设置用户表格表头
    userTable->setHorizontalHeaderLabels({"ID", "类型", "姓名", "密码", "电话", "邮箱"});
    // （设置列调整模式）：列自动拉伸填充
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // （设置行调整模式）：行高根据内容自动调整
    userTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // （设置编辑属性）：表格只读，不可编辑
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // （设置选择行为）：选择整行
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // （设置选择模式）：单选模式
    userTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // （设置主布局）：将操作区和表格添加到主布局
    userMainLayout->addWidget(operationWidget);
    userMainLayout->addWidget(userTable);

    // （连接按钮信号槽）：连接用户管理按钮信号槽
    connect(userSearchBtn, &QPushButton::clicked, this, &AdminWindow::onUserSearch);
    connect(userAddBtn, &QPushButton::clicked, this, &AdminWindow::onUserAdd);
    connect(userDeleteBtn, &QPushButton::clicked, this, &AdminWindow::onUserDelete);
    connect(userUpdateBtn, &QPushButton::clicked, this, &AdminWindow::onUserUpdate);
    connect(userClearBtn, &QPushButton::clicked, this, &AdminWindow::onUserClear);
}

// （初始化图书表格）：创建图书管理表格，设置列和属性
void AdminWindow::setupBookTable()
{
    // 修改5.16
    //  （创建图书管理容器）：创建图书管理界面的容器
    bookWidget = new QWidget(this);
    QVBoxLayout *bookMainLayout = new QVBoxLayout(bookWidget);

    // （创建操作区容器）：创建操作区的容器
    QWidget *bookOperationWidget = new QWidget(bookWidget);
    QHBoxLayout *bookOperationLayout = new QHBoxLayout(bookOperationWidget);

    // （创建ISBN输入框）：创建图书ISBN查找输入框
    bookISBNLineEdit = new QLineEdit(bookOperationWidget);
    bookISBNLineEdit->setPlaceholderText("ISBN");
    bookISBNLineEdit->setFixedWidth(120);

    // （创建书名输入框）：创建图书书名查找输入框
    bookTitleLineEdit = new QLineEdit(bookOperationWidget);
    bookTitleLineEdit->setPlaceholderText("书名");
    bookTitleLineEdit->setFixedWidth(100);

    // （创建作者输入框）：创建图书作者查找输入框
    bookAuthorLineEdit = new QLineEdit(bookOperationWidget);
    bookAuthorLineEdit->setPlaceholderText("作者");
    bookAuthorLineEdit->setFixedWidth(100);

    // （创建分类输入框）：创建图书分类查找输入框
    bookCategoryLineEdit = new QLineEdit(bookOperationWidget);
    bookCategoryLineEdit->setPlaceholderText("分类");
    bookCategoryLineEdit->setFixedWidth(100);

    // （创建按钮）：创建查找按钮
    bookSearchBtn = new QPushButton("查找", bookOperationWidget);
    // （创建按钮）：创建增加按钮
    bookAddBtn = new QPushButton("增加", bookOperationWidget);
    // （创建按钮）：创建删除按钮
    bookDeleteBtn = new QPushButton("删除", bookOperationWidget);
    // （创建按钮）：创建修改按钮
    bookUpdateBtn = new QPushButton("修改", bookOperationWidget);
    // （创建按钮）：创建清除按钮
    bookClearBtn = new QPushButton("清除所有图书", bookOperationWidget);
    // （创建按钮）：创建排序按钮
    bookSortBtn = new QPushButton("按借阅排序", bookOperationWidget);
    // （创建按钮）：创建按时间排序按钮
    bookSortByTimeBtn = new QPushButton("按入库时间排序", bookOperationWidget);

    // （设置布局）：将控件添加到操作区布局
    bookOperationLayout->addWidget(bookISBNLineEdit);
    bookOperationLayout->addWidget(bookTitleLineEdit);
    bookOperationLayout->addWidget(bookAuthorLineEdit);
    bookOperationLayout->addWidget(bookCategoryLineEdit);
    bookOperationLayout->addWidget(bookSearchBtn);
    bookOperationLayout->addWidget(bookAddBtn);
    bookOperationLayout->addWidget(bookDeleteBtn);
    bookOperationLayout->addWidget(bookUpdateBtn);
    bookOperationLayout->addWidget(bookClearBtn);
    bookOperationLayout->addWidget(bookSortBtn);
    bookOperationLayout->addWidget(bookSortByTimeBtn);
    bookOperationLayout->addStretch();

    // （创建表格）：创建图书表格实例
    bookTable = new QTableWidget(bookWidget);
    // （设置列数）：设置图书表格列数为10
    bookTable->setColumnCount(10);
    // （设置表头）：设置图书表格表头
    bookTable->setHorizontalHeaderLabels({"ISBN", "书名", "作者", "分类", "库存", "入库时间", "借阅次数", "当前借出", "状态", "预约人数"});
    // （设置列调整模式）：列自动拉伸填充
    bookTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // （设置行调整模式）：行高根据内容自动调整
    bookTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // （设置编辑属性）：表格只读，不可编辑
    bookTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // （设置选择行为）：选择整行
    bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // （设置选择模式）：单选模式
    bookTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // （设置主布局）：将操作区和表格添加到主布局
    bookMainLayout->addWidget(bookOperationWidget);
    bookMainLayout->addWidget(bookTable);

    // （连接按钮信号槽）：连接图书管理按钮信号槽
    connect(bookSearchBtn, &QPushButton::clicked, this, &AdminWindow::onBookSearch);
    connect(bookAddBtn, &QPushButton::clicked, this, &AdminWindow::onBookAdd);
    connect(bookDeleteBtn, &QPushButton::clicked, this, &AdminWindow::onBookDelete);
    connect(bookUpdateBtn, &QPushButton::clicked, this, &AdminWindow::onBookUpdate);
    connect(bookClearBtn, &QPushButton::clicked, this, &AdminWindow::onBookClear);
    connect(bookSortBtn, &QPushButton::clicked, this, &AdminWindow::onBookSort);
    connect(bookSortByTimeBtn, &QPushButton::clicked, this, &AdminWindow::onBookSortByTime);

    // 修改结束
}

// （初始化借阅表格）：创建借阅管理表格，设置列和属性
void AdminWindow::setupBorrowTable()
{
    // （创建借阅管理容器）：创建借阅管理界面的容器
    borrowWidget = new QWidget(this);
    QVBoxLayout *borrowMainLayout = new QVBoxLayout(borrowWidget);

    // （创建操作区容器）：创建操作区的容器
    QWidget *borrowOperationWidget = new QWidget(borrowWidget);
    QHBoxLayout *borrowOperationLayout = new QHBoxLayout(borrowOperationWidget);

    // （创建ISBN输入框）：创建图书ISBN查找输入框
    borrowISBNLineEdit = new QLineEdit(borrowOperationWidget);
    borrowISBNLineEdit->setPlaceholderText("ISBN");
    borrowISBNLineEdit->setFixedWidth(120);

    // （创建读者ID输入框）：创建读者ID查找输入框
    borrowReaderIdLineEdit = new QLineEdit(borrowOperationWidget);
    borrowReaderIdLineEdit->setPlaceholderText("读者ID");
    borrowReaderIdLineEdit->setFixedWidth(100);

    // （创建按钮）：创建查找按钮
    borrowSearchBtn = new QPushButton("查找", borrowOperationWidget);
    // （创建按钮）：创建借书按钮
    borrowAddBtn = new QPushButton("借书", borrowOperationWidget);
    // （创建按钮）：创建还书按钮
    borrowReturnBtn = new QPushButton("还书", borrowOperationWidget);
    // （创建按钮）：创建续借按钮
    borrowRenewBtn = new QPushButton("续借", borrowOperationWidget);

    // （设置布局）：将控件添加到操作区布局
    borrowOperationLayout->addWidget(borrowISBNLineEdit);
    borrowOperationLayout->addWidget(borrowReaderIdLineEdit);
    borrowOperationLayout->addWidget(borrowSearchBtn);
    borrowOperationLayout->addWidget(borrowAddBtn);
    borrowOperationLayout->addWidget(borrowReturnBtn);
    borrowOperationLayout->addWidget(borrowRenewBtn);
    borrowOperationLayout->addStretch();

    // （创建表格）：创建借阅表格实例
    borrowTable = new QTableWidget(borrowWidget);
    // （设置列数）：设置借阅表格列数为8
    borrowTable->setColumnCount(8);
    // （设置表头）：设置借阅表格表头
    borrowTable->setHorizontalHeaderLabels({"ISBN", "书名", "读者ID", "读者姓名", "借阅时间", "应还时间", "归还时间", "状态"});
    // （设置列调整模式）：列自动拉伸填充
    borrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // （设置行调整模式）：行高根据内容自动调整
    borrowTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // （设置编辑属性）：表格只读，不可编辑
    borrowTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // （设置选择行为）：选择整行
    borrowTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // （设置选择模式）：单选模式
    borrowTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // （设置主布局）：将操作区和表格添加到主布局
    borrowMainLayout->addWidget(borrowOperationWidget);
    borrowMainLayout->addWidget(borrowTable);

    // （连接按钮信号槽）：连接借阅管理按钮信号槽
    connect(borrowSearchBtn, &QPushButton::clicked, this, &AdminWindow::onBorrowSearch);
    connect(borrowAddBtn, &QPushButton::clicked, this, &AdminWindow::onBorrowAdd);
    connect(borrowReturnBtn, &QPushButton::clicked, this, &AdminWindow::onBorrowReturn);
    connect(borrowRenewBtn, &QPushButton::clicked, this, &AdminWindow::onBorrowRenew);
}

// （初始化预约表格）：创建预约管理表格，设置列和属性
void AdminWindow::setupReservationTable()
{
    // （创建预约管理容器）：创建预约管理界面的容器
    reservationWidget = new QWidget(this);
    QVBoxLayout *reservationMainLayout = new QVBoxLayout(reservationWidget);

    // （创建操作区容器）：创建操作区的容器
    QWidget *reservationOperationWidget = new QWidget(reservationWidget);
    QHBoxLayout *reservationOperationLayout = new QHBoxLayout(reservationOperationWidget);

    // （创建按钮）：创建处理预约按钮
    processReservationBtn = new QPushButton("处理预约", reservationOperationWidget);
    processReservationBtn->setStyleSheet("QPushButton { background-color: #27ae60; color: white; border-radius: 4px; padding: 6px 12px; font-size: 13px; } QPushButton:hover { background-color: #219a52; }");

    // （创建按钮）：创建取消预约按钮
    cancelReservationBtn = new QPushButton("取消预约", reservationOperationWidget);
    cancelReservationBtn->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; border-radius: 4px; padding: 6px 12px; font-size: 13px; } QPushButton:hover { background-color: #c0392b; }");

    // （设置布局）：将控件添加到操作区布局
    reservationOperationLayout->addWidget(processReservationBtn);
    reservationOperationLayout->addWidget(cancelReservationBtn);
    reservationOperationLayout->addStretch();

    // （创建表格）：创建预约表格实例
    reservationTable = new QTableWidget(reservationWidget);
    // （设置列数）：设置预约表格列数为5
    reservationTable->setColumnCount(5);
    // （设置表头）：设置预约表格表头
    reservationTable->setHorizontalHeaderLabels({"ISBN", "读者ID", "预约时间", "状态", "书名"});
    // （设置列调整模式）：列自动拉伸填充
    reservationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // （设置行调整模式）：行高根据内容自动调整
    reservationTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // （设置编辑属性）：表格只读，不可编辑
    reservationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // （设置选择行为）：选择整行
    reservationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // （设置选择模式）：单选模式
    reservationTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // （设置主布局）：将操作区和表格添加到主布局
    reservationMainLayout->addWidget(reservationOperationWidget);
    reservationMainLayout->addWidget(reservationTable);

    // （连接按钮信号槽）：连接预约管理按钮信号槽
    connect(processReservationBtn, &QPushButton::clicked, this, &AdminWindow::onProcessReservation);
    connect(cancelReservationBtn, &QPushButton::clicked, this, &AdminWindow::onCancelReservation);
}

// （初始化统计表格）：创建统计报表表格，设置列和属性
void AdminWindow::setupStatisticsTable()
{
    statisticsWidget = new StatisticsWidget(this);
    // // （设置表头）：设置统计表格表头
    // statisticsTable->setHorizontalHeaderLabels({"排名", "项目", "数量"});
    // // （设置列调整模式）：列自动拉伸填充
    // statisticsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // // （设置行调整模式）：行高根据内容自动调整
    // statisticsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    // // （设置编辑属性）：表格只读，不可编辑
    // statisticsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // // （设置选择行为）：选择整行
    // statisticsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    // // （设置选择模式）：单选模式
    // statisticsTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

// （加载用户数据）：从数据管理器加载用户数据到表格
void AdminWindow::loadUserData()
{
    // （获取数据管理器）：获取数据管理器单例实例
    DataManager *dm = DataManager::getInstance();
    // （清空表格）：清空用户表格内容
    userTable->setRowCount(0);

    // （获取所有用户）：获取所有用户数据
    std::vector<::User *> users = dm->getUsers();

    // （填充表格）：将用户数据填入表格
    for (::User *user : users)
    {
        int row = userTable->rowCount();
        userTable->insertRow(row);

        userTable->setItem(row, 0, new QTableWidgetItem(user->getID()));
        userTable->setItem(row, 1, new QTableWidgetItem(user->getType() == 1 ? "管理员" : "读者"));
        userTable->setItem(row, 2, new QTableWidgetItem(user->getName()));
        userTable->setItem(row, 3, new QTableWidgetItem(user->getPassword()));
        userTable->setItem(row, 4, new QTableWidgetItem(user->getPhone()));
        userTable->setItem(row, 5, new QTableWidgetItem(user->getEmail()));
    }
}

// （加载图书数据）：从数据管理器加载图书数据到表格
void AdminWindow::loadBookData()
{
    DataManager *dm = DataManager::getInstance();
    bookTable->setRowCount(0);

    Admin *admin = dynamic_cast<Admin *>(currentUser);
    if (admin)
    {
        std::vector<const Book *> books = admin->findBook("", "", "", "");
        for (const auto *book : books)
        {
            int row = bookTable->rowCount();
            bookTable->insertRow(row);
            bookTable->setItem(row, 0, new QTableWidgetItem(book->getISBN()));
            bookTable->setItem(row, 1, new QTableWidgetItem(book->getTitle()));
            bookTable->setItem(row, 2, new QTableWidgetItem(book->getAuthor()));
            bookTable->setItem(row, 3, new QTableWidgetItem(book->getCategory()));
            bookTable->setItem(row, 4, new QTableWidgetItem(QString::number(book->getStock())));
            bookTable->setItem(row, 5, new QTableWidgetItem(book->getInStockTime().toString("yyyy-MM-dd HH:mm:ss")));
            bookTable->setItem(row, 6, new QTableWidgetItem(QString::number(book->getBorrowCount())));
            bookTable->setItem(row, 7, new QTableWidgetItem(QString::number(book->getCurrentBorrowed())));
            bookTable->setItem(row, 8, new QTableWidgetItem(book->getStatus()));
            bookTable->setItem(row, 9, new QTableWidgetItem(QString::number(book->getReservationCount())));
        }
    }
}

// （图书查找）：查找按钮点击处理
void AdminWindow::onBookSearch()
{
    QString isbn = bookISBNLineEdit->text().trimmed();
    QString title = bookTitleLineEdit->text().trimmed();
    QString author = bookAuthorLineEdit->text().trimmed();
    QString category = bookCategoryLineEdit->text().trimmed();

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<const Book *> results = admin->findBook(isbn, title, author, category);
        displayBooks(results);
    }
}

// （显示图书）：将图书数据显示到表格
void AdminWindow::displayBooks(const std::vector<const Book *> &books)
{
    bookTable->setRowCount(0);
    for (const auto *book : books)
    { // 改为指针遍历
        int row = bookTable->rowCount();
        bookTable->insertRow(row);
        bookTable->setItem(row, 0, new QTableWidgetItem(book->getISBN())); // 使用 ->
        bookTable->setItem(row, 1, new QTableWidgetItem(book->getTitle()));
        bookTable->setItem(row, 2, new QTableWidgetItem(book->getAuthor()));
        bookTable->setItem(row, 3, new QTableWidgetItem(book->getCategory()));
        bookTable->setItem(row, 4, new QTableWidgetItem(QString::number(book->getStock())));
        bookTable->setItem(row, 5, new QTableWidgetItem(book->getInStockTime().toString("yyyy-MM-dd HH:mm:ss")));
        bookTable->setItem(row, 6, new QTableWidgetItem(QString::number(book->getBorrowCount())));
        bookTable->setItem(row, 7, new QTableWidgetItem(QString::number(book->getCurrentBorrowed())));
        bookTable->setItem(row, 8, new QTableWidgetItem(book->getStatus()));
        bookTable->setItem(row, 9, new QTableWidgetItem(QString::number(book->getReservationCount())));
    }
}

// （图书增加）：增加按钮点击处理
void AdminWindow::onBookAdd()
{
    QPair<QString, bool> result = showInputDialog("增加图书", "请输入图书ISBN：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString isbn = result.first;

    result = showInputDialog("增加图书", "请输入图书书名：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString title = result.first;

    result = showInputDialog("增加图书", "请输入图书作者：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString author = result.first;

    result = showInputDialog("增加图书", "请输入图书分类：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString category = result.first;

    result = showInputDialog("增加图书", "请输入图书库存数量：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    bool ok;
    int stock = result.first.toInt(&ok);
    if (!ok || stock < 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, "错误", "库存数量必须是大于等于0的整数！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        int addResult = admin->addBook(isbn, title, author, category, stock);

        std::vector<const Book *> books = admin->findAllBook();
        displayBooks(books);

        if (addResult == -1)
        {
            QMessageBox::warning(this, "警告", QString("ISBN %1 对应的书已存在，请重试").arg(isbn));
        }
        else if (addResult == 1)
        {
            QMessageBox::information(this, "成功", "图书库存已增加！");

            // 发送消息：库存增加
            DataManager *dm = DataManager::getInstance();
            QString msgContent = QString("管理员%1增加了图书库存：%2（ISBN: %3, 作者: %4, 分类: %5, 库存: %6）")
                                     .arg(currentUser->getName())
                                     .arg(title)
                                     .arg(isbn)
                                     .arg(author)
                                     .arg(category)
                                     .arg(stock);
            dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
        }
        else
        {
            QMessageBox::information(this, "成功", "图书添加成功！");

            // 发送消息：新增图书
            DataManager *dm = DataManager::getInstance();
            QString msgContent = QString("管理员%1添加了新图书：%2（ISBN: %3, 作者: %4, 分类: %5, 库存: %6）")
                                     .arg(currentUser->getName())
                                     .arg(title)
                                     .arg(isbn)
                                     .arg(author)
                                     .arg(category)
                                     .arg(stock);
            dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
        }
    }
}

// （图书删除）：删除按钮点击处理
void AdminWindow::onBookDelete()
{
    QPair<QString, bool> isbnResult = showInputDialog("删除图书", "请输入要删除的图书ISBN：", true);
    if (isbnResult.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书删除已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString isbn = isbnResult.first;

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (!admin)
    {
        return;
    }

    DataManager *dm = DataManager::getInstance();
    Book *book = dm->findBookByISBN(isbn);
    if (book == nullptr)
    {
        QMessageBox::warning(this, "失败", QString("不存在ISBN %1，请重试").arg(isbn));
        return;
    }

    QPair<QString, bool> stockResult = showInputDialog("删除图书", "请输入要减少的库存数目：", true);
    if (stockResult.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书删除已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    int decreaseStock = stockResult.first.toInt();

    // 保存图书信息（删除前获取）
    QString bookTitle = book->getTitle();
    QString bookAuthor = book->getAuthor();
    QString bookCategory = book->getCategory();
    int bookStock = book->getStock();

    int deleteResult = admin->deleteBook(isbn, decreaseStock);

    if (deleteResult == 0)
    {
        std::vector<const Book *> books = admin->findAllBook();
        displayBooks(books);
        QMessageBox::information(this, "成功", "图书删除成功！");

        // 发送消息：图书删除（已删除记录）
        DataManager *dm = DataManager::getInstance();
        QString msgContent = QString("管理员%1删除了图书：%2（ISBN: %3, 作者: %4, 分类: %5, 库存: %6）- 已删除记录")
                                 .arg(currentUser->getName())
                                 .arg(bookTitle)
                                 .arg(isbn)
                                 .arg(bookAuthor)
                                 .arg(bookCategory)
                                 .arg(bookStock);
        dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
    }
    else if (deleteResult == 1)
    {
        std::vector<const Book *> books = admin->findAllBook();
        displayBooks(books);
        QMessageBox::information(this, "成功", "图书库存已减少！");

        // 发送消息：库存减少
        DataManager *dm = DataManager::getInstance();
        QString msgContent = QString("管理员%1减少了图书库存：%2（ISBN: %3, 作者: %4, 分类: %5, 库存: %6）- 减少%7本")
                                 .arg(currentUser->getName())
                                 .arg(book->getTitle())
                                 .arg(isbn)
                                 .arg(book->getAuthor())
                                 .arg(book->getCategory())
                                 .arg(book->getStock())
                                 .arg(decreaseStock);
        dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
    }
    else if (deleteResult == -2)
    {
        QMessageBox::warning(this, "失败", "当前存在预约或借出，无法删除！");
    }
}

// （图书修改）：修改按钮点击处理
void AdminWindow::onBookUpdate()
{
    QPair<QString, bool> result = showInputDialog("修改图书", "请输入要修改的图书ISBN：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString oldIsbn = result.first;

    DataManager *dm = DataManager::getInstance();
    Book *foundBook = dm->findBookByISBN(oldIsbn);
    if (!foundBook)
    {
        QMessageBox::warning(this, "失败", QString("不存在ISBN %1，请重试").arg(oldIsbn));
        return;
    }

    // 检查是否有预约或借出
    if (foundBook->getReservationCount() > 0 || foundBook->getCurrentBorrowed() > 0)
    {
        QMessageBox::warning(this, "失败", "当前存在预约或借出，无法修改！");
        return;
    }

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (!admin)
    {
        return;
    }

    result = showInputDialog("修改图书", "请输入修改后的图书ISBN：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newIsbn = result.first;

    result = showInputDialog("修改图书", "请输入修改后的图书书名：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString title = result.first;

    result = showInputDialog("修改图书", "请输入修改后的图书作者：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString author = result.first;

    result = showInputDialog("修改图书", "请输入修改后的图书分类：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString category = result.first;

    result = showInputDialog("修改图书", "请输入修改后的图书库存数量：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "图书修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    bool ok;
    int stock = result.first.toInt(&ok);
    if (!ok || stock < 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, "错误", "库存数量必须是大于等于0的整数！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    QDateTime inStockTime = QDateTime::currentDateTime();

    int updateResult = admin->updateBook(oldIsbn, newIsbn, title, author, category, stock, inStockTime);

    if (updateResult == 0)
    {
        std::vector<const Book *> books = admin->findAllBook();
        displayBooks(books);
        QMessageBox::information(this, "成功", "图书修改成功！");

        // 发送消息：图书修改
        DataManager *dm = DataManager::getInstance();
        QString msgContent = QString("管理员%1修改了图书信息：%2（ISBN: %3, 作者: %4, 分类: %5, 库存: %6）→ %7（ISBN: %8, 作者: %9, 分类: %10, 库存: %11）")
                                 .arg(currentUser->getName())
                                 .arg(foundBook->getTitle())
                                 .arg(oldIsbn)
                                 .arg(foundBook->getAuthor())
                                 .arg(foundBook->getCategory())
                                 .arg(foundBook->getStock())
                                 .arg(title)
                                 .arg(newIsbn)
                                 .arg(title)
                                 .arg(author)
                                 .arg(category)
                                 .arg(stock);
        dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
    }
    else if (updateResult == -3)
    {
        QMessageBox::warning(this, "失败", QString("新ISBN %1 已存在，请重试").arg(newIsbn));
    }
}

// （图书清除）：清除按钮点击处理
void AdminWindow::onBookClear()
{
    // 检查是否有图书存在预约或借出
    DataManager *dm = DataManager::getInstance();
    std::vector<Book> &books = dm->getBooks();
    for (auto &book : books)
    {
        if (book.getReservationCount() > 0 || book.getCurrentBorrowed() > 0)
        {
            QMessageBox::warning(this, "失败", "存在预约或借出的图书，无法清除！");
            return;
        }
    }

    QMessageBox warningBox(QMessageBox::Warning, "警告", "确定要清除所有图书信息吗？此操作不可恢复！", QMessageBox::NoButton, this);
    QAbstractButton *yesBtn1 = warningBox.addButton("是", QMessageBox::YesRole);
    warningBox.addButton("否", QMessageBox::NoRole);
    warningBox.exec();
    if (warningBox.clickedButton() == yesBtn1)
    {
        QMessageBox confirmBox(QMessageBox::Question, "确认", "再次确认：是否真的要清除所有图书？", QMessageBox::NoButton, this);
        QAbstractButton *yesBtn2 = confirmBox.addButton("是", QMessageBox::YesRole);
        confirmBox.addButton("否", QMessageBox::NoRole);
        confirmBox.exec();
        if (confirmBox.clickedButton() == yesBtn2)
        {
            ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
            if (admin)
            {
                admin->clearBook();
                displayBooks(std::vector<const Book *>());
            }

            QMessageBox msgBox(QMessageBox::Information, "成功", "已清除所有图书！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （图书排序）：按借阅次数排序
void AdminWindow::onBookSort()
{
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<const Book *> sortedBooks = admin->sortBookByBorrowCount();
        displayBooks(sortedBooks);

        QMessageBox msgBox(QMessageBox::Information, "成功", "已按借阅次数排序！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();

        // 发送消息：图书排序
        DataManager *dm = DataManager::getInstance();
        QString msgContent = QString("管理员%1对图书按借阅次数进行了排序").arg(currentUser->getName());
        dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
    }
}

// （图书排序）：按入库时间排序
void AdminWindow::onBookSortByTime()
{
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<const Book *> sortedBooks = admin->sortBookByInStockTime();
        displayBooks(sortedBooks);

        QMessageBox msgBox(QMessageBox::Information, "成功", "已按入库时间排序！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();

        // 发送消息：图书排序
        DataManager *dm = DataManager::getInstance();
        QString msgContent = QString("管理员%1对图书按入库时间进行了排序").arg(currentUser->getName());
        dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);
    }
}
// 修改结束

// （加载借阅数据）：从数据管理器加载借阅数据到表格
void AdminWindow::loadBorrowData()
{
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<BorrowRecord> records = admin->viewBorrowRecords();
        displayBorrowRecords(records);
    }
}

// （借阅查找）：查找按钮点击处理
void AdminWindow::onBorrowSearch()
{
    QString isbn = borrowISBNLineEdit->text().trimmed();
    QString readerId = borrowReaderIdLineEdit->text().trimmed();

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<BorrowRecord> records = admin->viewBorrowRecords();

        if (!isbn.isEmpty() || !readerId.isEmpty())
        {
            std::vector<BorrowRecord> filtered;
            for (const auto &record : records)
            {
                bool match = true;
                if (!isbn.isEmpty() && !record.getISBN().contains(isbn))
                    match = false;
                if (!readerId.isEmpty() && !record.getReaderID().contains(readerId))
                    match = false;
                if (match)
                    filtered.push_back(record);
            }
            displayBorrowRecords(filtered);
        }
        else
        {
            displayBorrowRecords(records);
        }
    }
}

// （显示借阅记录）：将借阅记录显示到表格
void AdminWindow::displayBorrowRecords(const std::vector<BorrowRecord> &records)
{
    borrowTable->setRowCount(0);
    DataManager *dm = DataManager::getInstance();

    for (const auto &record : records)
    {
        int row = borrowTable->rowCount();
        borrowTable->insertRow(row);

        borrowTable->setItem(row, 0, new QTableWidgetItem(record.getISBN()));

        Book *book = dm->findBookByISBN(record.getISBN());
        QString bookTitle = book ? book->getTitle() : "未知";
        borrowTable->setItem(row, 1, new QTableWidgetItem(bookTitle));

        borrowTable->setItem(row, 2, new QTableWidgetItem(record.getReaderID()));

        ::User *user = dm->findUserById(record.getReaderID());
        QString userName = user ? user->getName() : "未知";
        borrowTable->setItem(row, 3, new QTableWidgetItem(userName));

        borrowTable->setItem(row, 4, new QTableWidgetItem(record.getBorrowTime().toString("yyyy-MM-dd HH:mm:ss")));
        borrowTable->setItem(row, 5, new QTableWidgetItem(record.getDueTime().toString("yyyy-MM-dd HH:mm:ss")));

        QString returnTime = record.isReturned() ? record.getReturnTime().toString("yyyy-MM-dd HH:mm:ss") : "";
        borrowTable->setItem(row, 6, new QTableWidgetItem(returnTime));

        QString status = "借阅中";
        if (record.isReturned())
        {
            status = "已归还";
        }
        else if (record.calculateOverdueDays() > 0)
        {
            status = QString("逾期(%1天)").arg(record.calculateOverdueDays());
        }
        borrowTable->setItem(row, 7, new QTableWidgetItem(status));
    }
}

// （借阅添加）：借书按钮点击处理
void AdminWindow::onBorrowAdd()
{
    QPair<QString, bool> result = showInputDialog("借书", "请输入图书ISBN：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "借书已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString isbn = result.first;

    result = showInputDialog("借书", "请输入读者ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "借书已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString readerId = result.first;

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        bool success = admin->borrowBook(isbn, readerId);

        if (success)
        {
            loadBorrowData();

            QMessageBox msgBox(QMessageBox::Information, "成功", "借书成功！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox(QMessageBox::Warning, "失败", "借书失败！可能图书不存在、无库存或读者有逾期未还。", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （借阅归还）：还书按钮点击处理
void AdminWindow::onBorrowReturn()
{
    QPair<QString, bool> result = showInputDialog("还书", "请输入图书ISBN：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "还书已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString isbn = result.first;

    result = showInputDialog("还书", "请输入读者ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "还书已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString readerId = result.first;

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        bool success = admin->returnBook(isbn, readerId);

        if (success)
        {
            loadBorrowData();

            QMessageBox msgBox(QMessageBox::Information, "成功", "还书成功！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox(QMessageBox::Warning, "失败", "还书失败！借阅记录不存在。", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （借阅续借）：续借按钮点击处理
void AdminWindow::onBorrowRenew()
{
    QPair<QString, bool> result = showInputDialog("续借", "请输入图书ISBN：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "续借已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString isbn = result.first;

    result = showInputDialog("续借", "请输入读者ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "续借已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString readerId = result.first;

    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        bool success = admin->renewBook(isbn, readerId);

        if (success)
        {
            loadBorrowData();

            QMessageBox msgBox(QMessageBox::Information, "成功", "续借成功！借阅期限延长30天。", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox(QMessageBox::Warning, "失败", "续借失败！借阅记录不存在或已归还。", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （加载预约数据）：从数据管理器加载预约数据到表格
void AdminWindow::loadReservationData()
{
    // （获取数据管理器）：获取数据管理器单例实例
    DataManager *dm = DataManager::getInstance();
    // （清空表格）：清空预约表格内容
    reservationTable->setRowCount(0);

    // （获取所有预约）：获取所有预约数据
    std::vector<Reservation> reservations = dm->getReservations();

    // （填充表格）：将预约数据填入表格
    for (const auto &reservation : reservations)
    {
        int row = reservationTable->rowCount();
        reservationTable->insertRow(row);

        reservationTable->setItem(row, 0, new QTableWidgetItem(reservation.getISBN()));
        reservationTable->setItem(row, 1, new QTableWidgetItem(reservation.getReaderID()));
        reservationTable->setItem(row, 2, new QTableWidgetItem(reservation.getReserveTime().toString("yyyy-MM-dd HH:mm:ss")));
        reservationTable->setItem(row, 3, new QTableWidgetItem(reservation.getStatusString()));

        Book *book = dm->findBookByISBN(reservation.getISBN());
        QString bookTitle = book ? book->getTitle() : "未知";
        reservationTable->setItem(row, 4, new QTableWidgetItem(bookTitle));
    }
}

// （加载统计数据）：从数据管理器加载统计数据到表格
void AdminWindow::loadStatisticsData()
{
    if (statisticsWidget)
    {
        statisticsWidget->updateStatistics();
    }
}

// （用户查找）：查找按钮点击处理
void AdminWindow::onUserSearch()
{
    // （获取输入）：获取用户输入的ID和姓名
    QString id = userIdLineEdit->text().trimmed();
    QString name = userNameLineEdit->text().trimmed();

    // （调用后端）：调用Admin的查找方法
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<::User *> results;
        if (id.isEmpty() && name.isEmpty())
        {
            // 查找全部用户
            results = admin->findAllUser();
        }
        else
        {
            // 多关键字查找
            results = admin->findUser(id, name);
        }

        // （显示结果）：将结果显示到表格
        displayUsers(results);
    }
}

// （显示用户）：将用户数据显示到表格
void AdminWindow::displayUsers(const std::vector<::User *> &users)
{
    // （清空表格）：清空现有表格内容
    userTable->setRowCount(0);

    // （填充表格）：将用户数据填入表格
    for (::User *user : users)
    {
        int row = userTable->rowCount();
        userTable->insertRow(row);

        userTable->setItem(row, 0, new QTableWidgetItem(user->getID()));
        userTable->setItem(row, 1, new QTableWidgetItem(user->getType() == 1 ? "管理员" : "读者"));
        userTable->setItem(row, 2, new QTableWidgetItem(user->getName()));
        userTable->setItem(row, 3, new QTableWidgetItem(user->getPassword()));
        userTable->setItem(row, 4, new QTableWidgetItem(user->getPhone()));
        userTable->setItem(row, 5, new QTableWidgetItem(user->getEmail()));
    }
}

// （中文输入对话框）：显示带中文按钮的输入对话框
//  返回 QPair：first=输入内容，second=true表示取消，false表示确定
QPair<QString, bool> AdminWindow::showInputDialog(const QString &title, const QString &label, bool required)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setModal(true);
    dialog.setFixedSize(750, 100);

    QLabel *labelWidget = new QLabel(label, &dialog);
    QLineEdit *lineEdit = new QLineEdit(&dialog);
    lineEdit->setMinimumWidth(150);
    QPushButton *okBtn = new QPushButton("确定", &dialog);
    QPushButton *cancelBtn = new QPushButton("取消", &dialog);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(labelWidget);
    hLayout->addSpacing(15);
    hLayout->addWidget(lineEdit);
    hLayout->addSpacing(20);
    hLayout->addWidget(okBtn);
    hLayout->addSpacing(10);
    hLayout->addWidget(cancelBtn);
    dialog.setLayout(hLayout);

    QObject::connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    while (true)
    {
        if (dialog.exec() == QDialog::Accepted)
        {
            QString text = lineEdit->text().trimmed();
            if (required && text.isEmpty())
            {
                QMessageBox msgBox(QMessageBox::Warning, "提示", "输入不能为空！", QMessageBox::NoButton, this);
                msgBox.addButton("确定", QMessageBox::AcceptRole);
                msgBox.exec();
                continue;
            }
            return qMakePair(text, false);
        }
        return qMakePair(QString(), true);
    }
}

// （用户增加）：增加按钮点击处理
void AdminWindow::onUserAdd()
{
    // （依次输入信息）：逐个弹出输入框让用户输入用户信息
    QPair<QString, bool> result = showInputDialog("增加用户", "请输入用户ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString id = result.first;

    // 检查用户ID是否已存在
    DataManager *dm = DataManager::getInstance();
    if (dm->findUserById(id))
    {
        QMessageBox::warning(this, "警告", QString("用户id %1 已存在，请重试").arg(id));
        return;
    }

    result = showInputDialog("增加用户", "请输入用户类型（1-管理员/2-读者）：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString type = result.first;
    // 检查用户类型是否合法
    if (type != "1" && type != "2")
    {
        QMessageBox msgBox(QMessageBox::Warning, "错误", "用户类型非法！请输入1（管理员）或2（读者）", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    result = showInputDialog("增加用户", "请输入用户姓名：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString name = result.first;

    result = showInputDialog("增加用户", "请输入用户密码：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString password = result.first;

    result = showInputDialog("增加用户", "请输入用户电话：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString phone = result.first;

    result = showInputDialog("增加用户", "请输入用户邮箱：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户添加已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString email = result.first;

    // （调用后端）：调用Admin的注册方法
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        admin->registerUser(id, type, name, password, phone, email);

        // （发送消息）：发送用户添加消息给管理员
        QString msgContent = QString("管理员%1添加了用户：%2（ID: %3, 类型: %4）")
                                 .arg(currentUser->getName())
                                 .arg(name)
                                 .arg(id)
                                 .arg(type == "1" ? "管理员" : "读者");
        dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);

        // （刷新表格）：重新加载用户列表
        std::vector<::User *> users = admin->findAllUser();
        displayUsers(users);
    }

    QMessageBox msgBox(QMessageBox::Information, "成功", "用户添加成功！", QMessageBox::NoButton, this);
    msgBox.addButton("确定", QMessageBox::AcceptRole);
    msgBox.exec();
}

// （用户删除）：删除按钮点击处理
void AdminWindow::onUserDelete()
{
    // （依次输入信息）：先输入ID，再输入姓名
    QPair<QString, bool> result = showInputDialog("删除用户", "请输入要删除的用户ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户删除已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString id = result.first;

    result = showInputDialog("删除用户", "请输入要删除的用户姓名：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户删除已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString name = result.first;

    // （调用后端）：调用Admin的删除方法
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        // 保存原用户类型（删除前获取）
        DataManager *dm = DataManager::getInstance();
        User *userToDelete = dm->findUserById(id);
        QString userTypeName = userToDelete && userToDelete->getType() == 1 ? "管理员" : "读者";

        bool success = admin->deleteUser(id, name);

        if (success)
        {
            // （发送消息）：发送用户删除消息给管理员
            QString msgContent = QString("管理员%1删除了用户：%2（ID: %3, 类型: %4）")
                                     .arg(currentUser->getName())
                                     .arg(name)
                                     .arg(id)
                                     .arg(userTypeName);
            dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);

            // （刷新表格）：重新加载用户列表
            std::vector<::User *> users = admin->findAllUser();
            displayUsers(users);

            QMessageBox msgBox(QMessageBox::Information, "成功", "用户删除成功！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox(QMessageBox::Warning, "失败", "用户删除失败，未找到匹配的用户！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （用户修改）：修改按钮点击处理
void AdminWindow::onUserUpdate()
{
    // （依次输入信息）：先输入要修改的用户ID和姓名
    QPair<QString, bool> result = showInputDialog("修改用户", "请输入要修改的用户ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString id = result.first;

    result = showInputDialog("修改用户", "请输入要修改的用户姓名：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString name = result.first;

    // （检查原用户是否存在）：提前验证要修改的用户是否存在
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        std::vector<::User *> foundUsers = admin->findUser(id, name);
        if (foundUsers.empty())
        {
            QMessageBox msgBox(QMessageBox::Warning, "失败", "用户修改失败，未找到匹配的用户！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
            return;
        }
    }

    // （依次输入修改后的信息）：逐个输入修改后的用户信息
    result = showInputDialog("修改用户", "请输入修改后的用户ID：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newId = result.first;

    result = showInputDialog("修改用户", "请输入修改后的用户类型（1-管理员/2-读者）：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newType = result.first;
    // 检查用户类型是否合法
    if (newType != "1" && newType != "2")
    {
        QMessageBox msgBox(QMessageBox::Warning, "错误", "用户类型非法！请输入1（管理员）或2（读者）", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    result = showInputDialog("修改用户", "请输入修改后的用户姓名：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newName = result.first;

    result = showInputDialog("修改用户", "请输入修改后的用户密码：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newPassword = result.first;

    result = showInputDialog("修改用户", "请输入修改后的用户电话：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newPhone = result.first;

    result = showInputDialog("修改用户", "请输入修改后的用户邮箱：", true);
    if (result.second)
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "用户修改已取消！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }
    QString newEmail = result.first;

    // （调用后端）：调用Admin的修改方法
    admin = dynamic_cast<::Admin *>(currentUser);
    if (admin)
    {
        // 保存原用户类型
        DataManager *dm = DataManager::getInstance();
        User *oldUser = dm->findUserById(id);
        QString oldTypeName = oldUser && oldUser->getType() == 1 ? "管理员" : "读者";

        bool success = admin->updateUser(id, name, newId, newType, newName, newPassword, newPhone, newEmail);

        if (success)
        {
            // （发送消息）：发送用户修改消息给管理员
            User *updatedUser = dm->findUserById(newId);
            QString msgContent = QString("管理员%1修改了用户信息：%2（ID: %3, 类型: %4）→ %5（ID: %6, 类型: %7）")
                                     .arg(currentUser->getName())
                                     .arg(name)
                                     .arg(id)
                                     .arg(oldTypeName)
                                     .arg(newName)
                                     .arg(newId)
                                     .arg(updatedUser && updatedUser->getType() == 1 ? "管理员" : "读者");
            dm->addAdminMessage(currentUser, currentUser->getID(), currentUser->getName(), msgContent);

            // （刷新表格）：重新加载用户列表
            std::vector<::User *> users = admin->findAllUser();
            displayUsers(users);

            QMessageBox msgBox(QMessageBox::Information, "成功", "用户修改成功！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox(QMessageBox::Warning, "失败", "用户修改失败，未找到匹配的用户！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （用户清除）：清除按钮点击处理
void AdminWindow::onUserClear()
{
    // （弹出确认框）：弹出确认框警告用户是否确认清除所有用户
    QMessageBox warningBox(QMessageBox::Warning, "警告", "确定要清除所有用户信息吗？此操作不可恢复！此操作会清除所有用户数据！", QMessageBox::NoButton, this);
    QAbstractButton *yesBtn1 = warningBox.addButton("是", QMessageBox::YesRole);
    warningBox.addButton("否", QMessageBox::NoRole);
    warningBox.exec();
    if (warningBox.clickedButton() == yesBtn1)
    {
        // （再次确认）：再次弹出确认框
        QMessageBox confirmBox(QMessageBox::Question, "确认", "再次确认：是否真的要清除所有用户？", QMessageBox::NoButton, this);
        QAbstractButton *yesBtn2 = confirmBox.addButton("是", QMessageBox::YesRole);
        confirmBox.addButton("否", QMessageBox::NoRole);
        confirmBox.exec();
        if (confirmBox.clickedButton() == yesBtn2)
        {
            // （调用后端）：调用Admin的清除方法，保留当前管理员
            ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
            if (admin)
            {
                // 更新 currentUser 指针为新创建的用户对象
                currentUser = admin->clearUser(currentUser);

                // （刷新表格）：重新加载用户列表（只显示当前管理员）
                std::vector<::User *> users = admin->findAllUser();
                displayUsers(users);

                // 刷新消息表格（消息已被清空）
                if (messageTable)
                {
                    messageTable->setRowCount(0);
                }
            }

            QMessageBox msgBox(QMessageBox::Information, "成功", "已清除所有用户！", QMessageBox::NoButton, this);
            msgBox.addButton("确定", QMessageBox::AcceptRole);
            msgBox.exec();
        }
    }
}

// （用户管理）：用户管理按钮点击处理，切换到用户管理表格
void AdminWindow::onUserManagement()
{
    // （加载数据）：加载用户数据
    loadUserData();
    // （切换视图）：切换到用户管理表格视图
    stackedWidget->setCurrentIndex(0);
}

// （图书管理）：图书管理按钮点击处理，切换到图书管理表格
void AdminWindow::onBookManagement()
{
    // （加载数据）：加载图书数据
    loadBookData();
    // （切换视图）：切换到图书管理表格视图
    stackedWidget->setCurrentIndex(1);
}

// （借阅管理）：借阅管理按钮点击处理，切换到借阅管理表格
void AdminWindow::onBorrowManagement()
{
    // （加载数据）：加载借阅数据
    loadBorrowData();
    // （切换视图）：切换到借阅管理表格视图
    stackedWidget->setCurrentIndex(2);
}

// （预约管理）：预约管理按钮点击处理，切换到预约管理表格
void AdminWindow::onReservationManagement()
{
    // （加载数据）：加载预约数据
    loadReservationData();
    // （切换视图）：切换到预约管理表格视图
    stackedWidget->setCurrentIndex(3);
}

// （统计报表）：统计报表按钮点击处理，切换到统计报表表格
void AdminWindow::onStatistics()
{
    loadStatisticsData();
    stackedWidget->setCurrentIndex(4);
}

// （处理预约）：处理预约按钮点击处理
void AdminWindow::onProcessReservation()
{
    // （获取选中行）：获取当前选中的预约行
    int row = reservationTable->currentRow();
    if (row < 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, "提示", "请先选择一条预约记录！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    QString isbn = reservationTable->item(row, 0)->text();
    QString readerId = reservationTable->item(row, 1)->text();
    QString status = reservationTable->item(row, 3)->text();

    if (status != "待处理")
    {
        QMessageBox msgBox(QMessageBox::Warning, "提示", "只能处理状态为【待处理】的预约！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    // （确认处理）：弹出确认框
    QMessageBox confirmBox(QMessageBox::Question, "确认", QString("确定要处理读者 %1 预约的图书(ISBN:%2)吗？").arg(readerId).arg(isbn), QMessageBox::NoButton, this);
    QAbstractButton *yesBtn = confirmBox.addButton("是", QMessageBox::YesRole);
    confirmBox.addButton("否", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == yesBtn)
    {
        ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
        if (admin)
        {
            bool success = admin->processSingleReservation(isbn, readerId);

            loadReservationData();

            if (success)
            {
                QMessageBox msgBox(QMessageBox::Information, "成功", "预约处理完成！已通知相关读者。", QMessageBox::NoButton, this);
                msgBox.addButton("确定", QMessageBox::AcceptRole);
                msgBox.exec();
            }
            else
            {
                QMessageBox msgBox(QMessageBox::Warning, "失败", "预约处理失败！可能该预约已不是待处理状态。", QMessageBox::NoButton, this);
                msgBox.addButton("确定", QMessageBox::AcceptRole);
                msgBox.exec();
            }
        }
    }
}

// （取消预约）：取消预约按钮点击处理
void AdminWindow::onCancelReservation()
{
    // （获取选中行）：获取当前选中的预约行
    int row = reservationTable->currentRow();
    if (row < 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, "提示", "请先选择一条预约记录！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    QString isbn = reservationTable->item(row, 0)->text();
    QString readerId = reservationTable->item(row, 1)->text();
    QString status = reservationTable->item(row, 3)->text();

    if (status == "已取消" || status == "已完成")
    {
        QMessageBox msgBox(QMessageBox::Warning, "提示", "该预约已" + status + "，无法操作！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    // （确认取消）：弹出确认框
    QMessageBox confirmBox(QMessageBox::Question, "确认", QString("确定要取消读者 %1 预约的图书(ISBN:%2)吗？").arg(readerId).arg(isbn), QMessageBox::NoButton, this);
    QAbstractButton *yesBtn = confirmBox.addButton("是", QMessageBox::YesRole);
    confirmBox.addButton("否", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == yesBtn)
    {
        // （调用后端）：调用Admin的取消预约方法
        ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
        if (admin)
        {
            bool success = admin->cancelReservation(isbn, readerId);

            if (success)
            {
                // （刷新表格）：重新加载预约数据
                loadReservationData();

                QMessageBox msgBox(QMessageBox::Information, "成功", "预约已取消！", QMessageBox::NoButton, this);
                msgBox.addButton("确定", QMessageBox::AcceptRole);
                msgBox.exec();
            }
            else
            {
                QMessageBox msgBox(QMessageBox::Warning, "失败", "取消预约失败！", QMessageBox::NoButton, this);
                msgBox.addButton("确定", QMessageBox::AcceptRole);
                msgBox.exec();
            }
        }
    }
}

// （消息管理）：初始化消息表格
void AdminWindow::setupMessageWidget()
{
    messageWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(messageWidget);

    messageTable = new QTableWidget(this);
    messageTable->setColumnCount(5);
    messageTable->setHorizontalHeaderLabels(QStringList() << "读者ID" << "读者名" << "消息时间" << "消息内容" << "消息状态");
    messageTable->horizontalHeader()->setStretchLastSection(true);
    messageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    messageTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    messageTable->setColumnWidth(0, 80);
    messageTable->setColumnWidth(1, 80);
    messageTable->setColumnWidth(2, 130);
    messageTable->setColumnWidth(3, 400);
    messageTable->setColumnWidth(4, 80);

    mainLayout->addWidget(messageTable);
    messageWidget->setLayout(mainLayout);
}

// （显示消息）：显示消息到消息表格
void AdminWindow::displayMessages(const std::vector<Message> &messages)
{
    messageTable->setRowCount(messages.size());
    for (size_t i = 0; i < messages.size(); ++i)
    {
        const Message &msg = messages[i];
        std::vector<QString> fields = msg.getAdminDisplayFields();

        for (int j = 0; j < fields.size() && j < 5; ++j)
        {
            messageTable->setItem(i, j, new QTableWidgetItem(fields[j]));
        }
    }
}

// （消息管理）：消息按钮点击处理
void AdminWindow::onMessage()
{
    ::Admin *admin = dynamic_cast<::Admin *>(currentUser);
    if (!admin)
    {
        return;
    }

    stackedWidget->setCurrentWidget(messageWidget);
    displayMessages(admin->getAllMessages());
}

// （退出登录）：退出按钮点击处理
void AdminWindow::onLogout()
{
    QMessageBox confirmBox(QMessageBox::Question, "确认退出", "确定要退出登录吗？", QMessageBox::NoButton, this);
    QAbstractButton *yesBtn = confirmBox.addButton("是", QMessageBox::YesRole);
    confirmBox.addButton("否", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == yesBtn)
    {
        QMessageBox::information(this, "成功", "退出登录成功！");
        emit logout();
    }
}