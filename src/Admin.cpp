#include "Admin.h"

Admin::Admin() : loggedIn(false), failedAttempts(0) {}
Admin::Admin(const std::string& user, const std::string& pass)
    : username(user), password(pass), loggedIn(false), failedAttempts(0) {}

void Admin::configure(const std::string& user, const std::string& pass) {
    username = user;
    password = pass;
    loggedIn = false;
    failedAttempts = 0;
}

bool Admin::isConfigured() const { return !username.empty() && !password.empty(); }

bool Admin::login(const std::string& user, const std::string& pass) {
    if (isLocked()) return false;
    if (user == username && pass == password) {
        loggedIn = true; failedAttempts = 0; return true;
    }
    ++failedAttempts; loggedIn = false; return false;
}
void Admin::logout() { loggedIn = false; }
bool Admin::isAuthenticated() const { return loggedIn; }
bool Admin::isLocked() const { return failedAttempts >= MAX_ATTEMPTS; }
int Admin::getFailedAttempts() const { return failedAttempts; }
int Admin::getRemainingAttempts() const {
    int remaining = MAX_ATTEMPTS - failedAttempts;
    return remaining > 0 ? remaining : 0;
}
std::string Admin::getUsername() const { return username; }
void Admin::resetLock() { failedAttempts = 0; loggedIn = false; }
void Admin::changePassword(const std::string& newPass) { password = newPass; }
