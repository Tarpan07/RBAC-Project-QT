#include "AuthManager.h"
#include "DatabaseManager.h"
#include "PasswordUtil.h"

#include <QSqlQuery>
#include <QVariant>
#include <algorithm>
#include <cctype>

using namespace std;

namespace {
QString qstr(const string &value)
{
    return QString::fromStdString(value);
}

string normalizeRole(string role)
{
    transform(role.begin(), role.end(), role.begin(), ::toupper);
    return role;
}
}

AuthManager::AuthManager()
{
    currentUser = nullptr;
    librarianEmails = {"librarian@university.edu"};
    staffEmails = {"staff@university.edu"};

    DatabaseManager::database();
    loadUsers();
    seedDebugUsers();
}

void AuthManager::saveUsers()
{
    QSqlQuery query(DatabaseManager::database());
    query.exec("DELETE FROM users");

    QSqlQuery insert(DatabaseManager::database());
    insert.prepare("INSERT INTO users (email, name, role, student_id, password_hash) VALUES (?, ?, ?, ?, ?)");

    for (auto &u : users) {
        insert.addBindValue(qstr(u.getEmail()));
        insert.addBindValue(qstr(u.getName()));
        insert.addBindValue(qstr(u.getRole()));
        insert.addBindValue(qstr(u.getStudentID()));
        insert.addBindValue(qstr(u.getPasswordHash()));
        insert.exec();
    }
}

void AuthManager::loadUsers()
{
    users.clear();

    QSqlQuery query(DatabaseManager::database());
    query.exec("SELECT name, email, role, student_id, password_hash FROM users ORDER BY name");

    while (query.next()) {
        users.push_back(User(query.value(0).toString().toStdString(),
                             query.value(1).toString().toStdString(),
                             query.value(2).toString().toStdString(),
                             query.value(3).toString().toStdString(),
                             query.value(4).toString().toStdString()));
    }
}

bool AuthManager::emailExists(const string &email) const
{
    for (const auto &u : users) {
        if (u.getEmail() == email) {
            return true;
        }
    }
    return false;
}

void AuthManager::seedDebugUsers()
{
    bool changed = false;
    const string password = "Debug@123";

    auto addUser = [&](const string &name,
                       const string &email,
                       const string &role,
                       const string &studentID) {
        if (emailExists(email)) {
            return;
        }

        users.push_back(User(name, email, role, studentID, PasswordUtil::hashPassword(password)));
        changed = true;
    };

    addUser("Debug Librarian", "librarian@university.edu", "LIBRARIAN", "NA");
    addUser("Debug Student", "student@nits.ac.in", "STUDENT", "2412001");
    addUser("Debug Staff", "staff@university.edu", "STAFF", "NA");
    addUser("Debug Faculty", "faculty@faculty.edu", "FACULTY", "NA");

    if (changed) {
        saveUsers();
        loadUsers();
    }
}

bool AuthManager::isValidStudentEmail(string email)
{
    string domain = "nits.ac.in";
    return email.size() >= domain.size() && email.substr(email.size() - domain.size()) == domain;
}

string AuthManager::registerUser(string name, string email, string password, string role, string studentID)
{
    role = normalizeRole(role);

    if (role == "LIBRARIAN") {
        return "Librarian accounts are created only by the system";
    }

    if (emailExists(email)) {
        return "Email already exists";
    }

    if (role == "STUDENT") {
        if (!isValidStudentEmail(email)) {
            return "Use institute email";
        }
        if (studentID.empty()) {
            return "Student ID is required";
        }
    } else if (role == "FACULTY") {
        if (email.find("@faculty.edu") == string::npos) {
            return "Invalid faculty email";
        }
        studentID = "NA";
    } else if (role == "STAFF") {
        bool authorized = false;
        for (string e : staffEmails) {
            if (e == email) {
                authorized = true;
                break;
            }
        }
        if (!authorized) {
            return "Unauthorized staff email";
        }
        studentID = "NA";
    }

    if (!PasswordUtil::validatePassword(password)) {
        return "Password must be 8+ chars, include upper, lower, digit, special";
    }

    users.push_back(User(name, email, role, studentID, PasswordUtil::hashPassword(password)));
    saveUsers();
    loadUsers();
    return "SUCCESS";
}

bool AuthManager::login(string email, string password)
{
    loadUsers();
    for (auto &u : users) {
        if (u.getEmail() == email && PasswordUtil::verifyPassword(password, u.getPasswordHash())) {
            currentUser = &u;
            currentToken = tokenManager.generateToken(u.getEmail());
            return true;
        }
    }

    return false;
}

void AuthManager::logout()
{
    tokenManager.invalidateToken(currentToken);
    currentUser = nullptr;
    currentToken = "";
}

bool AuthManager::isLoggedIn()
{
    return currentUser != nullptr;
}

User *AuthManager::getCurrentUser()
{
    return currentUser;
}

string AuthManager::getToken()
{
    return currentToken;
}
