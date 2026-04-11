#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <vector>
#include <string>
#include "../models/User.h"
#include "TokenManager.h"

using namespace std;

class AuthManager
{
private:
    vector<User> users;
    User *currentUser;

    vector<string> librarianEmails;
    vector<string> staffEmails;

    TokenManager tokenManager;
    string currentToken;

    void saveUsers();
    void loadUsers();

    bool isValidStudentEmail(string email);

public:
    AuthManager();

    bool login(string email, string password);

    // 🔥 IMPORTANT CHANGE
    string registerUser(string name, string email, string password,
                        string role, string studentID);

    void logout();

    bool isLoggedIn();
    User *getCurrentUser();
    string getToken();
};

#endif