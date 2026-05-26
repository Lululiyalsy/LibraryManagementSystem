#ifndef READERWINDOW_H
#define READERWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStackedWidget>
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDateTime>
#include <vector>
#include "User.h"
#include "Message.h"
#include "Book.h"
#include "Reservation.h"
#include "BorrowRecord.h"

class ReaderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReaderWindow(User *user, QWidget *parent = nullptr);
    ~ReaderWindow();

private slots:
    void onBookSearch();
    void onBookReserve();
    void onCancelReservation();
    void onBorrowBook();
    void onReturnBook();
    void onRenewBook();
    void switchToBookSearch();
    void switchToMyBorrow();
    void switchToMyReservation();
    void onCheckMessages();
    void onSendTestMessage();
    void onLogout();

signals:
    void logout();

private:
    User *currentUser;
    QToolBar *toolbar;
    QStackedWidget *stackedWidget;

    QWidget *bookSearchWidget;
    QWidget *myBorrowWidget;
    QWidget *myReservationWidget;
    QWidget *messageWidget;

    QTableWidget *bookSearchTable;
    QTableWidget *myBorrowTable;
    QTableWidget *myReservationTable;
    QTableWidget *messageTable;

    QLineEdit *bookISBNLineEdit;
    QLineEdit *bookTitleLineEdit;
    QLineEdit *bookAuthorLineEdit;
    QLineEdit *bookCategoryLineEdit;

    QPushButton *bookSearchBtn;
    QPushButton *bookReserveBtn;
    QPushButton *borrowBtn;
    QPushButton *returnBtn;
    QPushButton *renewBtn;
    QPushButton *cancelReserveBtn;

    void setupCentralWidget();
    void setupBookSearchWidget();
    void setupMyBorrowWidget();
    void setupMyReservationWidget();
    void setupMessageWidget();
    void displayBooks(const std::vector<const Book *> &books);
    void displayMyBorrowRecords();
    void displayMyReservations();
    void displayMessages(const std::vector<Message> &messages);
    QPair<QString, bool> showInputDialog(const QString &title, const QString &label, bool isPassword = false);
};

#endif // READERWINDOW_H