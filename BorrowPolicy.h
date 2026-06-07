/**
 * @file BorrowPolicy.h
 * @brief 借阅策略类定义
 *
 * BorrowPolicy类定义不同读者类型的借阅规则配置，包括最大借阅数量、
 * 借阅天数、续借天数、罚款标准、信用分扣减规则、预约权限和押金等。
 */

#ifndef BORROWPOLICY_H
#define BORROWPOLICY_H

/**
 * @class BorrowPolicy
 * @brief 借阅策略类
 *
 * 纯数据配置类，定义不同读者类型的借阅规则。
 * 通过预定义的静态工厂方法提供三种读者类型的策略配置：
 * - 学生读者：5本/30天/1元每天罚款
 * - 教师读者：10本/60天/0.5元每天罚款
 * - 外部读者：3本/15天/2元每天罚款/不可预约/需押金
 */
class BorrowPolicy
{
public:
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
    BorrowPolicy(int maxBooks, int borrowDays, int renewDays, int maxRenewTimes,
                 double finePerDay, int creditDeductPerDay, int creditReward,
                 bool canReserve, int maxReservations, double deposit);

    // ========== 静态方法 ==========

    /**
     * @brief 获取学生读者策略
     * @return 学生读者借阅策略
     *
     * 配置：5本/30天/续借30天/1次续借/1元每天罚款/
     *       1分每天信用扣减/2分按时奖励/可预约/3个预约上限/无押金
     */
    static BorrowPolicy studentPolicy();

    /**
     * @brief 获取教师读者策略
     * @return 教师读者借阅策略
     *
     * 配置：10本/60天/续借30天/2次续借/0.5元每天罚款/
     *       0分每天信用扣减/1分按时奖励/可预约/5个预约上限/无押金
     */
    static BorrowPolicy teacherPolicy();

    /**
     * @brief 获取外部读者策略
     * @return 外部读者借阅策略
     *
     * 配置：3本/15天/续借15天/1次续借/2元每天罚款/
     *       2分每天信用扣减/1分按时奖励/可预约（需缴纳押金）/3个预约上限/200元押金
     */
    static BorrowPolicy externalPolicy();

    // ========== getter 方法 ==========

    /**
     * @brief 获取最大借阅数量
     * @return 最大借阅数量
     */
    int getMaxBooks() const;

    /**
     * @brief 获取默认借阅天数
     * @return 默认借阅天数
     */
    int getBorrowDays() const;

    /**
     * @brief 获取续借延长天数
     * @return 续借延长天数
     */
    int getRenewDays() const;

    /**
     * @brief 获取最大续借次数
     * @return 最大续借次数
     */
    int getMaxRenewTimes() const;

    /**
     * @brief 获取每日逾期罚款金额
     * @return 每日逾期罚款金额
     */
    double getFinePerDay() const;

    /**
     * @brief 获取每日逾期信用分扣减
     * @return 每日逾期信用分扣减
     */
    int getCreditDeductPerDay() const;

    /**
     * @brief 获取按时还书信用分奖励
     * @return 按时还书信用分奖励
     */
    int getCreditReward() const;

    /**
     * @brief 获取是否允许预约
     * @return true表示允许预约，false表示不允许
     */
    bool canReserve() const;

    /**
     * @brief 获取最大预约数量
     * @return 最大预约数量
     */
    int getMaxReservations() const;

    /**
     * @brief 获取押金金额
     * @return 押金金额（0表示无需押金）
     */
    double getDeposit() const;

    /**
     * @brief 析构函数
     */
    ~BorrowPolicy();

private:
    int m_maxBooks;            ///< 最大借阅数量
    int m_borrowDays;          ///< 默认借阅天数
    int m_renewDays;           ///< 续借延长天数
    int m_maxRenewTimes;       ///< 最大续借次数
    double m_finePerDay;       ///< 每日逾期罚款金额
    int m_creditDeductPerDay;  ///< 每日逾期信用分扣减
    int m_creditReward;        ///< 按时还书信用分奖励
    bool m_canReserve;         ///< 是否允许预约
    int m_maxReservations;     ///< 最大预约数量
    double m_deposit;          ///< 押金金额（0表示无需押金）
};

#endif // BORROWPOLICY_H
