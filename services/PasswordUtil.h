#ifndef PASSWORD_UTIL_H
#define PASSWORD_UTIL_H

#include <string>
using namespace std;

class PasswordUtil {
public:
    static string hashPassword(string password);
    static bool verifyPassword(string inputPassword, string storedHash); // ✅ new
    static bool validatePassword(string password);
};

#endif