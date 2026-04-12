#include "StorageManager.h"
#include <fstream>
#include <sstream>
using namespace std;

void StorageManager::saveBooks(const vector<Book>& books) {
    ofstream file("books.txt");
    for (auto& b : books) {
        file << b.getBookId() << ","
             << b.getTitle() << ","
             << b.getAuthor() << ","
             << (b.isAvailable() ? "1" : "0") << "\n";
    }
    file.close();
}

vector<Book> StorageManager::loadBooks() {
    vector<Book> books;
    ifstream file("books.txt");
    if (!file) return books;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, author, avail;
        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, author, ',');
        getline(ss, avail);
        books.emplace_back(id, title, author, avail == "1");
    }
    file.close();
    return books;
}