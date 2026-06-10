/**
 * @file ExternalReader.cpp
 * @brief 外部读者类实现
 *
 * 实现ExternalReader类的构造函数、身份标识方法和押金管理功能。
 * 构造时设置外部读者借阅策略参数。
 */

#include "ExternalReader.h"
#include "DataManager.h"

// 静态常量定义
const double ExternalReader::DEPOSIT_AMOUNT = 200.0;

/**
 * @brief 构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 *
 * 初始化外部读者，设置角色为EXTERNAL，设置外部读者借阅策略：
 * - 最大借阅3本
 * - 借期15天
 * - 续借15天
 * - 3元/天罚款
 * - 2分/天信用扣减
 * - 1分按时奖励
 * - 可预约（需缴纳押金）
 * - 200元押金
 */
ExternalReader::ExternalReader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : Reader(I, n, pa, ph, e), depositPaid(false), deposit(DEPOSIT_AMOUNT)
{
    role = Role::EXTERNAL;
    maxBooks = 3;
    borrowDays = 15;
    renewDays = 15;
    maxRenewTimes = 1;
    finePerDay = 3.0;
    creditDeductPerDay = 2;
    creditReward = 1;
}

/**
 * @brief 获取用户身份标识（覆盖Reader的虚函数）
 * @return 返回"外部读者"
 */
QString ExternalReader::typeToIdentity()
{
    return QString("外部读者");
}

// ========== 押金管理 ==========

/**
 * @brief 缴纳押金
 * @return 缴纳成功返回true，已缴纳返回false
 *
 * 缴纳金额为策略中配置的押金金额，缴纳后设置depositPaid为true，
 * 并发送消息通知读者。
 */
bool ExternalReader::payDeposit()
{
    if (depositPaid)
    {
        return false; // 已缴纳，不可重复缴纳
    }

    depositPaid = true;

    // 发送消息通知读者
    DataManager *dm = DataManager::getInstance();

    QString msgContent = QString("您已成功缴纳押金%1元，现在可以借阅图书。")
                             .arg(deposit);
    Message msg(ID, getName(), msgContent);
    addMessage(msg);
    dm->writeMessage();

    return true;
}

/**
 * @brief 退还押金
 * @return 退还成功返回true，未缴纳或存在未结清罚款返回false
 *
 * 退还前检查是否存在未结清的罚款或未归还的图书，
 * 确认无欠款后退还押金并发送消息通知读者。
 */
bool ExternalReader::refundDeposit()
{
    if (!depositPaid)
    {
        return false; // 未缴纳，无法退还
    }

    // 检查是否有未归还的图书（归还后记录会被删除）
    DataManager *dm = DataManager::getInstance();
    std::vector<BorrowRecord> records = dm->getBorrowRecordsByReader(ID);
    if (!records.empty())
    {
        return false; // 存在未归还图书，无法退还押金
    }

    // 检查是否有未结清罚款（归还后不应该有罚款记录，但为了安全检查一下）
    for (const auto &record : records)
    {
        if (record.calculateFine(finePerDay) > record.getPaidFine() + 0.01)
        {
            return false; // 存在未结清罚款，无法退还押金
        }
    }

    depositPaid = false;

    QString msgContent = QString("您的押金%1元已退还，感谢使用。")
                             .arg(deposit);
    Message msg(ID, getName(), msgContent);
    addMessage(msg);
    dm->writeMessage();

    return true;
}

/**
 * @brief 检查是否已缴纳押金
 * @return true表示已缴纳，false表示未缴纳
 */
bool ExternalReader::isDepositPaid() const
{
    return depositPaid;
}

/**
 * @brief 设置押金缴纳状态
 * @param paid 是否已缴纳押金
 */
void ExternalReader::setDepositPaid(bool paid)
{
    depositPaid = paid;
}

/**
 * @brief 获取押金金额
 * @return 押金金额
 */
double ExternalReader::getDeposit() const
{
    return deposit;
}

/**
 * @brief 析构函数
 */
ExternalReader::~ExternalReader()
{
}
