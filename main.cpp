#include "logindialog.h"
#include "adminwindow.h"
#include "readerwindow.h"
#include "DataManager.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "librarySystem_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // 初始化数据管理器
    DataManager* dm = DataManager::getInstance();

    // 显示登录对话框
    LoginDialog log;
    int result = 0;
    if (log.exec() == QDialog::Accepted) {
        // 获取登录成功的用户
        User* loggedInUser = log.getLoggedInUser();
        
        // 根据用户类型打开不同窗口
        if (loggedInUser && loggedInUser->getType() == 1) {
            // 管理员登录，打开管理员窗口
            AdminWindow adminWindow(loggedInUser);
            adminWindow.show();
            result = a.exec();
        } else if (loggedInUser && loggedInUser->getType() == 2) {
            // 读者登录，打开读者窗口
            ReaderWindow readerWindow(loggedInUser);
            readerWindow.show();
            result = a.exec();
        }
    }

    // 清理数据管理器
    delete dm;
    return result;
}