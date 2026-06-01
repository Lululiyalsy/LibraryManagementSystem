/**
 * @file logindialog.cpp
 * @brief 登录对话框类实现
 * 
 * 实现LoginDialog类的所有成员函数，包括登录验证流程、验证码生成与验证等功能。
 */

#include "logindialog.h"
#include "ui_logindialog.h"
#include "DataManager.h"
#include <QString>
#include <QIcon>
#include <QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <vector>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>

/**
 * @brief 构造函数
 * @param parent 父窗口指针
 */
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), loggedInUser(nullptr)
{
    ui->setupUi(this);
    
    // 设定固定窗口大小
    setFixedSize(600, 400);

    // 设置窗口图标
    setWindowIcon(QIcon(":/image/book.png"));

    // 设置窗口标题
    setWindowTitle("登录");

    // 禁用关闭按钮（移除关闭按钮标志）
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    // 设置模态对话框属性
    setModal(true);
}

/**
 * @brief 获取登录成功的用户对象
 * @return 登录成功的用户指针，未登录则返回nullptr
 */
User *LoginDialog::getLoggedInUser() const
{
    return loggedInUser;
}

/**
 * @brief 确定按钮点击事件处理槽函数
 * 
 * 处理用户登录请求，包括：
 * 1. 输入校验（账号密码非空、身份选择）
 * 2. 用户身份验证（账号存在、密码正确、身份匹配）
 * 3. 验证码验证（生成验证码、保存到文件、用户输入验证）
 */
void LoginDialog::on_commitBtn_clicked()
{
    // 获取用户输入的账号和密码
    QString id = ui->idLineEdit->text();
    QString pwd = ui->pwdLineEdit->text();

    // 输入校验：检查账号密码是否为空
    if (id.isEmpty() || pwd.isEmpty())
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    // 输入校验：检查是否选择了登录身份
    if (!ui->adminBtn->isChecked() && !ui->readerBtn->isChecked())
    {
        QMessageBox::warning(this, "警告", "请选择登录身份！");
        return;
    }

    // 根据ID获取用户对象（精确匹配）
    User *currentUser = nullptr;
    std::vector<User *> users = DataManager::getInstance()->findUsersById(id);
    for (auto user : users)
    {
        if (user->getID() == id)
        {
            currentUser = user;
            break;
        }
    }
    
    // 用户不存在检查
    if (!currentUser)
    {
        QMessageBox::warning(this, "警告", "用户不存在！");
        return;
    }

    // 密码验证
    if (pwd != currentUser->password)
    {
        QMessageBox::warning(this, "警告", "用户密码输入错误！");
        return;
    }

    // 身份类型验证
    int selectedType = ui->adminBtn->isChecked() ? 1 : 2;
    if (currentUser->getType() != selectedType)
    {
        QMessageBox::warning(this, "警告", "用户身份选择错误！");
        return;
    }

    // 验证码验证流程（100%概率触发）
    if (QRandomGenerator::global()->bounded(10) < 15)
    {
        // 弹出验证方式选择对话框
        QDialog methodDialog(this);
        methodDialog.setWindowTitle("选择验证方式");
        methodDialog.setModal(true);
        methodDialog.setFixedSize(300, 150);
        methodDialog.setWindowFlags(methodDialog.windowFlags() & ~Qt::WindowCloseButtonHint);

        // 创建验证方式选择控件
        QLabel *label = new QLabel("请选择验证方式：", &methodDialog);
        QRadioButton *phoneBtn = new QRadioButton("电话验证", &methodDialog);
        QRadioButton *smsBtn = new QRadioButton("邮箱验证", &methodDialog);
        QPushButton *nextBtn = new QPushButton("下一步", &methodDialog);

        // 默认选中电话验证
        phoneBtn->setChecked(true);

        // 设置布局
        QVBoxLayout *vLayout = new QVBoxLayout(&methodDialog);
        vLayout->addWidget(label);
        vLayout->addWidget(phoneBtn);
        vLayout->addWidget(smsBtn);
        vLayout->addWidget(nextBtn);

        // 连接下一步按钮信号
        QObject::connect(nextBtn, &QPushButton::clicked, &methodDialog, &QDialog::accept);

        // 显示验证方式选择对话框（模态，阻塞等待）
        methodDialog.exec();

        // 根据选择确定验证方式
        bool usePhone = phoneBtn->isChecked();

        // 根据验证方式获取对应的联系信息
        QString contactInfo = usePhone ? currentUser->getPhone() : currentUser->getEmail();

        // 根据用户类型生成对应格式的验证码
        QString code = currentUser->generateVerificationCode();

        // 生成文件名（格式：身份+用户名+联系方式）
        QString identityStr = currentUser->typeToIdentity();
        QString fileName = QString("%1%2%3.txt")
                               .arg(identityStr)
                               .arg(currentUser->getName())
                               .arg(contactInfo);

        // 在 /verifyCode 目录下保存验证码文件
        QString appDir = QCoreApplication::applicationDirPath();
        QString verifyCodeDir = appDir + "/verifyCode";

        // 确保目录存在
        QDir dir(verifyCodeDir);
        if (!dir.exists())
        {
            dir.mkpath(".");
        }

        QString filePath = verifyCodeDir + "/" + fileName;
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << code;
            file.close();
        }

        // 验证码验证循环（无限次验证直到正确）
        while (true)
        {
            // 弹出验证码输入对话框
            QDialog verifyDialog(this);
            verifyDialog.setWindowTitle("验证码");
            verifyDialog.setModal(true);
            verifyDialog.setFixedSize(350, 100);
            verifyDialog.setWindowFlags(verifyDialog.windowFlags() & ~Qt::WindowCloseButtonHint);

            // 创建控件
            QLabel *codeLabel = new QLabel("请输入验证码：", &verifyDialog);
            QLineEdit *lineEdit = new QLineEdit(&verifyDialog);
            lineEdit->setPlaceholderText("请输入验证码");
            QPushButton *okBtn = new QPushButton("确定", &verifyDialog);

            // 设置布局
            QHBoxLayout *hLayout = new QHBoxLayout();
            hLayout->addWidget(codeLabel);
            hLayout->addWidget(lineEdit);
            hLayout->addWidget(okBtn);

            QVBoxLayout *verifyLayout = new QVBoxLayout(&verifyDialog);
            verifyLayout->addLayout(hLayout);

            // 连接确定按钮信号
            QObject::connect(okBtn, &QPushButton::clicked, &verifyDialog, &QDialog::accept);

            // 显示验证码对话框（模态，阻塞等待）
            verifyDialog.exec();

            // 验证用户输入的验证码
            QString inputCode = lineEdit->text();
            if (inputCode == code)
            {
                break; // 验证码正确，退出循环
            }
            else
            {
                QMessageBox::warning(this, "错误", "验证码错误，请重新输入！");
            }
        }
    }

    // 保存登录成功的用户对象
    loggedInUser = currentUser;

    // 显示登录成功信息
    QMessageBox::information(this, "提示", "登录成功！");
    
    // 关闭对话框并返回 Accepted
    accept();
}

/**
 * @brief 取消按钮点击事件处理槽函数
 * 
 * 处理用户退出登录请求，弹出确认对话框询问用户是否确定退出
 */
void LoginDialog::on_cancelBtn_clicked()
{
    // 弹出确认对话框询问是否退出
    int ret = QMessageBox::question(this, "确认", "确定要退出吗？");
    
    // 如果用户点击"是"则关闭对话框并返回 Rejected
    if (ret == QMessageBox::Yes)
    {
        reject();
    }
}

/**
 * @brief 析构函数
 */
LoginDialog::~LoginDialog()
{
    delete ui;
}