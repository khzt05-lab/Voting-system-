#include "FileHandler.h"

#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

const std::string FileHandler::STUDENTS_FILE = "data/students.txt";
const std::string FileHandler::VOTES_FILE = "data/votes.txt";
const std::string FileHandler::CLASS_RESULTS_FILE = "data/class_ec_results.txt";
const std::string FileHandler::BATCH_RESULTS_FILE = "data/batch_rep_results.txt";
const std::string FileHandler::STATUS_FILE = "data/election_status.txt";

void FileHandler::ensureDataDir() {
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0755);
#endif
    const std::vector<std::string> files = {STUDENTS_FILE, VOTES_FILE,
        CLASS_RESULTS_FILE, BATCH_RESULTS_FILE, STATUS_FILE};
    for (const std::string& path : files) {
        std::ifstream check(path);
        if (!check.good()) { std::ofstream create(path); }
    }
}

bool FileHandler::loadStudents(std::vector<Student>& students) {
    students.clear();
    std::ifstream file(STUDENTS_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Student student = Student::deserialize(line);
        if (!student.getRollNumber().empty()) students.push_back(student);
    }
    return true;
}

bool FileHandler::saveStudents(const std::vector<Student>& students) {
    std::ofstream file(STUDENTS_FILE);
    if (!file) return false;
    for (const Student& student : students) file << student.serialize() << '\n';
    return true;
}

bool FileHandler::loadVotes(std::vector<VoteRecord>& votes) {
    votes.clear();
    std::ifstream file(VOTES_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream stream(line);
        VoteRecord vote;
        std::string batchText;
        if (std::getline(stream, vote.studentRoll, '|') &&
            std::getline(stream, vote.electionType, '|') &&
            std::getline(stream, batchText, '|') &&
            std::getline(stream, vote.group, '|') &&
            std::getline(stream, vote.candidateRoll)) {
            try { vote.batch = std::stoi(batchText); votes.push_back(vote); } catch (...) {}
        }
    }
    return true;
}

bool FileHandler::appendVote(const VoteRecord& vote) {
    std::ofstream file(VOTES_FILE, std::ios::app);
    if (!file) return false;
    file << vote.studentRoll << '|' << vote.electionType << '|' << vote.batch << '|'
         << vote.group << '|' << vote.candidateRoll << '\n';
    return true;
}

bool FileHandler::loadResults(const std::string& filename, std::vector<ElectionResult>& results) {
    results.clear();
    std::ifstream file(filename);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream stream(line);
        ElectionResult result;
        std::string batchText, votesText;
        if (std::getline(stream, batchText, '|') && std::getline(stream, result.group, '|') &&
            std::getline(stream, result.winnerRoll, '|') && std::getline(stream, result.winnerName, '|') &&
            std::getline(stream, votesText)) {
            try { result.batch = std::stoi(batchText); result.votes = std::stoi(votesText); results.push_back(result); } catch (...) {}
        }
    }
    return true;
}

bool FileHandler::saveResults(const std::string& filename, const std::vector<ElectionResult>& results) {
    std::ofstream file(filename);
    if (!file) return false;
    for (const ElectionResult& result : results) {
        file << result.batch << '|' << result.group << '|' << result.winnerRoll << '|'
             << result.winnerName << '|' << result.votes << '\n';
    }
    return true;
}

bool FileHandler::loadPhases(std::vector<ElectionPhase>& phases) {
    phases.assign(5, ElectionPhase::NOT_STARTED);
    std::ifstream file(STATUS_FILE);
    if (!file) return false;
    int batch, phase;
    while (file >> batch >> phase) {
        if (batch >= 9 && batch <= 13 && phase >= 0 && phase <= 4)
            phases[batch - 9] = static_cast<ElectionPhase>(phase);
    }
    return true;
}

bool FileHandler::savePhases(const std::vector<ElectionPhase>& phases) {
    std::ofstream file(STATUS_FILE);
    if (!file) return false;
    for (int batch = 9; batch <= 13; ++batch)
        file << batch << ' ' << static_cast<int>(phases[batch - 9]) << '\n';
    return true;
}

