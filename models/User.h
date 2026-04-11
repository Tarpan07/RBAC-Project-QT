#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

class User {

private:
    string name;
    string email;
    string role;
    string studentID;
    string passwordHash;

public:
    User(string name, string email, string role, string studentID, string passwordHash);

    const string& getName() const;
    const string& getEmail() const;
    const string& getRole() const;
    const string& getStudentID() const;
    const string& getPasswordHash() const;
};

#endif