#pragma once

#include <string>
#include <vector>
#include "Admin.h"
#include "FileHandler.h"
#include "Student.h"

class VotingSystem {
private:
    std::vector<Student> students;
    std::vector<VoteRecord> votes;
    std::vector<ElectionResult> classResults;
    std::vector<ElectionResult> batchResults;
    std::vector<ElectionPhase> phases;
    Admin admin;
    bool running;

    void loadData();
    void showMainMenu();
    void adminLogin();
    void adminMenu();
    void manageStudents();
    void addStudent();
    void viewStudents() const;
    void searchStudent() const;
    void editStudent();
    void deleteStudent();

    void classElectionMenu();
    void batchElectionMenu();
    void startClassElection(int batch);
    void viewClassCandidates(int batch) const;
    void viewVotes(int batch, const std::string& type) const;
    void calculateClassWinners(int batch);
    void startBatchElection(int batch);
    void viewBatchCandidates(int batch) const;
    void calculateBatchWinner(int batch);

    void studentLogin();
    void studentMenu(Student& student);
    void voteClassEC(Student& student);
    void voteBatchRepresentative(Student& student);
    void showVotingStatus(const Student& student) const;

    void resultsMenu() const;
    void showBatchResults(int batch) const;
    void showAllResults() const;
    void showStatistics(int batch) const;
    void showAbout() const;

    Student* findStudent(const std::string& roll);
    const Student* findStudent(const std::string& roll) const;
    bool hasVoted(const std::string& roll, const std::string& type,
                  int batch, const std::string& group) const;
    int voteCount(const std::string& type, int batch, const std::string& group,
                  const std::string& candidateRoll) const;
    ElectionPhase phaseFor(int batch) const;
    void setPhase(int batch, ElectionPhase phase);

public:
    VotingSystem();
    void run();
};

