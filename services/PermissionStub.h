#ifndef PERMISSIONSTUB_H
#define PERMISSIONSTUB_H
#include <string>
using namespace std;

class Permission {
public:
    explicit Permission(string action) : action(action) {}
    string action;
};

class Role {
public:
    explicit Role(string name) : name(name) {}
    string name;
};

class RoleManager {
public:
    static int issueLimit(const string& role) {
        if (role == "FACULTY") return 20;
        if (role == "STUDENT") return 10;
        if (role == "STAFF") return 10;
        if (role == "LIBRARIAN") return 1000;
        return 0;
    }
};

class PermissionEngine {
public:
    static bool checkAccess(const string& role, const string& action) {
        if (role == "LIBRARIAN") return true;
        if (action == "SEARCH_BOOK" || action == "VIEW_BOOK" || action == "VIEW_PROFILE") return true;
        if ((role == "STUDENT" || role == "FACULTY" || role == "STAFF") &&
            (action == "ISSUE_BOOK" || action == "RETURN_BOOK")) return true;
        if (role == "STAFF" &&
            (action == "UPDATE_BOOK" || action == "CHANGE_AVAILABILITY")) return true;
        return false;
    }
};
#endif
