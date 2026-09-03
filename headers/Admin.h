#pragma once

#include <string>

class Admin {
private:
    std::string username;
    std::string password;
    bool loggedIn;
    int failedAttempts;
    static const int MAX_ATTEMPTS = 3;

public:
    Admin();
    Admin(const std::string& username, const std::string& password);
    void configure(const std::string& username, const std::string& password);
    bool isConfigured() const;
    bool login(const std::string& user, const std::string& pass);
    void logout();
    bool isAuthenticated() const;
    bool isLocked() const;
    int getFailedAttempts() const;
    int getRemainingAttempts() const;
    std::string getUsername() const;
    void resetLock();
    void changePassword(const std::string& newPass);
};
