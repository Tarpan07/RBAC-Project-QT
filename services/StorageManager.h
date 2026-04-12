#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H
#include "../models/Book.h"
#include <vector>
using namespace std;

class StorageManager {
public:
    void saveBooks(const vector<Book>& books);
    vector<Book> loadBooks();
};
#endif