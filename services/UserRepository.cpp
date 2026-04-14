#include "UserRepository.h"
#include "DatabaseManager.h"

#include <QSqlQuery>
#include <QVariant>

using namespace std;

namespace {
QString qstr(const string &value)
{
    return QString::fromStdString(value);
}
}

bool UserRepository::saveUser(const User& user)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("INSERT INTO users (email, name, role, student_id, password_hash) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(qstr(user.getEmail()));
    query.addBindValue(qstr(user.getName()));
    query.addBindValue(qstr(user.getRole()));
    query.addBindValue(qstr(user.getStudentID()));
    query.addBindValue(qstr(user.getPasswordHash()));
    return query.exec();
}

User* UserRepository::findByEmail(const string& email)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT name, email, role, student_id, password_hash FROM users WHERE email = ?");
    query.addBindValue(qstr(email));
    if (!query.exec() || !query.next()) {
        return nullptr;
    }

    return new User(query.value(0).toString().toStdString(),
                    query.value(1).toString().toStdString(),
                    query.value(2).toString().toStdString(),
                    query.value(3).toString().toStdString(),
                    query.value(4).toString().toStdString());
}

vector<User> UserRepository::getAllUsers()
{
    vector<User> users;
    QSqlQuery query(DatabaseManager::database());
    query.exec("SELECT name, email, role, student_id, password_hash FROM users ORDER BY name");

    while (query.next()) {
        users.emplace_back(query.value(0).toString().toStdString(),
                           query.value(1).toString().toStdString(),
                           query.value(2).toString().toStdString(),
                           query.value(3).toString().toStdString(),
                           query.value(4).toString().toStdString());
    }

    return users;
}

bool UserRepository::emailExists(const string& email)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT 1 FROM users WHERE email = ? LIMIT 1");
    query.addBindValue(qstr(email));
    return query.exec() && query.next();
}
