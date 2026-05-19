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

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog), loggedInUser(nullptr)
{
    ui->setupUi(this);
    // 设定固定窗口大小
    setFixedSize(600, 400);

    // 设置图标
    setWindowIcon(QIcon(":/image/book.png"));

    // 设置窗口标题
    setWindowTitle("登录");

    // 设置模态对话框属性
    setModal(true);
}

// （获取登录用户）：获取登录成功的用户对象
User *LoginDialog::getLoggedInUser() const
{
    return loggedInUser;
}

// （添加槽函数）：确定按钮点击事件的处理槽函数
void LoginDialog::on_commitBtn_clicked()
{
    // （获取用户输入）：获取用户输入的账号
    QString id = ui->idLineEdit->text();
    // （获取用户输入）：获取用户输入的密码
    QString pwd = ui->pwdLineEdit->text();

    // （输入校验）：检查账号密码是否为空
    if (id.isEmpty() || pwd.isEmpty())
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    // （输入校验）：检查是否选择了登录身份
    if (!ui->adminBtn->isChecked() && !ui->readerBtn->isChecked())
    {
        QMessageBox::warning(this, "警告", "请选择登录身份！");
        return;
    }

    // （获取用户对象）：根据ID获取用户对象（精确匹配）
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
    if (!currentUser)
    {
        QMessageBox::warning(this, "警告", "用户不存在！");
        return;
    }

    // ：检查用户密码是否输入正确
    if (pwd != currentUser->password)
    {
        QMessageBox::warning(this, "警告", "用户密码输入错误！");
        return;
    }

    // ：检查用户选择的类型是否正确
    int selectedType = ui->adminBtn->isChecked() ? 1 : 2;
    if (currentUser->getType() != selectedType)
    {
        QMessageBox::warning(this, "警告", "用户身份选择错误！");
        return;
    }

    // （验证流程）：100%概率触发验证流程
    if (QRandomGenerator::global()->bounded(10) < -1)
    {
        // （验证方式选择）：弹出验证方式选择对话框
        QDialog methodDialog(this);
        methodDialog.setWindowTitle("选择验证方式");
        methodDialog.setModal(true);
        methodDialog.setFixedSize(300, 150);
        // 禁止关闭按钮（只能通过下一步按钮关闭）
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

        // （获取验证方式）：根据选择确定验证方式
        bool usePhone = phoneBtn->isChecked();

        // （获取联系信息）：根据验证方式获取对应的联系信息
        QString contactInfo = usePhone ? currentUser->getPhone() : currentUser->getEmail();

        // （生成验证码）：根据用户类型生成对应格式的验证码
        QString code = currentUser->generateVerificationCode();

        // （生成文件名）：文件名格式为 管理员/读者 + 用户名 + 电话/邮箱
        QString identityStr = currentUser->typeToIdentity();
        QString fileName = QString("%1%2%3.txt")
                               .arg(identityStr)
                               .arg(currentUser->getName())
                               .arg(contactInfo);

        // （保存验证码文件）：在当前目录下创建验证码文件
        QString appDir = QCoreApplication::applicationDirPath();
        QString filePath = appDir + "/" + fileName;
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << code;
            file.close();
        }

        // （验证码验证循环）：无限次让用户输入验证码，直到正确为止
        while (true)
        {
            // （验证码验证）：弹出验证码窗口
            QDialog verifyDialog(this);
            verifyDialog.setWindowTitle("验证码");
            verifyDialog.setModal(true);
            verifyDialog.setFixedSize(350, 100);
            // 禁止关闭按钮（只能通过确定按钮关闭）
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

            // （验证输入）：获取用户输入的验证码并验证
            QString inputCode = lineEdit->text();
            if (inputCode == code)
            {
                break;
            }
            else
            {
                QMessageBox::warning(this, "错误", "验证码错误，请重新输入！");
            }
        }
    }

    // （保存登录用户）：保存登录成功的用户对象
    loggedInUser = currentUser;

    // （登录成功提示）：显示登录成功信息
    QMessageBox::information(this, "提示", "登录成功！");
    // （接受对话框）：登录成功，关闭对话框并返回 Accepted
    accept();
}

// （添加槽函数）：取消按钮点击事件的处理槽函数
void LoginDialog::on_cancelBtn_clicked()
{
    // （退出确认）：弹出确认对话框询问是否退出
    int ret = QMessageBox::question(this, "确认", "确定要退出吗？");
    // （退出判断）：如果用户点击"是"则关闭对话框并返回 Rejected
    if (ret == QMessageBox::Yes)
    {
        reject();
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}