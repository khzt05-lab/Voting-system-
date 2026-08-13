// ============================================================
//  Admin.h  —  Admin class declaration (authentication)
// ============================================================
#pragma once
#include <string>

class Admin {
private:
    std::string username;
    std::string password;       // stored in plain text (educational demo)
    bool        loggedIn;
    int         failedAttempts;

    static const int MAX_ATTEMPTS = 3;

public:
    // ── Constructors ──────────────────────────────────────────
    Admin();
    Admin(const std::string& username, const std::string& password);

    // ── Authentication ────────────────────────────────────────
    // Returns true if credentials match and account isn't locked.
    bool login(const std::string& user, const std::string& pass);
    void logout();

    // ── Status Queries ────────────────────────────────────────
    bool isAuthenticated()  const;
    bool isLocked()         const;
    int  getFailedAttempts()    const;
    int  getRemainingAttempts() const;
    std::string getUsername()   const;

    // ── Management ────────────────────────────────────────────
    void resetLock();
    void changePassword(const std::string& newPass);
};
