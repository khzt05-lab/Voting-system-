#pragma once

#include "Admin.h"
#include "FileHandler.h"
#include <map>
#include <string>
#include <vector>

class VotingSystem {
private:
    std::vector<Student> students;
    std::vector<VoteRecord> votes;
    ElectionStatus status;
    Admin admin;
    bool isRunning;

    void showMainMenu();
    void adminLogin();
    void adminPanel();
    void registerStudent();
    void listStudents() const;
    void studentLogin();
    void studentPanel(Student& student);
    void classVote(Student& voter);
    void batchVote(Student& voter);
    void showResults() const;
    void calculateClassResults();
    void calculateBatchResults();
    void resetElection();

    Student* findStudent(const std::string& roll);
    const Student* findStudent(const std::string& roll) const;
    bool hasVoted(const std::string& election, const std::string& roll) const;
    std::vector<const Student*> classCandidates(const Student& voter) const;
    std::vector<const Student*> batchCandidates(int batch) const;
    std::map<std::string, int> tally(const std::string& election) const;
    std::vector<std::string> classWinnerLines() const;
    void saveData() const;

public:
    VotingSystem();
    void run();
};
