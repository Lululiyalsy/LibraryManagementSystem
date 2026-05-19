#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "User.h"

namespace Ui
{
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    //（构造函数）：显式构造函数，用于创建登录对话框实例
    explicit LoginDialog(QWidget *parent = nullptr);
    //（析构函数）：析构函数，用于释放窗口资源
    ~LoginDialog();

    //（获取登录用户）：获取登录成功的用户对象
    User* getLoggedInUser() const;

private slots:
    //（添加槽函数）：确定按钮的点击槽函数，用于处理登录确认
    void on_commitBtn_clicked();
    //（添加槽函数）：取消按钮的点击槽函数，用于处理退出确认
    void on_cancelBtn_clicked();

private:
    //（UI对象）：指向自动生成的UI界面的指针
    Ui::LoginDialog *ui;
    //（登录用户）：存储登录成功的用户对象
    User* loggedInUser;
};

#endif // LOGINDIALOG_H