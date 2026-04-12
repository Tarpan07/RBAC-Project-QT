#include "AuditLogger.h"
#include <fstream>
#include <ctime>
using namespace std;

void AuditLogger::log(const string& username, const string& action, const string& status) {
    ofstream file("audit.log", ios::app);
    time_t now = time(0);
    string ts = string(ctime(&now));
    ts.pop_back();
    file << "[" << ts << "] USER=" << username
         << " ACTION=" << action << " STATUS=" << status << "\n";
    file.close();
}