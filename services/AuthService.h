#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <string>
#include "AuthManager.h"
#include "../models/User.h"

using namespace std;

class AuthService {
private:
    AuthManager authManager;

public:
    bool login(string email, string password);
    string registerUser(string name, string email, string password,
                        string role, string studentID);
    void logout();

    // ✅ NEW - needed by LoginWindow
    User* getCurrentUser() {
        return authManager.getCurrentUser();
    }
};

#endif