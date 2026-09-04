#include "FileHandler.h"

#include <filesystem>
#include <fstream>
#include <sstream>

const std::string FileHandler::STUDENTS_FILE = "data/students.txt";
const std::string FileHandler::VOTES_FILE = "data/votes.txt";
const std::string FileHandler::CLASS_RESULTS_FILE = "data/class_ec_results.txt";
const std::string FileHandler::BATCH_RESULTS_FILE = "data/batch_rep_results.txt";
const std::string FileHandler::STATUS_FILE = "data/election_status.txt";

void FileHandler::ensureDataDir() { std::filesystem::create_directories("data"); }

bool FileHandler::loadStudents(std::vector<Student>& students) {
    students.clear();
    std::ifstream file(STUDENTS_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        Student student = Student::deserialize(line);
        if (Student::isValidRoll(student.getRollNumber()) &&
            Student::isValidGroupForYear(student.getGroup(), student.getYear()))
            students.push_back(student);
    }
    return true;
}

bool FileHandler::saveStudents(const std::vector<Student>& students) {
    std::vector<std::string> lines;
    for (const auto& student : students) lines.push_back(student.serialize());
    return writeLines(STUDENTS_FILE, lines);
}

bool FileHandler::loadVotes(std::vector<VoteRecord>& votes) {
    votes.clear();
    std::ifstream file(VOTES_FILE);
    if (!file) return false;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream input(line);
        VoteRecord vote;
        if (std::getline(input, vote.election, '|') &&
            std::getline(input, vote.voterRoll, '|') &&
            std::getline(input, vote.candidateRoll) &&
            (vote.election == "CLASS_EC" || vote.election == "BATCH_REP"))
            votes.push_back(vote);
    }
    return true;
}

bool FileHandler::saveVotes(const std::vector<VoteRecord>& votes) {
    std::vector<std::string> lines;
    for (const auto& vote : votes)
        lines.push_back(vote.election + "|" + vote.voterRoll + "|" + vote.candidateRoll);
    return writeLines(VOTES_FILE, lines);
}

ElectionStatus FileHandler::loadStatus() {
    ElectionStatus status;
    std::ifstream file(STATUS_FILE);
    std::string key, value;
    while (file >> key >> value) {
        if (key == "CLASS_EC") status.classElectionOpen = value == "OPEN";
        if (key == "BATCH_REP") status.batchElectionOpen = value == "OPEN";
    }
    return status;
}

bool FileHandler::saveStatus(const ElectionStatus& status) {
    return writeLines(STATUS_FILE, {
        std::string("CLASS_EC ") + (status.classElectionOpen ? "OPEN" : "CLOSED"),
        std::string("BATCH_REP ") + (status.batchElectionOpen ? "OPEN" : "CLOSED")
    });
}

bool FileHandler::writeLines(const std::string& path,
                             const std::vector<std::string>& lines) {
    ensureDataDir();
    std::ofstream file(path, std::ios::trunc);
    if (!file) return false;
    for (const auto& line : lines) file << line << '\n';
    return static_cast<bool>(file);
}
