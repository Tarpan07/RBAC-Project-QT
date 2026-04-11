#include "AuthManager.h"
#include "PasswordUtil.h"

#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// ================= CONSTRUCTOR =================
AuthManager::AuthManager()
{
    currentUser = nullptr;

    librarianEmails = {"librarian@university.edu"};
    staffEmails = {"staff@university.edu"};

    loadUsers();
}

// ================= SAVE USERS =================
void AuthManager::saveUsers()
{
    ofstream file("users.txt");

    for (auto &u : users)
    {
        file << u.getName() << ","
             << u.getEmail() << ","
             << u.getRole() << ","
             << u.getStudentID() << ","
             << u.getPasswordHash()
             << endl;
    }

    file.close();
}

// ================= LOAD USERS =================
void AuthManager::loadUsers()
{
    ifstream file("users.txt");
    if (!file)
        return;

    string line;

    while (getline(file, line))
    {
        stringstream ss(line);

        string name, email, role, studentID, password;

        getline(ss, name, ',');
        getline(ss, email, ',');
        getline(ss, role, ',');
        getline(ss, studentID, ',');
        getline(ss, password);

        users.push_back(User(name, email, role, studentID, password));
    }

    file.close();
}

// ================= EMAIL VALIDATION =================
bool AuthManager::isValidStudentEmail(string email)
{
    string domain = "@nits.ac.in";

    if (email.size() >= domain.size() &&
        email.substr(email.size() - domain.size()) == domain)
    {
        return true;
    }

    return false;
}

// ================= REGISTER =================
string AuthManager::registerUser(string name, string email, string password,
                                 string role, string studentID)
{
    // 🔥 FIX: normalize role
    transform(role.begin(), role.end(), role.begin(), ::toupper);

    // 🔴 Check if email already exists
    for (auto &u : users)
    {
        if (u.getEmail() == email)
            return "Email already exists";
    }

    // 🔴 Role validation
    if (role == "STUDENT")
    {
        if (!isValidStudentEmail(email))
            return "Use institute email";
    }
    else if (role == "FACULTY")
    {
        if (email.find("@faculty.edu") == string::npos)
            return "Invalid faculty email";

        studentID = "NA";
    }
    else if (role == "LIBRARIAN")
    {
        bool authorized = false;

        for (string e : librarianEmails)
        {
            if (e == email)
            {
                authorized = true;
                break;
            }
        }

        if (!authorized)
            return "Unauthorized librarian email";

        studentID = "NA";
    }
    else if (role == "STAFF")
    {
        bool authorized = false;

        for (string e : staffEmails)
        {
            if (e == email)
            {
                authorized = true;
                break;
            }
        }

        if (!authorized)
            return "Unauthorized staff email";

        studentID = "NA";
    }

    // 🔴 Password validation
    if (!PasswordUtil::validatePassword(password))
        return "Password must be 8+ chars, include upper, lower, digit, special";

    // ✅ SUCCESS
    string hashed = PasswordUtil::hashPassword(password);

    users.push_back(User(name, email, role, studentID, hashed));

    saveUsers();

    return "SUCCESS";
}

// ================= LOGIN =================
bool AuthManager::login(string email, string password)
{
    for (auto &u : users)
    {
        if (u.getEmail() == email &&
            PasswordUtil::verifyPassword(password, u.getPasswordHash()))
        {
            currentUser = &u;

            // generate token
            currentToken = tokenManager.generateToken(u.getEmail());

            return true;
        }
    }

    return false;
}

// ================= LOGOUT =================
void AuthManager::logout()
{
    tokenManager.invalidateToken(currentToken);

    currentUser = nullptr;
    currentToken = "";
}

// ================= STATUS =================
bool AuthManager::isLoggedIn()
{
    return currentUser != nullptr;
}

User *AuthManager::getCurrentUser()
{
    return currentUser;
}

string AuthManager::getToken()
{
    return currentToken;
}