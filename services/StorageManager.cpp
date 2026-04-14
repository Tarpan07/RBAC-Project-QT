#include "StorageManager.h"
#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

using namespace std;

void StorageManager::saveBooks(const vector<Book>& books)
{
    QSqlDatabase db = DatabaseManager::database();
    QSqlQuery clear(db);
    clear.exec("DELETE FROM books");

    QSqlQuery insert(db);
    insert.prepare("INSERT INTO books (book_id, title, author, cover_path, total_count, available_count) "
                   "VALUES (?, ?, ?, ?, ?, ?)");

    for (const auto &b : books) {
        insert.addBindValue(QString::fromStdString(b.getBookId()));
        insert.addBindValue(QString::fromStdString(b.getTitle()));
        insert.addBindValue(QString::fromStdString(b.getAuthor()));
        insert.addBindValue(QString::fromStdString(b.getCoverPath()));
        insert.addBindValue(b.getTotalCount());
        insert.addBindValue(b.getAvailableCount());
        insert.exec();
    }
}

vector<Book> StorageManager::loadBooks()
{
    vector<Book> books;
    QSqlQuery query(DatabaseManager::database());
    query.exec("SELECT book_id, title, author, cover_path, total_count, available_count FROM books ORDER BY title");

    while (query.next()) {
        books.emplace_back(query.value(0).toString().toStdString(),
                           query.value(1).toString().toStdString(),
                           query.value(2).toString().toStdString(),
                           query.value(4).toInt(),
                           query.value(5).toInt(),
                           query.value(3).toString().toStdString());
    }

    return books;
}

bool StorageManager::addBook(const Book &book)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("INSERT INTO books (book_id, title, author, cover_path, total_count, available_count) "
                  "VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString::fromStdString(book.getBookId()));
    query.addBindValue(QString::fromStdString(book.getTitle()));
    query.addBindValue(QString::fromStdString(book.getAuthor()));
    query.addBindValue(QString::fromStdString(book.getCoverPath()));
    query.addBindValue(book.getTotalCount());
    query.addBindValue(book.getAvailableCount());
    return query.exec();
}

bool StorageManager::deleteBook(const string &bookId)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("DELETE FROM books WHERE book_id = ?");
    query.addBindValue(QString::fromStdString(bookId));
    return query.exec() && query.numRowsAffected() > 0;
}

bool StorageManager::updateBook(const string &bookId,
                                const string &title,
                                const string &author,
                                int totalCount,
                                int availableCount,
                                const string &coverPath)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("UPDATE books SET title = ?, author = ?, cover_path = ?, total_count = ?, available_count = ? "
                  "WHERE book_id = ?");
    query.addBindValue(QString::fromStdString(title));
    query.addBindValue(QString::fromStdString(author));
    query.addBindValue(QString::fromStdString(coverPath));
    query.addBindValue(totalCount);
    query.addBindValue(availableCount);
    query.addBindValue(QString::fromStdString(bookId));
    return query.exec() && query.numRowsAffected() > 0;
}

bool StorageManager::changeAvailableCount(const string &bookId, int delta)
{
    QSqlQuery query(DatabaseManager::database());
    query.prepare("UPDATE books SET available_count = available_count + ? WHERE book_id = ?");
    query.addBindValue(delta);
    query.addBindValue(QString::fromStdString(bookId));
    return query.exec();
}
