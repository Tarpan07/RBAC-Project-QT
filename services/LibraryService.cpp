#include "LibraryService.h"
#include "DatabaseManager.h"

#include <QDate>
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

string upperRole(string role)
{
    transform(role.begin(), role.end(), role.begin(), ::toupper);
    return role;
}

vector<BorrowRecord> loadBorrowRecords(const string &userName, bool activeOnly)
{
    vector<BorrowRecord> records;
    QSqlQuery query(DatabaseManager::database());

    QString sql =
        "SELECT r.user_email, COALESCE(u.name, r.user_email), r.book_id, "
        "COALESCE(b.title, r.book_id), COALESCE(b.author, ''), "
        "r.issue_date, COALESCE(r.return_date, ''), r.status "
        "FROM borrow_records r "
        "LEFT JOIN books b ON b.book_id = r.book_id "
        "LEFT JOIN users u ON u.email = r.user_email ";
    if (!userName.empty() || activeOnly) {
        sql += "WHERE ";
    }
    if (!userName.empty()) {
        sql += "r.user_email = ?";
    }
    if (!userName.empty() && activeOnly) {
        sql += " AND ";
    }
    if (activeOnly) {
        sql += "r.status = 'ISSUED'";
    }
    sql += " ORDER BY r.id DESC";

    query.prepare(sql);
    if (!userName.empty()) {
        query.addBindValue(qstr(userName));
    }

    if (!query.exec()) {
        return records;
    }

    while (query.next()) {
        BorrowRecord record;
        record.userEmail = query.value(0).toString().toStdString();
        record.userName = query.value(1).toString().toStdString();
        record.bookId = query.value(2).toString().toStdString();
        record.title = query.value(3).toString().toStdString();
        record.author = query.value(4).toString().toStdString();
        record.issueDate = query.value(5).toString().toStdString();
        record.returnDate = query.value(6).toString().toStdString();
        record.status = query.value(7).toString().toStdString();
        records.push_back(record);
    }

    return records;
}
}

LibraryService::LibraryService()
{
    books = storage.loadBooks();
}

string LibraryService::addBook(const string& userRole,
                               const string& userName,
                               const string& bookId,
                               const string& title,
                               const string& author,
                               int totalCount,
                               const string &coverPath)
{
    if (!PermissionEngine::checkAccess(upperRole(userRole), "ADD_BOOK")) {
        AuditLogger::getInstance().log(userName, "ADD_BOOK", "DENIED");
        return "Access denied";
    }

    if (bookId.empty() || title.empty() || author.empty() || totalCount <= 0) {
        return "Enter book id, title, author, and a valid count";
    }

    for (auto& b : storage.loadBooks()) {
        if (b.getBookId() == bookId) {
            return "Book ID already exists";
        }
    }

    if (!storage.addBook(Book(bookId, title, author, totalCount, totalCount, coverPath))) {
        return "Could not add book";
    }

    books = storage.loadBooks();
    AuditLogger::getInstance().log(userName, "ADD_BOOK", "SUCCESS");
    return "SUCCESS";
}

string LibraryService::updateBook(const string& userRole,
                                  const string& userName,
                                  const string& bookId,
                                  const string& title,
                                  const string& author,
                                  int totalCount,
                                  int availableCount,
                                  const string &coverPath)
{
    if (!PermissionEngine::checkAccess(upperRole(userRole), "UPDATE_BOOK")) {
        AuditLogger::getInstance().log(userName, "UPDATE_BOOK", "DENIED");
        return "Access denied";
    }

    if (bookId.empty() || title.empty() || author.empty() || totalCount < 0 || availableCount < 0 ||
        availableCount > totalCount) {
        return "Enter valid book details";
    }

    if (!storage.updateBook(bookId, title, author, totalCount, availableCount, coverPath)) {
        return "Could not update book";
    }

    books = storage.loadBooks();
    AuditLogger::getInstance().log(userName, "UPDATE_BOOK", "SUCCESS");
    return "SUCCESS";
}

string LibraryService::deleteBook(const string& userRole, const string& userName, const string& bookId)
{
    if (!PermissionEngine::checkAccess(upperRole(userRole), "DELETE_BOOK")) {
        AuditLogger::getInstance().log(userName, "DELETE_BOOK", "DENIED");
        return "Access denied";
    }

    if (!storage.deleteBook(bookId)) {
        return "Could not delete book";
    }

    books = storage.loadBooks();
    AuditLogger::getInstance().log(userName, "DELETE_BOOK", "SUCCESS");
    return "SUCCESS";
}

string LibraryService::issueBook(const string& userRole, const string& userName, const string& bookId)
{
    const string role = upperRole(userRole);
    if (!PermissionEngine::checkAccess(role, "ISSUE_BOOK")) {
        AuditLogger::getInstance().log(userName, "ISSUE_BOOK", "DENIED");
        return "Access denied";
    }

    QSqlQuery duplicateCheck(DatabaseManager::database());
    duplicateCheck.prepare("SELECT COUNT(*) FROM borrow_records WHERE user_email = ? AND book_id = ? "
                           "AND status = 'ISSUED'");
    duplicateCheck.addBindValue(qstr(userName));
    duplicateCheck.addBindValue(qstr(bookId));
    if (duplicateCheck.exec() && duplicateCheck.next() && duplicateCheck.value(0).toInt() > 0) {
        return "You already issued this book";
    }

    if (activeIssueCount(userName) >= RoleManager::issueLimit(role)) {
        return "Issue limit reached";
    }

    Book selected("", "", "", false);
    bool found = false;
    for (auto &book : storage.loadBooks()) {
        if (book.getBookId() == bookId) {
            selected = book;
            found = true;
            break;
        }
    }

    if (!found) {
        return "Book not found";
    }
    if (selected.getAvailableCount() <= 0) {
        return "Book not available";
    }

    QSqlDatabase db = DatabaseManager::database();
    db.transaction();

    QSqlQuery borrow(db);
    borrow.prepare("INSERT INTO borrow_records (user_email, book_id, issue_date, status) VALUES (?, ?, ?, 'ISSUED')");
    borrow.addBindValue(qstr(userName));
    borrow.addBindValue(qstr(bookId));
    borrow.addBindValue(QDate::currentDate().toString(Qt::ISODate));

    if (!borrow.exec() || !storage.changeAvailableCount(bookId, -1)) {
        db.rollback();
        return "Could not issue book";
    }

    db.commit();
    books = storage.loadBooks();
    AuditLogger::getInstance().log(userName, "ISSUE_BOOK", "SUCCESS");
    return "SUCCESS";
}

string LibraryService::returnBook(const string& userRole, const string& userName, const string& bookId)
{
    if (!PermissionEngine::checkAccess(upperRole(userRole), "RETURN_BOOK")) {
        AuditLogger::getInstance().log(userName, "RETURN_BOOK", "DENIED");
        return "Access denied";
    }

    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery find(db);
    find.prepare("SELECT id FROM borrow_records WHERE user_email = ? AND book_id = ? AND status = 'ISSUED' "
                 "ORDER BY id DESC LIMIT 1");
    find.addBindValue(qstr(userName));
    find.addBindValue(qstr(bookId));

    if (!find.exec() || !find.next()) {
        return "No active issue found";
    }

    const int recordId = find.value(0).toInt();
    db.transaction();

    QSqlQuery ret(db);
    ret.prepare("UPDATE borrow_records SET status = 'RETURNED', return_date = ? WHERE id = ?");
    ret.addBindValue(QDate::currentDate().toString(Qt::ISODate));
    ret.addBindValue(recordId);

    if (!ret.exec() || !storage.changeAvailableCount(bookId, 1)) {
        db.rollback();
        return "Could not return book";
    }

    db.commit();
    books = storage.loadBooks();
    AuditLogger::getInstance().log(userName, "RETURN_BOOK", "SUCCESS");
    return "SUCCESS";
}

vector<Book> LibraryService::searchBook(const string& keyword)
{
    vector<Book> results;
    const string needle = keyword;

    for (auto& b : storage.loadBooks()) {
        if (needle.empty() ||
            b.getTitle().find(needle) != string::npos ||
            b.getAuthor().find(needle) != string::npos ||
            b.getBookId().find(needle) != string::npos) {
            results.push_back(b);
        }
    }

    return results;
}

vector<Book> LibraryService::getAllBooks()
{
    return storage.loadBooks();
}

vector<Book> LibraryService::getBorrowedBooks(const string &userName)
{
    vector<Book> borrowed;
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT b.book_id, b.title, b.author, b.cover_path, b.total_count, b.available_count "
                  "FROM borrow_records r "
                  "JOIN books b ON b.book_id = r.book_id "
                  "WHERE r.user_email = ? AND r.status = 'ISSUED' "
                  "ORDER BY r.issue_date DESC");
    query.addBindValue(qstr(userName));

    if (!query.exec()) {
        return borrowed;
    }

    while (query.next()) {
        borrowed.emplace_back(query.value(0).toString().toStdString(),
                              query.value(1).toString().toStdString(),
                              query.value(2).toString().toStdString(),
                              query.value(4).toInt(),
                              query.value(5).toInt(),
                              query.value(3).toString().toStdString());
    }

    return borrowed;
}

vector<BorrowRecord> LibraryService::getActiveBorrowRecords(const string &userName)
{
    return loadBorrowRecords(userName, true);
}

vector<BorrowRecord> LibraryService::getBorrowHistory(const string &userName)
{
    return loadBorrowRecords(userName, false);
}

int LibraryService::activeIssueCount(const string &userName)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("SELECT COUNT(*) FROM borrow_records WHERE user_email = ? AND status = 'ISSUED'");
    query.addBindValue(qstr(userName));
    if (!query.exec() || !query.next()) {
        return 0;
    }
    return query.value(0).toInt();
}
