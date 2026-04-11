#ifndef TOKEN_MANAGER_H
#define TOKEN_MANAGER_H

#include <string>
#include <unordered_map>
#include <ctime>

using namespace std;

//  Session structure
struct Session {
    string email;
    time_t expiry;
};

class TokenManager {
private:
    unordered_map<string, Session> sessions;

public:
    string generateToken(const string& email);
    bool validateToken(const string& token);
    string getEmailFromToken(const string& token);
    void invalidateToken(const string& token);
};

#endif