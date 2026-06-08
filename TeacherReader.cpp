/**
 * @file TeacherReader.cpp
 * @brief 教师读者类实现
 *
 * 实现TeacherReader类的构造函数和身份标识方法。
 * 构造时设置教师借阅策略参数。
 */

#include "TeacherReader.h"

/**
 * @brief 构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 *
 * 初始化教师读者，设置角色为TEACHER，设置教师借阅策略：
 * - 最大借阅10本
 * - 借期60天
 * - 续借30天
 * - 2次续借机会
 * - 1元/天罚款
 * - 1分逾期扣除
 * - 2分按时奖励
 * - 无押金
 */
TeacherReader::TeacherReader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : Reader(I, n, pa, ph, e)
{
    role = Role::TEACHER;
    maxBooks = 10;
    borrowDays = 60;
    renewDays = 30;
    maxRenewTimes = 2;
    finePerDay = 1.0;
    creditDeductPerDay = 1;
    creditReward = 2;
}

/**
 * @brief 获取用户身份标识（覆盖Reader的虚函数）
 * @return 返回"教师读者"
 */
QString TeacherReader::typeToIdentity()
{
    return QString("教师读者");
}

/**
 * @brief 析构函数
 */
TeacherReader::~TeacherReader()
{
}
