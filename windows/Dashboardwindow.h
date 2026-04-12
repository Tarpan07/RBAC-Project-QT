#ifndef DASHBOARDWINDOW_H
#define DASHBOARDWINDOW_H

#include <QDialog>
#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <string>

// Include your LibraryService header here so the dashboard knows about it
// (Adjust the path if your structure is different)
#include "../services/libraryservice.h"

namespace Ui {
class DashboardWindow;
}

class DashboardWindow : public QDialog
{
    Q_OBJECT

public:
    // 1. Updated constructor to accept the two QStrings
    explicit DashboardWindow(QString userName, QString userRole, QWidget *parent = nullptr);
    ~DashboardWindow();

private slots:
    // 2. Added declarations for all the slots (button clicks) used in your .cpp
    void refreshBookTable();
    void handleAddBook();
    void handleIssueBook();
    void handleReturnBook();
    void handleSearch();
    void handleLogout();

private:
    Ui::DashboardWindow *ui;

    // 3. Added the member variables that your .cpp file is trying to use
    std::string userName;
    std::string userRole;
    LibraryService libraryService;
};

#endif // DASHBOARDWINDOW_H