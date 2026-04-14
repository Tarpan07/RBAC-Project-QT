#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H
#include "../models/Book.h"
#include <vector>
using namespace std;

class StorageManager {
public:
    void saveBooks(const vector<Book>& books);
    vector<Book> loadBooks();
    bool addBook(const Book &book);
    bool deleteBook(const string &bookId);
    bool updateBook(const string &bookId,
                    const string &title,
                    const string &author,
                    int totalCount,
                    int availableCount,
                    const string &coverPath);
    bool changeAvailableCount(const string &bookId, int delta);
};
#endif
