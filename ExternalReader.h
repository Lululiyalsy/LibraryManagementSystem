/**
 * @file ExternalReader.h
 * @brief 外部读者类定义
 *
 * ExternalReader类继承自Reader类，表示外部类型的读者。
 * 构造时设置外部读者借阅策略：最大借阅3本，借期15天，续借15天，2元/天罚款，需缴纳200元押金。
 */

#ifndef EXTERNALREADER_H
#define EXTERNALREADER_H

#include "Reader.h"

/**
 * @class ExternalReader
 * @brief 外部读者类
 *
 * 继承自Reader类，设置外部读者借阅策略：
 * - 最大借阅3本
 * - 借期15天
 * - 续借15天
 * - 1次续借机会
 * - 2元/天罚款
 * - 2分/天信用扣减
 * - 1分按时奖励
 * - 可预约（需缴纳押金）
 * - 预约上限3本
 * - 200元押金
 * 新增押金管理功能，借书前需先缴纳押金。
 */
class ExternalReader : public Reader
{
public:
    /**
     * @brief 构造函数
     * @param I 用户ID
     * @param n 用户姓名
     * @param pa 用户密码
     * @param ph 联系电话
     * @param e 电子邮箱
     */
    ExternalReader(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    /**
     * @brief 获取用户身份标识（覆盖Reader的虚函数）
     * @return 返回"外部读者"
     */
    QString typeToIdentity() override;

    // ========== 押金管理 ==========

    /**
     * @brief 缴纳押金
     * @return 缴纳成功返回true，已缴纳返回false
     *
     * 缴纳金额为策略中配置的押金金额，缴纳后设置depositPaid为true。
     */
    bool payDeposit();

    /**
     * @brief 退还押金
     * @return 退还成功返回true，未缴纳或存在未结清罚款返回false
     *
     * 退还前检查是否存在未结清的罚款或未归还的图书。
     */
    bool refundDeposit();

    /**
     * @brief 检查是否已缴纳押金
     * @return true表示已缴纳，false表示未缴纳
     */
    bool isDepositPaid() const;

    /**
     * @brief 设置押金缴纳状态
     * @param paid 是否已缴纳押金
     */
    void setDepositPaid(bool paid);

    /**
     * @brief 获取押金金额
     * @return 押金金额
     */
    double getDeposit() const;

    /**
     * @brief 析构函数
     */
    ~ExternalReader();

private:
    static const double DEPOSIT_AMOUNT; ///< 押金金额常量（200元）

    bool depositPaid; ///< 是否已缴纳押金
    double deposit;  ///< 押金金额
};

#endif // EXTERNALREADER_H
