#include "TokenManager.h"
#include <random>

using namespace std;

//  Generate random token
string generateRandomToken() {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    string token;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    for (int i = 0; i < 32; i++) {
        token += charset[dis(gen)];
    }

    return token;
}

//  Create session + token
string TokenManager::generateToken(const string& email) {
    string token = generateRandomToken();

    Session session;
    session.email = email;
    session.expiry = time(0) + 3600; // 1 hour expiry

    sessions[token] = session;

    return token;
}

// Validate token
bool TokenManager::validateToken(const string& token) {
    if (sessions.find(token) == sessions.end())
        return false;

    // check expiry
    if (sessions[token].expiry < time(0)) {
        sessions.erase(token);
        return false;
    }

    return true;
}

//  Get email from token
string TokenManager::getEmailFromToken(const string& token) {
    if (validateToken(token))
        return sessions[token].email;

    return "";
}

//  Logout (invalidate token)
void TokenManager::invalidateToken(const string& token) {
    sessions.erase(token);
}