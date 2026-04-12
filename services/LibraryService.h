#ifndef LIBRARYSERVICE_H
#define LIBRARYSERVICE_H
#include "../models/Book.h"
#include "StorageManager.h"
#include "AuditLogger.h"
#include "PermissionStub.h"
#include <vector>
#include <string>
using namespace std;

class LibraryService {
private:
    vector<Book> books;
    StorageManager storage;

public:
    LibraryService();

    string addBook(const string& userRole, const string& userName,
                   const string& bookId, const string& title, const string& author);

    string issueBook(const string& userRole, const string& userName,
                     const string& bookId);

    string returnBook(const string& userRole, const string& userName,
                      const string& bookId);

    vector<Book> searchBook(const string& keyword);
    vector<Book> getAllBooks();
};
#endif