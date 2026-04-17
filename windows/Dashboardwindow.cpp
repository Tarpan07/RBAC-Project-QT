#include "Dashboardwindow.h"
#include "LoginWindow.h"
#include "../services/DatabaseManager.h"

#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScreen>
#include <QScrollArea>
#include <QSet>
#include <QSizePolicy>
#include <QSqlQuery>
#include <QStringList>
#include <QStyle>
#include <algorithm>

using namespace std;

namespace {
QLabel *makeLabel(const QString &text, const char *name = nullptr)
{
    auto *label = new QLabel(text);
    if (name) {
        label->setObjectName(name);
    }
    label->setWordWrap(true);
    return label;
}

QFrame *makeFrame(const char *name)
{
    auto *frame = new QFrame;
    frame->setObjectName(name);
    frame->setFrameShape(QFrame::NoFrame);
    return frame;
}

QString bookSearchText(const Book &book)
{
    return QString::fromStdString(book.getBookId() + " " + book.getTitle() + " " + book.getAuthor()).toLower();
}
}

DashboardWindow::DashboardWindow(QString userName,
                                 QString userRole,
                                 QString userEmail,
                                 QString studentId,
                                 QWidget *parent)
    : QDialog(parent),
    userName(userName.toStdString()),
    userRole(normalizeRole(userRole).toStdString()),
    userEmail(userEmail.toStdString()),
    displayName(userName),
    displayRole(normalizeRole(userRole)),
    displayEmail(userEmail),
    displayStudentId(studentId)
{
    setObjectName("DashboardWindow");
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setWindowTitle(displayRole.left(1) + displayRole.mid(1).toLower() + " Dashboard - " + displayName);
    resize(1280, 800);
    setMinimumSize(1060, 700);

    if (displayEmail.trimmed().isEmpty()) {
        displayEmail = displayName.toLower().replace(" ", ".") + "@library.local";
    }
    if (displayStudentId.trimmed().isEmpty()) {
        displayStudentId = "NA";
    }

    QRect sg = QGuiApplication::primaryScreen()->availableGeometry();
    move((sg.width() - width()) / 2, (sg.height() - height()) / 2);

    buildUi();
    applyTheme();
    navigate(DashboardPage);
}

DashboardWindow::~DashboardWindow() = default;

void DashboardWindow::buildUi()
{
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    auto *sidebar = makeFrame("sidebar");
    sidebar->setFixedWidth(220);
    auto *sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(14, 14, 14, 14);
    sideLayout->setSpacing(12);

    auto *brandCard = makeFrame("brandCard");
    auto *brandLayout = new QHBoxLayout(brandCard);
    brandLayout->setContentsMargins(10, 10, 10, 10);
    auto *logo = makeLabel("RB", "logoLabel");
    logo->setFixedSize(40, 40);
    logo->setAlignment(Qt::AlignCenter);
    brandLayout->addWidget(logo);
    brandLayout->addWidget(makeLabel("RBAC\nLibrary System", "brandLabel"), 1);
    sideLayout->addWidget(brandCard);

    sideLayout->addWidget(createNavButton("Dashboard", DashboardPage));
    sideLayout->addWidget(createNavButton("Books", BooksPage));
    sideLayout->addWidget(createNavButton("My Books", MyBooksPage));
    if (canManageUsers()) {
        sideLayout->addWidget(createNavButton("Users", UsersPage));
        sideLayout->addWidget(createNavButton("Profile", ProfilePage));
    } else {
        sideLayout->addWidget(createNavButton("Profile", ProfilePage));
    }

    sideLayout->addStretch(1);
    auto *logoutButton = new QPushButton("Logout");
    logoutButton->setObjectName("navButton");
    logoutButton->setMinimumHeight(44);
    connect(logoutButton, &QPushButton::clicked, this, &DashboardWindow::handleLogout);
    sideLayout->addWidget(logoutButton);

    auto *mainArea = makeFrame("mainArea");
    auto *mainLayout = new QVBoxLayout(mainArea);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(14);

    auto *header = new QHBoxLayout;
    pageTitleLabel = makeLabel("Dashboard", "pageTitleLabel");
    auto *roleTag = makeLabel(displayRole.left(1) + displayRole.mid(1).toLower(), "roleTag");
    roleTag->setAlignment(Qt::AlignCenter);
    roleTag->setFixedWidth(110);
    header->addWidget(pageTitleLabel, 1);
    header->addWidget(roleTag);
    mainLayout->addLayout(header);

    pages = new QStackedWidget;
    pages->addWidget(buildDashboardPage());
    pages->addWidget(buildBooksPage());
    pages->addWidget(buildMyBooksPage());
    pages->addWidget(buildProfilePage());
    pages->addWidget(buildUsersPage());
    mainLayout->addWidget(pages, 1);

    statusLabel = makeLabel("", "statusLabel");
    mainLayout->addWidget(statusLabel);

    root->addWidget(sidebar);
    root->addWidget(mainArea, 1);
}

QPushButton *DashboardWindow::createNavButton(const QString &text, PageIndex page)
{
    auto *button = new QPushButton(text);
    button->setObjectName("navButton");
    button->setMinimumHeight(44);
    button->setCursor(Qt::PointingHandCursor);
    navButtons[page] = button;
    connect(button, &QPushButton::clicked, this, [this, page]() { navigate(page); });
    return button;
}

QWidget *DashboardWindow::buildDashboardPage()
{
    auto *page = new QWidget;
    auto *pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);

    dashboardScroll = new QScrollArea;
    dashboardScroll->setObjectName("plainScroll");
    dashboardScroll->setWidgetResizable(true);
    dashboardScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Container will be replaced on every refresh
    dashboardContainer = new QWidget;
    dashboardScroll->setWidget(dashboardContainer);
    pageLayout->addWidget(dashboardScroll, 1);
    return page;
}

QWidget *DashboardWindow::buildBooksPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    bookSearchEdit = new QLineEdit;
    bookSearchEdit->setObjectName("searchEdit");
    bookSearchEdit->setPlaceholderText("Search for books...");
    bookSearchEdit->setMinimumHeight(42);
    connect(bookSearchEdit, &QLineEdit::textChanged, this, &DashboardWindow::refreshBooks);
    layout->addWidget(bookSearchEdit);

    if (canManageBooks()) {
        auto *manageCard = makeFrame("panelCard");
        auto *manage = new QGridLayout(manageCard);
        manage->setContentsMargins(14, 14, 14, 14);
        manage->setHorizontalSpacing(10);
        manage->setVerticalSpacing(10);

        bookIdEdit = new QLineEdit;
        bookTitleEdit = new QLineEdit;
        bookAuthorEdit = new QLineEdit;
        bookCountEdit = new QLineEdit;
        bookAvailableEdit = new QLineEdit;
        bookIdEdit->setPlaceholderText("Book ID");
        bookTitleEdit->setPlaceholderText("Title");
        bookAuthorEdit->setPlaceholderText("Author");
        bookCountEdit->setPlaceholderText("Total");
        bookAvailableEdit->setPlaceholderText("Available");
        bookCountEdit->setValidator(new QIntValidator(1, 999, bookCountEdit));
        bookAvailableEdit->setValidator(new QIntValidator(0, 999, bookAvailableEdit));

        manage->addWidget(bookIdEdit, 0, 0);
        manage->addWidget(bookTitleEdit, 0, 1);
        manage->addWidget(bookAuthorEdit, 0, 2);
        manage->addWidget(bookCountEdit, 0, 3);
        manage->addWidget(bookAvailableEdit, 0, 4);

        auto *saveButton = new QPushButton(displayRole == "LIBRARIAN" ? "Add/Update" : "Update");
        saveButton->setObjectName("primaryButton");
        connect(saveButton, &QPushButton::clicked, this, &DashboardWindow::handleBookSave);
        manage->addWidget(saveButton, 0, 5, 1, 1);

        if (displayRole == "LIBRARIAN") {
            auto *deleteButton = new QPushButton("Delete");
            deleteButton->setObjectName("dangerButton");
            connect(deleteButton, &QPushButton::clicked, this, &DashboardWindow::handleBookDelete);
            manage->addWidget(deleteButton, 0, 6, 1, 1);
        }
        manage->setColumnStretch(1, 1);
        manage->setColumnStretch(2, 1);

        layout->addWidget(manageCard);
    }

    booksScrollArea = new QScrollArea;
    booksScrollArea->setObjectName("plainScroll");
    booksScrollArea->setWidgetResizable(true);
    booksScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *container = new QWidget;
    booksGrid = new QGridLayout(container);
    booksGrid->setContentsMargins(0, 0, 0, 0);
    booksGrid->setHorizontalSpacing(14);
    booksGrid->setVerticalSpacing(14);
    booksGrid->setAlignment(Qt::AlignTop);
    booksScrollArea->setWidget(container);
    layout->addWidget(booksScrollArea, 1);

    return page;
}

QWidget *DashboardWindow::buildMyBooksPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);
    layout->addWidget(makeLabel("Active Borrowed Books", "sectionTitle"));

    auto *scroll = new QScrollArea;
    scroll->setObjectName("plainScroll");
    scroll->setWidgetResizable(true);
    auto *container = new QWidget;
    myBooksLayout = new QVBoxLayout(container);
    myBooksLayout->setContentsMargins(0, 0, 0, 0);
    myBooksLayout->setSpacing(10);
    scroll->setWidget(container);
    layout->addWidget(scroll, 1);
    return page;
}

QWidget *DashboardWindow::buildProfilePage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    auto *profileCard = makeFrame("panelCard");
    auto *profileLayout = new QVBoxLayout(profileCard);
    profileLayout->setContentsMargins(16, 16, 16, 16);
    profileLayout->setSpacing(10);

    auto *avatar = makeLabel(initialsForName(displayName), "avatarLabel");
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setFixedSize(82, 82);

    profileNameLabel = makeLabel(displayName, "profileName");
    profileRoleLabel = makeLabel("", "profileMeta");
    profileEmailLabel = makeLabel("", "profileMeta");
    profileIdLabel = makeLabel("", "profileMeta");
    profileIssueCountLabel = makeLabel("", "profileMeta");
    profileIssueLimitLabel = makeLabel("", "profileMeta");

    profileLayout->addWidget(avatar, 0, Qt::AlignLeft);
    profileLayout->addWidget(profileNameLabel);
    profileLayout->addWidget(profileRoleLabel);
    profileLayout->addWidget(profileEmailLabel);
    profileLayout->addWidget(profileIdLabel);
    profileLayout->addWidget(profileIssueCountLabel);
    profileLayout->addWidget(profileIssueLimitLabel);
    layout->addWidget(profileCard);
    layout->addStretch(1);
    return page;
}

QWidget *DashboardWindow::buildUsersPage()
{
    auto *page = new QWidget;
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    if (!canManageUsers()) {
        layout->addWidget(makeLabel("Users page is available only for librarian role.", "profileMeta"));
        layout->addStretch(1);
        return page;
    }

    userSearchEdit = new QLineEdit;
    userSearchEdit->setObjectName("searchEdit");
    userSearchEdit->setPlaceholderText("Search users...");
    userSearchEdit->setMinimumHeight(42);
    connect(userSearchEdit, &QLineEdit::textChanged, this, &DashboardWindow::refreshUsers);
    layout->addWidget(userSearchEdit);

    auto *manageCard = makeFrame("panelCard");
    auto *manage = new QHBoxLayout(manageCard);
    manage->setContentsMargins(12, 12, 12, 12);
    manage->setSpacing(8);

    userNameEdit = new QLineEdit;
    userEmailEdit = new QLineEdit;
    userRoleEdit = new QLineEdit;
    userStudentIdEdit = new QLineEdit;
    userNameEdit->setPlaceholderText("Name");
    userEmailEdit->setPlaceholderText("Email");
    userRoleEdit->setPlaceholderText("Role");
    userStudentIdEdit->setPlaceholderText("Student ID");
    userEmailEdit->setReadOnly(true);

    manage->addWidget(userNameEdit);
    manage->addWidget(userEmailEdit);
    manage->addWidget(userRoleEdit);
    manage->addWidget(userStudentIdEdit);

    auto *updateButton = new QPushButton("Update");
    updateButton->setObjectName("primaryButton");
    connect(updateButton, &QPushButton::clicked, this, &DashboardWindow::handleUserUpdate);
    manage->addWidget(updateButton);

    auto *deleteButton = new QPushButton("Delete");
    deleteButton->setObjectName("dangerButton");
    connect(deleteButton, &QPushButton::clicked, this, &DashboardWindow::handleUserDelete);
    manage->addWidget(deleteButton);
    layout->addWidget(manageCard);

    auto *scroll = new QScrollArea;
    scroll->setObjectName("plainScroll");
    scroll->setWidgetResizable(true);
    auto *container = new QWidget;
    usersLayout = new QVBoxLayout(container);
    usersLayout->setContentsMargins(0, 0, 0, 0);
    usersLayout->setSpacing(10);
    scroll->setWidget(container);
    layout->addWidget(scroll, 1);
    return page;
}

QWidget *DashboardWindow::createStatCard(const QString &value, const QString &labelText)
{
    auto *card = makeFrame("statCard");
    card->setMinimumHeight(110);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(4);
    layout->addWidget(makeLabel(value, "statValue"));
    layout->addWidget(makeLabel(labelText, "statLabel"));
    return card;
}

QWidget *DashboardWindow::createBookCard(const Book &book, bool featured, bool alreadyIssued)
{
    auto *card = makeFrame(featured ? "bookCardFeatured" : "bookCard");
    card->setFixedHeight(featured ? 260 : 220);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(9);

    layout->addWidget(makeLabel(QString::fromStdString(book.getTitle()), featured ? "bookTitleFeatured" : "bookTitle"));
    layout->addWidget(makeLabel(QString::fromStdString(book.getAuthor()), featured ? "bookMetaFeatured" : "bookMeta"));

    auto *availabilityLabel = makeLabel(
        alreadyIssued
            ? "Already issued by you"
            : QString("%1/%2 copies available").arg(book.getAvailableCount()).arg(book.getTotalCount()),
        featured ? "bookMetaFeatured" : "bookMeta");
    layout->addWidget(availabilityLabel);
    layout->addStretch(1);

    auto *row = new QHBoxLayout;
    const bool canIssueBook = book.getAvailableCount() > 0 && !alreadyIssued;
    auto *issueButton = new QPushButton(canIssueBook ? "Issue" : (alreadyIssued ? "Already Issued" : "Unavailable"));
    issueButton->setObjectName("issueButton");
    issueButton->setEnabled(canIssueBook);
    if (!canIssueBook) {
        issueButton->setStyleSheet(
            "QPushButton#issueButton, QPushButton#issueButton:disabled {"
            "  background: rgba(210, 205, 195, 200);"
            "  color: #8a8278;"
            "  border: none;"
            "  border-radius: 10px;"
            "  padding: 8px 14px;"
            "  font-size: 13px;"
            "  font-weight: 800;"
            "}"
            );
    }
    connect(issueButton, &QPushButton::clicked, this, [this, book]() {
        if (!confirmAction("Confirm Issue",
                           "Issue \"" + QString::fromStdString(book.getTitle()) + "\" to your account?")) {
            return;
        }
        string result = libraryService.issueBook(userRole, userEmail, book.getBookId());
        statusLabel->setText(QString::fromStdString(result == "SUCCESS" ? "Book issued." : result));
        refreshAll();
    });
    row->addWidget(issueButton);
    row->addStretch(1);

    if (canManageBooks()) {
        auto *selectButton = new QPushButton("Select");
        selectButton->setObjectName("secondaryButton");
        connect(selectButton, &QPushButton::clicked, this, [this, book]() {
            if (!bookIdEdit) return;
            bookIdEdit->setText(QString::fromStdString(book.getBookId()));
            bookTitleEdit->setText(QString::fromStdString(book.getTitle()));
            bookAuthorEdit->setText(QString::fromStdString(book.getAuthor()));
            bookCountEdit->setText(QString::number(book.getTotalCount()));
            bookAvailableEdit->setText(QString::number(book.getAvailableCount()));
        });
        row->addWidget(selectButton);
    }
    layout->addLayout(row);
    return card;
}

QWidget *DashboardWindow::createBorrowedCard(const Book &book)
{
    auto *card = makeFrame("panelCard");
    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(10);

    auto *left = new QVBoxLayout;
    left->setSpacing(4);
    left->addWidget(makeLabel(QString::fromStdString(book.getTitle()), "bookTitle"));
    left->addWidget(makeLabel(QString::fromStdString(book.getAuthor()), "bookMeta"));
    layout->addLayout(left, 1);

    auto *returnButton = new QPushButton("Return");
    returnButton->setObjectName("secondaryButton");
    connect(returnButton, &QPushButton::clicked, this, [this, book]() {
        if (!confirmAction("Confirm Return",
                           "Return \"" + QString::fromStdString(book.getTitle()) + "\" now?")) {
            return;
        }
        string result = libraryService.returnBook(userRole, userEmail, book.getBookId());
        statusLabel->setText(QString::fromStdString(result == "SUCCESS" ? "Book returned." : result));
        refreshAll();
    });
    layout->addWidget(returnButton);
    return card;
}

QWidget *DashboardWindow::createUserCard(const QString &name, const QString &email, const QString &role, const QString &studentId)
{
    auto *card = makeFrame("panelCard");
    auto *layout = new QHBoxLayout(card);
    layout->setContentsMargins(14, 10, 14, 10);

    auto *left = new QVBoxLayout;
    left->setSpacing(3);
    left->addWidget(makeLabel(name, "bookTitle"));
    left->addWidget(makeLabel(email + " | " + role + " | ID: " + studentId, "bookMeta"));
    layout->addLayout(left, 1);

    auto *selectButton = new QPushButton("Select");
    selectButton->setObjectName("secondaryButton");
    connect(selectButton, &QPushButton::clicked, this, [this, name, email, role, studentId]() {
        selectedUserEmail = email;
        userNameEdit->setText(name);
        userEmailEdit->setText(email);
        userRoleEdit->setText(role);
        userStudentIdEdit->setText(studentId);
    });
    layout->addWidget(selectButton);

    return card;
}

void DashboardWindow::navigate(PageIndex page)
{
    pages->setCurrentIndex(page);
    const QString titles[] = {"Dashboard", "Books", "My Books", "Profile", "Users"};
    pageTitleLabel->setText(titles[page]);

    for (auto &item : navButtons) {
        item.second->setObjectName(item.first == page ? "activeNavButton" : "navButton");
        item.second->style()->unpolish(item.second);
        item.second->style()->polish(item.second);
    }

    refreshAll();
}

void DashboardWindow::refreshAll()
{
    refreshDashboard();
    refreshBooks(bookSearchEdit ? bookSearchEdit->text() : QString());
    refreshMyBooks();
    refreshProfile();
    refreshUsers(userSearchEdit ? userSearchEdit->text() : QString());
}

void DashboardWindow::refreshDashboard()
{
    // Delete old container and replace — avoids all nested layout deletion issues
    delete dashboardContainer;
    dashboardContainer = new QWidget;
    dashboardScroll->setWidget(dashboardContainer);

    auto *dashboardLayout = new QVBoxLayout(dashboardContainer);
    dashboardLayout->setContentsMargins(0, 4, 0, 12);
    dashboardLayout->setSpacing(12);

    const auto books = libraryService.getAllBooks();
    const auto borrowed = libraryService.getBorrowedBooks(userEmail);
    QSet<QString> borrowedIds;
    for (const auto &book : borrowed) {
        borrowedIds.insert(QString::fromStdString(book.getBookId()));
    }

    int availableCopies = 0;
    for (const auto &book : books) {
        availableCopies += book.getAvailableCount();
    }

    auto *heroCard = makeFrame("heroCard");
    auto *heroLayout = new QVBoxLayout(heroCard);
    heroLayout->setContentsMargins(18, 16, 18, 16);
    heroLayout->setSpacing(7);
    heroLayout->addWidget(makeLabel("Welcome back, " + displayName, "heroTitle"));
    heroLayout->addWidget(makeLabel(
        "You have " + QString::number(borrowed.size()) + " active issue(s). "
            + QString::number(availableCopies) + " copies are available across the library.",
        "heroSubtitle"));
    heroLayout->addWidget(makeLabel("Track top books and your current issues from here.", "heroSubtitle"));
    dashboardLayout->addWidget(heroCard);

    auto *statsRow = new QGridLayout;
    statsRow->setContentsMargins(0, 0, 0, 0);
    statsRow->setHorizontalSpacing(12);
    statsRow->setVerticalSpacing(12);
    statsRow->addWidget(createStatCard(QString::number(books.size()), "Books"), 0, 0);
    statsRow->addWidget(createStatCard(QString::number(availableCopies), "Available Copies"), 0, 1);
    statsRow->addWidget(createStatCard(QString::number(borrowed.size()), "My Active Issues"), 0, 2);
    statsRow->addWidget(createStatCard(issueLimitText(), "Issue Limit"), 0, 3);
    for (int i = 0; i < 4; ++i) {
        statsRow->setColumnStretch(i, 1);
    }
    dashboardLayout->addLayout(statsRow);

    dashboardLayout->addWidget(makeLabel("Top Books", "sectionTitle"));
    auto *previewRow = new QGridLayout;
    previewRow->setContentsMargins(0, 0, 0, 0);
    previewRow->setHorizontalSpacing(12);
    previewRow->setVerticalSpacing(12);
    const int previewCount = min(3, static_cast<int>(books.size()));
    for (int i = 0; i < previewCount; ++i) {
        const bool alreadyIssued = borrowedIds.contains(QString::fromStdString(books[i].getBookId()));
        previewRow->addWidget(createBookCard(books[i], i == 0, alreadyIssued), 0, i);
        previewRow->setColumnStretch(i, 1);
    }
    if (previewCount == 0) {
        previewRow->addWidget(makeLabel("No books available.", "bookMeta"), 0, 0);
    }
    dashboardLayout->addLayout(previewRow);
    dashboardLayout->addWidget(makeLabel("Use Books page for full book list and management.", "bookMeta"));
    dashboardLayout->addStretch(1);
}

void DashboardWindow::refreshBooks(const QString &filter)
{
    if (!booksGrid) {
        return;
    }

    while (auto *item = booksGrid->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    vector<Book> books = libraryService.getAllBooks();
    const auto borrowed = libraryService.getBorrowedBooks(userEmail);
    QSet<QString> borrowedIds;
    for (const auto &book : borrowed) {
        borrowedIds.insert(QString::fromStdString(book.getBookId()));
    }

    const QString needle = filter.trimmed().toLower();
    if (!needle.isEmpty()) {
        vector<Book> filtered;
        copy_if(books.begin(), books.end(), back_inserter(filtered), [needle](const Book &book) {
            return bookSearchText(book).contains(needle);
        });
        books = filtered;
    }

    if (books.empty()) {
        booksGrid->addWidget(makeLabel("No books found.", "bookMeta"), 0, 0);
        return;
    }

    const int viewportWidth = booksScrollArea ? booksScrollArea->viewport()->width() : width();
    const int cardWidthHint = 315;
    const int columns = std::max(1, viewportWidth / cardWidthHint);
    for (int i = 0; i < 8; ++i) {
        booksGrid->setColumnStretch(i, 0);
    }

    for (int i = 0; i < static_cast<int>(books.size()); ++i) {
        const bool alreadyIssued = borrowedIds.contains(QString::fromStdString(books[i].getBookId()));
        booksGrid->addWidget(createBookCard(books[i], false, alreadyIssued), i / columns, i % columns);
    }
    for (int i = 0; i < columns; ++i) {
        booksGrid->setColumnStretch(i, 1);
    }
}

void DashboardWindow::refreshMyBooks()
{
    if (!myBooksLayout) {
        return;
    }

    while (auto *item = myBooksLayout->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    const auto borrowed = libraryService.getBorrowedBooks(userEmail);
    if (borrowed.empty()) {
        myBooksLayout->addWidget(makeLabel("No active borrowed books.", "bookMeta"));
    } else {
        for (const auto &book : borrowed) {
            myBooksLayout->addWidget(createBorrowedCard(book));
        }
    }
    myBooksLayout->addStretch(1);
}

void DashboardWindow::refreshUsers(const QString &filter)
{
    if (!canManageUsers() || !usersLayout) {
        return;
    }

    while (auto *item = usersLayout->takeAt(0)) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    const QString needle = filter.trimmed().toLower();
    QSqlQuery query(DatabaseManager::database());
    query.exec("SELECT name, email, role, student_id FROM users ORDER BY role, name");

    while (query.next()) {
        const QString name = query.value(0).toString();
        const QString email = query.value(1).toString();
        const QString role = query.value(2).toString();
        const QString studentId = query.value(3).toString();
        const QString combined = (name + " " + email + " " + role + " " + studentId).toLower();
        if (needle.isEmpty() || combined.contains(needle)) {
            usersLayout->addWidget(createUserCard(name, email, role, studentId));
        }
    }
    usersLayout->addStretch(1);
}

void DashboardWindow::refreshProfile()
{
    if (!profileRoleLabel) {
        return;
    }

    const auto borrowed = libraryService.getBorrowedBooks(userEmail);
    profileRoleLabel->setText("Role: " + displayRole);
    profileEmailLabel->setText("Email: " + displayEmail);
    profileIdLabel->setText("Student ID: " + displayStudentId);
    profileIssueCountLabel->setText("Active issues: " + QString::number(borrowed.size()));
    profileIssueLimitLabel->setText("Issue limit: " + issueLimitText());
}

void DashboardWindow::handleBookSave()
{
    if (!canManageBooks() || !bookIdEdit) {
        return;
    }

    const string id = bookIdEdit->text().trimmed().toStdString();
    const string title = bookTitleEdit->text().trimmed().toStdString();
    const string author = bookAuthorEdit->text().trimmed().toStdString();
    const int total = bookCountEdit->text().toInt();
    const int available = bookAvailableEdit->text().isEmpty() ? total : bookAvailableEdit->text().toInt();

    if (id.empty() || title.empty() || author.empty() || total <= 0 || available < 0 || available > total) {
        statusLabel->setText("Enter valid book id/title/author/count values.");
        return;
    }

    bool exists = false;
    for (const auto &book : libraryService.getAllBooks()) {
        if (book.getBookId() == id) {
            exists = true;
            break;
        }
    }

    string result;
    if (!exists) {
        if (displayRole != "LIBRARIAN") {
            statusLabel->setText("Staff can only update existing books.");
            return;
        }
        result = libraryService.addBook(userRole, userEmail, id, title, author, total);
    } else {
        result = libraryService.updateBook(userRole, userEmail, id, title, author, total, available);
    }

    statusLabel->setText(QString::fromStdString(result == "SUCCESS" ? "Book saved." : result));
    refreshAll();
}

void DashboardWindow::handleBookDelete()
{
    if (displayRole != "LIBRARIAN" || !bookIdEdit) {
        return;
    }

    const string id = bookIdEdit->text().trimmed().toStdString();
    if (id.empty()) {
        statusLabel->setText("Enter book ID to delete.");
        return;
    }

    const string result = libraryService.deleteBook(userRole, userEmail, id);
    statusLabel->setText(QString::fromStdString(result == "SUCCESS" ? "Book deleted." : result));
    refreshAll();
}

void DashboardWindow::handleUserUpdate()
{
    if (!canManageUsers() || selectedUserEmail.isEmpty()) {
        statusLabel->setText("Select a user first.");
        return;
    }

    const QString role = normalizeRole(userRoleEdit->text().trimmed());
    if (role != "STUDENT" && role != "FACULTY" && role != "STAFF" && role != "LIBRARIAN") {
        statusLabel->setText("Role must be STUDENT/FACULTY/STAFF/LIBRARIAN.");
        return;
    }
    if (selectedUserEmail == "librarian@university.edu" && role != "LIBRARIAN") {
        statusLabel->setText("Default librarian role cannot be changed.");
        return;
    }
    if (!confirmAction("Confirm User Update",
                       "Update user \"" + selectedUserEmail + "\" with current form values?")) {
        statusLabel->setText("User update cancelled.");
        return;
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare("UPDATE users SET name = ?, role = ?, student_id = ? WHERE email = ?");
    query.addBindValue(userNameEdit->text().trimmed());
    query.addBindValue(role);
    query.addBindValue(userStudentIdEdit->text().trimmed());
    query.addBindValue(selectedUserEmail);
    statusLabel->setText(query.exec() && query.numRowsAffected() > 0 ? "User updated." : "Could not update user.");
    refreshAll();
}

void DashboardWindow::handleUserDelete()
{
    if (!canManageUsers() || selectedUserEmail.isEmpty()) {
        statusLabel->setText("Select a user first.");
        return;
    }
    if (selectedUserEmail == "librarian@university.edu") {
        statusLabel->setText("Default librarian cannot be deleted.");
        return;
    }
    if (!confirmAction("Confirm User Delete",
                       "Delete user \"" + selectedUserEmail + "\"? This cannot be undone.")) {
        statusLabel->setText("User delete cancelled.");
        return;
    }

    QSqlQuery query(DatabaseManager::database());
    query.prepare("DELETE FROM users WHERE email = ?");
    query.addBindValue(selectedUserEmail);
    statusLabel->setText(query.exec() && query.numRowsAffected() > 0 ? "User deleted." : "Could not delete user.");

    selectedUserEmail.clear();
    userNameEdit->clear();
    userEmailEdit->clear();
    userRoleEdit->clear();
    userStudentIdEdit->clear();
    refreshAll();
}

void DashboardWindow::handleLogout()
{
    auto *login = new LoginWindow();
    login->show();
    close();
}

void DashboardWindow::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    if (pages && pages->currentIndex() == BooksPage) {
        refreshBooks(bookSearchEdit ? bookSearchEdit->text() : QString());
    }
}

QString DashboardWindow::initialsForName(const QString &name) const
{
    const QStringList parts = name.split(' ', Qt::SkipEmptyParts);
    QString initials;
    for (const auto &part : parts) {
        initials += part.left(1).toUpper();
        if (initials.size() == 2) {
            break;
        }
    }
    return initials.isEmpty() ? "RB" : initials;
}

bool DashboardWindow::confirmAction(const QString &title, const QString &message)
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Question);
    box.setWindowTitle(title);
    box.setText(message);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setDefaultButton(QMessageBox::No);
    box.setStyleSheet(
        "QMessageBox { background: #fff8ea; }"
        "QLabel { color: #25221d; font-size: 13px; font-weight: 700; min-width: 320px; }"
        "QPushButton { background: #1f6f54; color: #fffaf0; border: none; border-radius: 8px; padding: 6px 16px; min-width: 76px; }"
        "QPushButton:hover { background: #14533f; }");
    return box.exec() == QMessageBox::Yes;
}

QString DashboardWindow::issueLimitText() const
{
    if (displayRole == "FACULTY") return "20";
    if (displayRole == "STUDENT" || displayRole == "STAFF") return "10";
    return "Full";
}

bool DashboardWindow::canManageBooks() const
{
    return displayRole == "STAFF" || displayRole == "LIBRARIAN";
}

bool DashboardWindow::canManageUsers() const
{
    return displayRole == "LIBRARIAN";
}

QString DashboardWindow::normalizeRole(const QString &role) const
{
    return role.trimmed().toUpper();
}

void DashboardWindow::applyTheme()
{
    setStyleSheet(R"(
        QWidget#DashboardWindow {
            background:
                qradialgradient(cx: 0.08, cy: 0.08, radius: 0.48,
                                fx: 0.08, fy: 0.08,
                                stop: 0 rgba(224, 163, 35, 70),
                                stop: 1 rgba(224, 163, 35, 0)),
                qradialgradient(cx: 0.9, cy: 0.12, radius: 0.5,
                                fx: 0.9, fy: 0.12,
                                stop: 0 rgba(31, 111, 84, 54),
                                stop: 1 rgba(31, 111, 84, 0)),
                qlineargradient(x1:0, y1:0, x2:1, y2:1,
                                stop:0 #fff8e9,
                                stop:0.48 #f5e6c8,
                                stop:1 #e9cc98);
            color: #27231e;
            font-family: 'Trebuchet MS';
        }
        QFrame#sidebar {
            background: rgba(255, 249, 236, 205);
            border-right: 1px solid rgba(30, 27, 22, 24);
        }
        QFrame#mainArea {
            background: rgba(255, 252, 246, 145);
        }
        QFrame#brandCard, QFrame#panelCard, QFrame#bookCard, QFrame#bookCardFeatured, QFrame#statCard, QFrame#heroCard {
            background: rgba(255, 250, 239, 220);
            border: 1px solid rgba(33, 31, 26, 22);
            border-radius: 14px;
        }
        QFrame#bookCardFeatured {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #124734, stop:1 #2f6d53);
            border: 1px solid rgba(20, 71, 53, 170);
        }
        QFrame#heroCard {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fff4df, stop:1 #f2e0be);
            border: 1px solid rgba(30, 27, 22, 28);
        }
        QLabel#logoLabel {
            background: #0f3f31;
            color: #fffaf0;
            border-radius: 10px;
            font-size: 12px;
            font-weight: 800;
        }
        QLabel#brandLabel {
            color: #211e19;
            font-family: Georgia;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#pageTitleLabel, QLabel#sectionTitle {
            color: #25221d;
            font-family: Georgia;
            font-size: 24px;
            font-weight: 700;
        }
        QLabel#profileName {
            color: #25221d;
            font-family: Georgia;
            font-size: 24px;
            font-weight: 700;
        }
        QLabel#heroTitle {
            color: #1f1b16;
            font-family: Georgia;
            font-size: 23px;
            font-weight: 700;
        }
        QLabel#heroSubtitle, QLabel#profileMeta, QLabel#bookMeta, QLabel#bookMetaFeatured, QLabel#statLabel {
            color: #6c6458;
            font-size: 13px;
            font-weight: 700;
        }
        QLabel#bookMetaFeatured {
            color: #e4ede7;
        }
        QLabel#bookTitle, QLabel#bookTitleFeatured {
            color: #23201b;
            font-family: Georgia;
            font-size: 18px;
            font-weight: 700;
        }
        QLabel#bookTitleFeatured {
            color: #fff9ee;
        }
        QLabel#statValue {
            color: #25231f;
            font-family: Georgia;
            font-size: 30px;
            font-weight: 700;
        }
        QLabel#roleTag {
            background: rgba(238, 227, 204, 220);
            color: #524b42;
            border-radius: 10px;
            padding: 8px 10px;
            font-size: 12px;
            font-weight: 800;
        }
        QLabel#avatarLabel {
            background: #d7e5dd;
            color: #0f3f31;
            border-radius: 12px;
            font-size: 22px;
            font-weight: 800;
        }
        QLabel#statusLabel {
            color: #9b5138;
            font-size: 13px;
            font-weight: 700;
        }
        QPushButton#activeNavButton, QPushButton#navButton {
            border: none;
            border-radius: 10px;
            padding: 9px 12px;
            text-align: left;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton#activeNavButton {
            background: #0f3f31;
            color: #fffaf0;
        }
        QPushButton#navButton {
            background: transparent;
            color: #5c554b;
        }
        QPushButton#navButton:hover {
            background: rgba(236, 223, 198, 180);
            color: #0f3f31;
        }
        QLineEdit#searchEdit, QLineEdit {
            background: rgba(255, 249, 237, 220);
            border: 1px solid rgba(30, 27, 22, 24);
            border-radius: 10px;
            padding: 9px 12px;
            color: #25231f;
            font-size: 14px;
        }
        QLineEdit:focus {
            border: 1px solid #1f6f54;
        }
        QPushButton#primaryButton, QPushButton#secondaryButton, QPushButton#ghostButton, QPushButton#dangerButton, QPushButton#issueButton {
            border: none;
            border-radius: 10px;
            padding: 8px 14px;
            font-size: 13px;
            font-weight: 800;
        }
        QPushButton#primaryButton {
            background: #1f6f54;
            color: #fffaf0;
        }
        QPushButton#issueButton {
            background: #0f3f31;
            color: #fffaf0;
            min-width: 90px;
            min-height: 34px;
        }
        QPushButton#issueButton:hover {
            background: #1f6f54;
        }
        QPushButton#issueButton:disabled {
            background: rgba(210, 205, 195, 200);
            color: #8a8278;
        }
        QPushButton#secondaryButton {
            background: rgba(236, 224, 201, 225);
            color: #0f3f31;
        }
        QPushButton#ghostButton {
            background: rgba(236, 227, 211, 210);
            color: #6a6258;
        }
        QPushButton#dangerButton {
            background: #efdccf;
            color: #9b5138;
        }
        QPushButton#primaryButton:hover {
            background: #14533f;
        }
        QPushButton#secondaryButton:hover {
            background: rgba(210, 198, 175, 225);
        }
        QScrollArea#plainScroll {
            border: none;
            background: transparent;
        }
        QScrollArea#plainScroll > QWidget > QWidget {
            background: transparent;
        }
    )");
}