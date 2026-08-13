// ============================================================
//  VotingSystem.h  —  Core controller (ties all modules)
// ============================================================
#pragma once
#include <vector>
#include <string>
#include "Candidate.h"
#include "Student.h"
#include "Admin.h"
#include "FileHandler.h"

class VotingSystem {
private:
    // ── State ─────────────────────────────────────────────────
    std::vector<Candidate> candidates;
    std::vector<Student>   students;
    Admin                  admin;
    bool                   isRunning;
    int                    nextCandidateId;

    // ── Navigation / Sections ─────────────────────────────────
    void showMainMenu();

    // Admin
    void adminLogin();
    void adminPanel();
    void candidateManagement();
    void addCandidate();
    void editCandidate();
    void removeCandidate();
    void listCandidates();
    void resetAllVotes();

    // Students & Voting
    void studentRegistration();
    void votingBooth();

    // Results
    void showResults();
    void calculateWinner();

    // ── Internal Helpers ──────────────────────────────────────
    Candidate* findCandidateById(int id);
    Student*   findStudentById(const std::string& id);
    bool       candidateExists(int id)                const;
    bool       studentExists(const std::string& id)   const;
    int        getTotalVotes()                         const;

    // Persistence
    void loadData();
    void saveData();

public:
    VotingSystem();
    void run();
};
