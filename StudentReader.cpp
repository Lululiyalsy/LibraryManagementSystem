/**
 * @file StudentReader.cpp
 * @brief 学生读者类实现
 *
 * 实现StudentReader类的构造函数和身份标识方法。
 * 构造时自动设置角色为学生并注入学生借阅策略。
 */

#include "StudentReader.h"
#include "BorrowPolicy.h"

/**
 * @brief 构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 *
 * 初始化学生读者，设置角色为STUDENT，注入学生借阅策略：
 * - 最大借阅5本
 * - 借期30天
 * - 续借30天
 * - 1元/天罚款
 * - 可预约，预约上限3本
 * - 无押金
 */
StudentReader::StudentReader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : Reader(I, n, pa, ph, e)
{
    role = Role::STUDENT;
    maxBooks = 5;
    BorrowPolicy policy = BorrowPolicy::studentPolicy();
    setPolicy(new BorrowPolicy(policy));
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
