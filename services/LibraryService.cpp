#include "LibraryService.h"
using namespace std;

LibraryService::LibraryService() {
    books = storage.loadBooks();
}

string LibraryService::addBook(const string& userRole, const string& userName,
                               const string& bookId, const string& title,
                               const string& author) {
    if (!PermissionEngine::checkAccess(userRole, "ADD_BOOK")) {
        AuditLogger::getInstance().log(userName, "ADD_BOOK", "DENIED");
        return "Access denied";
    }
    for (auto& b : books)
        if (b.getBookId() == bookId) return "Book ID already exists";

    books.emplace_back(bookId, title, author, true);
    storage.saveBooks(books);
    AuditLogger::getInstance().log(userName, "ADD_BOOK", "SUCCESS");
    return "SUCCESS";
}

string LibraryService::issueBook(const string& userRole, const string& userName,
                                 const string& bookId) {
    if (!PermissionEngine::checkAccess(userRole, "ISSUE_BOOK")) {
        AuditLogger::getInstance().log(userName, "ISSUE_BOOK", "DENIED");
        return "Access denied";
    }
    for (auto& b : books) {
        if (b.getBookId() == bookId) {
            if (!b.isAvailable()) return "Book not available";
            b.setAvailable(false);
            storage.saveBooks(books);
            AuditLogger::getInstance().log(userName, "ISSUE_BOOK", "SUCCESS");
            return "SUCCESS";
        }
    }
    return "Book not found";
}

string LibraryService::returnBook(const string& userRole, const string& userName,
                                  const string& bookId) {
    if (!PermissionEngine::checkAccess(userRole, "RETURN_BOOK")) {
        AuditLogger::getInstance().log(userName, "RETURN_BOOK", "DENIED");
        return "Access denied";
    }
    for (auto& b : books) {
        if (b.getBookId() == bookId) {
            b.setAvailable(true);
            storage.saveBooks(books);
            AuditLogger::getInstance().log(userName, "RETURN_BOOK", "SUCCESS");
            return "SUCCESS";
        }
    }
    return "Book not found";
}

vector<Book> LibraryService::searchBook(const string& keyword) {
    vector<Book> results;
    for (auto& b : books) {
        if (b.getTitle().find(keyword) != string::npos ||
            b.getAuthor().find(keyword) != string::npos ||
            b.getBookId().find(keyword) != string::npos) {
            results.push_back(b);
        }
    }
    return results;
}

vector<Book> LibraryService::getAllBooks() {
    return books;
}