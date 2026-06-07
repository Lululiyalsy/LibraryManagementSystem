/**
 * @file TeacherReader.h
 * @brief 教师读者类定义
 *
 * TeacherReader类继承自Reader类，表示教师类型的读者。
 * 构造时自动注入教师借阅策略，借阅上限10本，借期60天。
 */

#ifndef TEACHERREADER_H
#define TEACHERREADER_H

#include "Reader.h"

/**
 * @class TeacherReader
 * @brief 教师读者类
 *
 * 继承自Reader类，自动注入教师借阅策略（BorrowPolicy::teacherPolicy）。
 * 教师读者特点：10本借阅上限、60天借期、0.5元/天罚款、可预约、无押金。
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
