#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

bool DatabaseManager::initialized = false;

QSqlDatabase DatabaseManager::database()
{
    initialize();
    return QSqlDatabase::database("library_connection");
}

QString DatabaseManager::databasePath()
{
    return QDir(QCoreApplication::applicationDirPath()).filePath("library.db");
}

void DatabaseManager::initialize()
{
    if (initialized) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::contains("library_connection")
                          ? QSqlDatabase::database("library_connection")
                          : QSqlDatabase::addDatabase("QSQLITE", "library_connection");
    db.setDatabaseName(databasePath());

    if (!db.open()) {
        qWarning() << "Unable to open SQLite database:" << db.lastError().text();
        return;
    }

    createTables();
    seedBooks();
    initialized = true;
}

void DatabaseManager::createTables()
{
    QSqlDatabase db = QSqlDatabase::database("library_connection");
    QSqlQuery query(db);

    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "email TEXT PRIMARY KEY,"
               "name TEXT NOT NULL,"
               "role TEXT NOT NULL,"
               "student_id TEXT,"
               "password_hash TEXT NOT NULL)");

    query.exec("CREATE TABLE IF NOT EXISTS books ("
               "book_id TEXT PRIMARY KEY,"
               "title TEXT NOT NULL,"
               "author TEXT NOT NULL,"
               "cover_path TEXT NOT NULL DEFAULT '',"
               "total_count INTEGER NOT NULL DEFAULT 1,"
               "available_count INTEGER NOT NULL DEFAULT 1)");

    bool hasCoverPath = false;
    QSqlQuery columns(db);
    if (columns.exec("PRAGMA table_info(books)")) {
        while (columns.next()) {
            if (columns.value(1).toString() == "cover_path") {
                hasCoverPath = true;
                break;
            }
        }
    }
    if (!hasCoverPath) {
        query.exec("ALTER TABLE books ADD COLUMN cover_path TEXT NOT NULL DEFAULT ''");
    }

    query.exec("CREATE TABLE IF NOT EXISTS borrow_records ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "user_email TEXT NOT NULL,"
               "book_id TEXT NOT NULL,"
               "issue_date TEXT NOT NULL,"
               "return_date TEXT,"
               "status TEXT NOT NULL DEFAULT 'ISSUED',"
               "FOREIGN KEY(user_email) REFERENCES users(email),"
               "FOREIGN KEY(book_id) REFERENCES books(book_id))");

    query.exec("DELETE FROM borrow_records "
               "WHERE status = 'ISSUED' AND id NOT IN ("
               "SELECT MAX(id) FROM borrow_records WHERE status = 'ISSUED' GROUP BY user_email, book_id)");

    query.exec("CREATE UNIQUE INDEX IF NOT EXISTS idx_active_issue_per_user_book "
               "ON borrow_records(user_email, book_id) WHERE status = 'ISSUED'");
}

void DatabaseManager::seedBooks()
{
    QSqlDatabase db = QSqlDatabase::database("library_connection");
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM books") || !countQuery.next()) {
        return;
    }
    const int currentCount = countQuery.value(0).toInt();

    bool reseed = currentCount == 0 || currentCount < 30;
    if (!reseed) {
        QSqlQuery legacyCheck(db);
        legacyCheck.exec(
            "SELECT COUNT(*) FROM books WHERE "
            "title IN ('Data Structures & Algorithms','Clean Code','Compiler Design','Cloud Computing')");
        if (legacyCheck.next() && legacyCheck.value(0).toInt() > 0) {
            reseed = true;
        }
    }
    if (!reseed) {
        return;
    }

    db.transaction();
    QSqlQuery clearBorrow(db);
    clearBorrow.exec("DELETE FROM borrow_records");
    QSqlQuery clearBooks(db);
    clearBooks.exec("DELETE FROM books");

    struct SeedBook {
        const char *id;
        const char *title;
        const char *author;
        const char *coverPath;
        int total;
        int available;
    };

    const SeedBook books[] = {
        {"B001", "The Midnight Library", "Matt Haig", "", 7, 6},
        {"B002", "Atomic Habits", "James Clear", "", 9, 8},
        {"B003", "The Alchemist", "Paulo Coelho", "", 6, 5},
        {"B004", "Sapiens", "Yuval Noah Harari", "", 8, 6},
        {"B005", "The Silent Patient", "Alex Michaelides", "", 5, 4},
        {"B006", "Educated", "Tara Westover", "", 6, 4},
        {"B007", "A Brief History of Time", "Stephen Hawking", "", 5, 4},
        {"B008", "To Kill a Mockingbird", "Harper Lee", "", 7, 6},
        {"B009", "Pride and Prejudice", "Jane Austen", "", 6, 5},
        {"B010", "1984", "George Orwell", "", 8, 7},
        {"B011", "The Martian", "Andy Weir", "", 6, 4},
        {"B012", "Dune", "Frank Herbert", "", 5, 3},
        {"B013", "The Hobbit", "J. R. R. Tolkien", "", 7, 6},
        {"B014", "Thinking Fast and Slow", "Daniel Kahneman", "", 6, 4},
        {"B015", "The Psychology of Money", "Morgan Housel", "", 8, 7},
        {"B016", "The Lean Startup", "Eric Ries", "", 5, 4},
        {"B017", "Deep Work", "Cal Newport", "", 6, 5},
        {"B018", "The Book Thief", "Markus Zusak", "", 5, 4},
        {"B019", "The Name of the Wind", "Patrick Rothfuss", "", 4, 3},
        {"B020", "Kafka on the Shore", "Haruki Murakami", "", 4, 3},
        {"B021", "The Kite Runner", "Khaled Hosseini", "", 7, 6},
        {"B022", "Man's Search for Meaning", "Viktor Frankl", "", 6, 5},
        {"B023", "The Great Gatsby", "F. Scott Fitzgerald", "", 6, 5},
        {"B024", "The Catcher in the Rye", "J. D. Salinger", "", 5, 4},
        {"B025", "The Girl with the Dragon Tattoo", "Stieg Larsson", "", 5, 3},
        {"B026", "The Da Vinci Code", "Dan Brown", "", 7, 6},
        {"B027", "The Road", "Cormac McCarthy", "", 4, 3},
        {"B028", "Murder on the Orient Express", "Agatha Christie", "", 6, 5},
        {"B029", "The Immortal Life of Henrietta Lacks", "Rebecca Skloot", "", 5, 4},
        {"B030", "The Art of War", "Sun Tzu", "", 8, 7}
    };

    QSqlQuery insert(db);
    insert.prepare("INSERT INTO books (book_id, title, author, cover_path, total_count, available_count) "
                   "VALUES (?, ?, ?, ?, ?, ?)");

    for (const auto &book : books) {
        insert.addBindValue(book.id);
        insert.addBindValue(book.title);
        insert.addBindValue(book.author);
        insert.addBindValue(book.coverPath);
        insert.addBindValue(book.total);
        insert.addBindValue(book.available);
        if (!insert.exec()) {
            qWarning() << "Unable to seed book:" << book.id << insert.lastError().text();
        }
    }
    db.commit();
}
