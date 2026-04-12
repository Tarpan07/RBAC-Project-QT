#ifndef BOOK_H
#define BOOK_H
#include <string>
using namespace std;

class Book {
private:
    string bookId;
    string title;
    string author;
    bool available;

public:
    Book(string bookId, string title, string author, bool available = true);

    const string& getBookId() const;
    const string& getTitle() const;
    const string& getAuthor() const;
    bool isAvailable() const;
    void setAvailable(bool val);
};
#endif