/**
 * @file main.cpp
 * @brief 图书馆管理系统主入口文件
 *
 * 程序的主入口函数，负责初始化应用程序、设置国际化翻译、
 * 初始化数据管理器、显示登录对话框，并根据用户类型打开相应的窗口。
 */

#include "logindialog.h"
#include "adminwindow.h"
#include "readerwindow.h"
#include "DataManager.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

/**
 * @brief 主函数，程序入口
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组
 * @return 程序退出码
 *
 * 程序启动流程：
 * 1. 创建 QApplication 对象
 * 2. 加载国际化翻译文件
 * 3. 初始化数据管理器（单例模式）
 * 4. 显示登录对话框
 * 5. 根据用户类型（管理员/读者）打开相应窗口
 * 6. 处理登出后重新登录逻辑
 * 7. 程序退出时清理数据管理器
 */
int main(int argc, char *argv[])
{
    // 创建 Qt 应用程序对象
    QApplication a(argc, argv);

    // 加载国际化翻译文件
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "librarySystem_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }

    // 初始化数据管理器（单例模式）
    // 负责管理所有数据的加载、保存和查询
    DataManager *dm = DataManager::getInstance();

    // 创建并显示登录对话框
    LoginDialog log;
    int result = 0;

    // 主循环：处理登录和窗口切换
    while (true)
    {
        // 显示登录对话框，等待用户登录
        if (log.exec() == QDialog::Accepted)
        {
            // 获取登录成功的用户对象
            User *loggedInUser = log.getLoggedInUser();

            // 根据用户类型打开不同窗口
            if (loggedInUser && loggedInUser->getType() == 1)
            {
                // 管理员登录（类型1），打开管理员窗口
                AdminWindow adminWindow(loggedInUser);

                // 连接登出信号，关闭管理员窗口后重新显示登录对话框
                QObject::connect(&adminWindow, &AdminWindow::logout, [&]()
                                 { adminWindow.close(); });

                adminWindow.show();
                result = a.exec();

                // 登出后重新显示登录对话框
                log.show();
            }
            else if (loggedInUser && loggedInUser->getType() == 2)
            {
                // 读者登录（类型2），打开读者窗口
                ReaderWindow readerWindow(loggedInUser);

                // 连接登出信号，关闭读者窗口后重新显示登录对话框
                QObject::connect(&readerWindow, &ReaderWindow::logout, [&]()
                                 { readerWindow.close(); });

                readerWindow.show();
                result = a.exec();

                // 登出后重新显示登录对话框
                log.show();
            }
        }
        else
        {
            // 用户取消登录，退出主循环
            break;
        }
    }

    // 清理数据管理器，释放资源
    delete dm;
    return result;
}