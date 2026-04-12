#include "dashboardwindow.h"
#include "ui_DashboardWindow.h"
#include "LoginWindow.h"
#include <QTableWidgetItem>

using namespace std;

DashboardWindow::DashboardWindow(QString userName, QString userRole, QWidget *parent)
    : QDialog(parent), // Fixed: Changed QWidget to QDialog to match the header file
    ui(new Ui::DashboardWindow),
    userName(userName.toStdString()),
    userRole(userRole.toStdString())
{
    ui->setupUi(this);
    setWindowTitle("Library Dashboard - " + userName);
    resize(1200, 800);

    QRect sg = QGuiApplication::primaryScreen()->availableGeometry();
    move((sg.width() - width()) / 2, (sg.height() - height()) / 2);

    ui->welcomeLabel->setText("Welcome, " + userName + " (" + userRole + ")");
    ui->eyebrowLabel->setText("RBAC LIBRARY  •  DASHBOARD");

    if (userRole.toUpper() != "LIBRARIAN") {
        ui->addBookGroup->setVisible(false);
    }

    setStyleSheet(R"(
        QWidget#DashboardWindow {
            background:
                qradialgradient(cx:0.14, cy:0.12, radius:0.5,
                                fx:0.14, fy:0.12,
                                stop:0 rgba(224,163,35,90),
                                stop:1 rgba(224,163,35,0)),
                qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 #fff9ec, stop:0.5 #f7e7c7, stop:1 #e6c68e);
            color: #1e1b16;
            font-family: 'Trebuchet MS';
        }
        QFrame#shellFrame {
            background: rgba(255,249,236,120);
            border: 1px solid rgba(30,27,22,28);
            border-radius: 36px;
        }
        QFrame#leftPanel, QFrame#rightPanel {
            background: rgba(255,251,244,225);
            border: 1px solid rgba(30,27,22,28);
            border-radius: 24px;
        }
        QFrame#iconFrame {
            background: #0f3f31;
            border-radius: 14px;
            border: none;
        }
        QLabel#iconLabel {
            color: #fffaf0;
            font-size: 13px;
            font-weight: 800;
        }
        QLabel#eyebrowLabel {
            color: #ac4f2d;
            font-size: 11px;
            font-weight: 700;
            letter-spacing: 2px;
        }
        QLabel#welcomeLabel {
            color: #1e1b16;
            font-family: Georgia;
            font-size: 20px;
            font-weight: 700;
        }
        QLabel#tableTitle {
            color: #1e1b16;
            font-family: Georgia;
            font-size: 18px;
            font-weight: 700;
        }
        QGroupBox {
            color: #1e1b16;
            font-size: 13px;
            font-weight: 700;
            border: 1px solid rgba(30,27,22,28);
            border-radius: 16px;
            margin-top: 8px;
            background: rgba(255,249,236,180);
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px;
            color: #716657;
        }
        QLabel {
            font-size: 12px;
            font-weight: 700;
            color: #716657;
            background: transparent;
            border: none;
        }
        QLineEdit {
            background: rgba(255,249,236,0.86);
            border: 1px solid rgba(30,27,22,28);
            border-radius: 14px;
            padding: 10px 14px;
            color: #1e1b16;
            font-size: 13px;
        }
        QLineEdit:focus { border: 1px solid #1f6f54; }
        QPushButton#addBookButton {
            background: #0f3f31;
            color: #fffaf0;
            border: none;
            border-radius: 18px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton#addBookButton:hover { background: #1f6f54; }
        QPushButton#issueButton {
            background: #1f6f54;
            color: #fffaf0;
            border: none;
            border-radius: 16px;
            font-size: 13px;
            font-weight: 700;
        }
        QPushButton#issueButton:hover { background: #0f3f31; }
        QPushButton#returnButton {
            background: rgba(255,249,236,0.72);
            color: #0f3f31;
            border: 1px solid rgba(30,27,22,28);
            border-radius: 16px;
            font-size: 13px;
            font-weight: 700;
        }
        QPushButton#returnButton:hover { background: rgba(255,243,207,0.88); }
        QPushButton#searchButton {
            background: #0f3f31;
            color: #fffaf0;
            border: none;
            border-radius: 16px;
            font-size: 13px;
            font-weight: 700;
        }
        QPushButton#searchButton:hover { background: #1f6f54; }
        QPushButton#logoutButton {
            background: rgba(255,249,236,0.72);
            color: #0f3f31;
            border: 1px solid rgba(30,27,22,28);
            border-radius: 18px;
            font-size: 13px;
            font-weight: 700;
            padding: 8px 20px;
        }
        QPushButton#logoutButton:hover { background: rgba(255,243,207,0.88); }
        QTableWidget {
            background: rgba(255,251,244,200);
            border: 1px solid rgba(30,27,22,28);
            border-radius: 12px;
            gridline-color: rgba(30,27,22,20);
            font-size: 13px;
        }
        QTableWidget::item:selected {
            background: rgba(31,111,84,0.15);
            color: #1e1b16;
        }
        QHeaderView::section {
            background: rgba(255,243,207,200);
            color: #1e1b16;
            font-weight: 700;
            font-size: 12px;
            border: none;
            padding: 8px;
        }
        QLabel#statusLabel {
            color: #ac4f2d;
            font-size: 12px;
            font-weight: 600;
        }
        QFrame#dividerLine {
            color: rgba(30,27,22,20);
        }
    )");

    connect(ui->addBookButton, &QPushButton::clicked,
            this, &DashboardWindow::handleAddBook);
    connect(ui->issueButton, &QPushButton::clicked,
            this, &DashboardWindow::handleIssueBook);
    connect(ui->returnButton, &QPushButton::clicked,
            this, &DashboardWindow::handleReturnBook);
    connect(ui->searchButton, &QPushButton::clicked,
            this, &DashboardWindow::handleSearch);
    connect(ui->logoutButton, &QPushButton::clicked,
            this, &DashboardWindow::handleLogout);

    refreshBookTable();
}

DashboardWindow::~DashboardWindow() { delete ui; }

void DashboardWindow::refreshBookTable() {
    auto books = libraryService.getAllBooks();
    ui->bookTable->setRowCount(0);
    ui->bookTable->setColumnCount(4);
    ui->bookTable->setHorizontalHeaderLabels({"ID", "Title", "Author", "Status"});
    ui->bookTable->horizontalHeader()->setStretchLastSection(true);
    ui->bookTable->verticalHeader()->setVisible(false);

    for (auto& b : books) {
        int row = ui->bookTable->rowCount();
        ui->bookTable->insertRow(row);
        ui->bookTable->setItem(row, 0, new QTableWidgetItem(
                                           QString::fromStdString(b.getBookId())));
        ui->bookTable->setItem(row, 1, new QTableWidgetItem(
                                           QString::fromStdString(b.getTitle())));
        ui->bookTable->setItem(row, 2, new QTableWidgetItem(
                                           QString::fromStdString(b.getAuthor())));
        ui->bookTable->setItem(row, 3, new QTableWidgetItem(
                                           b.isAvailable() ? "✓ Available" : "✗ Issued"));
    }
}

void DashboardWindow::handleAddBook() {
    string id    = ui->bookIdEdit->text().toStdString();
    string title  = ui->bookTitleEdit->text().toStdString();
    string author = ui->bookAuthorEdit->text().toStdString();

    if (id.empty() || title.empty() || author.empty()) {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText("Please fill all fields.");
        return;
    }

    string result = libraryService.addBook(userRole, userName, id, title, author);
    if (result == "SUCCESS") {
        ui->statusLabel->setStyleSheet("color: #1f6f54;");
        ui->statusLabel->setText("Book added successfully!");
        ui->bookIdEdit->clear();
        ui->bookTitleEdit->clear();
        ui->bookAuthorEdit->clear();
        refreshBookTable();
    } else {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText(QString::fromStdString(result));
    }
}

void DashboardWindow::handleIssueBook() {
    string id = ui->actionBookIdEdit->text().toStdString();
    if (id.empty()) {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText("Enter a Book ID.");
        return;
    }
    string result = libraryService.issueBook(userRole, userName, id);
    if (result == "SUCCESS") {
        ui->statusLabel->setStyleSheet("color: #1f6f54;");
        ui->statusLabel->setText("Book issued successfully!");
        refreshBookTable();
    } else {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText(QString::fromStdString(result));
    }
}

void DashboardWindow::handleReturnBook() {
    string id = ui->actionBookIdEdit->text().toStdString();
    if (id.empty()) {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText("Enter a Book ID.");
        return;
    }
    string result = libraryService.returnBook(userRole, userName, id);
    if (result == "SUCCESS") {
        ui->statusLabel->setStyleSheet("color: #1f6f54;");
        ui->statusLabel->setText("Book returned successfully!");
        refreshBookTable();
    } else {
        ui->statusLabel->setStyleSheet("color: #ac4f2d;");
        ui->statusLabel->setText(QString::fromStdString(result));
    }
}

void DashboardWindow::handleSearch() {
    string keyword = ui->searchEdit->text().toStdString();
    auto results = keyword.empty()
                       ? libraryService.getAllBooks()
                       : libraryService.searchBook(keyword);

    ui->bookTable->setRowCount(0);
    for (auto& b : results) {
        int row = ui->bookTable->rowCount();
        ui->bookTable->insertRow(row);
        ui->bookTable->setItem(row, 0, new QTableWidgetItem(
                                           QString::fromStdString(b.getBookId())));
        ui->bookTable->setItem(row, 1, new QTableWidgetItem(
                                           QString::fromStdString(b.getTitle())));
        ui->bookTable->setItem(row, 2, new QTableWidgetItem(
                                           QString::fromStdString(b.getAuthor())));
        ui->bookTable->setItem(row, 3, new QTableWidgetItem(
                                           b.isAvailable() ? "✓ Available" : "✗ Issued"));
    }
}

void DashboardWindow::handleLogout() {
    LoginWindow* login = new LoginWindow();
    login->show();
    this->close();
}