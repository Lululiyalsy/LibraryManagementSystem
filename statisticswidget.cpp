#include "statisticswidget.h"
#include "DataManager.h"
#include "Admin.h"
#include "User.h"
#include "Book.h"
#include "BorrowRecord.h"
#include <QHeaderView>
#include <QGroupBox>
#include <QFrame>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent)
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
    mainLayout->setContentsMargins(20, 20, 20, 20);

    titleLabel = new QLabel("📊 统计报表", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *statsRow1 = new QHBoxLayout();
    statsRow1->setSpacing(15);

    QGroupBox *userGroup = new QGroupBox("👥 用户统计", this);
    userGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *userLayout = new QVBoxLayout(userGroup);
    userStatsLabel = new QLabel("总用户: 0\n管理员: 0\n读者: 0", userGroup);
    userStatsLabel->setStyleSheet("font-size: 14px; color: #34495e;");
    userLayout->addWidget(userStatsLabel);
    statsRow1->addWidget(userGroup);

    QGroupBox *bookGroup = new QGroupBox("📚 图书统计", this);
    bookGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *bookLayout = new QVBoxLayout(bookGroup);
    bookStatsLabel = new QLabel("总图书: 0", bookGroup);
    bookStatsLabel->setStyleSheet("font-size: 14px; color: #34495e;");
    bookLayout->addWidget(bookStatsLabel);
    statsRow1->addWidget(bookGroup);

    QGroupBox *borrowGroup = new QGroupBox("📖 借阅统计", this);
    borrowGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *borrowLayout = new QVBoxLayout(borrowGroup);
    borrowStatsLabel = new QLabel("总借阅: 0\n已归还: 0\n借阅率: 0%", borrowGroup);
    borrowStatsLabel->setStyleSheet("font-size: 14px; color: #34495e;");
    borrowLayout->addWidget(borrowStatsLabel);
    statsRow1->addWidget(borrowGroup);

    QGroupBox *overdueGroup = new QGroupBox("⚠️ 逾期统计", this);
    overdueGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *overdueLayout = new QVBoxLayout(overdueGroup);
    overdueStatsLabel = new QLabel("逾期数量: 0", overdueGroup);
    overdueStatsLabel->setStyleSheet("font-size: 14px; color: #e74c3c;");
    overdueLayout->addWidget(overdueStatsLabel);
    statsRow1->addWidget(overdueGroup);

    mainLayout->addLayout(statsRow1);

    QHBoxLayout *tablesLayout = new QHBoxLayout();
    tablesLayout->setSpacing(15);

    QGroupBox *hotBooksGroup = new QGroupBox("🔥 热门图书 TOP5", this);
    hotBooksGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *hotBooksLayout = new QVBoxLayout(hotBooksGroup);
    hotBooksTable = new QTableWidget(this);
    hotBooksTable->setColumnCount(3);
    hotBooksTable->setHorizontalHeaderLabels({"排名", "书名", "借阅次数"});
    hotBooksTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    hotBooksTable->verticalHeader()->setVisible(false);
    hotBooksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    hotBooksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    hotBooksTable->setStyleSheet("QTableWidget { background-color: white; }");
    hotBooksLayout->addWidget(hotBooksTable);
    tablesLayout->addWidget(hotBooksGroup);

    QGroupBox *overdueRecordsGroup = new QGroupBox("⚠️ 逾期图书列表", this);
    overdueRecordsGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout *overdueLayout2 = new QVBoxLayout(overdueRecordsGroup);
    overdueTable = new QTableWidget(this);
    overdueTable->setColumnCount(4);
    overdueTable->setHorizontalHeaderLabels({"ISBN", "书名", "读者ID", "逾期天数"});
    overdueTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    overdueTable->verticalHeader()->setVisible(false);
    overdueTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    overdueTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    overdueTable->setStyleSheet("QTableWidget { background-color: white; }");
    overdueLayout2->addWidget(overdueTable);
    tablesLayout->addWidget(overdueRecordsGroup);

    mainLayout->addLayout(tablesLayout);

    generateReportBtn = new QPushButton("📄 生成报表", this);
    generateReportBtn->setFixedHeight(40);
    generateReportBtn->setStyleSheet("QPushButton { background-color: #3498db; color: white; border-radius: 5px; font-size: 14px; } QPushButton:hover { background-color: #2980b9; }");
    connect(generateReportBtn, &QPushButton::clicked, this, &StatisticsWidget::onGenerateReportClicked);
    mainLayout->addWidget(generateReportBtn);

    mainLayout->addStretch();
}

void StatisticsWidget::setTotalUsers(int total, int admins, int readers)
{
    userStatsLabel->setText(QString("总用户: %1\n管理员: %2\n读者: %3").arg(total).arg(admins).arg(readers));
}

void StatisticsWidget::setTotalBooks(int total)
{
    bookStatsLabel->setText(QString("总图书: %1").arg(total));
}

void StatisticsWidget::setBorrowStats(int total, int returned, int overdue)
{
    double returnRate = total > 0 ? (double)returned / total * 100 : 0;
    borrowStatsLabel->setText(QString("总借阅: %1\n已归还: %2\n归还率: %3%").arg(total).arg(returned).arg(QString::number(returnRate, 'f', 1)));

    if (overdue > 0) {
        overdueStatsLabel->setText(QString("⚠️ 逾期数量: %1").arg(overdue));
        overdueStatsLabel->setStyleSheet("font-size: 14px; color: #e74c3c; font-weight: bold;");
    } else {
        overdueStatsLabel->setText("✅ 暂无逾期图书");
        overdueStatsLabel->setStyleSheet("font-size: 14px; color: #27ae60;");
    }
}

void StatisticsWidget::setHotBooks(const QVector<QPair<QString, int>>& hotBooks)
{
    hotBooksTable->setRowCount(0);
    for (int i = 0; i < hotBooks.size() && i < 5; ++i) {
        hotBooksTable->insertRow(i);
        hotBooksTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        hotBooksTable->setItem(i, 1, new QTableWidgetItem(hotBooks[i].first));
        hotBooksTable->setItem(i, 2, new QTableWidgetItem(QString::number(hotBooks[i].second)));
    }
}

void StatisticsWidget::setOverdueRecords(const QVector<QStringList>& records)
{
    overdueTable->setRowCount(0);
    for (int i = 0; i < records.size(); ++i) {
        overdueTable->insertRow(i);
        for (int j = 0; j < records[i].size() && j < 4; ++j) {
            overdueTable->setItem(i, j, new QTableWidgetItem(records[i][j]));
        }
    }
}

void StatisticsWidget::onGenerateReportClicked()
{
    Admin *admin = dynamic_cast<Admin*>(sender());
    Q_UNUSED(admin);

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

    report += "【用户统计】\n";
    report += QString("  总用户数: %1\n").arg(totalUsers);
    report += QString("  管理员数: %1\n").arg(admins);
    report += QString("  读者数: %1\n\n").arg(readers);

    report += "【图书统计】\n";
    report += QString("  图书总数: %1\n\n").arg((int)books.size());

    double returnRate = total > 0 ? (double)returned / total * 100 : 0;
    report += "【借阅统计】\n";
    report += QString("  总借阅次数: %1\n").arg(total);
    report += QString("  已归还次数: %1\n").arg(returned);
    report += QString("  借阅归还率: %1%\n").arg(QString::number(returnRate, 'f', 2));
    report += QString("  逾期数量: %1\n\n").arg(overdueCount);

    report += "【热门图书 TOP5】\n";
    for (int i = 0; i < sortedBooks.size() && i < 5; i++) {
        report += QString("  %1. 《%2》 - 借阅%3次\n").arg(i+1).arg(sortedBooks[i].getTitle()).arg(sortedBooks[i].getBorrowCount());
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
    }

    QMessageBox::information(this, "提示", QString("报表已生成！\n保存位置: %1").arg(reportPath));
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
    setTotalUsers(totalUsers, admins, readers);

    std::vector<Book> books = dm->getBooks();
    setTotalBooks(books.size());

    std::vector<BorrowRecord> records = dm->getBorrowRecords();
    int total = records.size();
    int returned = 0, overdue = 0;
    for (auto& record : records) {
        if (record.isReturned()) returned++;
        if (!record.isReturned() && record.calculateOverdueDays() > 0) overdue++;
    }
    setBorrowStats(total, returned, overdue);

    std::vector<Book> sortedBooks = dm->sortBooksByBorrowCount();
    QVector<QPair<QString, int>> hotBooks;
    for (int i = 0; i < sortedBooks.size() && i < 5; ++i) {
        hotBooks.append(qMakePair(sortedBooks[i].getTitle(), sortedBooks[i].getBorrowCount()));
    }
    setHotBooks(hotBooks);

    QVector<QStringList> overdueRecords;
    for (auto& record : records) {
        if (!record.isReturned() && record.calculateOverdueDays() > 0) {
            Book* book = dm->findBookByISBN(record.getISBN());
            QStringList row;
            row << record.getISBN();
            row << (book ? book->getTitle() : "未知");
            row << record.getReaderID();
            row << QString::number(record.calculateOverdueDays());
            overdueRecords.append(row);
        }
    }
    setOverdueRecords(overdueRecords);
}