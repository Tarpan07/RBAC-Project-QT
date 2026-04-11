#include "AuthService.h"

bool AuthService::login(string email, string password) {
    return authManager.login(email, password);
}

// 🔥 FIXED VERSION
string AuthService::registerUser(string name, string email, string password,
                                 string role, string studentID) {
    return authManager.registerUser(name, email, password, role, studentID);
}

void AuthService::logout() {
    authManager.logout();
}