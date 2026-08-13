// ============================================================
//  FileHandler.cpp  —  Persistent file save / load
// ============================================================
#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <filesystem>

// ── Static File Path Constants ────────────────────────────────
const std::string FileHandler::CANDIDATES_FILE = "data/candidates.dat";
const std::string FileHandler::STUDENTS_FILE   = "data/students.dat";
const std::string FileHandler::RESULTS_FILE    = "data/results.txt";

// ── Utility ───────────────────────────────────────────────────
bool FileHandler::fileExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}

void FileHandler::ensureDataDir() {
    std::filesystem::create_directories("data");
}

// ── Candidate Persistence ─────────────────────────────────────
bool FileHandler::saveCandidates(const std::vector<Candidate>& candidates) {
    ensureDataDir();
    std::ofstream file(CANDIDATES_FILE, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& c : candidates)
        file << c.serialize() << "\n";

    file.close();
    return true;
}

bool FileHandler::loadCandidates(std::vector<Candidate>& candidates) {
    candidates.clear();
    if (!fileExists(CANDIDATES_FILE)) return false;

    std::ifstream file(CANDIDATES_FILE);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            candidates.push_back(Candidate::deserialize(line));
    }
    file.close();
    return true;
}

// ── Student Persistence ───────────────────────────────────────
bool FileHandler::saveStudents(const std::vector<Student>& students) {
    ensureDataDir();
    std::ofstream file(STUDENTS_FILE, std::ios::trunc);
    if (!file.is_open()) return false;

    for (const auto& s : students)
        file << s.serialize() << "\n";

    file.close();
    return true;
}

bool FileHandler::loadStudents(std::vector<Student>& students) {
    students.clear();
    if (!fileExists(STUDENTS_FILE)) return false;

    std::ifstream file(STUDENTS_FILE);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty())
            students.push_back(Student::deserialize(line));
    }
    file.close();
    return true;
}

// ── Human-Readable Results Export ────────────────────────────
bool FileHandler::exportResults(const std::vector<Candidate>& candidates,
                                const std::string& winnerName,
                                int totalVotes) {
    ensureDataDir();
    std::ofstream file(RESULTS_FILE, std::ios::trunc);
    if (!file.is_open()) return false;

    file << "======================================================\n";
    file << "              VOTING SYSTEM — RESULTS\n";
    file << "======================================================\n\n";
    file << "WINNER: " << winnerName << "\n\n";
    file << "------------------------------------------------------\n";
    file << "  RANK  NAME                    PARTY           VOTES\n";
    file << "------------------------------------------------------\n";

    // Copy & sort by votes descending
    std::vector<Candidate> sorted = candidates;
    for (size_t i = 0; i < sorted.size(); ++i)
        for (size_t j = i + 1; j < sorted.size(); ++j)
            if (sorted[j] > sorted[i]) std::swap(sorted[i], sorted[j]);

    int rank = 1;
    for (const auto& c : sorted) {
        double pct = (totalVotes > 0)
            ? (100.0 * c.getVoteCount() / totalVotes) : 0.0;

        char buf[128];
        snprintf(buf, sizeof(buf), "  #%-4d %-22s %-15s %5d  (%.1f%%)\n",
                 rank++,
                 c.getName().c_str(),
                 c.getParty().c_str(),
                 c.getVoteCount(),
                 pct);
        file << buf;
    }

    file << "------------------------------------------------------\n";
    file << "Total votes cast: " << totalVotes << "\n";
    file << "======================================================\n";
    file.close();
    return true;
}
