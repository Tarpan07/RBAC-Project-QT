#include "Book.h"

#include <algorithm>

using std::max;

Book::Book(string bookId, string title, string author, bool available, string coverPath)
    : bookId(bookId),
      title(title),
      author(author),
      coverPath(coverPath),
      available(available),
      totalCount(1),
      availableCount(available ? 1 : 0) {}

Book::Book(string bookId, string title, string author, int totalCount, int availableCount, string coverPath)
    : bookId(bookId),
      title(title),
      author(author),
      coverPath(coverPath),
      available(availableCount > 0),
      totalCount(totalCount),
      availableCount(availableCount) {}

const string& Book::getBookId() const { return bookId; }
const string& Book::getTitle() const { return title; }
const string& Book::getAuthor() const { return author; }
const string& Book::getCoverPath() const { return coverPath; }
void Book::setCoverPath(const string &path) { coverPath = path; }
bool Book::isAvailable() const { return available; }
void Book::setAvailable(bool val) {
    available = val;
    availableCount = val ? max(1, availableCount) : 0;
}
int Book::getTotalCount() const { return totalCount; }
int Book::getAvailableCount() const { return availableCount; }
void Book::setCounts(int total, int availableCopies) {
    totalCount = total;
    availableCount = availableCopies;
    available = availableCopies > 0;
}
