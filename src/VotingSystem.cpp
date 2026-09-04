#include "VotingSystem.h"
#include "colors.h"
#include "ui.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>

namespace {
const std::vector<std::string> MAJORS = {
    "Software Engineering", "Business Information Systems", "Knowledge Engineering",
    "High Performance Computing", "Embedded Systems", "Cyber Security",
    "Communication and Networking"
};

bool confirmed() {
    std::string answer;
    std::getline(std::cin, answer);
    return answer == "y" || answer == "Y" || answer == "yes" || answer == "YES";
}
}

VotingSystem::VotingSystem()
    : status(FileHandler::loadStatus()), admin("admin", "admin123"), isRunning(true) {
    enableANSI();
    FileHandler::loadStudents(students);
    FileHandler::loadVotes(votes);
}

void VotingSystem::run() { showMainMenu(); }

void VotingSystem::showMainMenu() {
    while (isRunning) {
        printBanner();
        drawTitleBox("  MAIN MENU  ");
        drawMenu({"Admin Login / Panel", "Student Login", "View Election Results",
                  "Save & Exit"});
        drawMidBorder();
        const std::string summary = "Registered: " + std::to_string(students.size()) +
            " | Class EC: " + (status.classElectionOpen ? "OPEN" : "CLOSED") +
            " | Batch Rep: " + (status.batchElectionOpen ? "OPEN" : "CLOSED");
        drawBoxRow(C_DIM C_WHITE + summary + C_RESET, static_cast<int>(summary.size()));
        drawBottomBorder();
        int choice = getIntInput("Select an option [1-4]: ", 1, 4);
        if (choice == 1) adminLogin();
        else if (choice == 2) studentLogin();
        else if (choice == 3) showResults();
        else { saveData(); isRunning = false; }
    }
}

void VotingSystem::adminLogin() {
    if (!admin.isAuthenticated()) {
        clearScreen();
        drawTitleBox("  ADMIN LOGIN  ");
        std::string user = getStringInput("Username: ");
        std::string password = getMaskedInput("Password: ");
        if (!admin.login(user, password)) {
            printError(admin.isLocked() ? "Account locked until restart."
                                        : "Invalid credentials.");
            pause();
            return;
        }
        printSuccess("Login successful.");
        pause();
    }
    adminPanel();
}

void VotingSystem::adminPanel() {
    bool active = true;
    while (active) {
        clearScreen();
        drawTitleBox("  ADMIN PANEL  ");
        drawMenu({"Register Student", "View Registered Students",
                  status.classElectionOpen ? "Close Class/Major EC Election"
                                           : "Open Class/Major EC Election",
                  status.batchElectionOpen ? "Close Batch Representative Election"
                                           : "Open Batch Representative Election",
                  "Calculate / Export Class EC Winners",
                  "Calculate / Export Batch Rep Winners", "Reset Election Data", "Logout"});
        drawBottomBorder();
        const int choice = getIntInput("Select an option [1-8]: ", 1, 8);
        switch (choice) {
            case 1: registerStudent(); break;
            case 2: listStudents(); break;
            case 3:
                if (status.classElectionOpen) {
                    status.classElectionOpen = false;
                    calculateClassResults();
                } else {
                    status.classElectionOpen = true;
                    status.batchElectionOpen = false;
                    printSuccess("Class/Major EC election opened."); pause();
                }
                saveData(); break;
            case 4:
                if (!status.batchElectionOpen) {
                    if (status.classElectionOpen || classWinnerLines().empty()) {
                        printError("Close and calculate the Class/Major EC election first.");
                    } else {
                        status.batchElectionOpen = true;
                        printSuccess("Batch Representative election opened.");
                    }
                } else {
                    status.batchElectionOpen = false;
                    calculateBatchResults();
                }
                saveData(); pause(); break;
            case 5: calculateClassResults(); break;
            case 6: calculateBatchResults(); pause(); break;
            case 7: resetElection(); break;
            case 8: admin.logout(); active = false; break;
        }
    }
}

void VotingSystem::registerStudent() {
    clearScreen(); drawTitleBox("  REGISTER UIT STUDENT  ");
    std::string roll = Student::normalizeRoll(getStringInput("Roll number (TNT - XXXX): "));
    if (!Student::isValidRoll(roll)) {
        printError("Roll must be TNT - XXXX and within 1600-2600."); pause(); return;
    }
    if (findStudent(roll)) { printError("That roll number is already registered."); pause(); return; }
    const int year = Student::yearFromRoll(roll);
    const int batch = Student::batchFromRoll(roll);
    printInfo("Detected Batch " + std::to_string(batch) + ", Year " + std::to_string(year) + ".");
    std::string name = getStringInput("Student name: ");
    std::string group;
    if (year <= 3) {
        group = getStringInput("Class (A-E): ");
        if (group.size() == 1) group[0] = static_cast<char>(std::toupper(group[0]));
    } else {
        std::cout << "\n";
        for (std::size_t i = 0; i < MAJORS.size(); ++i)
            std::cout << "  [" << i + 1 << "] " << MAJORS[i] << '\n';
        group = MAJORS[getIntInput("Select major [1-7]: ", 1, 7) - 1];
    }
    if (name.empty() || !Student::isValidGroupForYear(group, year)) {
        printError("Name or class/major is invalid."); pause(); return;
    }
    students.emplace_back(roll, name, group);
    saveData(); printSuccess("Student registered. Batch/year were assigned automatically."); pause();
}

void VotingSystem::listStudents() const {
    clearScreen(); drawTitleBox("  REGISTERED STUDENTS  ");
    for (const auto& student : students)
        std::cout << "  " << student.getRollNumber() << " | " << student.getName()
                  << " | Batch " << student.getBatch() << " | " << student.getYearLabel()
                  << " | " << student.getGroup() << '\n';
    if (students.empty()) printWarning("No students are registered.");
    drawBottomBorder(); pause();
}

void VotingSystem::studentLogin() {
    clearScreen(); drawTitleBox("  STUDENT LOGIN  ");
    const std::string roll = Student::normalizeRoll(getStringInput("UIT roll number: "));
    Student* student = findStudent(roll);
    if (!Student::isValidRoll(roll) || !student) {
        printError("Roll number is invalid or not in the registered student data."); pause(); return;
    }
    studentPanel(*student);
}

void VotingSystem::studentPanel(Student& student) {
    bool active = true;
    while (active) {
        clearScreen(); drawTitleBox("  STUDENT ELECTION PORTAL  ");
        std::cout << "  Welcome, " << student.getName() << " (" << student.getRollNumber()
                  << ")\n  Batch " << student.getBatch() << " | " << student.getYearLabel()
                  << " | " << student.getGroup() << "\n\n";
        drawMenu({"Vote for Class/Major EC", "Vote for Batch Representative", "Logout"});
        int choice = getIntInput("Select an option [1-3]: ", 1, 3);
        if (choice == 1) classVote(student);
        else if (choice == 2) batchVote(student);
        else active = false;
    }
}

void VotingSystem::classVote(Student& voter) {
    if (!status.classElectionOpen) { printWarning("The Class/Major EC election is closed."); pause(); return; }
    if (hasVoted("CLASS_EC", voter.getRollNumber())) { printWarning("You already voted in this election."); pause(); return; }
    auto candidates = classCandidates(voter);
    if (candidates.empty()) { printError("No eligible candidates in your group."); pause(); return; }
    std::cout << "\n  Your " << voter.getGroup() << " ballot:\n";
    for (std::size_t i = 0; i < candidates.size(); ++i)
        std::cout << "  [" << i + 1 << "] " << candidates[i]->getName() << " ("
                  << candidates[i]->getRollNumber() << ")\n";
    const int choice = getIntInput("Candidate number: ", 1, static_cast<int>(candidates.size()));
    std::cout << "Confirm vote for " << candidates[choice - 1]->getName() << "? [y/n]: ";
    if (confirmed()) {
        votes.push_back({"CLASS_EC", voter.getRollNumber(), candidates[choice - 1]->getRollNumber()});
        saveData(); printSuccess("Class/Major EC vote recorded.");
    } else printWarning("Vote cancelled.");
    pause();
}

void VotingSystem::batchVote(Student& voter) {
    if (!status.batchElectionOpen) { printWarning("The Batch Representative election is closed."); pause(); return; }
    if (hasVoted("BATCH_REP", voter.getRollNumber())) { printWarning("You already voted in this election."); pause(); return; }
    auto candidates = batchCandidates(voter.getBatch());
    if (candidates.empty()) { printError("No Class/Major EC winners are available for your batch."); pause(); return; }
    std::cout << "\n  Batch " << voter.getBatch() << " Representative ballot:\n";
    for (std::size_t i = 0; i < candidates.size(); ++i)
        std::cout << "  [" << i + 1 << "] " << candidates[i]->getName() << " | "
                  << candidates[i]->getGroup() << " (" << candidates[i]->getRollNumber() << ")\n";
    const int choice = getIntInput("Candidate number: ", 1, static_cast<int>(candidates.size()));
    std::cout << "Confirm vote for " << candidates[choice - 1]->getName() << "? [y/n]: ";
    if (confirmed()) {
        votes.push_back({"BATCH_REP", voter.getRollNumber(), candidates[choice - 1]->getRollNumber()});
        saveData(); printSuccess("Batch Representative vote recorded.");
    } else printWarning("Vote cancelled.");
    pause();
}

std::map<std::string, int> VotingSystem::tally(const std::string& election) const {
    std::map<std::string, int> counts;
    for (const auto& vote : votes) if (vote.election == election) ++counts[vote.candidateRoll];
    return counts;
}

std::vector<std::string> VotingSystem::classWinnerLines() const {
    const auto counts = tally("CLASS_EC");
    std::map<std::string, std::vector<const Student*>> groups;
    for (const auto& student : students)
        groups[std::to_string(student.getBatch()) + "|" + student.getGroup()].push_back(&student);
    std::vector<std::string> lines;
    for (const auto& entry : groups) {
        int maximum = -1;
        for (const Student* candidate : entry.second) {
            auto it = counts.find(candidate->getRollNumber());
            maximum = std::max(maximum, it == counts.end() ? 0 : it->second);
        }
        for (const Student* candidate : entry.second) {
            auto it = counts.find(candidate->getRollNumber());
            const int count = it == counts.end() ? 0 : it->second;
            if (count == maximum)
                lines.push_back(std::to_string(candidate->getBatch()) + "|" + candidate->getGroup() +
                                "|" + candidate->getRollNumber() + "|" + candidate->getName() +
                                "|" + std::to_string(count));
        }
    }
    return lines;
}

void VotingSystem::calculateClassResults() {
    const auto lines = classWinnerLines();
    FileHandler::writeLines(FileHandler::CLASS_RESULTS_FILE, lines);
    printSuccess("Class/Major EC winners calculated; tied winners are retained."); pause();
}

std::vector<const Student*> VotingSystem::batchCandidates(int batch) const {
    std::vector<const Student*> result;
    for (const auto& line : classWinnerLines()) {
        std::stringstream input(line); std::string batchText, group, roll;
        std::getline(input, batchText, '|'); std::getline(input, group, '|'); std::getline(input, roll, '|');
        if (std::stoi(batchText) == batch) if (const Student* s = findStudent(roll)) result.push_back(s);
    }
    return result;
}

void VotingSystem::calculateBatchResults() {
    const auto counts = tally("BATCH_REP");
    std::vector<std::string> lines;
    for (int batch = 9; batch <= 13; ++batch) {
        auto candidates = batchCandidates(batch);
        if (candidates.empty()) continue;
        int maximum = -1;
        for (const Student* candidate : candidates) {
            auto it = counts.find(candidate->getRollNumber());
            maximum = std::max(maximum, it == counts.end() ? 0 : it->second);
        }
        for (const Student* candidate : candidates) {
            auto it = counts.find(candidate->getRollNumber());
            const int count = it == counts.end() ? 0 : it->second;
            if (count == maximum)
                lines.push_back(std::to_string(batch) + "|" + candidate->getRollNumber() + "|" +
                                candidate->getName() + "|" + std::to_string(count));
        }
    }
    FileHandler::writeLines(FileHandler::BATCH_RESULTS_FILE, lines);
    printSuccess("Batch Representative winners calculated; tied winners are retained.");
}

void VotingSystem::showResults() const {
    clearScreen(); drawTitleBox("  ELECTION RESULTS  ");
    std::cout << "\n  CLASS / MAJOR EC WINNERS\n";
    for (const auto& line : classWinnerLines()) std::cout << "  " << line << '\n';
    std::cout << "\n  BATCH REPRESENTATIVE WINNERS\n";
    std::ifstream file(FileHandler::BATCH_RESULTS_FILE); std::string line;
    while (std::getline(file, line)) std::cout << "  " << line << '\n';
    drawBottomBorder(); pause();
}

void VotingSystem::resetElection() {
    std::cout << "Type YES to erase votes and results: "; std::string answer; std::getline(std::cin, answer);
    if (answer != "YES") { printWarning("Reset cancelled."); pause(); return; }
    votes.clear(); status = {};
    FileHandler::writeLines(FileHandler::CLASS_RESULTS_FILE, {});
    FileHandler::writeLines(FileHandler::BATCH_RESULTS_FILE, {});
    saveData(); printSuccess("Election votes, results, and status reset."); pause();
}

Student* VotingSystem::findStudent(const std::string& roll) {
    for (auto& student : students) if (student.getRollNumber() == Student::normalizeRoll(roll)) return &student;
    return nullptr;
}
const Student* VotingSystem::findStudent(const std::string& roll) const {
    for (const auto& student : students) if (student.getRollNumber() == Student::normalizeRoll(roll)) return &student;
    return nullptr;
}
bool VotingSystem::hasVoted(const std::string& election, const std::string& roll) const {
    return std::any_of(votes.begin(), votes.end(), [&](const VoteRecord& vote) {
        return vote.election == election && vote.voterRoll == roll;
    });
}
std::vector<const Student*> VotingSystem::classCandidates(const Student& voter) const {
    std::vector<const Student*> result;
    for (const auto& student : students)
        if (student.getBatch() == voter.getBatch() && student.getGroup() == voter.getGroup())
            result.push_back(&student);
    return result;
}
void VotingSystem::saveData() const {
    FileHandler::saveStudents(students); FileHandler::saveVotes(votes); FileHandler::saveStatus(status);
}
