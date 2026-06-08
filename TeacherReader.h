/**
 * @file TeacherReader.h
 * @brief 教师读者类定义
 *
 * TeacherReader类继承自Reader类，表示教师类型的读者。
 * 构造时设置教师借阅策略：最大借阅10本，借期60天，续借30天，0.5元/天罚款。
 */

#ifndef TEACHERREADER_H
#define TEACHERREADER_H

#include "Reader.h"

/**
 * @class TeacherReader
 * @brief 教师读者类
 *
 * 继承自Reader类，设置教师借阅策略：
 * - 最大借阅10本
 * - 借期60天
 * - 续借30天
 * - 2次续借机会
 * - 1元/天罚款
 * - 1分逾期扣除
 * - 2分按时奖励
 * - 无押金
 */
class TeacherReader : public Reader
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
    TeacherReader(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    /**
     * @brief 获取用户身份标识（覆盖Reader的虚函数）
     * @return 返回"教师读者"
     */
    QString typeToIdentity() override;

    /**
     * @brief 析构函数
     */
    ~TeacherReader();
};

#endif // TEACHERREADER_H
