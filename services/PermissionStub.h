#ifndef PERMISSIONSTUB_H
#define PERMISSIONSTUB_H
#include <string>
using namespace std;

// Member 2 will replace this with real PermissionEngine
class PermissionEngine {
public:
    static bool checkAccess(const string& role, const string& action) {
        if (role == "LIBRARIAN") return true;
        if (action == "SEARCH_BOOK") return true;
        if (role == "STUDENT" && action == "ISSUE_BOOK") return true;
        if (role == "STUDENT" && action == "RETURN_BOOK") return true;
        return false;
    }
};
#endif