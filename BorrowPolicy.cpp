/**
 * @file BorrowPolicy.cpp
 * @brief 借阅策略类实现
 *
 * 实现BorrowPolicy类的所有成员函数，包括构造函数、静态工厂方法和getter方法。
 * 提供三种预定义的读者类型策略配置。
 */

#include "BorrowPolicy.h"

/**
 * @brief 构造函数
 * @param maxBooks 最大借阅数量
 * @param borrowDays 默认借阅天数
 * @param renewDays 续借延长天数
 * @param maxRenewTimes 最大续借次数
 * @param finePerDay 每日逾期罚款金额
 * @param creditDeductPerDay 每日逾期信用分扣减
 * @param creditReward 按时还书信用分奖励
 * @param canReserve 是否允许预约
 * @param maxReservations 最大预约数量
 * @param deposit 押金金额（0表示无需押金）
 */
BorrowPolicy::BorrowPolicy(int maxBooks, int borrowDays, int renewDays, int maxRenewTimes,
                           double finePerDay, int creditDeductPerDay, int creditReward,
                           bool canReserve, int maxReservations, double deposit)
    : m_maxBooks(maxBooks), m_borrowDays(borrowDays), m_renewDays(renewDays),
      m_maxRenewTimes(maxRenewTimes), m_finePerDay(finePerDay),
      m_creditDeductPerDay(creditDeductPerDay), m_creditReward(creditReward),
      m_canReserve(canReserve), m_maxReservations(maxReservations), m_deposit(deposit)
{
}

// ========== 静态工厂方法 ==========

/**
 * @brief 获取学生读者策略
 * @return 学生读者借阅策略
 *
 * 配置：5本/30天/续借30天/1次续借/1元每天罚款/
 *       1分每天信用扣减/2分按时奖励/可预约/3个预约上限/无押金
 */
BorrowPolicy BorrowPolicy::studentPolicy()
{
    return BorrowPolicy(5, 30, 30, 1, 1.0, 1, 2, true, 3, 0);
}

/**
 * @brief 获取教师读者策略
 * @return 教师读者借阅策略
 *
 * 配置：10本/60天/续借30天/2次续借/0.5元每天罚款/
 *       0分每天信用扣减/1分按时奖励/可预约/5个预约上限/无押金
 */
BorrowPolicy BorrowPolicy::teacherPolicy()
{
    return BorrowPolicy(10, 60, 30, 2, 0.5, 0, 1, true, 5, 0);
}

/**
 * @brief 获取外部读者策略
 * @return 外部读者借阅策略
 *
 * 配置：3本/15天/续借15天/1次续借/2元每天罚款/
 *       2分每天信用扣减/1分按时奖励/可预约（需缴纳押金）/3个预约上限/200元押金
 */
BorrowPolicy BorrowPolicy::externalPolicy()
{
    return BorrowPolicy(3, 15, 15, 1, 2.0, 2, 1, true, 3, 200);
}

// ========== getter 方法 ==========

/**
 * @brief 获取最大借阅数量
 * @return 最大借阅数量
 */
int BorrowPolicy::getMaxBooks() const { return m_maxBooks; }

/**
 * @brief 获取默认借阅天数
 * @return 默认借阅天数
 */
int BorrowPolicy::getBorrowDays() const { return m_borrowDays; }

/**
 * @brief 获取续借延长天数
 * @return 续借延长天数
 */
int BorrowPolicy::getRenewDays() const { return m_renewDays; }

/**
 * @brief 获取最大续借次数
 * @return 最大续借次数
 */
int BorrowPolicy::getMaxRenewTimes() const { return m_maxRenewTimes; }

/**
 * @brief 获取每日逾期罚款金额
 * @return 每日逾期罚款金额
 */
double BorrowPolicy::getFinePerDay() const { return m_finePerDay; }

/**
 * @brief 获取每日逾期信用分扣减
 * @return 每日逾期信用分扣减
 */
int BorrowPolicy::getCreditDeductPerDay() const { return m_creditDeductPerDay; }

/**
 * @brief 获取按时还书信用分奖励
 * @return 按时还书信用分奖励
 */
int BorrowPolicy::getCreditReward() const { return m_creditReward; }

/**
 * @brief 获取是否允许预约
 * @return true表示允许预约，false表示不允许
 */
bool BorrowPolicy::canReserve() const { return m_canReserve; }

/**
 * @brief 获取最大预约数量
 * @return 最大预约数量
 */
int BorrowPolicy::getMaxReservations() const { return m_maxReservations; }

/**
 * @brief 获取押金金额
 * @return 押金金额（0表示无需押金）
 */
double BorrowPolicy::getDeposit() const { return m_deposit; }

/**
 * @brief 析构函数
 */
BorrowPolicy::~BorrowPolicy()
{
}
