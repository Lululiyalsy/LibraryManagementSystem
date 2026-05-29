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

// 构造函数：初始化读者窗口
ReaderWindow::ReaderWindow(User *user, QWidget *parent)
    : QMainWindow(parent)
{
    currentUser = user;
    resize(800, 800);
    setWindowIcon(QIcon(":/image/book.png"));

    // 禁用关闭按钮
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    if (user)
    {
        QString title = QString("%1 - %2").arg(user->typeToIdentity()).arg(user->getName());
        setWindowTitle(title);
    }

    setupCentralWidget();
}

// 析构函数
ReaderWindow::~ReaderWindow()
{
}

// 设置中心窗口：初始化工具栏和堆叠窗口
void ReaderWindow::setupCentralWidget()
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    toolbar = new QToolBar("工具栏", this);
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addToolBar(Qt::LeftToolBarArea, toolbar);

    // 创建工具栏动作
    QAction *bookSearchAction = new QAction(QIcon(":/image/book2.png"), "图书查询", this);
    QAction *myBorrowAction = new QAction(QIcon(":/image/readerborrow.png"), "我的借阅", this);
    QAction *myReservationAction = new QAction(QIcon(":/image/readerReservation.png"), "我的预约", this);
    QAction *messageAction = new QAction(QIcon(":/image/message.png"), "消息管理", this);
    QAction *logoutAction = new QAction(QIcon(":/image/logout.png"), "退出登录", this);

    // 添加动作到工具栏
    toolbar->addAction(bookSearchAction);
    toolbar->addAction(myBorrowAction);
    toolbar->addAction(myReservationAction);
    toolbar->addAction(messageAction);
    toolbar->addAction(logoutAction);

    // 连接信号槽
    connect(bookSearchAction, &QAction::triggered, this, &ReaderWindow::switchToBookSearch);
    connect(myBorrowAction, &QAction::triggered, this, &ReaderWindow::switchToMyBorrow);
    connect(myReservationAction, &QAction::triggered, this, &ReaderWindow::switchToMyReservation);
    connect(messageAction, &QAction::triggered, this, &ReaderWindow::onCheckMessages);
    connect(logoutAction, &QAction::triggered, this, &ReaderWindow::onLogout);

    // 设置各个页面
    setupBookSearchWidget();
    setupMyBorrowWidget();
    setupMyReservationWidget();
    setupMessageWidget();

    // 添加页面到堆叠窗口
    stackedWidget->addWidget(bookSearchWidget);
    stackedWidget->addWidget(myBorrowWidget);
    stackedWidget->addWidget(myReservationWidget);
    stackedWidget->addWidget(messageWidget);

    // 默认显示图书查询页面
    stackedWidget->setCurrentIndex(0);
}

// 设置图书查询页面
void ReaderWindow::setupBookSearchWidget()
{
    bookSearchWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(bookSearchWidget);

    // 搜索区域
    QWidget *searchWidget = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(searchWidget);

    // ISBN输入框
    searchLayout->addWidget(new QLabel("ISBN:"));
    bookISBNLineEdit = new QLineEdit(this);
    bookISBNLineEdit->setPlaceholderText("输入ISBN");
    searchLayout->addWidget(bookISBNLineEdit);

    // 书名输入框
    searchLayout->addWidget(new QLabel("书名:"));
    bookTitleLineEdit = new QLineEdit(this);
    bookTitleLineEdit->setPlaceholderText("输入书名");
    searchLayout->addWidget(bookTitleLineEdit);

    // 作者输入框
    searchLayout->addWidget(new QLabel("作者:"));
    bookAuthorLineEdit = new QLineEdit(this);
    bookAuthorLineEdit->setPlaceholderText("输入作者");
    searchLayout->addWidget(bookAuthorLineEdit);

    // 分类输入框
    searchLayout->addWidget(new QLabel("分类:"));
    bookCategoryLineEdit = new QLineEdit(this);
    bookCategoryLineEdit->setPlaceholderText("输入分类");
    searchLayout->addWidget(bookCategoryLineEdit);

    // 查询按钮
    bookSearchBtn = new QPushButton("查找", this);
    searchLayout->addWidget(bookSearchBtn);

    // 预约按钮
    bookReserveBtn = new QPushButton("预约", this);
    searchLayout->addWidget(bookReserveBtn);

    // 借书按钮
    borrowBtn = new QPushButton("借书", this);
    searchLayout->addWidget(borrowBtn);

    mainLayout->addWidget(searchWidget);

    // 图书表格
    bookSearchTable = new QTableWidget(this);
    bookSearchTable->setColumnCount(10);
    QStringList headers = {"ISBN", "书名", "作者", "分类", "库存", "入库时间", "借阅次数", "当前借出", "状态", "预约人数"};
    bookSearchTable->setHorizontalHeaderLabels(headers);
    bookSearchTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    bookSearchTable->setSelectionBehavior(QTableWidget::SelectRows);
    mainLayout->addWidget(bookSearchTable);

    // 连接信号槽
    connect(bookSearchBtn, &QPushButton::clicked, this, &ReaderWindow::onBookSearch);
    connect(bookReserveBtn, &QPushButton::clicked, this, &ReaderWindow::onBookReserve);
    connect(borrowBtn, &QPushButton::clicked, this, &ReaderWindow::onBorrowBook);

    // 初始化显示所有图书
    onBookSearch();
}

// 设置我的借阅页面
void ReaderWindow::setupMyBorrowWidget()
{
    myBorrowWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(myBorrowWidget);

    // 按钮区域
    QWidget *btnWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);

    // 还书按钮
    returnBtn = new QPushButton("还书", this);
    btnLayout->addWidget(returnBtn);

    // 续借按钮
    renewBtn = new QPushButton("续借", this);
    btnLayout->addWidget(renewBtn);

    mainLayout->addWidget(btnWidget);

    // 借阅记录表格
    myBorrowTable = new QTableWidget(this);
    myBorrowTable->setColumnCount(7);
    QStringList headers = {"ISBN", "书名", "借阅时间", "应还时间", "归还时间", "是否已还", "状态"};
    myBorrowTable->setHorizontalHeaderLabels(headers);
    myBorrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    myBorrowTable->setSelectionBehavior(QTableWidget::SelectRows);
    mainLayout->addWidget(myBorrowTable);

    // 连接信号槽
    connect(returnBtn, &QPushButton::clicked, this, &ReaderWindow::onReturnBook);
    connect(renewBtn, &QPushButton::clicked, this, &ReaderWindow::onRenewBook);

    // 初始化显示借阅记录
    displayMyBorrowRecords();
}

// 设置我的预约页面
void ReaderWindow::setupMyReservationWidget()
{
    myReservationWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(myReservationWidget);

    // 顶部按钮区域
    QHBoxLayout *topLayout = new QHBoxLayout(this);

    QPushButton *deleteReservationBtn = new QPushButton("删除预约", this);
    QPushButton *clearAllReservationsBtn = new QPushButton("清除所有预约", this);

    topLayout->addWidget(deleteReservationBtn);
    topLayout->addWidget(clearAllReservationsBtn);

    // 添加分隔线
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    topLayout->addWidget(separator);

    // 预约查找输入框
    reservationTimeEdit = new QLineEdit(this);
    reservationTimeEdit->setPlaceholderText("预约时间");
    reservationTimeEdit->setFixedWidth(150);

    reservationISBNEdit = new QLineEdit(this);
    reservationISBNEdit->setPlaceholderText("ISBN");
    reservationISBNEdit->setFixedWidth(150);

    reservationTitleEdit = new QLineEdit(this);
    reservationTitleEdit->setPlaceholderText("书名");
    reservationTitleEdit->setFixedWidth(200);

    reservationStatusCombo = new QComboBox(this);
    reservationStatusCombo->addItem("");
    reservationStatusCombo->addItem("待审核");
    reservationStatusCombo->addItem("审核成功");
    reservationStatusCombo->addItem("审核失败");
    reservationStatusCombo->addItem("已取消");
    reservationStatusCombo->setFixedWidth(80);

    QPushButton *searchReservationBtn = new QPushButton("查找预约", this);

    topLayout->addWidget(reservationTimeEdit);
    topLayout->addWidget(reservationISBNEdit);
    topLayout->addWidget(reservationTitleEdit);
    topLayout->addWidget(reservationStatusCombo);
    topLayout->addWidget(searchReservationBtn);
    topLayout->addStretch();

    // 连接信号槽
    connect(deleteReservationBtn, &QPushButton::clicked, this, &ReaderWindow::onDeleteReservation);
    connect(clearAllReservationsBtn, &QPushButton::clicked, this, &ReaderWindow::onClearAllReservations);
    connect(searchReservationBtn, &QPushButton::clicked, this, &ReaderWindow::onSearchReservation);

    mainLayout->addLayout(topLayout);

    // 预约记录表格
    myReservationTable = new QTableWidget(this);
    myReservationTable->setColumnCount(4);
    QStringList headers = {"ISBN", "书名", "预约时间", "状态"};
    myReservationTable->setHorizontalHeaderLabels(headers);
    myReservationTable->horizontalHeader()->setStretchLastSection(true);
    myReservationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    myReservationTable->setSelectionBehavior(QTableWidget::SelectRows);
    myReservationTable->setColumnWidth(0, 120);
    myReservationTable->setColumnWidth(1, 250);
    myReservationTable->setColumnWidth(2, 150);
    myReservationTable->setColumnWidth(3, 80);

    mainLayout->addWidget(myReservationTable);

    // 初始化显示预约记录
    displayMyReservations();
}

// 图书查询：根据输入条件查询图书
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

// 显示图书列表
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

// 显示我的借阅记录
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

// 显示我的预约记录
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

// 预约图书
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

// 取消预约（弹出对话框输入ISBN）
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
            QMessageBox::warning(this, "失败", "取消预约失败！预约记录不存在或已审核。");
        }
    }
}

// 删除预约（选中行）
void ReaderWindow::onDeleteReservation()
{
    int currentRow = myReservationTable->currentRow();
    if (currentRow < 0)
    {
        QMessageBox::warning(this, "提示", "请先选择要删除的预约记录！");
        return;
    }

    QString isbn = myReservationTable->item(currentRow, 0)->text();
    QString status = myReservationTable->item(currentRow, 3)->text();
    if (status == "待审核")
    {
        QMessageBox::warning(this, "提示", "待审核的预约记录不能删除，请使用取消预约功能！");
        return;
    }

    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (reader)
    {
        bool success = reader->deleteReservation(isbn);
        if (success)
        {
            QMessageBox::information(this, "成功", "删除预约成功！");
            displayMyReservations();
            onBookSearch();
        }
        else
        {
            QMessageBox::warning(this, "失败", "删除预约失败！预约记录不存在。");
        }
    }
}

// 清除所有非待审核状态的预约
void ReaderWindow::onClearAllReservations()
{
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (!reader)
        return;

    std::vector<Reservation> reservations = reader->viewMyReservations();
    bool hasNonPending = false;
    for (const auto &r : reservations)
    {
        if (r.getStatus() != Reservation::PENDING)
        {
            hasNonPending = true;
            break;
        }
    }

    if (!hasNonPending)
    {
        QMessageBox::information(this, "提示", "没有可清除的预约记录（待审核的预约不会被清除）！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认", "确定要清除所有已审核/已取消的预约记录吗？（待审核的预约将保留）",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        int deletedCount = reader->clearAllReservationsExceptPending();
        QMessageBox::information(this, "成功", QString("成功清除 %1 条预约记录！").arg(deletedCount));
        displayMyReservations();
        onBookSearch();
    }
}

// 查找预约
void ReaderWindow::onSearchReservation()
{
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (!reader)
        return;

    QString timeFilter = reservationTimeEdit->text().trimmed();
    QString isbnFilter = reservationISBNEdit->text().trimmed();
    QString titleFilter = reservationTitleEdit->text().trimmed();
    QString statusFilter = reservationStatusCombo->currentText();

    std::vector<Reservation> allReservations = reader->viewMyReservations();
    std::vector<Reservation> filteredReservations;

    for (const auto &reservation : allReservations)
    {
        bool match = true;

        if (!timeFilter.isEmpty())
        {
            if (!reservation.getReserveTime().toString("yyyy-MM-dd HH:mm:ss").contains(timeFilter))
                match = false;
        }

        if (!isbnFilter.isEmpty())
        {
            if (!reservation.getISBN().contains(isbnFilter))
                match = false;
        }

        if (!titleFilter.isEmpty())
        {
            DataManager *dm = DataManager::getInstance();
            Book *book = dm->findBookByISBN(reservation.getISBN());
            if (!book || !book->getTitle().contains(titleFilter))
                match = false;
        }

        if (!statusFilter.isEmpty())
        {
            if (reservation.getStatusString() != statusFilter)
                match = false;
        }

        if (match)
            filteredReservations.push_back(reservation);
    }

    myReservationTable->setRowCount(0);
    for (const auto &reservation : filteredReservations)
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

    myReservationTable->setColumnWidth(0, 120);
    myReservationTable->setColumnWidth(1, 250);
    myReservationTable->setColumnWidth(2, 150);
    myReservationTable->setColumnWidth(3, 80);
}

// 借书
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

// 还书
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

// 续借
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

// 切换到图书查询页面
void ReaderWindow::switchToBookSearch()
{
    stackedWidget->setCurrentWidget(bookSearchWidget);
}

// 切换到我的借阅页面
void ReaderWindow::switchToMyBorrow()
{
    displayMyBorrowRecords();
    stackedWidget->setCurrentWidget(myBorrowWidget);
}

// 切换到我的预约页面
void ReaderWindow::switchToMyReservation()
{
    displayMyReservations();
    stackedWidget->setCurrentWidget(myReservationWidget);
}

// 显示输入对话框
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

// 设置消息管理页面
void ReaderWindow::setupMessageWidget()
{
    messageWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(messageWidget);

    // 操作按钮和查找输入框布局（同一行）
    QHBoxLayout *topLayout = new QHBoxLayout();

    // 操作按钮
    QPushButton *deleteMessageBtn = new QPushButton("删除消息", this);
    QPushButton *clearAllMessagesBtn = new QPushButton("清除所有消息", this);
    QPushButton *markAllReadBtn = new QPushButton("全部设为已读", this);

    topLayout->addWidget(deleteMessageBtn);
    topLayout->addWidget(clearAllMessagesBtn);
    topLayout->addWidget(markAllReadBtn);

    // 添加分隔线
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    topLayout->addWidget(separator);

    // 消息查找输入框
    messageTimeEdit = new QLineEdit(this);
    messageTimeEdit->setPlaceholderText("消息时间");
    messageTimeEdit->setFixedWidth(150);

    messageContentEdit = new QLineEdit(this);
    messageContentEdit->setPlaceholderText("消息内容");
    messageContentEdit->setFixedWidth(300);

    messageStatusCombo = new QComboBox(this);
    messageStatusCombo->addItem("");
    messageStatusCombo->addItem("已读");
    messageStatusCombo->addItem("未读");
    messageStatusCombo->setFixedWidth(80);

    QPushButton *searchMessageBtn = new QPushButton("查找消息", this);

    topLayout->addWidget(messageTimeEdit);
    topLayout->addWidget(messageContentEdit);
    topLayout->addWidget(messageStatusCombo);
    topLayout->addWidget(searchMessageBtn);
    topLayout->addStretch();

    // 连接信号槽
    connect(deleteMessageBtn, &QPushButton::clicked, this, &ReaderWindow::onDeleteMessage);
    connect(clearAllMessagesBtn, &QPushButton::clicked, this, &ReaderWindow::onClearAllMessages);
    connect(markAllReadBtn, &QPushButton::clicked, this, &ReaderWindow::onMarkAllRead);
    connect(searchMessageBtn, &QPushButton::clicked, this, &ReaderWindow::onSearchMessage);

    mainLayout->addLayout(topLayout);

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

// 显示消息列表
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

// 查看消息
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

// 退出登录
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

// 删除消息
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
        currentUser->deleteMessage(currentRow);
        DataManager::getInstance()->writeMessage();
        displayMessages(currentUser->getMessages());

        QMessageBox msgBox(QMessageBox::Information, "成功", "消息删除成功！", QMessageBox::NoButton, this);
        msgBox.addButton("确定", QMessageBox::AcceptRole);
        msgBox.exec();
    }
}

// 清除所有消息
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

// 全部设为已读
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

// 查找消息
void ReaderWindow::onSearchMessage()
{
    QString time = messageTimeEdit->text().trimmed();
    QString content = messageContentEdit->text().trimmed();
    QString status = messageStatusCombo->currentText().trimmed();

    std::vector<Message> allMessages = currentUser->getMessages();
    std::vector<Message> filteredMessages;

    // 如果所有条件都为空，显示全部消息
    if (time.isEmpty() && content.isEmpty() && status.isEmpty())
    {
        displayMessages(allMessages);
        return;
    }

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

    displayMessages(filteredMessages);
}