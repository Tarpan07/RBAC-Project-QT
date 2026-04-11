#include "PasswordUtil.h"
#include <functional>
#include <cctype>
#include <random>

using namespace std;

string generateSalt() {
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    string salt;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    for (int i = 0; i < 16; i++) {
        salt += charset[dis(gen)];
    }

    return salt;
}

string PasswordUtil::hashPassword(string password) {

    string salt = generateSalt();

    hash<string> hasher;
    string combined = password + salt;

    size_t hashed = hasher(combined);

    for (int i = 0; i < 1000; i++) {
        hashed = hasher(to_string(hashed));
    }
    return salt + ":" + to_string(hashed);
}

bool PasswordUtil::verifyPassword(string inputPassword, string storedHash) {

    int pos = storedHash.find(":");
    if (pos == string::npos) return false;

    string salt = storedHash.substr(0, pos);
    string hashValue = storedHash.substr(pos + 1);

    hash<string> hasher;
    string combined = inputPassword + salt;

    size_t hashed = hasher(combined);

    for (int i = 0; i < 1000; i++) {
        hashed = hasher(to_string(hashed));
    }

    return to_string(hashed) == hashValue;
}

bool PasswordUtil::validatePassword(string password) {

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    if (password.length() < 8)
        return false;

    for (char c : password) {

        if (isupper(c))
            hasUpper = true;

        else if (islower(c))
            hasLower = true;

        else if (isdigit(c))
            hasDigit = true;

        else if (ispunct(c))  // better special char check
            hasSpecial = true;
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}