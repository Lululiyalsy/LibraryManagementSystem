/**
 * @file TeacherReader.cpp
 * @brief 教师读者类实现
 *
 * 实现TeacherReader类的构造函数和身份标识方法。
 * 构造时自动设置角色为教师并注入教师借阅策略。
 */

#include "TeacherReader.h"
#include "BorrowPolicy.h"

/**
 * @brief 构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 *
 * 初始化教师读者，设置角色为TEACHER，注入教师借阅策略：
 * - 最大借阅10本
 * - 借期60天
 * - 续借30天
 * - 0.5元/天罚款
 * - 可预约，预约上限5本
 * - 无押金
 */
TeacherReader::TeacherReader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : Reader(I, n, pa, ph, e)
{
    role = Role::TEACHER;
    maxBooks = 10;
    BorrowPolicy policy = BorrowPolicy::teacherPolicy();
    setPolicy(new BorrowPolicy(policy));
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
