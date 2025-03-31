#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTableWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;

    // Book Management
    QLineEdit *titleInput, *authorInput, *genreInput;
    QPushButton *addBookButton, *updateBookButton, *deleteBookButton;
    QTableWidget *booksTable;

    // Member Management (Fixed)
    QLineEdit *nameInput, *contactInput;
    QPushButton *addMemberButton, *deleteMemberButton;
    QTableWidget *membersTable;

    // Reports (Fixed)
    QPushButton *viewBorrowedButton, *viewAvailableButton;

    QSqlDatabase db;

    void connectDatabase();
    void createTables();
    void loadBooks();
    void loadMembers();
    void loadBorrowedBooks();
    void loadAvailableBooks();

private slots:
    void addBook();
    void updateBook();
    void deleteBook();
    void addMember();      // Fixed
    void deleteMember();   // Fixed
};

#endif // MAINWINDOW_H
