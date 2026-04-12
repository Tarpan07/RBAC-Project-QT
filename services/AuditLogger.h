#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H
#include <string>
using namespace std;

class AuditLogger {
private:
    AuditLogger() {}

public:
    static AuditLogger& getInstance() {
        static AuditLogger instance;
        return instance;
    }
    void log(const string& username, const string& action, const string& status);
};
#endif
