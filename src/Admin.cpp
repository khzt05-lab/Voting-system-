// ============================================================
//  Admin.cpp  —  Admin class implementation
// ============================================================
#include "Admin.h"
#include <iostream>

// ── Constructors ──────────────────────────────────────────────
Admin::Admin()
    : username("admin"), password("admin123"),
      loggedIn(false), failedAttempts(0) {}

Admin::Admin(const std::string& user, const std::string& pass)
    : username(user), password(pass),
      loggedIn(false), failedAttempts(0) {}

// ── Authentication ────────────────────────────────────────────
bool Admin::login(const std::string& user, const std::string& pass) {
    if (isLocked()) return false;

    if (user == username && pass == password) {
        loggedIn       = true;
        failedAttempts = 0;
        return true;
    }

    ++failedAttempts;
    loggedIn = false;
    return false;
}

void Admin::logout() {
    loggedIn = false;
}

// ── Status Queries ────────────────────────────────────────────
bool Admin::isAuthenticated()    const { return loggedIn; }
bool Admin::isLocked()           const { return failedAttempts >= MAX_ATTEMPTS; }
int  Admin::getFailedAttempts()  const { return failedAttempts; }
int  Admin::getRemainingAttempts() const {
    int rem = MAX_ATTEMPTS - failedAttempts;
    return rem > 0 ? rem : 0;
}
std::string Admin::getUsername() const { return username; }

// ── Management ────────────────────────────────────────────────
void Admin::resetLock() {
    failedAttempts = 0;
    loggedIn       = false;
}

void Admin::changePassword(const std::string& newPass) {
    password = newPass;
}
