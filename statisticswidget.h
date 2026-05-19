#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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

public slots:
    void onGenerateReportClicked();
    void onRefreshStatistics();

private:
    void setupUI();

    QTableWidget *summaryTable;
    QTableWidget *hotBooksTable;
    QTableWidget *overdueTable;
    QPushButton *generateReportBtn;
    QPushButton *refreshBtn;
};

#endif // STATISTICSWIDGET_H
