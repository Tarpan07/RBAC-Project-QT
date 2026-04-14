#ifndef BOOK_H
#define BOOK_H
#include <string>
using namespace std;

class Book {
private:
    string bookId;
    string title;
    string author;
    string coverPath;
    bool available;
    int totalCount;
    int availableCount;

public:
    Book(string bookId, string title, string author, bool available = true, string coverPath = "");
    Book(string bookId, string title, string author, int totalCount, int availableCount, string coverPath = "");

    const string& getBookId() const;
    const string& getTitle() const;
    const string& getAuthor() const;
    const string& getCoverPath() const;
    void setCoverPath(const string &path);
    bool isAvailable() const;
    void setAvailable(bool val);
    int getTotalCount() const;
    int getAvailableCount() const;
    void setCounts(int total, int available);
};
#endif
