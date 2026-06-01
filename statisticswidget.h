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

/**
 * @class StatisticsWidget
 * @brief 统计报表界面组件
 * 
 * 该组件用于显示图书管理系统的统计报表，包括：
 * - 用户统计（总用户数、管理员数、读者数）
 * - 图书统计（图书总数、分类分布）
 * - 借阅统计（总借阅次数、当前借阅数量、已归还次数、归还率、逾期率）
 * - 热门图书 TOP5
 * - 逾期图书列表
 * 
 * 支持功能：
 * - 刷新统计数据
 * - 生成统计报表文件
 * - 发送报表生成通知消息
 */
class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     * @param user 当前登录用户（管理员）
     */
    explicit StatisticsWidget(QWidget *parent = nullptr, User *user = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~StatisticsWidget();

    /**
     * @brief 更新统计数据
     * 
     * 重新从 DataManager 获取最新数据并更新界面显示
     */
    void updateStatistics();

public slots:
    /**
     * @brief 生成报表按钮点击事件
     * 
     * 生成统计报表文件并保存到 reports 目录，同时发送消息通知管理员
     */
    void onGenerateReportClicked();
    
    /**
     * @brief 刷新数据按钮点击事件
     * 
     * 调用 updateStatistics() 更新统计数据显示
     */
    void onRefreshStatistics();

private:
    /**
     * @brief 初始化界面布局
     * 
     * 创建三个可拖动的区域：统计摘要、热门图书、逾期图书
     */
    void setupUI();

    QTableWidget *summaryTable;      ///< 统计摘要表格
    QTableWidget *hotBooksTable;     ///< 热门图书表格
    QTableWidget *overdueTable;      ///< 逾期图书表格
    QPushButton *generateReportBtn;  ///< 生成报表按钮
    QPushButton *refreshBtn;         ///< 刷新数据按钮
    User *currentUser;               ///< 当前登录用户（用于发送消息）
};

#endif // STATISTICSWIDGET_H
