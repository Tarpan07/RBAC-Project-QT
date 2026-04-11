#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "../models/User.h"
#include <vector>
#include <string>

using namespace std;

class UserRepository {
public:
    // Save new user
    bool saveUser(const User& user);

    // Find user by email (used for login)
    User* findByEmail(const string& email);

    // Get all users (for admin/debug)
    vector<User> getAllUsers();

    // Check if email already exists (for registration)
    bool emailExists(const string& email);
};

#endif