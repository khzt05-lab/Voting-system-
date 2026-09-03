#pragma once

#include <string>
#include <vector>
#include "Student.h"

struct VoteRecord {
    std::string studentRoll;
    std::string electionType;
    int batch;
    std::string group;
    std::string candidateRoll;
};

struct ElectionResult {
    int batch;
    std::string group;
    std::string winnerRoll;
    std::string winnerName;
    int votes;
};

enum class ElectionPhase {
    NOT_STARTED = 0,
    CLASS_EC_RUNNING = 1,
    CLASS_EC_COMPLETED = 2,
    BATCH_REP_RUNNING = 3,
    BATCH_REP_COMPLETED = 4
};

class FileHandler {
public:
    static const std::string STUDENTS_FILE;
    static const std::string VOTES_FILE;
    static const std::string CLASS_RESULTS_FILE;
    static const std::string BATCH_RESULTS_FILE;
    static const std::string STATUS_FILE;

    static void ensureDataDir();
    static bool loadStudents(std::vector<Student>& students);
    static bool saveStudents(const std::vector<Student>& students);
    static bool loadVotes(std::vector<VoteRecord>& votes);
    static bool appendVote(const VoteRecord& vote);
    static bool loadResults(const std::string& filename, std::vector<ElectionResult>& results);
    static bool saveResults(const std::string& filename, const std::vector<ElectionResult>& results);
    static bool loadPhases(std::vector<ElectionPhase>& phases);
    static bool savePhases(const std::vector<ElectionPhase>& phases);
};

