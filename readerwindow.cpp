#include "readerwindow.h"
#include "Reader.h"
#include "DataManager.h"
#include <QIcon>
#include <QDialog>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDateTime>
#include <QInputDialog>

ReaderWindow::ReaderWindow(User *user, QWidget *parent)
    : QMainWindow(parent)
{
    currentUser = user;
    resize(800, 800);
    setWindowIcon(QIcon(":/image/book.png"));

    // 禁用关闭按钮（移除关闭按钮标志）
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    if (user)
    {
        QString title = QString("%1 - %2").arg(user->typeToIdentity()).arg(user->getName());
        setWindowTitle(title);
    }

    setupCentralWidget();
}

ReaderWindow::~ReaderWindow()
{
}

void ReaderWindow::setupCentralWidget()
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    toolbar = new QToolBar("工具栏", this);
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    QAction *bookSearchAction = new QAction(QIcon(":/image/book2.png"), "图书查询", this);
    QAction *myBorrowAction = new QAction(QIcon(":/image/readerborrow.png"), "我的借阅", this);
    QAction *myReservationAction = new QAction(QIcon(":/image/readerReservation.png"), "我的预约", this);
    QAction *messageAction = new QAction(QIcon(":/image/message.png"), "消息管理", this);
    QAction *logoutAction = new QAction(QIcon(":/image/logout.png"), "退出登录", this);

    toolbar->addAction(bookSearchAction);
    toolbar->addAction(myBorrowAction);
    toolbar->addAction(myReservationAction);
    toolbar->addAction(messageAction);
    toolbar->addAction(logoutAction);

    connect(bookSearchAction, &QAction::triggered, this, &ReaderWindow::switchToBookSearch);
    connect(myBorrowAction, &QAction::triggered, this, &ReaderWindow::switchToMyBorrow);
    connect(myReservationAction, &QAction::triggered, this, &ReaderWindow::switchToMyReservation);
    connect(messageAction, &QAction::triggered, this, &ReaderWindow::onCheckMessages);
    connect(logoutAction, &QAction::triggered, this, &ReaderWindow::onLogout);

    setupBookSearchWidget();
    setupMyBorrowWidget();
    setupMyReservationWidget();
    setupMessageWidget();

    stackedWidget->addWidget(bookSearchWidget);
    stackedWidget->addWidget(myBorrowWidget);
    stackedWidget->addWidget(myReservationWidget);
    stackedWidget->addWidget(messageWidget);

    stackedWidget->setCurrentIndex(0);
}

void ReaderWindow::setupBookSearchWidget()
{
    bookSearchWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(bookSearchWidget);

    QWidget *searchWidget = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);

    searchLayout->addWidget(new QLabel("ISBN:"));
    bookISBNLineEdit = new QLineEdit(this);
    bookISBNLineEdit->setPlaceholderText("输入ISBN");
    searchLayout->addWidget(bookISBNLineEdit);

    searchLayout->addWidget(new QLabel("书名:"));
    bookTitleLineEdit = new QLineEdit(this);
    bookTitleLineEdit->setPlaceholderText("输入书名");
    searchLayout->addWidget(bookTitleLineEdit);

    searchLayout->addWidget(new QLabel("作者:"));
    bookAuthorLineEdit = new QLineEdit(this);
    bookAuthorLineEdit->setPlaceholderText("输入作者");
    searchLayout->addWidget(bookAuthorLineEdit);

    searchLayout->addWidget(new QLabel("分类:"));
    bookCategoryLineEdit = new QLineEdit(this);
    bookCategoryLineEdit->setPlaceholderText("输入分类");
    searchLayout->addWidget(bookCategoryLineEdit);

    bookSearchBtn = new QPushButton("查找", this);
    searchLayout->addWidget(bookSearchBtn);

    bookReserveBtn = new QPushButton("预约", this);
    searchLayout->addWidget(bookReserveBtn);

    borrowBtn = new QPushButton("借书", this);
    searchLayout->addWidget(borrowBtn);

    mainLayout->addWidget(searchWidget);

    bookSearchTable = new QTableWidget(this);
    bookSearchTable->setColumnCount(10);
    QStringList headers = {"ISBN", "书名", "作者", "分类", "库存", "入库时间", "借阅次数", "当前借出", "状态", "预约人数"};
    bookSearchTable->setHorizontalHeaderLabels(headers);
    bookSearchTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bookSearchTable->setSelectionBehavior(QTableWidget::SelectRows);
    mainLayout->addWidget(bookSearchTable);

    connect(bookSearchBtn, &QPushButton::clicked, this, &ReaderWindow::onBookSearch);
    connect(bookReserveBtn, &QPushButton::clicked, this, &ReaderWindow::onBookReserve);
    connect(borrowBtn, &QPushButton::clicked, this, &ReaderWindow::onBorrowBook);

    onBookSearch();
}

void ReaderWindow::setupMyBorrowWidget()
{
    myBorrowWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(myBorrowWidget);

    QWidget *btnWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);

    returnBtn = new QPushButton("还书", this);
    btnLayout->addWidget(returnBtn);

    renewBtn = new QPushButton("续借", this);
    btnLayout->addWidget(renewBtn);

    mainLayout->addWidget(btnWidget);

    myBorrowTable = new QTableWidget(this);
    myBorrowTable->setColumnCount(7);
    QStringList headers = {"ISBN", "书名", "借阅时间", "应还时间", "归还时间", "是否已还", "状态"};
    myBorrowTable->setHorizontalHeaderLabels(headers);
    myBorrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    myBorrowTable->setSelectionBehavior(QTableWidget::SelectRows);
    mainLayout->addWidget(myBorrowTable);

    connect(returnBtn, &QPushButton::clicked, this, &ReaderWindow::onReturnBook);
    connect(renewBtn, &QPushButton::clicked, this, &ReaderWindow::onRenewBook);

    displayMyBorrowRecords();
}

void ReaderWindow::setupMyReservationWidget()
{
    myReservationWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(myReservationWidget);

    QWidget *btnWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);

    cancelReserveBtn = new QPushButton("取消预约", this);
    btnLayout->addWidget(cancelReserveBtn);

    mainLayout->addWidget(btnWidget);

    myReservationTable = new QTableWidget(this);
    myReservationTable->setColumnCount(5);
    QStringList headers = {"ISBN", "书名", "预约时间", "状态", "操作"};
    myReservationTable->setHorizontalHeaderLabels(headers);
    myReservationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    myReservationTable->setSelectionBehavior(QTableWidget::SelectRows);
    mainLayout->addWidget(myReservationTable);

    connect(cancelReserveBtn, &QPushButton::clicked, this, &ReaderWindow::onCancelReservation);

    displayMyReservations();
}

void ReaderWindow::onBookSearch()
{
    QString isbn = bookISBNLineEdit->text();
    QString title = bookTitleLineEdit->text();
    QString author = bookAuthorLineEdit->text();
    QString category = bookCategoryLineEdit->text();

    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        std::vector<const Book *> books = reader->findBook(isbn, title, author, category);
        displayBooks(books);
    }
}

void ReaderWindow::displayBooks(const std::vector<const Book *> &books)
{
    bookSearchTable->setRowCount(0);
    for (const auto *book : books)
    {
        int row = bookSearchTable->rowCount();
        bookSearchTable->insertRow(row);
        bookSearchTable->setItem(row, 0, new QTableWidgetItem(book->getISBN()));
        bookSearchTable->setItem(row, 1, new QTableWidgetItem(book->getTitle()));
        bookSearchTable->setItem(row, 2, new QTableWidgetItem(book->getAuthor()));
        bookSearchTable->setItem(row, 3, new QTableWidgetItem(book->getCategory()));
        bookSearchTable->setItem(row, 4, new QTableWidgetItem(QString::number(book->getStock())));
        bookSearchTable->setItem(row, 5, new QTableWidgetItem(book->getInStockTime().toString("yyyy-MM-dd HH:mm:ss")));
        bookSearchTable->setItem(row, 6, new QTableWidgetItem(QString::number(book->getBorrowCount())));
        bookSearchTable->setItem(row, 7, new QTableWidgetItem(QString::number(book->getCurrentBorrowed())));
        bookSearchTable->setItem(row, 8, new QTableWidgetItem(book->getStatus()));
        bookSearchTable->setItem(row, 9, new QTableWidgetItem(QString::number(book->getReservationCount())));
    }
}
void ReaderWindow::displayMyBorrowRecords()
{
    myBorrowTable->setRowCount(0);
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        std::vector<BorrowRecord> records = reader->viewMyBorrowRecords();
        for (const auto &record : records)
        {
            int row = myBorrowTable->rowCount();
            myBorrowTable->insertRow(row);
            myBorrowTable->setItem(row, 0, new QTableWidgetItem(record.getISBN()));

            DataManager *dm = DataManager::getInstance();
            Book *book = dm->findBookByISBN(record.getISBN());
            QString bookTitle = book ? book->getTitle() : "未知";
            myBorrowTable->setItem(row, 1, new QTableWidgetItem(bookTitle));

            myBorrowTable->setItem(row, 2, new QTableWidgetItem(record.getBorrowTime().toString("yyyy-MM-dd HH:mm:ss")));
            myBorrowTable->setItem(row, 3, new QTableWidgetItem(record.getDueTime().toString("yyyy-MM-dd HH:mm:ss")));
            myBorrowTable->setItem(row, 4, new QTableWidgetItem(record.getReturnTime().toString("yyyy-MM-dd HH:mm:ss")));
            myBorrowTable->setItem(row, 5, new QTableWidgetItem(record.isReturned() ? "是" : "否"));

            QString status = "借阅中";
            if (record.isReturned())
            {
                status = "已归还";
            }
            else if (record.calculateOverdueDays() > 0)
            {
                status = QString("逾期(%1天)").arg(record.calculateOverdueDays());
            }
            myBorrowTable->setItem(row, 6, new QTableWidgetItem(status));
        }
    }
}

void ReaderWindow::displayMyReservations()
{
    myReservationTable->setRowCount(0);
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        std::vector<Reservation> reservations = reader->viewMyReservations();
        for (const auto &reservation : reservations)
        {
            int row = myReservationTable->rowCount();
            myReservationTable->insertRow(row);
            myReservationTable->setItem(row, 0, new QTableWidgetItem(reservation.getISBN()));

            DataManager *dm = DataManager::getInstance();
            Book *book = dm->findBookByISBN(reservation.getISBN());
            QString bookTitle = book ? book->getTitle() : "未知";
            myReservationTable->setItem(row, 1, new QTableWidgetItem(bookTitle));

            myReservationTable->setItem(row, 2, new QTableWidgetItem(reservation.getReserveTime().toString("yyyy-MM-dd HH:mm:ss")));
            myReservationTable->setItem(row, 3, new QTableWidgetItem(reservation.getStatusString()));
        }
    }
}

void ReaderWindow::onBookReserve()
{
    QPair<QString, bool> result = showInputDialog("预约图书", "请输入要预约的图书ISBN：");
    if (result.second)
        return;

    QString isbn = result.first;
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        bool success = reader->reserveBook(isbn);
        if (success)
        {
            QMessageBox::information(this, "成功", "预约成功！");
            onBookSearch();
            displayMyReservations();
        }
        else
        {
            QMessageBox::warning(this, "失败", "预约失败！可能图书不存在、已可借或已预约。");
        }
    }
}

void ReaderWindow::onCancelReservation()
{
    QPair<QString, bool> result = showInputDialog("取消预约", "请输入要取消预约的图书ISBN：");
    if (result.second)
        return;

    QString isbn = result.first;
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        bool success = reader->cancelReservation(isbn);
        if (success)
        {
            QMessageBox::information(this, "成功", "取消预约成功！");
            displayMyReservations();
            onBookSearch();
        }
        else
        {
            QMessageBox::warning(this, "失败", "取消预约失败！预约记录不存在。");
        }
    }
}

void ReaderWindow::onBorrowBook()
{
    QPair<QString, bool> result = showInputDialog("借书", "请输入要借阅的图书ISBN：");
    if (result.second)
        return;

    QString isbn = result.first;
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        bool success = reader->borrowBook(isbn);
        if (success)
        {
            QMessageBox::information(this, "成功", "借书成功！");
            onBookSearch();
            displayMyBorrowRecords();
        }
        else
        {
            QMessageBox::warning(this, "失败", "借书失败！可能图书不存在、无库存或您有逾期未还图书。");
        }
    }
}

void ReaderWindow::onReturnBook()
{
    QPair<QString, bool> result = showInputDialog("还书", "请输入要归还的图书ISBN：");
    if (result.second)
        return;

    QString isbn = result.first;
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        bool success = reader->returnBook(isbn);
        if (success)
        {
            QMessageBox::information(this, "成功", "还书成功！");
            displayMyBorrowRecords();
            onBookSearch();
        }
        else
        {
            QMessageBox::warning(this, "失败", "还书失败！借阅记录不存在。");
        }
    }
}

void ReaderWindow::onRenewBook()
{
    QPair<QString, bool> result = showInputDialog("续借", "请输入要续借的图书ISBN：");
    if (result.second)
        return;

    QString isbn = result.first;
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        bool success = reader->renewBook(isbn);
        if (success)
        {
            QMessageBox::information(this, "成功", "续借成功！借阅期限延长30天。");
            displayMyBorrowRecords();
        }
        else
        {
            QMessageBox::warning(this, "失败", "续借失败！借阅记录不存在或已归还。");
        }
    }
}

void ReaderWindow::switchToBookSearch()
{
    stackedWidget->setCurrentWidget(bookSearchWidget);
}

void ReaderWindow::switchToMyBorrow()
{
    displayMyBorrowRecords();
    stackedWidget->setCurrentWidget(myBorrowWidget);
}

void ReaderWindow::switchToMyReservation()
{
    displayMyReservations();
    stackedWidget->setCurrentWidget(myReservationWidget);
}

QPair<QString, bool> ReaderWindow::showInputDialog(const QString &title, const QString &label, bool isPassword)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    QVBoxLayout layout(&dialog);

    QLabel *labelWidget = new QLabel(label);
    layout.addWidget(labelWidget);

    QLineEdit *lineEdit = new QLineEdit();
    if (isPassword)
        lineEdit->setEchoMode(QLineEdit::Password);
    layout.addWidget(lineEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addWidget(&buttonBox);

    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    int result = dialog.exec();
    if (result == QDialog::Accepted)
    {
        return qMakePair(lineEdit->text(), false);
    }
    return qMakePair("", true);
}

// （消息管理）：初始化消息表格
void ReaderWindow::setupMessageWidget()
{
    messageWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(messageWidget);

    // 查找输入框布局
    QHBoxLayout *searchLayout = new QHBoxLayout();

    // 消息查找输入框（每列一个）
    messageTimeEdit = new QLineEdit(this);
    messageTimeEdit->setPlaceholderText("消息时间");
    messageTimeEdit->setFixedWidth(150);

    messageContentEdit = new QLineEdit(this);
    messageContentEdit->setPlaceholderText("消息内容");
    messageContentEdit->setFixedWidth(300);

    messageStatusEdit = new QLineEdit(this);
    messageStatusEdit->setPlaceholderText("消息状态");
    messageStatusEdit->setFixedWidth(80);

    QPushButton *searchMessageBtn = new QPushButton("查找消息", this);

    searchLayout->addWidget(messageTimeEdit);
    searchLayout->addWidget(messageContentEdit);
    searchLayout->addWidget(messageStatusEdit);
    searchLayout->addWidget(searchMessageBtn);
    searchLayout->addStretch();

    mainLayout->addLayout(searchLayout);

    // 操作按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *deleteMessageBtn = new QPushButton("删除消息", this);
    QPushButton *clearAllMessagesBtn = new QPushButton("清除所有消息", this);
    QPushButton *markAllReadBtn = new QPushButton("全部设为已读", this);

    buttonLayout->addWidget(deleteMessageBtn);
    buttonLayout->addWidget(clearAllMessagesBtn);
    buttonLayout->addWidget(markAllReadBtn);
    buttonLayout->addStretch();

    connect(deleteMessageBtn, &QPushButton::clicked, this, &ReaderWindow::onDeleteMessage);
    connect(clearAllMessagesBtn, &QPushButton::clicked, this, &ReaderWindow::onClearAllMessages);
    connect(markAllReadBtn, &QPushButton::clicked, this, &ReaderWindow::onMarkAllRead);
    connect(searchMessageBtn, &QPushButton::clicked, this, &ReaderWindow::onSearchMessage);

    mainLayout->addLayout(buttonLayout);

    // 消息表格
    messageTable = new QTableWidget(this);
    messageTable->setColumnCount(3);
    QStringList headers = {"消息时间", "消息内容", "消息状态"};
    messageTable->setHorizontalHeaderLabels(headers);
    messageTable->horizontalHeader()->setStretchLastSection(true);
    messageTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    messageTable->setSelectionBehavior(QTableWidget::SelectRows);
    messageTable->setColumnWidth(0, 150);
    messageTable->setColumnWidth(1, 600);
    messageTable->setColumnWidth(2, 80);

    mainLayout->addWidget(messageTable);
}

// （显示消息）：显示消息到消息表格
void ReaderWindow::displayMessages(const std::vector<Message> &messages)
{
    messageTable->setRowCount(messages.size());

    for (size_t i = 0; i < messages.size(); ++i)
    {
        const Message &msg = messages[i];
        std::vector<QString> fields = msg.getReaderDisplayFields();

        for (int j = 0; j < fields.size() && j < 3; ++j)
        {
            messageTable->setItem(i, j, new QTableWidgetItem(fields[j]));
        }
    }

    // 设置列宽
    messageTable->setColumnWidth(0, 150);
    messageTable->setColumnWidth(1, 600);
    messageTable->setColumnWidth(2, 80);
}

// （消息管理）：消息按钮点击处理
void ReaderWindow::onCheckMessages()
{
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (!reader)
    {
        return;
    }

    stackedWidget->setCurrentWidget(messageWidget);
    displayMessages(reader->getMessages());
}

// （退出登录）：退出按钮点击处理
void ReaderWindow::onLogout()
{
    QMessageBox confirmBox(QMessageBox::Question, "确认退出", "确定要退出登录吗？", QMessageBox::NoButton, this);
    QAbstractButton *yesBtn = confirmBox.addButton("是", QMessageBox::YesRole);
    confirmBox.addButton("否", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == yesBtn)
    {
        QMessageBox::information(this, "成功", "退出登录成功！");
        emit logout();
    }
}

// （删除消息）：删除选中消息
void ReaderWindow::onDeleteMessage()
{
    int currentRow = messageTable->currentRow();
    if (currentRow < 0)
    {
        QMessageBox msgBox(QMessageBox::Warning, "提示", "请先选择要删除的消息！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    QMessageBox confirmBox(QMessageBox::Question, "确认删除", "确定要删除选中的消息吗？", QMessageBox::NoButton, this);
    QAbstractButton *yesBtn = confirmBox.addButton("是", QMessageBox::YesRole);
    confirmBox.addButton("否", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == yesBtn)
    {
        std::vector<Message> &messages = currentUser->getMessages();
        messages.erase(messages.begin() + currentRow);
        DataManager::getInstance()->writeMessage();
        displayMessages(messages);

        QMessageBox msgBox(QMessageBox::Information, "成功", "消息删除成功！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
}

// （清除所有消息）：清除所有消息
void ReaderWindow::onClearAllMessages()
{
    QMessageBox confirmBox(QMessageBox::Question, "确认清除", "确定要清除所有消息吗？此操作不可恢复！", QMessageBox::NoButton, this);
    QAbstractButton *yesBtn = confirmBox.addButton("是", QMessageBox::YesRole);
    confirmBox.addButton("否", QMessageBox::NoRole);
    confirmBox.exec();

    if (confirmBox.clickedButton() == yesBtn)
    {
        currentUser->clearMessages();
        DataManager::getInstance()->writeMessage();
        displayMessages(std::vector<Message>());

        QMessageBox msgBox(QMessageBox::Information, "成功", "所有消息已清除！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
}

// （全部设为已读）：将所有未读消息设为已读
void ReaderWindow::onMarkAllRead()
{
    std::vector<Message> &messages = currentUser->getMessages();
    bool hasUnread = false;

    for (auto &msg : messages)
    {
        if (msg.getStatus() == MessageStatus::UNREAD)
        {
            msg.setStatus(MessageStatus::READ);
            hasUnread = true;
        }
    }

    if (hasUnread)
    {
        DataManager::getInstance()->writeMessage();
        displayMessages(messages);

        QMessageBox msgBox(QMessageBox::Information, "成功", "所有消息已设为已读！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "没有未读消息！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
}

// （查找消息）：查找消息
void ReaderWindow::onSearchMessage()
{
    QString time = messageTimeEdit->text().trimmed();
    QString content = messageContentEdit->text().trimmed();
    QString status = messageStatusEdit->text().trimmed();

    // 检查是否至少有一个条件
    if (time.isEmpty() && content.isEmpty() && status.isEmpty())
    {
        QMessageBox msgBox(QMessageBox::Warning, "提示", "请至少输入一个查找条件！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
        return;
    }

    std::vector<Message> allMessages = currentUser->getMessages();
    std::vector<Message> filteredMessages;

    for (const auto &msg : allMessages)
    {
        std::vector<QString> fields = msg.getReaderDisplayFields();
        bool match = true;

        // 消息时间匹配
        if (!time.isEmpty() && !fields[0].contains(time, Qt::CaseInsensitive))
        {
            match = false;
        }
        // 消息内容匹配
        if (match && !content.isEmpty() && !fields[1].contains(content, Qt::CaseInsensitive))
        {
            match = false;
        }
        // 消息状态匹配
        if (match && !status.isEmpty() && !fields[2].contains(status, Qt::CaseInsensitive))
        {
            match = false;
        }

        if (match)
        {
            filteredMessages.push_back(msg);
        }
    }

    if (filteredMessages.empty())
    {
        QMessageBox msgBox(QMessageBox::Information, "提示", "未找到匹配的消息！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
    else
    {
        displayMessages(filteredMessages);
        QMessageBox msgBox(QMessageBox::Information, "查找结果", QString("共找到 %1 条匹配的消息！").arg(filteredMessages.size()), QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
}