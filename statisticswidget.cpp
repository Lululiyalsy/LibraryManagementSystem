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

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent), summaryTable(nullptr), hotBooksTable(nullptr),
      overdueTable(nullptr), generateReportBtn(nullptr), refreshBtn(nullptr)
{
    setupUI();
}

StatisticsWidget::~StatisticsWidget()
{
}

void StatisticsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QWidget *operationWidget = new QWidget(this);
    QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);

    refreshBtn = new QPushButton("刷新数据", operationWidget);
    generateReportBtn = new QPushButton("生成报表", operationWidget);

    operationLayout->addWidget(refreshBtn);
    operationLayout->addWidget(generateReportBtn);
    operationLayout->addStretch();

    mainLayout->addWidget(operationWidget);

    summaryTable = new QTableWidget(this);
    summaryTable->setColumnCount(2);
    summaryTable->setHorizontalHeaderLabels({"统计项目", "数值"});
    summaryTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    summaryTable->verticalHeader()->setVisible(false);
    summaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    summaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    summaryTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(summaryTable);

    hotBooksTable = new QTableWidget(this);
    hotBooksTable->setColumnCount(3);
    hotBooksTable->setHorizontalHeaderLabels({"排名", "书名", "借阅次数"});
    hotBooksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    hotBooksTable->verticalHeader()->setVisible(false);
    hotBooksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hotBooksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    hotBooksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(hotBooksTable);

    overdueTable = new QTableWidget(this);
    overdueTable->setColumnCount(4);
    overdueTable->setHorizontalHeaderLabels({"ISBN", "书名", "读者ID", "逾期天数"});
    overdueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    overdueTable->verticalHeader()->setVisible(false);
    overdueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    overdueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    overdueTable->setSelectionMode(QAbstractItemView::SingleSelection);
    mainLayout->addWidget(overdueTable);

    connect(generateReportBtn, &QPushButton::clicked, this, &StatisticsWidget::onGenerateReportClicked);
    connect(refreshBtn, &QPushButton::clicked, this, &StatisticsWidget::onRefreshStatistics);
}

void StatisticsWidget::updateStatistics()
{
    DataManager *dm = DataManager::getInstance();

    std::vector<User*> users = dm->getUsers();
    int totalUsers = users.size();
    int admins = 0, readers = 0;
    for (auto user : users) {
        if (user->getType() == 1) admins++;
        else readers++;
    }

    std::vector<Book> books = dm->getBooks();
    int totalBooks = books.size();

    std::vector<BorrowRecord> records = dm->getBorrowRecords();
    int totalBorrow = records.size();
    int returned = 0, overdue = 0;
    for (auto& record : records) {
        if (record.isReturned()) returned++;
        if (!record.isReturned() && record.calculateOverdueDays() > 0) overdue++;
    }
    double returnRate = totalBorrow > 0 ? (double)returned / totalBorrow * 100 : 0;

    summaryTable->setRowCount(0);
    QStringList items = {
        "总用户数", QString::number(totalUsers),
        "管理员数", QString::number(admins),
        "读者数", QString::number(readers),
        "图书总数", QString::number(totalBooks),
        "总借阅次数", QString::number(totalBorrow),
        "已归还次数", QString::number(returned),
        "归还率", QString::number(returnRate, 'f', 1) + "%",
        "逾期数量", QString::number(overdue)
    };
    for (int i = 0; i < items.size(); i += 2) {
        int row = summaryTable->rowCount();
        summaryTable->insertRow(row);
        summaryTable->setItem(row, 0, new QTableWidgetItem(items[i]));
        summaryTable->setItem(row, 1, new QTableWidgetItem(items[i + 1]));
    }

    std::vector<Book> sortedBooks = dm->sortBooksByBorrowCount();
    hotBooksTable->setRowCount(0);
    for (int i = 0; i < (int)sortedBooks.size() && i < 5; ++i) {
        int row = hotBooksTable->rowCount();
        hotBooksTable->insertRow(row);
        hotBooksTable->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));
        hotBooksTable->setItem(row, 1, new QTableWidgetItem(sortedBooks[i].getTitle()));
        hotBooksTable->setItem(row, 2, new QTableWidgetItem(QString::number(sortedBooks[i].getBorrowCount())));
    }

    overdueTable->setRowCount(0);
    for (auto& record : records) {
        if (!record.isReturned() && record.calculateOverdueDays() > 0) {
            Book* book = dm->findBookByISBN(record.getISBN());
            int row = overdueTable->rowCount();
            overdueTable->insertRow(row);
            overdueTable->setItem(row, 0, new QTableWidgetItem(record.getISBN()));
            overdueTable->setItem(row, 1, new QTableWidgetItem(book ? book->getTitle() : "未知"));
            overdueTable->setItem(row, 2, new QTableWidgetItem(record.getReaderID()));
            overdueTable->setItem(row, 3, new QTableWidgetItem(QString::number(record.calculateOverdueDays())));
        }
    }
}

void StatisticsWidget::onRefreshStatistics()
{
    updateStatistics();
}

void StatisticsWidget::onGenerateReportClicked()
{
    DataManager *dm = DataManager::getInstance();
    std::vector<User*> users = dm->getUsers();
    int totalUsers = users.size();
    int admins = 0, readers = 0;
    for (auto user : users) {
        if (user->getType() == 1) admins++;
        else readers++;
    }

    std::vector<Book> books = dm->getBooks();
    std::vector<BorrowRecord> records = dm->getBorrowRecords();
    int total = records.size();
    int returned = 0, overdueCount = 0;
    for (auto& record : records) {
        if (record.isReturned()) returned++;
        if (!record.isReturned() && record.calculateOverdueDays() > 0) overdueCount++;
    }

    std::vector<Book> sortedBooks = dm->sortBooksByBorrowCount();

    QString report;
    report += "========================================\n";
    report += "          图书管理系统统计报表\n";
    report += "========================================\n\n";
    report += QString("统计时间: %1\n\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    report += QString("[用户统计]\n  总用户数: %1\n  管理员数: %2\n  读者数: %3\n\n").arg(totalUsers).arg(admins).arg(readers);
    report += QString("[图书统计]\n  图书总数: %1\n\n").arg((int)books.size());

    double returnRate = total > 0 ? (double)returned / total * 100 : 0;
    report += QString("[借阅统计]\n  总借阅次数: %1\n  已归还次数: %2\n  借阅归还率: %3%\n  逾期数量: %4\n\n")
              .arg(total).arg(returned).arg(QString::number(returnRate, 'f', 2)).arg(overdueCount);

    report += "[热门图书 TOP5]\n";
    for (int i = 0; i < (int)sortedBooks.size() && i < 5; i++) {
        report += QString("  %1. %2 - 借阅%3次\n").arg(i+1).arg(sortedBooks[i].getTitle()).arg(sortedBooks[i].getBorrowCount());
    }

    report += "\n========================================\n";

    QString appDir = QCoreApplication::applicationDirPath();
    QString reportFileName = QString("report_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    QString reportPath = appDir + "/reports/" + reportFileName;

    QDir reportDir(appDir + "/reports");
    if (!reportDir.exists()) {
        reportDir.mkpath(".");
    }

    QFile reportFile(reportPath);
    if (reportFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&reportFile);
        out << report;
        reportFile.close();
        QMessageBox::information(this, "成功", QString("报表已生成！\n保存位置: %1").arg(reportPath));
    } else {
        QMessageBox::warning(this, "失败", "报表生成失败，无法写入文件。");
    }
}
