#pragma once

#include "Student.h"
#include <string>
#include <vector>

struct VoteRecord {
    std::string election; // CLASS_EC or BATCH_REP
    std::string voterRoll;
    std::string candidateRoll;
};

struct ElectionStatus {
    bool classElectionOpen = false;
    bool batchElectionOpen = false;
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
    static bool saveVotes(const std::vector<VoteRecord>& votes);
    static ElectionStatus loadStatus();
    static bool saveStatus(const ElectionStatus& status);
    static bool writeLines(const std::string& path,
                           const std::vector<std::string>& lines);
};
