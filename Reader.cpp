/**
 * @file Reader.cpp
 * @brief 读者类实现
 *
 * 实现Reader类的所有成员函数，包括图书预约、借阅、还书、续借等操作，
 * 以及信用分管理和限制机制。
 */

#include "Reader.h"
#include "ExternalReader.h"
#include "DataManager.h"
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>

/**
 * @brief 构造函数
 * @param I 用户ID
 * @param n 用户姓名
 * @param pa 用户密码
 * @param ph 联系电话
 * @param e 电子邮箱
 */
Reader::Reader(QString &I, QString &n, QString &pa, QString &ph, QString &e)
    : User(I, n, pa, ph, e)
{
    type = 2;               // 读者类型为2
    maxBooks = 10;          // 默认最大借阅数量为10本
    borrowDays = 30;        // 默认借阅天数为30天
    renewDays = 30;         // 默认续借延长天数为30天
    maxRenewTimes = 1;      // 默认最大续借次数为1次
    finePerDay = 1.0;       // 默认每日逾期罚款为1元
    creditDeductPerDay = 1; // 默认每日信用分扣减为1分
    creditReward = 2;       // 默认按时还书信用分奖励为2分
    creditScore = 100;      // 默认信用分为100分
    prevCreditScore = 100;  // 初始时之前的信用分等于当前信用分
    role = Role::STUDENT;   // 默认角色为学生读者
}

/**
 * @brief 获取用户身份标识（实现User的纯虚函数）
 * @return 返回"读者"
 */
QString Reader::typeToIdentity()
{
    return QString("读者");
}

/**
 * @brief 生成验证码（实现User的纯虚函数）
 * @return 6位数字验证码
 */
QString Reader::generateVerificationCode()
{
    QString code;
    for (int i = 0; i < 6; ++i)
    {
        int digit = QRandomGenerator::global()->bounded(10);
        code.append(QString::number(digit));
    }
    return code;
}

// ========== getter 和 setter 方法 ==========

/**
 * @brief 获取最大借阅数量
 * @return 最大借阅数量
 */
int Reader::getMaxBooks() { return maxBooks; }

/**
 * @brief 获取当前信用分
 * @return 当前信用分
 */
int Reader::getCreditScore() { return creditScore; }

/**
 * @brief 获取之前的信用分
 * @return 之前的信用分（用于判断信用分是否下跌）
 */
int Reader::getPrevCreditScore() { return prevCreditScore; }

/**
 * @brief 获取限制到期时间
 * @return 限制到期时间
 */
QDateTime Reader::getBanUntil() { return banUntil; }

/**
 * @brief 设置最大借阅数量
 * @param max 最大借阅数量
 */
void Reader::setMaxBooks(int max) { maxBooks = max; }

/**
 * @brief 设置当前信用分
 * @param score 信用分数值
 */
void Reader::setCreditScore(int score) { creditScore = score; }

/**
 * @brief 设置之前的信用分
 * @param score 之前的信用分数值
 */
void Reader::setPrevCreditScore(int score) { prevCreditScore = score; }

/**
 * @brief 设置限制到期时间
 * @param time 限制到期时间
 */
void Reader::setBanUntil(QDateTime time) { banUntil = time; }

// ========== 借阅策略 getter 方法 ==========

/**
 * @brief 获取借阅天数
 * @return 借阅天数
 */
int Reader::getBorrowDays() const { return borrowDays; }

/**
 * @brief 获取续借延长天数
 * @return 续借延长天数
 */
int Reader::getRenewDays() const { return renewDays; }

/**
 * @brief 获取最大续借次数
 * @return 最大续借次数
 */
int Reader::getMaxRenewTimes() const { return maxRenewTimes; }

/**
 * @brief 获取每日逾期罚款金额
 * @return 每日逾期罚款金额
 */
double Reader::getFinePerDay() const { return finePerDay; }

/**
 * @brief 获取每日逾期信用分扣减
 * @return 每日逾期信用分扣减
 */
int Reader::getCreditDeductPerDay() const { return creditDeductPerDay; }

/**
 * @brief 获取按时还书信用分奖励
 * @return 按时还书信用分奖励
 */
int Reader::getCreditReward() const { return creditReward; }

// ========== 角色管理 ==========

/**
 * @brief 获取读者角色
 * @return 读者角色
 */
Reader::Role Reader::getRole() const { return role; }

/**
 * @brief 获取读者角色字符串描述
 * @return 角色中文名称（"学生读者"/"教师读者"/"外部读者"/"普通读者"）
 */
QString Reader::getRoleString() const
{
    switch (role)
    {
    case Role::STUDENT:
        return QString("学生读者");
    case Role::TEACHER:
        return QString("教师读者");
    case Role::EXTERNAL:
        return QString("外部读者");
    default:
        return QString("普通读者");
    }
}

/**
 * @brief 设置读者角色
 * @param r 读者角色
 */
void Reader::setRole(Role r) { role = r; }

// ========== 信用分管理 ==========

/**
 * @brief 检查是否在限制期间
 * @return true表示在限制期间，false表示正常
 */
bool Reader::isBanned() const
{
    if (!banUntil.isValid())
    {
        return false;
    }
    return QDateTime::currentDateTime() < banUntil;
}

/**
 * @brief 检查信用分是否允许操作
 * @return 信用分检查结果
 *
 * 根据当前信用分和限制状态，返回对应的限制级别：
 * - OK: 信用分正常（90分及以上）
 * - LOW_CREDIT_90: 低于90分，限制1天
 * - LOW_CREDIT_80: 低于80分，限制3天
 * - LOW_CREDIT_70: 低于70分，限制1周
 * - LOW_CREDIT_60: 低于60分，限制2周
 * - LOW_CREDIT_50: 低于50分，限制1个月
 */
Reader::CreditCheckResult Reader::checkCreditScore() const
{
    if (isBanned())
    {
        // 在限制期间，根据信用分返回对应的限制级别
        if (creditScore < 50)
        {
            return CreditCheckResult::LOW_CREDIT_50;
        }
        else if (creditScore < 60)
        {
            return CreditCheckResult::LOW_CREDIT_60;
        }
        else if (creditScore < 70)
        {
            return CreditCheckResult::LOW_CREDIT_70;
        }
        else if (creditScore < 80)
        {
            return CreditCheckResult::LOW_CREDIT_80;
        }
        else
        {
            return CreditCheckResult::LOW_CREDIT_90;
        }
    }

    // 不在限制期间，根据信用分判断是否需要限制
    if (creditScore < 50)
    {
        return CreditCheckResult::LOW_CREDIT_50;
    }
    else if (creditScore < 60)
    {
        return CreditCheckResult::LOW_CREDIT_60;
    }
    else if (creditScore < 70)
    {
        return CreditCheckResult::LOW_CREDIT_70;
    }
    else if (creditScore < 80)
    {
        return CreditCheckResult::LOW_CREDIT_80;
    }
    else if (creditScore < 90)
    {
        return CreditCheckResult::LOW_CREDIT_90;
    }
    return CreditCheckResult::OK;
}

/**
 * @brief 计算信用分变化
 * @param overdueDays 逾期天数
 * @param book 图书对象指针
 * @param isbn 图书ISBN
 *
 * 信用分计算规则：
 * - 逾期还书不扣分不加分（逾期扣分在登录时已计算）
 * - 按时还书根据策略奖励信用分（最高100分）
 */
void Reader::calculateCreditScore(int overdueDays, Book *book, const QString &isbn)
{
    if (overdueDays > 0)
    {
        return; // 逾期还书不扣分不加分（登录时已计算）
    }

    // 按时还书，奖励信用分
    int newScore = creditScore + creditReward;
    creditScore = qMin(newScore, 100); // 最高100分
}

// ========== 预约管理 ==========

/**
 * @brief 预约图书（按ISBN）
 * @param isbn 图书ISBN
 * @return 预约结果
 *
 * 预约流程：
 * 1. 检查图书是否存在
 * 2. 检查是否允许预约（根据策略配置）
 * 3. 检查是否已预约该图书
 * 4. 检查信用分是否允许操作
 * 5. 检查是否已借阅该图书
 * 6. 检查预约人数是否超过限制
 * 7. 创建预约记录并发送消息
 */
Reader::ReserveResult Reader::reserveBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 检查图书是否存在
    Book *book = dm->findBookByISBN(isbn);
    if (!book)
    {
        return ReserveResult::BOOK_NOT_FOUND;
    }

    // （检查押金）：校外读者未缴纳押金时不允许预约
    ExternalReader *extReader = dynamic_cast<ExternalReader *>(this);
    if (extReader && !extReader->isDepositPaid())
    {
        return ReserveResult::DEPOSIT_REQUIRED;
    }

    // 检查是否已预约
    if (dm->hasReservation(isbn, ID))
    {
        return ReserveResult::ALREADY_EXISTS;
    }

    // 检查信用分
    CreditCheckResult creditResult = checkCreditScore();
    if (creditResult != CreditCheckResult::OK)
    {
        return ReserveResult::LOW_CREDIT;
    }

    // 检查是否已借阅该图书
    std::vector<BorrowRecord> &allRecords = dm->getBorrowRecords();
    for (const auto &record : allRecords)
    {
        if (record.getISBN() == isbn && record.getReaderID() == ID && !record.isReturned())
        {
            return ReserveResult::ALREADY_EXISTS;
        }
    }

    // 检查预约人数限制
    int available = book->getStock() - book->getCurrentBorrowed();
    int currentReservationCount = dm->getReservationsByISBN(isbn).size();

    // 有库存时，预约量不超过剩余借书量的2倍
    if (available > 0 && currentReservationCount >= available * 2)
    {
        return ReserveResult::EXCEED_LIMIT;
    }

    // 创建预约记录
    QDateTime now = QDateTime::currentDateTime();
    Reservation reservation(isbn, ID, now, Reservation::PENDING);
    if (dm->addReservation(reservation))
    {
        // 发送消息给读者自己
        QString messageContent = QString("您已成功预约图书(ISBN:%1)，请等待管理员审核。").arg(isbn);
        Message message(ID, name, messageContent);
        this->addMessage(message);
        dm->writeMessage();

        return ReserveResult::SUCCESS;
    }
    return ReserveResult::BOOK_NOT_FOUND;
}

/**
 * @brief 取消预约（按ISBN）
 * @param isbn 图书ISBN
 * @return 取消成功返回true，失败返回false
 */
bool Reader::cancelReservation(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 调用 DataManager 的取消预约方法
    bool success = dm->cancelReservation(isbn, ID);
    if (success)
    {
        // 发送消息给读者自己
        QString messageContent = QString("您已取消预约图书(ISBN:%1)").arg(isbn);
        Message message(ID, name, messageContent);
        this->addMessage(message);
        dm->writeMessage();
    }
    return success;
}

/**
 * @brief 查看我的预约
 * @return 预约记录列表
 */
std::vector<Reservation> Reader::viewMyReservations()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getReservationsByReader(ID);
}

// ========== 借阅管理 ==========

/**
 * @brief 借书（按ISBN）
 * @param isbn 图书ISBN
 * @return 借书结果
 *
 * 借书流程：
 * 1. 检查图书是否存在
 * 2. 检查库存是否充足
 * 3. 检查信用分是否允许操作
 * 4. 检查是否已借阅该图书
 * 5. 检查是否有逾期未还图书
 * 6. 检查是否已达最大借阅数量
 * 7. 检查是否有有效的预约
 * 8. 创建借阅记录并发送消息
 */
Reader::BorrowResult Reader::borrowBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 检查图书是否存在
    Book *book = dm->findBookByISBN(isbn);
    if (!book)
        return BorrowResult::BOOK_NOT_FOUND;

    // 检查库存
    int available = book->getStock() - book->getCurrentBorrowed();
    if (available <= 0)
        return BorrowResult::NO_STOCK;

    // （检查押金）：校外读者未缴纳押金时不允许借书
    ExternalReader *extReader = dynamic_cast<ExternalReader *>(this);
    if (extReader && !extReader->isDepositPaid())
    {
        return BorrowResult::DEPOSIT_REQUIRED;
    }

    // 检查信用分
    CreditCheckResult creditResult = checkCreditScore();
    if (creditResult != CreditCheckResult::OK)
    {
        return BorrowResult::LOW_CREDIT;
    }

    // 检查是否已借阅该图书
    std::vector<BorrowRecord> myRecords = dm->getBorrowRecordsByReader(ID);
    for (const auto &record : myRecords)
    {
        if (record.getISBN() == isbn && !record.isReturned())
        {
            return BorrowResult::ALREADY_BORROWED;
        }
    }

    // 检查是否有逾期未还图书
    if (dm->hasOverdueBooks(ID))
        return BorrowResult::HAS_OVERDUE;

    // 检查是否已达最大借阅数量
    int currentBorrowCount = dm->getBorrowCountByReader(ID);
    if (currentBorrowCount >= maxBooks)
        return BorrowResult::EXCEED_LIMIT;

    // 检查是否有有效的预约
    std::vector<Reservation> &allReservations = dm->getReservations();
    bool hasValidReservation = false;
    int reservationIndex = -1;

    for (int i = 0; i < allReservations.size(); ++i)
    {
        const auto &r = allReservations[i];
        if (r.getISBN() == isbn && r.getReaderID() == ID &&
            r.getStatus() == Reservation::APPROVED)
        {
            hasValidReservation = true;
            reservationIndex = i;
            break;
        }
    }

    if (!hasValidReservation)
    {
        return BorrowResult::NO_VALID_RESERVATION;
    }

    // 创建借阅记录
    QDateTime now = QDateTime::currentDateTime();
    QDateTime dueTime = now.addDays(borrowDays);

    BorrowRecord record(isbn, ID, now, dueTime);
    bool success = dm->addBorrowRecord(record);

    if (success)
    {
        // 删除已使用的预约记录
        if (reservationIndex >= 0)
        {
            allReservations.erase(allReservations.begin() + reservationIndex);
            if (book->getReservationCount() > 0)
            {
                book->setReservationCount(book->getReservationCount() - 1);
            }
            dm->writeBook();
            dm->writeReservation();
        }

        // 发送消息给读者
        QString myMsgContent = QString("您已成功借阅图书《%1》(ISBN:%2)，借阅时间：%3，应还时间：%4")
                                   .arg(book->getTitle())
                                   .arg(isbn)
                                   .arg(now.toString("yyyy-MM-dd HH:mm:ss"))
                                   .arg(dueTime.toString("yyyy-MM-dd HH:mm:ss"));
        Message myMsg(ID, getName(), myMsgContent);
        addMessage(myMsg);

        dm->writeMessage();

        return BorrowResult::SUCCESS;
    }

    return BorrowResult::BOOK_NOT_FOUND;
}

/**
 * @brief 还书（按ISBN）
 * @param isbn 图书ISBN
 * @return 还书结果
 *
 * 还书流程：
 * 1. 查找借阅记录
 * 2. 检查是否存在未支付罚款
 * 3. 计算逾期天数并更新信用分
 * 4. 删除借阅记录
 * 5. 更新图书库存
 * 6. 通知等待预约的读者
 * 7. 发送消息通知读者和管理员
 */
Reader::ReturnResult Reader::returnBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 查找借阅记录
    std::vector<BorrowRecord> &allRecords = dm->getBorrowRecords();
    BorrowRecord *targetRecord = nullptr;
    int recordIndex = -1;

    for (int i = 0; i < allRecords.size(); ++i)
    {
        if (allRecords[i].getISBN() == isbn && allRecords[i].getReaderID() == ID && !allRecords[i].isReturned())
        {
            targetRecord = &allRecords[i];
            recordIndex = i;
            break;
        }
    }

    if (!targetRecord)
    {
        return ReturnResult::NOT_FOUND;
    }

    // 检查是否存在未支付罚款
    double fineAmount = targetRecord->calculateFine(finePerDay);
    double paidFine = targetRecord->getPaidFine();

    if (fineAmount > paidFine)
    {
        return ReturnResult::HAS_UNPAID_FINE;
    }

    QDateTime now = QDateTime::currentDateTime();
    Book *book = dm->findBookByISBN(isbn);

    // 计算逾期天数并更新信用分
    int overdueDays = targetRecord->calculateOverdueDays();
    calculateCreditScore(overdueDays, book, isbn);
    dm->writeUser();

    // 如果逾期归还，增加图书的逾期归还次数
    if (overdueDays > 0 && book)
    {
        book->incrementOverdueReturnCount();
        dm->writeBook();
    }

    // 删除借阅记录
    allRecords.erase(allRecords.begin() + recordIndex);
    dm->writeBorrowRecord();

    // 更新图书库存
    if (book)
    {
        book->setCurrentBorrowed(book->getCurrentBorrowed() - 1);
        dm->writeBook();

        // 通知等待预约的读者
        std::vector<Reservation> &allReservations = dm->getReservations();
        for (auto &reservation : allReservations)
        {
            if (reservation.getISBN() == isbn && reservation.getStatus() == Reservation::APPROVED)
            {
                User *reserver = dm->findUserById(reservation.getReaderID());
                if (reserver)
                {
                    QString msgContent = QString("您预约的图书(ISBN:%1)有新的入库或归还，您可以尝试前往借阅。")
                                             .arg(isbn);
                    Message msg(reserver->getID(), reserver->getName(), msgContent);
                    reserver->addMessage(msg);
                }
            }
        }
    }

    // 发送消息给读者
    QString myMsgContent = QString("您已成功归还图书《%1》(ISBN:%2)，归还时间：%3")
                               .arg(book ? book->getTitle() : "未知")
                               .arg(isbn)
                               .arg(now.toString("yyyy-MM-dd HH:mm:ss"));
    Message myMsg(ID, getName(), myMsgContent);
    addMessage(myMsg);

    // 发送消息给所有管理员
    QString adminMsgContent = QString("读者 %1 归还了图书《%2》(ISBN:%3)，归还时间：%4")
                                  .arg(getName())
                                  .arg(book ? book->getTitle() : "未知")
                                  .arg(isbn)
                                  .arg(now.toString("yyyy-MM-dd HH:mm:ss"));

    std::vector<User *> &users = dm->getUsers();
    for (auto user : users)
    {
        if (user->getType() == 1)
        {
            Message msg(user->getID(), user->getName(), ID, getName(), adminMsgContent);
            user->addMessage(msg);
        }
    }

    dm->writeMessage();

    return ReturnResult::SUCCESS;
}

/**
 * @brief 续借（按ISBN）
 * @param isbn 图书ISBN
 * @return 续借结果
 *
 * 续借流程：
 * 1. 查找借阅记录
 * 2. 检查图书是否逾期
 * 3. 检查信用分是否允许操作
 * 4. 检查是否已有待审核的续借申请
 * 5. 检查续借后借期是否超过90天
 * 6. 设置续借状态为待审核
 * 7. 发送消息通知读者和管理员
 */
Reader::RenewResult Reader::renewBook(const QString &isbn)
{
    DataManager *dm = DataManager::getInstance();

    // 查找借阅记录
    std::vector<BorrowRecord> &allRecords = dm->getBorrowRecords();
    BorrowRecord *targetRecord = nullptr;

    for (auto &record : allRecords)
    {
        if (record.getISBN() == isbn && record.getReaderID() == ID && !record.isReturned())
        {
            targetRecord = &record;
            break;
        }
    }

    if (!targetRecord)
    {
        return RenewResult::NOT_FOUND;
    }

    // 检查图书是否逾期
    if (targetRecord->calculateOverdueDays() > 0)
    {
        return RenewResult::HAS_OVERDUE;
    }

    // 检查信用分
    CreditCheckResult creditResult = checkCreditScore();
    if (creditResult != CreditCheckResult::OK)
    {
        return RenewResult::LOW_CREDIT;
    }

    // 检查是否已有待审核的续借申请
    if (targetRecord->getRenewStatus() == BorrowRecord::RenewStatus::PENDING)
    {
        return RenewResult::ALREADY_PENDING;
    }

    // 检查续借次数是否已达上限
    if (targetRecord->getRenewCount() >= maxRenewTimes)
    {
        return RenewResult::EXCEED_LIMIT;
    }

    // 设置续借状态为待审核
    targetRecord->setRenewStatus(BorrowRecord::RenewStatus::PENDING);
    dm->writeBorrowRecord();

    Book *book = dm->findBookByISBN(isbn);
    QString bookTitle = book ? book->getTitle() : "未知";

    // 发送消息给读者
    QString myMsgContent = QString("您已提交续借申请，图书《%1》(ISBN:%2)，请等待管理员审核。")
                               .arg(bookTitle)
                               .arg(isbn);
    Message myMsg(ID, getName(), myMsgContent);
    addMessage(myMsg);

    // 发送消息给所有管理员
    QString adminMsgContent = QString("读者 %1 (ID:%2) 申请续借图书《%3》(ISBN:%4)，请审核。")
                                  .arg(getName())
                                  .arg(ID)
                                  .arg(bookTitle)
                                  .arg(isbn);

    std::vector<User *> &users = dm->getUsers();
    for (auto user : users)
    {
        if (user->getType() == 1)
        {
            Message msg(user->getID(), user->getName(), ID, getName(), adminMsgContent);
            user->addMessage(msg);
        }
    }

    dm->writeMessage();

    return RenewResult::SUCCESS;
}

/**
 * @brief 查看我的借阅记录
 * @return 借阅记录列表
 */
std::vector<BorrowRecord> Reader::viewMyBorrowRecords()
{
    DataManager *dm = DataManager::getInstance();
    return dm->getBorrowRecordsByReader(ID);
}

// ========== 图书查找 ==========

/**
 * @brief 查找图书（模糊搜索）
 * @param isbn ISBN（可选）
 * @param title 书名（可选）
 * @param author 作者（可选）
 * @param category 分类（可选）
 * @return 匹配的图书指针列表
 */
std::vector<const Book *> Reader::findBook(const QString &isbn, const QString &title,
                                           const QString &author, const QString &category)
{
    DataManager *dm = DataManager::getInstance();
    return dm->searchBooks(isbn, title, author, category);
}

/**
 * @brief 析构函数
 */
Reader::~Reader()
{
}
