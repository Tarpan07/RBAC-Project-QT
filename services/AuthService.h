#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <string>
#include "AuthManager.h"

using namespace std;

class AuthService {
private:
    AuthManager authManager;

public:
    bool login(string email, string password);

    // 🔥 UPDATED → returns string (error message or SUCCESS)
    string registerUser(string name, string email, string password,
                        string role, string studentID);

    void logout();
};

#endif