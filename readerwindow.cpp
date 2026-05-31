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
    QHBoxLayout *searchLayout = new QHBoxLayout();

    // 操作按钮（预约、借书）
    bookReserveBtn = new QPushButton("预约", this);
    borrowBtn = new QPushButton("借书", this);

    searchLayout->addWidget(bookReserveBtn);
    searchLayout->addWidget(borrowBtn);

    // 添加分隔线
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    searchLayout->addWidget(separator);

    // 输入框（无标签，使用placeholder提示）
    bookISBNLineEdit = new QLineEdit(this);
    bookISBNLineEdit->setPlaceholderText("ISBN");
    bookISBNLineEdit->setFixedWidth(150);

    bookTitleLineEdit = new QLineEdit(this);
    bookTitleLineEdit->setPlaceholderText("书名");
    bookTitleLineEdit->setFixedWidth(150);

    bookAuthorLineEdit = new QLineEdit(this);
    bookAuthorLineEdit->setPlaceholderText("作者");
    bookAuthorLineEdit->setFixedWidth(120);

    bookCategoryLineEdit = new QLineEdit(this);
    bookCategoryLineEdit->setPlaceholderText("分类");
    bookCategoryLineEdit->setFixedWidth(100);

    searchLayout->addWidget(bookISBNLineEdit);
    searchLayout->addWidget(bookTitleLineEdit);
    searchLayout->addWidget(bookAuthorLineEdit);
    searchLayout->addWidget(bookCategoryLineEdit);

    // 查找按钮紧跟在分类输入框后面
    bookSearchBtn = new QPushButton("查找", this);
    searchLayout->addWidget(bookSearchBtn);

    // 添加伸缩空间到末尾，保持控件紧凑
    searchLayout->addStretch();

    mainLayout->addLayout(searchLayout);

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

    // 顶部工具栏区域
    QHBoxLayout *topLayout = new QHBoxLayout();

    // 操作按钮
    returnBtn = new QPushButton("还书", this);
    renewBtn = new QPushButton("续借", this);
    payFineBtn = new QPushButton("支付所有罚款", this);

    topLayout->addWidget(returnBtn);
    topLayout->addWidget(renewBtn);
    topLayout->addWidget(payFineBtn);

    // 添加分隔线
    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    topLayout->addWidget(separator);

    // 查询输入框
    borrowISBNEdit = new QLineEdit(this);
    borrowISBNEdit->setPlaceholderText("ISBN");
    borrowISBNEdit->setFixedWidth(120);

    borrowTitleEdit = new QLineEdit(this);
    borrowTitleEdit->setPlaceholderText("书名");
    borrowTitleEdit->setFixedWidth(150);

    borrowTimeEdit = new QLineEdit(this);
    borrowTimeEdit->setPlaceholderText("借阅时间");
    borrowTimeEdit->setFixedWidth(120);

    borrowDueTimeEdit = new QLineEdit(this);
    borrowDueTimeEdit->setPlaceholderText("应还时间");
    borrowDueTimeEdit->setFixedWidth(120);

    borrowReturnTimeEdit = new QLineEdit(this);
    borrowReturnTimeEdit->setPlaceholderText("归还时间");
    borrowReturnTimeEdit->setFixedWidth(120);

    borrowStatusEdit = new QLineEdit(this);
    borrowStatusEdit->setPlaceholderText("状态");
    borrowStatusEdit->setFixedWidth(80);

    borrowFineCombo = new QComboBox(this);
    borrowFineCombo->addItem("");
    borrowFineCombo->addItem("未支付");
    borrowFineCombo->addItem("已支付");
    borrowFineCombo->addItem("已减免");
    borrowFineCombo->setFixedWidth(80);

    QPushButton *searchBorrowBtn = new QPushButton("查找", this);

    topLayout->addWidget(borrowISBNEdit);
    topLayout->addWidget(borrowTitleEdit);
    topLayout->addWidget(borrowTimeEdit);
    topLayout->addWidget(borrowDueTimeEdit);
    topLayout->addWidget(borrowReturnTimeEdit);
    topLayout->addWidget(borrowStatusEdit);
    topLayout->addWidget(borrowFineCombo);
    topLayout->addWidget(searchBorrowBtn);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);

    // 借阅记录表格
    myBorrowTable = new QTableWidget(this);
    myBorrowTable->setColumnCount(9);
    QStringList headers = {"ISBN", "书名", "借阅时间", "应还时间", "归还时间", "状态", "罚款金额", "已支付罚款", "罚款状态"};
    myBorrowTable->setHorizontalHeaderLabels(headers);
    myBorrowTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    myBorrowTable->setSelectionBehavior(QTableWidget::SelectRows);
    mainLayout->addWidget(myBorrowTable);

    // 连接信号槽
    connect(returnBtn, &QPushButton::clicked, this, &ReaderWindow::onReturnBook);
    connect(renewBtn, &QPushButton::clicked, this, &ReaderWindow::onRenewBook);
    connect(payFineBtn, &QPushButton::clicked, this, &ReaderWindow::onPayAllFines);
    connect(searchBorrowBtn, &QPushButton::clicked, this, &ReaderWindow::onSearchBorrow);

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

    QPushButton *cancelReservationBtn = new QPushButton("取消预约", this);

    topLayout->addWidget(cancelReservationBtn);

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
    reservationStatusCombo->setFixedWidth(80);

    QPushButton *searchReservationBtn = new QPushButton("查找预约", this);

    topLayout->addWidget(reservationISBNEdit);
    topLayout->addWidget(reservationTitleEdit);
    topLayout->addWidget(reservationTimeEdit);
    topLayout->addWidget(reservationStatusCombo);
    topLayout->addWidget(searchReservationBtn);
    topLayout->addStretch();

    // 连接信号槽
    connect(cancelReservationBtn, &QPushButton::clicked, this, &ReaderWindow::onCancelReservation);
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

            QString status = "借阅中";
            if (record.isReturned())
            {
                status = "已归还";
            }
            else if (record.calculateOverdueDays() > 0)
            {
                status = QString("逾期(%1天)").arg(record.calculateOverdueDays());
            }
            myBorrowTable->setItem(row, 5, new QTableWidgetItem(status));

            // 罚款信息
            double fineAmount = record.calculateFine();
            double paidFine = record.getPaidFine();
            myBorrowTable->setItem(row, 6, new QTableWidgetItem(QString::number(fineAmount, 'f', 2) + "元"));
            myBorrowTable->setItem(row, 7, new QTableWidgetItem(QString::number(paidFine, 'f', 2) + "元"));

            QString fineStatus;
            switch (record.getFineStatus())
            {
            case BorrowRecord::FineStatus::UNPAID:
                fineStatus = "未支付";
                break;
            case BorrowRecord::FineStatus::PAID:
                fineStatus = "已支付";
                break;
            case BorrowRecord::FineStatus::WAIVED:
                fineStatus = "已减免";
                break;
            default:
                fineStatus = "未知";
            }
            myBorrowTable->setItem(row, 8, new QTableWidgetItem(fineStatus));
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
        Reader::ReserveResult result = reader->reserveBook(isbn);
        if (result == Reader::ReserveResult::SUCCESS)
        {
            QMessageBox::information(this, "成功", "预约成功！等待管理员审核。");
            onBookSearch();
            displayMyReservations();
        }
        else if (result == Reader::ReserveResult::BOOK_NOT_FOUND)
        {
            QMessageBox::warning(this, "失败", "预约失败！图书不存在。");
        }
        else if (result == Reader::ReserveResult::ALREADY_EXISTS)
        {
            QMessageBox::warning(this, "失败", "预约失败！您已预约或已借阅该图书。");
        }
        else if (result == Reader::ReserveResult::EXCEED_LIMIT)
        {
            QMessageBox::warning(this, "失败", "预约失败！该图书预约人数已达上限（预约量不超过库存的2倍）。");
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
        Reader::BorrowResult borrowResult = reader->borrowBook(isbn);
        switch (borrowResult)
        {
        case Reader::BorrowResult::SUCCESS:
            QMessageBox::information(this, "成功", "借书成功！");
            onBookSearch();
            displayMyBorrowRecords();
            break;
        case Reader::BorrowResult::BOOK_NOT_FOUND:
            QMessageBox::warning(this, "失败", "借书失败！图书不存在。");
            break;
        case Reader::BorrowResult::NO_STOCK:
            QMessageBox::warning(this, "失败", "借书失败！图书无库存。");
            break;
        case Reader::BorrowResult::HAS_OVERDUE:
            QMessageBox::warning(this, "失败", "借书失败！您有逾期未还图书，请先归还。");
            break;
        case Reader::BorrowResult::EXCEED_LIMIT:
            QMessageBox::warning(this, "失败", "借书失败！您已达到最大借阅数量（10本）。");
            break;
        case Reader::BorrowResult::ALREADY_BORROWED:
            QMessageBox::warning(this, "失败", "借书失败！您已借阅该图书且尚未归还，不能重复借阅。");
            break;
        case Reader::BorrowResult::NO_VALID_RESERVATION:
            QMessageBox::warning(this, "失败", "借书失败！您未预约成功该图书，请先预约并等待管理员审核。");
            break;
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
        Reader::ReturnResult returnResult = reader->returnBook(isbn);
        switch (returnResult)
        {
        case Reader::ReturnResult::SUCCESS:
            QMessageBox::information(this, "成功", "还书成功！");
            displayMyBorrowRecords();
            onBookSearch();
            break;
        case Reader::ReturnResult::NOT_FOUND:
            QMessageBox::warning(this, "失败", "还书失败！借阅记录不存在。");
            break;
        case Reader::ReturnResult::HAS_UNPAID_FINE:
            QMessageBox::warning(this, "失败", "还书失败！该图书存在未支付罚款，请先支付罚款后再还书。");
            break;
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
        Reader::RenewResult renewResult = reader->renewBook(isbn);
        switch (renewResult)
        {
        case Reader::RenewResult::SUCCESS:
            QMessageBox::information(this, "成功", "续借申请已提交，请等待管理员审核！");
            displayMyBorrowRecords();
            break;
        case Reader::RenewResult::NOT_FOUND:
            QMessageBox::warning(this, "失败", "续借失败！借阅记录不存在。");
            break;
        case Reader::RenewResult::ALREADY_PENDING:
            QMessageBox::warning(this, "失败", "续借失败！已有待审核的续借申请。");
            break;
        case Reader::RenewResult::HAS_OVERDUE:
            QMessageBox::warning(this, "失败", "续借失败！该图书已逾期，不能续借。");
            break;
        }
    }
}

// 查找借阅记录
void ReaderWindow::onSearchBorrow()
{
    QString isbn = borrowISBNEdit->text();
    QString title = borrowTitleEdit->text();
    QString time = borrowTimeEdit->text();
    QString dueTime = borrowDueTimeEdit->text();
    QString returnTime = borrowReturnTimeEdit->text();
    QString status = borrowStatusEdit->text();
    QString fineStatus = borrowFineCombo->currentText();

    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (!reader)
        return;

    DataManager *dm = DataManager::getInstance();
    std::vector<BorrowRecord> allRecords = dm->getBorrowRecords();
    std::vector<BorrowRecord> filteredRecords;

    for (const auto &record : allRecords)
    {
        if (record.getReaderID() != reader->getID())
            continue;

        bool match = true;

        if (!isbn.isEmpty() && !record.getISBN().contains(isbn))
            match = false;

        if (!title.isEmpty())
        {
            Book *book = dm->findBookByISBN(record.getISBN());
            if (!book || !book->getTitle().contains(title))
                match = false;
        }

        if (!time.isEmpty() && !record.getBorrowTime().toString("yyyy-MM-dd").contains(time))
            match = false;

        if (!dueTime.isEmpty() && !record.getDueTime().toString("yyyy-MM-dd").contains(dueTime))
            match = false;

        if (!returnTime.isEmpty() && !record.getReturnTime().toString("yyyy-MM-dd").contains(returnTime))
            match = false;

        if (!status.isEmpty())
        {
            QString recordStatus;
            if (record.isReturned())
                recordStatus = "已归还";
            else if (record.calculateOverdueDays() > 0)
                recordStatus = QString("逾期%1天").arg(record.calculateOverdueDays());
            else
                recordStatus = "借阅中";

            if (!recordStatus.contains(status))
                match = false;
        }

        if (!fineStatus.isEmpty())
        {
            BorrowRecord::FineStatus fs = record.getFineStatus();
            if (fineStatus == "未支付" && fs != BorrowRecord::FineStatus::UNPAID)
                match = false;
            if (fineStatus == "已支付" && fs != BorrowRecord::FineStatus::PAID)
                match = false;
            if (fineStatus == "已减免" && fs != BorrowRecord::FineStatus::WAIVED)
                match = false;
        }

        if (match)
            filteredRecords.push_back(record);
    }

    myBorrowTable->setRowCount(0);
    for (const auto &record : filteredRecords)
    {
        Book *book = dm->findBookByISBN(record.getISBN());
        int row = myBorrowTable->rowCount();
        myBorrowTable->insertRow(row);
        myBorrowTable->setItem(row, 0, new QTableWidgetItem(record.getISBN()));
        myBorrowTable->setItem(row, 1, new QTableWidgetItem(book ? book->getTitle() : "未知"));
        myBorrowTable->setItem(row, 2, new QTableWidgetItem(record.getBorrowTime().toString("yyyy-MM-dd hh:mm:ss")));
        myBorrowTable->setItem(row, 3, new QTableWidgetItem(record.getDueTime().toString("yyyy-MM-dd hh:mm:ss")));
        myBorrowTable->setItem(row, 4, new QTableWidgetItem(record.isReturned() ? record.getReturnTime().toString("yyyy-MM-dd hh:mm:ss") : "未归还"));

        QString statusText;
        if (record.isReturned())
            statusText = "已归还";
        else if (record.calculateOverdueDays() > 0)
            statusText = "逾期";
        else
            statusText = "正常";
        myBorrowTable->setItem(row, 5, new QTableWidgetItem(statusText));

        double fineAmount = record.calculateFine();
        double paidFine = record.getPaidFine();
        myBorrowTable->setItem(row, 6, new QTableWidgetItem(QString::number(fineAmount, 'f', 2) + "元"));
        myBorrowTable->setItem(row, 7, new QTableWidgetItem(QString::number(paidFine, 'f', 2) + "元"));

        QString fineStatusText;
        switch (record.getFineStatus())
        {
        case BorrowRecord::FineStatus::UNPAID:
            fineStatusText = "未支付";
            break;
        case BorrowRecord::FineStatus::PAID:
            fineStatusText = "已支付";
            break;
        case BorrowRecord::FineStatus::WAIVED:
            fineStatusText = "已减免";
            break;
        default:
            fineStatusText = "未知";
        }
        myBorrowTable->setItem(row, 8, new QTableWidgetItem(fineStatusText));
    }
}

// 支付所有罚款
void ReaderWindow::onPayAllFines()
{
    Reader *reader = dynamic_cast<Reader *>(currentUser);
    if (!reader)
        return;

    DataManager *dm = DataManager::getInstance();
    std::vector<BorrowRecord> allRecords = dm->getBorrowRecords();

    double totalFine = 0;
    for (const auto &record : allRecords)
    {
        if (record.getReaderID() == reader->getID() && !record.isReturned())
        {
            totalFine += record.calculateFine() - record.getPaidFine();
        }
    }

    if (totalFine <= 0)
    {
        QMessageBox::information(this, "提示", "您当前没有需要支付的罚款。");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "确认支付",
                                                              QString("您当前需要支付罚款总额：%1元，是否确认支付？").arg(totalFine),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        for (auto &record : allRecords)
        {
            if (record.getReaderID() == reader->getID() && !record.isReturned())
            {
                double remaining = record.calculateFine() - record.getPaidFine();
                if (remaining > 0)
                {
                    record.collectFine(remaining);
                }
            }
        }
        dm->writeBorrowRecord();
        QMessageBox::information(this, "成功", QString("成功支付罚款：%1元").arg(totalFine));
        displayMyBorrowRecords();
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