// ============================================================
//  FileHandler.h  —  File save / load for persistent data
// ============================================================
#pragma once
#include <vector>
#include <string>
#include "Candidate.h"
#include "Student.h"

class FileHandler {
public:
    // ── File paths ────────────────────────────────────────────
    static const std::string CANDIDATES_FILE;  // "data/candidates.dat"
    static const std::string STUDENTS_FILE;    // "data/students.dat"
    static const std::string RESULTS_FILE;     // "data/results.txt"

    // ── Candidate persistence ─────────────────────────────────
    static bool saveCandidates(const std::vector<Candidate>& candidates);
    static bool loadCandidates(std::vector<Candidate>& candidates);

    // ── Student persistence ───────────────────────────────────
    static bool saveStudents(const std::vector<Student>& students);
    static bool loadStudents(std::vector<Student>& students);

    // ── Human-readable results export ─────────────────────────
    static bool exportResults(const std::vector<Candidate>& candidates,
                              const std::string& winnerName,
                              int totalVotes);

    // ── Utility ───────────────────────────────────────────────
    static bool fileExists(const std::string& filename);
    static void ensureDataDir();   // Creates "data/" directory if absent
};
