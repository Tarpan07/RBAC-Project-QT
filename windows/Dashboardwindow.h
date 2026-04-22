#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>
#include <map>
#include <string>

#include "../models/Book.h"
#include "../services/LibraryService.h"

class DashboardWindow : public QDialog
{
public:
    explicit DashboardWindow(QString userName,
                             QString userRole,
                             QString userEmail = QString(),
                             QString studentId = QString(),
                             QWidget *parent = nullptr);
    ~DashboardWindow();

private:
    enum PageIndex {
        DashboardPage = 0,
        BooksPage,
        MyBooksPage,
        ProfilePage,
        UsersPage
    };

    void buildUi();
    void applyTheme();
    void navigate(PageIndex page);
    void refreshAll();
    void refreshDashboard();
    void refreshBooks(const QString &filter = QString());
    void refreshMyBooks();
    void refreshUsers(const QString &filter = QString());
    void refreshProfile();

    QWidget *buildDashboardPage();
    QWidget *buildBooksPage();
    QWidget *buildMyBooksPage();
    QWidget *buildProfilePage();
    QWidget *buildUsersPage();
    QWidget *createBookCard(const Book &book, bool featured = false, bool alreadyIssued = false);
    QWidget *createBorrowedCard(const Book &book);
    QWidget *createBorrowRecordCard(const BorrowRecord &record, bool showUser = false);
    QWidget *createUserCard(const QString &name, const QString &email, const QString &role, const QString &studentId);
    QWidget *createStatCard(const QString &value, const QString &label);
    QPushButton *createNavButton(const QString &text, PageIndex page);

    void handleLogout();
    void handleBookSave();
    void handleBookDelete();
    void handleUserUpdate();
    void handleUserDelete();

    void resizeEvent(QResizeEvent *event) override;

    QString initialsForName(const QString &name) const;
    QString issueLimitText() const;
    bool canManageBooks() const;
    bool canManageUsers() const;
    bool confirmAction(const QString &title, const QString &message);
    QString normalizeRole(const QString &role) const;
    QString formatRecordTimeline(const BorrowRecord &record) const;

    std::string userName;
    std::string userRole;
    std::string userEmail;
    QString displayName;
    QString displayRole;
    QString displayEmail;
    QString displayStudentId;
    LibraryService libraryService;

    QStackedWidget *pages = nullptr;
    QLabel *pageTitleLabel = nullptr;
    QLabel *statusLabel = nullptr;

    QScrollArea *dashboardScroll = nullptr;
    QWidget    *dashboardContainer = nullptr;
    QLabel *profileIssueCountLabel = nullptr;
    QLabel *profileIssueLimitLabel = nullptr;

    QLineEdit *bookSearchEdit = nullptr;
    QGridLayout *booksGrid = nullptr;
    QLineEdit *bookIdEdit = nullptr;
    QLineEdit *bookTitleEdit = nullptr;
    QLineEdit *bookAuthorEdit = nullptr;
    QLineEdit *bookCountEdit = nullptr;
    QLineEdit *bookAvailableEdit = nullptr;
    QScrollArea *booksScrollArea = nullptr;

    QVBoxLayout *activeBooksLayout = nullptr;
    QVBoxLayout *historyLayout = nullptr;

    QLabel *profileNameLabel = nullptr;
    QLabel *profileRoleLabel = nullptr;
    QLabel *profileEmailLabel = nullptr;
    QLabel *profileIdLabel = nullptr;

    QLineEdit *userSearchEdit = nullptr;
    QVBoxLayout *usersLayout = nullptr;
    QLineEdit *userNameEdit = nullptr;
    QLineEdit *userEmailEdit = nullptr;
    QLineEdit *userRoleEdit = nullptr;
    QLineEdit *userStudentIdEdit = nullptr;
    QLabel *selectedUserHistoryTitle = nullptr;
    QVBoxLayout *userHistoryLayout = nullptr;
    QString selectedUserEmail;
    QString selectedUserName;

    std::map<PageIndex, QPushButton *> navButtons;
};

#endif
