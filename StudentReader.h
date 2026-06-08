/**
 * @file StudentReader.h
 * @brief 学生读者类定义
 *
 * StudentReader类继承自Reader类，表示学生类型的读者。
 * 构造时设置学生借阅策略：最大借阅5本，借期30天，续借30天，1元/天罚款。
 */

#ifndef STUDENTREADER_H
#define STUDENTREADER_H

#include "Reader.h"

/**
 * @class StudentReader
 * @brief 学生读者类
 *
 * 继承自Reader类，设置学生借阅策略：
 * - 最大借阅5本
 * - 借期30天
 * - 续借30天
 * - 1次续借机会
 * - 2元/天罚款
 * - 无押金
 */
class StudentReader : public Reader
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
    StudentReader(QString &I, QString &n, QString &pa, QString &ph, QString &e);

    /**
     * @brief 获取用户身份标识（覆盖Reader的虚函数）
     * @return 返回"学生读者"
     */
    QString typeToIdentity() override;

    /**
     * @brief 析构函数
     */
    ~StudentReader();
};

#endif // STUDENTREADER_H
