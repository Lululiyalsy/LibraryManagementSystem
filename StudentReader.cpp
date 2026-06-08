/**
 * @file StudentReader.cpp
 * @brief 学生读者类实现
 *
 * 实现StudentReader类的构造函数和身份标识方法。
 * 构造时设置学生借阅策略参数。
 */

#include "StudentReader.h"

/**
 * @brief 构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 *
 * 初始化学生读者，设置角色为STUDENT，设置学生借阅策略：
 * - 最大借阅5本
 * - 借期30天
 * - 续借30天
 * - 2元/天罚款
 * - 1分/天信用扣减
 * - 2分按时奖励
 * - 无押金
 */
StudentReader::StudentReader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : Reader(I, n, pa, ph, e)
{
    role = Role::STUDENT;
    maxBooks = 5;
    borrowDays = 30;
    renewDays = 30;
    maxRenewTimes = 1;
    finePerDay = 2.0;
    creditDeductPerDay = 2;
    creditReward = 2;
}

/**
 * @brief 获取用户身份标识（覆盖Reader的虚函数）
 * @return 返回"学生读者"
 */
QString StudentReader::typeToIdentity()
{
    return QString("学生读者");
}

/**
 * @brief 析构函数
 */
StudentReader::~StudentReader()
{
}
