#include "Book.h"

Book::Book(string bookId, string title, string author, bool available)
    : bookId(bookId), title(title), author(author), available(available) {}

const string& Book::getBookId() const { return bookId; }
const string& Book::getTitle() const { return title; }
const string& Book::getAuthor() const { return author; }
bool Book::isAvailable() const { return available; }
void Book::setAvailable(bool val) { available = val; }