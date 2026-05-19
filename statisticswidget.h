#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVector>
#include <QPair>
#include <QStringList>
#include <QMessageBox>

class DataManager;
class User;
class Admin;
class Book;
class BorrowRecord;

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

    void updateStatistics();
    void setTotalUsers(int total, int admins, int readers);
    void setTotalBooks(int total);
    void setBorrowStats(int total, int returned, int overdue);
    void setHotBooks(const QVector<QPair<QString, int>>& hotBooks);
    void setOverdueRecords(const QVector<QStringList>& records);

public slots:
    void onGenerateReportClicked();

private:
    void setupUI();

    QLabel *titleLabel;
    QLabel *userStatsLabel;
    QLabel *bookStatsLabel;
    QLabel *borrowStatsLabel;
    QLabel *overdueStatsLabel;
    QTableWidget *hotBooksTable;
    QTableWidget *overdueTable;
    QPushButton *generateReportBtn;
};

#endif // STATISTICSWIDGET_H