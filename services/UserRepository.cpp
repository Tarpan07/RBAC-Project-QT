#include "UserRepository.h"
#include "../models/User.h"
#include <fstream>
#include <sstream>
using namespace std;

//  Save new user to file
bool UserRepository::saveUser(const User& user) {
    ofstream file("users.txt", ios::app);
    if (!file) return false;

    file << user.getName() << ","
         << user.getEmail() << ","
         << user.getRole() << ","
         << user.getStudentID() << ","
         << user.getPasswordHash() << "\n";

    file.close();
    return true;
}

//  Find user by email (used for login)
User* UserRepository::findByEmail(const string& email) {
    ifstream file("users.txt");
    if (!file) return nullptr;

    string line;

    while (getline(file, line)) {
        stringstream ss(line);

        string name, mail, role, studentID, password;

        getline(ss, name, ',');
        getline(ss, mail, ',');
        getline(ss, role, ',');
        getline(ss, studentID, ',');
        getline(ss, password); // last field

        if (mail == email) {
            file.close();
            return new User(name, mail, role, studentID, password);
        }
    }

    file.close();
    return nullptr;
}

//  Get all users (useful for admin/debug)
vector<User> UserRepository::getAllUsers() {
    vector<User> users;

    ifstream file("users.txt");
    if (!file) return users;

    string line;

    while (getline(file, line)) {
        stringstream ss(line);

        string name, mail, role, studentID, password;

        getline(ss, name, ',');
        getline(ss, mail, ',');
        getline(ss, role, ',');
        getline(ss, studentID, ',');
        getline(ss, password);

        users.emplace_back(name, mail, role, studentID, password);
    }

    file.close();
    return users;
}

//  Check if email already exists (for registration)
bool UserRepository::emailExists(const string& email) {
    ifstream file("users.txt");
    if (!file) return false;

    string line;

    while (getline(file, line)) {
        stringstream ss(line);

        string name, mail;

        getline(ss, name, ',');
        getline(ss, mail, ',');

        if (mail == email) {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}