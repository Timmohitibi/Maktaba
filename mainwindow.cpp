#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Library Management System");
    resize(900, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout();

    // Book Management UI
    QHBoxLayout *bookLayout = new QHBoxLayout();
    titleInput = new QLineEdit();
    authorInput = new QLineEdit();
    genreInput = new QLineEdit();
    addBookButton = new QPushButton("Add Book");
    updateBookButton = new QPushButton("Update Book");
    deleteBookButton = new QPushButton("Delete Book");

    bookLayout->addWidget(new QLabel("Title:"));
    bookLayout->addWidget(titleInput);
    bookLayout->addWidget(new QLabel("Author:"));
    bookLayout->addWidget(authorInput);
    bookLayout->addWidget(new QLabel("Genre:"));
    bookLayout->addWidget(genreInput);
    bookLayout->addWidget(addBookButton);
    bookLayout->addWidget(updateBookButton);
    bookLayout->addWidget(deleteBookButton);

    mainLayout->addLayout(bookLayout);

    booksTable = new QTableWidget();
    booksTable->setColumnCount(4);
    booksTable->setHorizontalHeaderLabels({"ID", "Title", "Author", "Genre"});
    booksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(booksTable);

    // Member Management UI
    QHBoxLayout *memberLayout = new QHBoxLayout();
    nameInput = new QLineEdit();
    contactInput = new QLineEdit();
    addMemberButton = new QPushButton("Add Member");
    deleteMemberButton = new QPushButton("Delete Member");

    memberLayout->addWidget(new QLabel("Name:"));
    memberLayout->addWidget(nameInput);
    memberLayout->addWidget(new QLabel("Contact:"));
    memberLayout->addWidget(contactInput);
    memberLayout->addWidget(addMemberButton);
    memberLayout->addWidget(deleteMemberButton);

    mainLayout->addLayout(memberLayout);

    membersTable = new QTableWidget();
    membersTable->setColumnCount(3);
    membersTable->setHorizontalHeaderLabels({"ID", "Name", "Contact"});
    membersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(membersTable);

    // Reports UI
    QHBoxLayout *reportsLayout = new QHBoxLayout();
    viewBorrowedButton = new QPushButton("View Borrowed Books");
    viewAvailableButton = new QPushButton("View Available Books");
    reportsLayout->addWidget(viewBorrowedButton);
    reportsLayout->addWidget(viewAvailableButton);
    mainLayout->addLayout(reportsLayout);

    centralWidget->setLayout(mainLayout);

    connectDatabase();
    createTables();
    loadBooks();
    loadMembers(); // ✅ Fixed: Added function call

    // Connect slots
    connect(addBookButton, &QPushButton::clicked, this, &MainWindow::addBook);
    connect(updateBookButton, &QPushButton::clicked, this, &MainWindow::updateBook);
    connect(deleteBookButton, &QPushButton::clicked, this, &MainWindow::deleteBook);
    connect(addMemberButton, &QPushButton::clicked, this, &MainWindow::addMember);
    connect(deleteMemberButton, &QPushButton::clicked, this, &MainWindow::deleteMember);
    connect(viewBorrowedButton, &QPushButton::clicked, this, &MainWindow::loadBorrowedBooks);
    connect(viewAvailableButton, &QPushButton::clicked, this, &MainWindow::loadAvailableBooks);
}

void MainWindow::connectDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("library.db");

    if (!db.open()) {
        qDebug() << "Database connection failed!";
    }
}

void MainWindow::createTables() {
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS Books (id INTEGER PRIMARY KEY AUTOINCREMENT, title TEXT, author TEXT, genre TEXT, available INTEGER DEFAULT 1)");
    query.exec("CREATE TABLE IF NOT EXISTS Members (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, contact TEXT)");
    query.exec("CREATE TABLE IF NOT EXISTS BorrowedBooks (id INTEGER PRIMARY KEY AUTOINCREMENT, book_id INTEGER, member_id INTEGER, borrow_date TEXT, return_date TEXT, status TEXT DEFAULT 'Borrowed')");
}

// ✅ Fixed: Implemented missing function
void MainWindow::loadMembers() {
    membersTable->setRowCount(0);
    QSqlQuery query("SELECT * FROM Members");
    while (query.next()) {
        int row = membersTable->rowCount();
        membersTable->insertRow(row);
        for (int col = 0; col < 3; col++) {
            membersTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
    }
}

void MainWindow::loadBooks() {
    booksTable->setRowCount(0);
    QSqlQuery query("SELECT * FROM Books");
    while (query.next()) {
        int row = booksTable->rowCount();
        booksTable->insertRow(row);
        for (int col = 0; col < 4; col++) {
            booksTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
    }
}

void MainWindow::addBook() {
    QSqlQuery query;
    query.prepare("INSERT INTO Books (title, author, genre) VALUES (:title, :author, :genre)");
    query.bindValue(":title", titleInput->text());
    query.bindValue(":author", authorInput->text());
    query.bindValue(":genre", genreInput->text());
    if (query.exec()) {
        loadBooks();
    }
}

void MainWindow::updateBook() {
    int row = booksTable->currentRow();
    if (row < 0) return;
    QString id = booksTable->item(row, 0)->text();
    QSqlQuery query;
    query.prepare("UPDATE Books SET title=?, author=?, genre=? WHERE id=?");
    query.addBindValue(titleInput->text());
    query.addBindValue(authorInput->text());
    query.addBindValue(genreInput->text());
    query.addBindValue(id);
    if (query.exec()) loadBooks();
}

void MainWindow::deleteBook() {
    int row = booksTable->currentRow();
    if (row < 0) return;
    QString id = booksTable->item(row, 0)->text();
    QSqlQuery query;
    query.prepare("DELETE FROM Books WHERE id=?");
    query.addBindValue(id);
    if (query.exec()) loadBooks();
}

void MainWindow::addMember() {
    QSqlQuery query;
    query.prepare("INSERT INTO Members (name, contact) VALUES (:name, :contact)");
    query.bindValue(":name", nameInput->text());
    query.bindValue(":contact", contactInput->text());
    if (query.exec()) {
        loadMembers(); // ✅ Fixed: Added function call
    }
}

void MainWindow::deleteMember() {
    int row = membersTable->currentRow();
    if (row < 0) return;
    QString id = membersTable->item(row, 0)->text();
    QSqlQuery query;
    query.prepare("DELETE FROM Members WHERE id=?");
    query.addBindValue(id);
    if (query.exec()) loadMembers(); // ✅ Fixed: Added function call
}

// ✅ Fixed: Implemented missing function
void MainWindow::loadBorrowedBooks() {
    booksTable->setRowCount(0);
    QSqlQuery query("SELECT Books.id, Books.title, Members.name, BorrowedBooks.borrow_date FROM Books "
                    "JOIN BorrowedBooks ON Books.id = BorrowedBooks.book_id "
                    "JOIN Members ON Members.id = BorrowedBooks.member_id "
                    "WHERE BorrowedBooks.status = 'Borrowed'");
    while (query.next()) {
        int row = booksTable->rowCount();
        booksTable->insertRow(row);
        for (int col = 0; col < 4; col++) {
            booksTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
    }
}

// ✅ Fixed: Implemented missing function
void MainWindow::loadAvailableBooks() {
    booksTable->setRowCount(0);
    QSqlQuery query("SELECT * FROM Books WHERE available = 1");
    while (query.next()) {
        int row = booksTable->rowCount();
        booksTable->insertRow(row);
        for (int col = 0; col < 4; col++) {
            booksTable->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
    }
}

MainWindow::~MainWindow() {
    if (db.isOpen()) db.close();
}
