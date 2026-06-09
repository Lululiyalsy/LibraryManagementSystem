/**
 * @file statisticswidget.cpp
 * @brief 统计报表界面组件实现
 *
 * 该文件实现了图书管理系统的统计报表功能，包括：
 * - 用户统计（管理员、读者数量）
 * - 图书统计（总数）
 * - 借阅统计（借阅次数、归还率、逾期率）
 * - 热门图书排行
 * - 逾期图书列表
 * - 报表生成与消息通知
 */

#include "statisticswidget.h"
#include "DataManager.h"
#include "Admin.h"
#include "User.h"
#include "Book.h"
#include "BorrowRecord.h"
#include <QHeaderView>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSplitter>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 * @param user 当前登录的管理员用户
 */
StatisticsWidget::StatisticsWidget(QWidget *parent, User *user)
    : QWidget(parent), currentUser(user), summaryTable(nullptr), hotBooksTable(nullptr),
      overdueTable(nullptr), generateReportBtn(nullptr), refreshBtn(nullptr)
{
    setupUI();
}

/**
 * @brief 析构函数
 */
StatisticsWidget::~StatisticsWidget()
{
}

/**
 * @brief 初始化界面布局
 *
 * 创建三个可拖动的区域：
 * 1. 统计摘要 - 显示用户、图书、借阅等统计数据
 * 2. 热门图书 TOP5 - 按借阅次数排序的图书排行
 * 3. 逾期图书 - 当前逾期未归还的图书列表
 */
void StatisticsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 操作按钮区域
    QWidget *operationWidget = new QWidget(this);
    QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);

    refreshBtn = new QPushButton("刷新数据", operationWidget);
    generateReportBtn = new QPushButton("生成报表", operationWidget);

    operationLayout->addWidget(refreshBtn);
    operationLayout->addWidget(generateReportBtn);
    operationLayout->addStretch();

    mainLayout->addWidget(operationWidget);

    // 创建可拖动的垂直分割器
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setHandleWidth(5);
    splitter->setStyleSheet("QSplitter::handle { background-color: #4a4a4a; }");

    // 第一部分：统计摘要
    QWidget *summaryWidget = new QWidget(splitter);
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryWidget);

    QLabel *summaryTitle = new QLabel("<b>📊 统计摘要</b>", summaryWidget);
    summaryTitle->setStyleSheet("font-size: 14px; color: #ffffff; padding: 5px; background-color: #3a3a3a;");
    summaryLayout->addWidget(summaryTitle);

    summaryTable = new QTableWidget(summaryWidget);
    summaryTable->setColumnCount(2);
    summaryTable->setHorizontalHeaderLabels({"统计项目", "数值"});
    summaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    summaryTable->verticalHeader()->setVisible(false);
    summaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    summaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    summaryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    summaryLayout->addWidget(summaryTable);

    // 第二部分：热门图书 TOP5
    QWidget *hotBooksWidget = new QWidget(splitter);
    QVBoxLayout *hotBooksLayout = new QVBoxLayout(hotBooksWidget);

    QLabel *hotBooksTitle = new QLabel("<b>🔥 热门图书 TOP5</b>", hotBooksWidget);
    hotBooksTitle->setStyleSheet("font-size: 14px; color: #ffffff; padding: 5px; background-color: #3a3a3a;");
    hotBooksLayout->addWidget(hotBooksTitle);

    hotBooksTable = new QTableWidget(hotBooksWidget);
    hotBooksTable->setColumnCount(3);
    hotBooksTable->setHorizontalHeaderLabels({"排名", "书名", "借阅次数"});
    hotBooksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    hotBooksTable->verticalHeader()->setVisible(false);
    hotBooksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hotBooksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    hotBooksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    hotBooksLayout->addWidget(hotBooksTable);

    // 第三部分：逾期图书
    QWidget *overdueWidget = new QWidget(splitter);
    QVBoxLayout *overdueLayout = new QVBoxLayout(overdueWidget);

    QLabel *overdueTitle = new QLabel("<b>⚠️ 逾期图书</b>", overdueWidget);
    overdueTitle->setStyleSheet("font-size: 14px; color: #ffffff; padding: 5px; background-color: #3a3a3a;");
    overdueLayout->addWidget(overdueTitle);

    overdueTable = new QTableWidget(overdueWidget);
    overdueTable->setColumnCount(4);
    overdueTable->setHorizontalHeaderLabels({"ISBN", "书名", "读者ID", "逾期天数"});
    overdueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    overdueTable->verticalHeader()->setVisible(false);
    overdueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    overdueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    overdueTable->setSelectionMode(QAbstractItemView::SingleSelection);
    overdueLayout->addWidget(overdueTable);

    mainLayout->addWidget(splitter);

    // 连接信号槽
    connect(generateReportBtn, &QPushButton::clicked, this, &StatisticsWidget::onGenerateReportClicked);
    connect(refreshBtn, &QPushButton::clicked, this, &StatisticsWidget::onRefreshStatistics);
}

/**
 * @brief 更新统计数据并刷新界面显示
 *
 * 从 DataManager 获取最新数据，计算各项统计指标，然后更新三个表格：
 * 1. 统计摘要表格 - 用户、图书、借阅等统计数据
 * 2. 热门图书表格 - 按借阅次数排序的前5本图书
 * 3. 逾期图书表格 - 当前逾期未归还的图书列表
 */
void StatisticsWidget::updateStatistics()
{
    DataManager *dm = DataManager::getInstance();

    // 统计用户数据
    std::vector<User *> users = dm->getUsers();
    int totalUsers = users.size();
    int admins = 0, readers = 0;
    for (auto user : users)
    {
        if (user->getType() == 1)
            admins++;
        else
            readers++;
    }

    // 获取图书数据
    std::vector<Book> books = dm->getBooks();
    int totalBooks = books.size();

    // 获取借阅记录
    std::vector<BorrowRecord> records = dm->getBorrowRecords();
    int currentBorrowCount = records.size(); // 当前借阅数量（未归还）
    int totalBorrowCount = 0;                // 总借阅次数（历史总和）
    int totalOverdueReturnCount = 0;         // 图书的总逾期归还次数

    // 计算历史借阅总次数和图书的总逾期归还次数
    for (auto &book : books)
    {
        totalBorrowCount += book.getBorrowCount();
        totalOverdueReturnCount += book.getOverdueReturnCount();
    }

    // 统计当前逾期数量
    int currentOverdueCount = 0; // 当前逾期数量（未归还且逾期）

    for (auto &record : records)
    {
        // 未归还记录
        if (!record.isReturned() && record.calculateOverdueDays() > 0)
        {
            currentOverdueCount++; // 当前逾期
        }
    }

    // 计算归还率和按时归还率
    int returnedCount = totalBorrowCount - currentBorrowCount;        // 已归还次数
    int historicalOverdueCount = totalOverdueReturnCount;             // 历史逾期数量（图书的逾期归还次数总和）
    int onTimeReturnedCount = returnedCount - historicalOverdueCount; // 按时归还次数
    double returnRate = totalBorrowCount > 0 ? (double)returnedCount / totalBorrowCount * 100 : 0;
    double onTimeReturnRate = returnedCount > 0 ? (double)onTimeReturnedCount / returnedCount * 100 : 0;

    // 更新统计摘要表格
    summaryTable->setRowCount(0);
    QStringList items = {
        "总用户数", QString::number(totalUsers),
        "管理员数", QString::number(admins),
        "读者数", QString::number(readers),
        "图书总数", QString::number(totalBooks),
        "总借阅次数", QString::number(totalBorrowCount),     // 历史总和
        "当前借阅数量", QString::number(currentBorrowCount), // 当前未归还数量
        "已归还次数", QString::number(returnedCount),        // 历史已归还
        "归还率", QString::number(returnRate, 'f', 1) + "%",
        "历史逾期数量", QString::number(historicalOverdueCount), // 已归还但曾逾期
        "当前逾期数量", QString::number(currentOverdueCount),    // 当前未归还且逾期
        "按时归还次数", QString::number(onTimeReturnedCount),    // 按时归还次数
        "按时归还率", QString::number(onTimeReturnRate, 'f', 1) + "%"};
    for (int i = 0; i < items.size(); i += 2)
    {
        int row = summaryTable->rowCount();
        summaryTable->insertRow(row);
        summaryTable->setItem(row, 0, new QTableWidgetItem(items[i]));
        summaryTable->setItem(row, 1, new QTableWidgetItem(items[i + 1]));
    }

    // 更新热门图书表格（TOP5）
    std::vector<Book> sortedBooks = dm->sortBooksByBorrowCount();
    hotBooksTable->setRowCount(0);
    for (int i = 0; i < (int)sortedBooks.size() && i < 5; ++i)
    {
        int row = hotBooksTable->rowCount();
        hotBooksTable->insertRow(row);
        hotBooksTable->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        hotBooksTable->setItem(row, 1, new QTableWidgetItem(sortedBooks[i].getTitle()));
        hotBooksTable->setItem(row, 2, new QTableWidgetItem(QString::number(sortedBooks[i].getBorrowCount())));
    }

    // 更新逾期图书表格
    overdueTable->setRowCount(0);
    for (auto &record : records)
    {
        if (!record.isReturned() && record.calculateOverdueDays() > 0)
        {
            Book *book = dm->findBookByISBN(record.getISBN());
            int row = overdueTable->rowCount();
            overdueTable->insertRow(row);
            overdueTable->setItem(row, 0, new QTableWidgetItem(record.getISBN()));
            overdueTable->setItem(row, 1, new QTableWidgetItem(book ? book->getTitle() : "未知"));
            overdueTable->setItem(row, 2, new QTableWidgetItem(record.getReaderID()));
            overdueTable->setItem(row, 3, new QTableWidgetItem(QString::number(record.calculateOverdueDays())));
        }
    }
}

/**
 * @brief 刷新数据按钮点击事件处理
 *
 * 调用 updateStatistics() 重新加载并显示统计数据
 */
void StatisticsWidget::onRefreshStatistics()
{
    updateStatistics();
}

/**
 * @brief 生成报表按钮点击事件处理
 *
 * 生成统计报表文本文件，保存到应用程序目录下的 reports 文件夹，
 * 文件名格式为 report_yyyyMMdd_HHmmss.txt
 *
 * 报表内容包括：
 * - 统计时间
 * - 用户统计（总用户数、管理员数、读者数）
 * - 图书统计（图书总数）
 * - 借阅统计（总借阅次数、当前借阅数量、已归还次数、归还率、逾期数量、逾期率）
 * - 热门图书 TOP5
 * - 逾期图书列表
 *
 * 生成成功后会向当前管理员发送消息通知
 */
void StatisticsWidget::onGenerateReportClicked()
{
    DataManager *dm = DataManager::getInstance();

    // 统计用户数据
    std::vector<User *> users = dm->getUsers();
    int totalUsers = users.size();
    int admins = 0, readers = 0;
    for (auto user : users)
    {
        if (user->getType() == 1)
            admins++;
        else
            readers++;
    }

    // 获取图书和借阅记录数据
    std::vector<Book> books = dm->getBooks();
    std::vector<BorrowRecord> records = dm->getBorrowRecords();

    int currentBorrowCount = records.size(); // 当前借阅数量（未归还）
    int totalBorrowCount = 0;                // 总借阅次数（历史总和）
    int totalOverdueReturnCount = 0;         // 图书的总逾期归还次数

    // 计算历史借阅总次数和图书的总逾期归还次数
    for (auto &book : books)
    {
        totalBorrowCount += book.getBorrowCount();
        totalOverdueReturnCount += book.getOverdueReturnCount();
    }

    // 统计当前逾期数量
    int currentOverdueCount = 0; // 当前逾期数量（未归还且逾期）

    for (auto &record : records)
    {
        // 未归还记录
        if (!record.isReturned() && record.calculateOverdueDays() > 0)
        {
            currentOverdueCount++; // 当前逾期
        }
    }

    // 计算已归还次数、历史逾期数量和按时归还次数
    int returned = totalBorrowCount - currentBorrowCount;        // 已归还次数
    int historicalOverdueCount = totalOverdueReturnCount;        // 历史逾期数量（图书的逾期归还次数总和）
    int onTimeReturnedCount = returned - historicalOverdueCount; // 按时归还次数

    // 获取热门图书排行
    std::vector<Book> sortedBooks = dm->sortBooksByBorrowCount();

    // 构建报表内容
    QString report;
    report += "========================================\n";
    report += "          图书管理系统统计报表\n";
    report += "========================================\n\n";
    report += QString("统计时间: %1\n\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    report += QString("[用户统计]\n  总用户数: %1\n  管理员数: %2\n  读者数: %3\n\n").arg(totalUsers).arg(admins).arg(readers);
    report += QString("[图书统计]\n  图书总数: %1\n\n").arg((int)books.size());

    // 计算比率
    double returnRate = totalBorrowCount > 0 ? (double)returned / totalBorrowCount * 100 : 0;
    double onTimeReturnRate = returned > 0 ? (double)onTimeReturnedCount / returned * 100 : 0;

    report += QString("[借阅统计]\n  总借阅次数: %1\n  当前借阅数量: %2\n  已归还次数: %3\n  借阅归还率: %4%\n  历史逾期数量: %5\n  当前逾期数量: %6\n  按时归还次数: %7\n  按时归还率: %8%\n\n")
                  .arg(totalBorrowCount)
                  .arg(currentBorrowCount)
                  .arg(returned)
                  .arg(QString::number(returnRate, 'f', 2))
                  .arg(historicalOverdueCount)
                  .arg(currentOverdueCount)
                  .arg(onTimeReturnedCount)
                  .arg(QString::number(onTimeReturnRate, 'f', 2));

    // 添加热门图书 TOP5
    report += "[热门图书 TOP5]\n";
    for (int i = 0; i < (int)sortedBooks.size() && i < 5; i++)
    {
        report += QString("  %1. %2 - 借阅%3次\n").arg(i + 1).arg(sortedBooks[i].getTitle()).arg(sortedBooks[i].getBorrowCount());
    }

    // 添加逾期图书列表
    report += "\n[逾期图书]\n";
    bool hasOverdue = false;
    for (auto &record : records)
    {
        if (!record.isReturned() && record.calculateOverdueDays() > 0)
        {
            hasOverdue = true;
            Book *book = dm->findBookByISBN(record.getISBN());
            QString bookTitle = book ? book->getTitle() : "未知";
            report += QString("  ISBN: %1, 书名: %2, 读者ID: %3, 逾期天数: %4\n")
                          .arg(record.getISBN())
                          .arg(bookTitle)
                          .arg(record.getReaderID())
                          .arg(record.calculateOverdueDays());
        }
    }
    if (!hasOverdue)
    {
        report += "  暂无逾期图书\n";
    }

    report += "\n========================================\n";

    // 保存报表文件
    QString appDir = QCoreApplication::applicationDirPath();
    QString reportFileName = QString("report_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString reportPath = appDir + "/reports/" + reportFileName;

    // 确保 reports 目录存在
    QDir reportDir(appDir + "/reports");
    if (!reportDir.exists())
    {
        reportDir.mkpath(".");
    }

    // 写入文件
    QFile reportFile(reportPath);
    if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&reportFile);
        out << report;
        reportFile.close();

        // 发送消息给当前管理员（自己发给自己）
        if (currentUser)
        {
            Message msg(currentUser->getID(), currentUser->getName(),
                        currentUser->getID(), currentUser->getName(),
                        "统计报表已生成！");
            currentUser->addMessage(msg);
            dm->writeMessage();
        }

        QMessageBox::information(this, "成功", QString("报表已生成！\n保存位置: %1").arg(reportPath));
    }
    else
    {
        QMessageBox::warning(this, "失败", "报表生成失败，无法写入文件。");
    }
}
