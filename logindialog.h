/**
 * @file logindialog.h
 * @brief 登录对话框类定义
 *
 * LoginDialog类提供用户登录界面，支持管理员和读者两种身份登录，
 * 包含账号密码验证和验证码验证功能。
 */

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "User.h"

namespace Ui
{
    class LoginDialog;
}

/**
 * @class LoginDialog
 * @brief 登录对话框类
 *
 * 提供用户登录界面，支持管理员和读者两种身份登录，包含账号密码验证、
 * 身份选择验证和验证码验证功能。验证通过后保存登录用户对象。
 */
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口指针
     */
    explicit LoginDialog(QWidget *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~LoginDialog();

    /**
     * @brief 获取登录成功的用户对象
     * @return 登录成功的用户指针，未登录则返回nullptr
     */
    User *getLoggedInUser() const;

private slots:
    /**
     * @brief 确定按钮点击事件处理槽函数
     *
     * 处理用户登录请求，包括输入校验、身份验证和验证码验证
     */
    void on_commitBtn_clicked();

    /**
     * @brief 取消按钮点击事件处理槽函数
     *
     * 处理用户退出登录请求，弹出确认对话框
     */
    void on_cancelBtn_clicked();

private:
    Ui::LoginDialog *ui; ///< 指向自动生成的UI界面的指针
    User *loggedInUser;  ///< 存储登录成功的用户对象
};

#endif // LOGINDIALOG_H