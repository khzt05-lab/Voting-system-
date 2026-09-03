#include "VotingSystem.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>

namespace {
const char* RESET = "\033[0m";
const char* CYAN = "\033[96m";
const char* GREEN = "\033[92m";
const char* YELLOW = "\033[93m";
const char* MAGENTA = "\033[95m";
const char* RED = "\033[91m";

void clearScreen() { std::cout << "\033[2J\033[1;1H"; }
void divider() { std::cout << CYAN << "============================================================\n" << RESET; }
void heading(const std::string& title) {
    clearScreen(); divider();
    std::cout << CYAN << std::setw(30 + static_cast<int>(title.size()) / 2) << title << "\n" << RESET;
    divider();
}
void pauseScreen() {
    std::cout << "\nPress Enter to continue...";
    std::string ignored; std::getline(std::cin, ignored);
}
std::string input(const std::string& prompt) {
    std::cout << YELLOW << prompt << RESET;
    std::string value; std::getline(std::cin, value); return value;
}
int inputInt(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::string text = input(prompt);
        std::stringstream stream(text);
        int value; char extra;
        if (stream >> value && !(stream >> extra) && value >= minimum && value <= maximum) return value;
        std::cout << RED << "Invalid choice. Enter " << minimum << " to " << maximum << ".\n" << RESET;
    }
}
bool confirm(const std::string& prompt) {
    std::string answer = input(prompt + " (y/n): ");
    return answer == "y" || answer == "Y" || answer == "yes" || answer == "YES";
}
void success(const std::string& message) { std::cout << GREEN << message << "\n" << RESET; }
void error(const std::string& message) { std::cout << RED << message << "\n" << RESET; }
void info(const std::string& message) { std::cout << YELLOW << message << "\n" << RESET; }

std::vector<std::string> groupsForYear(int year) {
    if (year <= 3) return {"A", "B", "C", "D", "E"};
    return {"Software Engineering", "Business Information Systems", "Knowledge Engineering",
            "High Performance Computing", "Embedded Systems", "Cyber Security",
            "Communication and Networking"};
}

std::string phaseName(ElectionPhase phase) {
    if (phase == ElectionPhase::CLASS_EC_RUNNING) return "Class/Major EC Running";
    if (phase == ElectionPhase::CLASS_EC_COMPLETED) return "Class/Major EC Completed";
    if (phase == ElectionPhase::BATCH_REP_RUNNING) return "Batch Representative Running";
    if (phase == ElectionPhase::BATCH_REP_COMPLETED) return "Batch Representative Completed";
    return "Not Started";
}
}

VotingSystem::VotingSystem() : running(true) { loadData(); }

void VotingSystem::loadData() {
    FileHandler::ensureDataDir();
    FileHandler::loadStudents(students);
    FileHandler::loadVotes(votes);
    FileHandler::loadResults(FileHandler::CLASS_RESULTS_FILE, classResults);
    FileHandler::loadResults(FileHandler::BATCH_RESULTS_FILE, batchResults);
    FileHandler::loadPhases(phases);
    std::ifstream credentials("data/admin_credentials.txt");
    std::string savedUsername, savedPassword;
    if (std::getline(credentials, savedUsername) && std::getline(credentials, savedPassword))
        admin.configure(savedUsername, savedPassword);
}

void VotingSystem::run() {
    while (running) showMainMenu();
    success("Thank you for using the UIT Student Election System.");
}

void VotingSystem::showMainMenu() {
    heading("UIT STUDENT ELECTION SYSTEM");
    std::cout << "1. Student Login\n2. Admin Login\n3. Election Results\n4. About\n5. Exit\n\n";
    int choice = inputInt("Enter your choice: ", 1, 5);
    if (choice == 1) studentLogin();
    else if (choice == 2) adminLogin();
    else if (choice == 3) resultsMenu();
    else if (choice == 4) showAbout();
    else running = false;
}

void VotingSystem::adminLogin() {
    heading("ADMIN LOGIN");
    if (!admin.isConfigured()) {
        info("First-time setup: create the administrator login for this computer.");
        std::string newUsername = input("New username: ");
        std::string newPassword = input("New password: ");
        std::string confirmation = input("Confirm password: ");
        if (newUsername.empty() || newPassword.empty()) {
            error("Username and password cannot be empty."); pauseScreen(); return;
        }
        if (newPassword != confirmation) {
            error("Passwords do not match."); pauseScreen(); return;
        }
        std::ofstream credentials("data/admin_credentials.txt");
        if (!credentials) { error("Could not save the administrator login."); pauseScreen(); return; }
        credentials << newUsername << '\n' << newPassword << '\n';
        admin.configure(newUsername, newPassword);
        success("Administrator login created. Keep it private.");
        pauseScreen();
        return;
    }
    if (admin.isLocked()) {
        error("Admin login is locked after three failed attempts. Restart the program to retry.");
        pauseScreen(); return;
    }
    std::string username = input("Username: ");
    std::string password = input("Password: ");
    if (admin.login(username, password)) {
        success("Login successful."); pauseScreen(); adminMenu();
    } else {
        error("Incorrect username or password. Remaining attempts: " +
              std::to_string(admin.getRemainingAttempts()));
        pauseScreen();
    }
}

void VotingSystem::adminMenu() {
    while (admin.isAuthenticated()) {
        heading("ADMIN MENU");
        std::cout << "1. Manage Students\n2. Class/Major EC Election\n"
                  << "3. Batch Representative Election\n4. Election Results\n"
                  << "5. Voting Statistics\n6. Logout\n\n";
        int choice = inputInt("Enter choice: ", 1, 6);
        if (choice == 1) manageStudents();
        else if (choice == 2) classElectionMenu();
        else if (choice == 3) batchElectionMenu();
        else if (choice == 4) resultsMenu();
        else if (choice == 5) {
            int batch = inputInt("Batch (9-13): ", 9, 13);
            showStatistics(batch); pauseScreen();
        } else { admin.logout(); success("Logged out."); pauseScreen(); }
    }
}

void VotingSystem::manageStudents() {
    bool back = false;
    while (!back) {
        heading("MANAGE STUDENTS");
        std::cout << "1. Add Student\n2. View Students\n3. Search Student\n"
                  << "4. Edit Student\n5. Delete Student\n6. Back\n\n";
        int choice = inputInt("Enter choice: ", 1, 6);
        if (choice == 1) addStudent();
        else if (choice == 2) { viewStudents(); pauseScreen(); }
        else if (choice == 3) { searchStudent(); pauseScreen(); }
        else if (choice == 4) editStudent();
        else if (choice == 5) deleteStudent();
        else back = true;
    }
}

void VotingSystem::addStudent() {
    heading("ADD STUDENT");
    std::string name = input("Student name: ");
    std::string enteredRoll = input("Roll number (TNT - XXXX): ");
    std::string roll = normalizeRollNumber(enteredRoll);
    if (roll.empty()) { error("Invalid Roll Number Format. Please use TNT - XXXX."); pauseScreen(); return; }
    int number = extractRollNumber(roll);
    int batch = determineBatch(number);
    if (batch == 0) { error("Invalid UIT Roll Number / Batch not found."); pauseScreen(); return; }
    if (findStudent(roll)) { error("A student with this roll number already exists."); pauseScreen(); return; }
    int year = determineYear(batch);

    std::cout << "\nDetected Batch " << batch << " - " << yearLabel(year) << "\n";
    std::vector<std::string> groups = groupsForYear(year);
    for (std::size_t i = 0; i < groups.size(); ++i)
        std::cout << i + 1 << ". " << (year <= 3 ? "Class " : "") << groups[i] << '\n';
    int groupChoice = inputInt("Select class/major: ", 1, static_cast<int>(groups.size()));
    students.emplace_back(roll, name, batch, year, groups[groupChoice - 1]);
    if (FileHandler::saveStudents(students)) success("Student added successfully.");
    else error("Could not save the student file.");
    pauseScreen();
}

void VotingSystem::viewStudents() const {
    heading("REGISTERED STUDENTS");
    if (students.empty()) { info("No students are registered."); return; }
    std::cout << std::left << std::setw(13) << "Roll" << std::setw(24) << "Name"
              << std::setw(8) << "Batch" << "Class / Major\n";
    divider();
    for (const Student& student : students) {
        std::cout << std::left << std::setw(13) << student.getRollNumber()
                  << std::setw(24) << student.getName().substr(0, 22)
                  << std::setw(8) << student.getBatch() << student.getClassOrMajor() << '\n';
    }
}

void VotingSystem::searchStudent() const {
    heading("SEARCH STUDENT");
    std::string roll = normalizeRollNumber(input("Roll number: "));
    const Student* student = findStudent(roll);
    if (!student) { error("Student not found."); return; }
    std::cout << "Name  : " << student->getName() << "\nRoll  : " << student->getRollNumber()
              << "\nBatch : " << student->getBatch() << "\nYear  : " << yearLabel(student->getYear())
              << "\n" << (student->getYear() <= 3 ? "Class : " : "Major : ")
              << student->getClassOrMajor() << '\n';
}

void VotingSystem::editStudent() {
    heading("EDIT STUDENT");
    Student* student = findStudent(normalizeRollNumber(input("Roll number: ")));
    if (!student) { error("Student not found."); pauseScreen(); return; }
    std::string name = input("New name (blank keeps current): ");
    if (!name.empty()) student->setName(name);
    std::vector<std::string> groups = groupsForYear(student->getYear());
    std::cout << "0. Keep " << student->getClassOrMajor() << '\n';
    for (std::size_t i = 0; i < groups.size(); ++i) std::cout << i + 1 << ". " << groups[i] << '\n';
    int choice = inputInt("Select class/major: ", 0, static_cast<int>(groups.size()));
    if (choice > 0) student->setClassOrMajor(groups[choice - 1]);
    FileHandler::saveStudents(students); success("Student updated."); pauseScreen();
}

void VotingSystem::deleteStudent() {
    heading("DELETE STUDENT");
    std::string roll = normalizeRollNumber(input("Roll number: "));
    Student* student = findStudent(roll);
    if (!student) { error("Student not found."); pauseScreen(); return; }
    for (const VoteRecord& vote : votes) {
        if (vote.studentRoll == roll || vote.candidateRoll == roll) {
            error("This student is linked to election votes and cannot be deleted."); pauseScreen(); return;
        }
    }
    if (confirm("Delete " + student->getName() + "?")) {
        students.erase(std::remove_if(students.begin(), students.end(),
            [&](const Student& item) { return item.getRollNumber() == roll; }), students.end());
        FileHandler::saveStudents(students); success("Student deleted.");
    } else info("Deletion cancelled.");
    pauseScreen();
}

void VotingSystem::classElectionMenu() {
    int batch = inputInt("Manage which batch (9-13): ", 9, 13);
    bool back = false;
    while (!back) {
        heading("CLASS / MAJOR EC - BATCH " + std::to_string(batch));
        std::cout << "Current phase: " << phaseName(phaseFor(batch)) << "\n\n"
                  << "1. Start Class/Major EC Election\n2. View Candidates\n"
                  << "3. View Votes\n4. Calculate Winners and Close\n5. Back\n\n";
        int choice = inputInt("Enter choice: ", 1, 5);
        if (choice == 1) startClassElection(batch);
        else if (choice == 2) { viewClassCandidates(batch); pauseScreen(); }
        else if (choice == 3) { viewVotes(batch, "CLASS_EC"); pauseScreen(); }
        else if (choice == 4) calculateClassWinners(batch);
        else back = true;
    }
}

void VotingSystem::startClassElection(int batch) {
    ElectionPhase phase = phaseFor(batch);
    if (phase != ElectionPhase::NOT_STARTED) { error("This batch election has already started or completed."); pauseScreen(); return; }
    bool found = false;
    for (const Student& student : students) if (student.getBatch() == batch) found = true;
    if (!found) { error("No students are registered in this batch."); pauseScreen(); return; }
    setPhase(batch, ElectionPhase::CLASS_EC_RUNNING);
    success("Class/Major EC election started. Candidates were generated from student records."); pauseScreen();
}

void VotingSystem::viewClassCandidates(int batch) const {
    heading("AUTOMATIC EC CANDIDATES - BATCH " + std::to_string(batch));
    int year = determineYear(batch);
    for (const std::string& group : groupsForYear(year)) {
        std::cout << MAGENTA << "\n" << (year <= 3 ? "Class " : "Major: ") << group << RESET << '\n';
        int number = 0;
        for (const Student& student : students) {
            if (student.getBatch() == batch && student.getClassOrMajor() == group)
                std::cout << "  " << ++number << ". " << student.getName() << " (" << student.getRollNumber() << ")\n";
        }
        if (number == 0) std::cout << "  No registered students.\n";
    }
}

void VotingSystem::viewVotes(int batch, const std::string& type) const {
    heading((type == "CLASS_EC" ? "CLASS / MAJOR EC VOTES - BATCH " : "BATCH REPRESENTATIVE VOTES - BATCH ") + std::to_string(batch));
    int count = 0;
    for (const VoteRecord& vote : votes) {
        if (vote.batch == batch && vote.electionType == type) {
            const Student* voter = findStudent(vote.studentRoll);
            const Student* candidate = findStudent(vote.candidateRoll);
            std::cout << ++count << ". " << (voter ? voter->getName() : vote.studentRoll)
                      << " -> " << (candidate ? candidate->getName() : vote.candidateRoll)
                      << " [" << vote.group << "]\n";
        }
    }
    if (count == 0) info("No votes have been cast.");
}

void VotingSystem::calculateClassWinners(int batch) {
    if (phaseFor(batch) != ElectionPhase::CLASS_EC_RUNNING) {
        error("Start the Class/Major EC election before calculating winners."); pauseScreen(); return;
    }
    int year = determineYear(batch);
    std::vector<ElectionResult> newResults;
    bool problem = false;
    for (const std::string& group : groupsForYear(year)) {
        std::vector<const Student*> candidates;
        for (const Student& student : students)
            if (student.getBatch() == batch && student.getClassOrMajor() == group) candidates.push_back(&student);
        if (candidates.empty()) continue;

        int highest = -1;
        std::vector<const Student*> winners;
        for (const Student* candidate : candidates) {
            int count = voteCount("CLASS_EC", batch, group, candidate->getRollNumber());
            if (count > highest) { highest = count; winners.assign(1, candidate); }
            else if (count == highest) winners.push_back(candidate);
        }
        if (highest <= 0) { error("No votes in " + group + "."); problem = true; }
        else if (winners.size() != 1) { error("Tie in " + group + ". A unique winner is required."); problem = true; }
        else newResults.push_back({batch, group, winners[0]->getRollNumber(), winners[0]->getName(), highest});
    }
    if (newResults.empty() || problem) {
        info("Winners were not saved. Resolve every group before closing the election."); pauseScreen(); return;
    }
    classResults.erase(std::remove_if(classResults.begin(), classResults.end(),
        [&](const ElectionResult& result) { return result.batch == batch; }), classResults.end());
    classResults.insert(classResults.end(), newResults.begin(), newResults.end());
    FileHandler::saveResults(FileHandler::CLASS_RESULTS_FILE, classResults);
    setPhase(batch, ElectionPhase::CLASS_EC_COMPLETED);
    success("Class/Major EC winners saved. They are now the Batch Representative candidates."); pauseScreen();
}

void VotingSystem::batchElectionMenu() {
    int batch = inputInt("Manage which batch (9-13): ", 9, 13);
    bool back = false;
    while (!back) {
        heading("BATCH REPRESENTATIVE - BATCH " + std::to_string(batch));
        std::cout << "Current phase: " << phaseName(phaseFor(batch)) << "\n\n"
                  << "1. View EC Winners / Candidates\n2. Start Batch Representative Election\n"
                  << "3. View Votes\n4. Calculate Winner and Close\n5. Back\n\n";
        int choice = inputInt("Enter choice: ", 1, 5);
        if (choice == 1) { viewBatchCandidates(batch); pauseScreen(); }
        else if (choice == 2) startBatchElection(batch);
        else if (choice == 3) { viewVotes(batch, "BATCH_REP"); pauseScreen(); }
        else if (choice == 4) calculateBatchWinner(batch);
        else back = true;
    }
}

void VotingSystem::viewBatchCandidates(int batch) const {
    heading("BATCH " + std::to_string(batch) + " REPRESENTATIVE CANDIDATES");
    int number = 0;
    for (const ElectionResult& result : classResults) {
        if (result.batch == batch)
            std::cout << ++number << ". " << result.winnerName << " (" << result.winnerRoll
                      << ") - " << result.group << " EC\n";
    }
    if (number == 0) info("No candidates yet. Calculate Class/Major EC winners first.");
}

void VotingSystem::startBatchElection(int batch) {
    if (phaseFor(batch) != ElectionPhase::CLASS_EC_COMPLETED) {
        error("Batch voting is locked until Class/Major EC winners are calculated."); pauseScreen(); return;
    }
    int count = 0;
    for (const ElectionResult& result : classResults) if (result.batch == batch) ++count;
    if (count == 0) { error("No EC winners are available."); pauseScreen(); return; }
    setPhase(batch, ElectionPhase::BATCH_REP_RUNNING);
    success("Batch Representative election started."); pauseScreen();
}

void VotingSystem::calculateBatchWinner(int batch) {
    if (phaseFor(batch) != ElectionPhase::BATCH_REP_RUNNING) {
        error("Start the Batch Representative election before calculating the winner."); pauseScreen(); return;
    }
    int highest = -1;
    std::vector<const ElectionResult*> winners;
    for (const ElectionResult& candidate : classResults) {
        if (candidate.batch != batch) continue;
        int count = voteCount("BATCH_REP", batch, "ALL", candidate.winnerRoll);
        if (count > highest) { highest = count; winners.assign(1, &candidate); }
        else if (count == highest) winners.push_back(&candidate);
    }
    if (highest <= 0) { error("No Batch Representative votes have been cast."); pauseScreen(); return; }
    if (winners.size() != 1) { error("The election is tied. A unique winner is required before closing."); pauseScreen(); return; }
    batchResults.erase(std::remove_if(batchResults.begin(), batchResults.end(),
        [&](const ElectionResult& result) { return result.batch == batch; }), batchResults.end());
    batchResults.push_back({batch, "ALL", winners[0]->winnerRoll, winners[0]->winnerName, highest});
    FileHandler::saveResults(FileHandler::BATCH_RESULTS_FILE, batchResults);
    setPhase(batch, ElectionPhase::BATCH_REP_COMPLETED);
    success("Batch Representative winner saved: " + winners[0]->winnerName); pauseScreen();
}

void VotingSystem::studentLogin() {
    heading("STUDENT LOGIN");
    std::string entered = input("Enter UIT Roll Number: ");
    std::string roll = normalizeRollNumber(entered);
    if (roll.empty()) { error("Invalid Roll Number Format. Please use TNT - XXXX."); pauseScreen(); return; }
    if (determineBatch(extractRollNumber(roll)) == 0) { error("Invalid UIT Roll Number / Batch not found."); pauseScreen(); return; }
    Student* student = findStudent(roll);
    if (!student) { error("Student not found in the registered student database."); pauseScreen(); return; }
    std::cout << "\nName  : " << student->getName() << "\nRoll  : " << student->getRollNumber()
              << "\nBatch : " << student->getBatch() << "\nYear  : " << yearLabel(student->getYear())
              << "\n" << (student->getYear() <= 3 ? "Class : " : "Major : ")
              << student->getClassOrMajor() << "\n\n";
    success("Login Successful!"); pauseScreen(); studentMenu(*student);
}

void VotingSystem::studentMenu(Student& student) {
    bool logout = false;
    while (!logout) {
        heading("UIT STUDENT ELECTION");
        std::cout << "Welcome, " << student.getName() << "\nBatch: " << student.getBatch()
                  << " | " << yearLabel(student.getYear()) << " | "
                  << (student.getYear() <= 3 ? "Class " : "Major: ") << student.getClassOrMajor() << "\n\n";
        std::cout << "1. " << (student.getYear() <= 3 ? "Class " : "Major ") << student.getClassOrMajor() << " EC Election";
        if (phaseFor(student.getBatch()) != ElectionPhase::CLASS_EC_RUNNING) std::cout << " [LOCKED]";
        std::cout << "\n2. Batch " << student.getBatch() << " Representative";
        if (phaseFor(student.getBatch()) != ElectionPhase::BATCH_REP_RUNNING) std::cout << " [LOCKED]";
        std::cout << "\n3. My Voting Status\n4. Logout\n\n";
        int choice = inputInt("Enter choice: ", 1, 4);
        if (choice == 1) voteClassEC(student);
        else if (choice == 2) voteBatchRepresentative(student);
        else if (choice == 3) { showVotingStatus(student); pauseScreen(); }
        else logout = true;
    }
}

void VotingSystem::voteClassEC(Student& student) {
    int batch = student.getBatch();
    std::string group = student.getClassOrMajor();
    if (phaseFor(batch) != ElectionPhase::CLASS_EC_RUNNING) { error("This Class/Major EC election is not open."); pauseScreen(); return; }
    if (hasVoted(student.getRollNumber(), "CLASS_EC", batch, group)) { error("You have already voted in this election."); pauseScreen(); return; }
    std::vector<const Student*> candidates;
    for (const Student& item : students)
        if (item.getBatch() == batch && item.getClassOrMajor() == group) candidates.push_back(&item);
    if (candidates.empty()) { error("No candidates are available."); pauseScreen(); return; }
    heading("CLASS / MAJOR EC BALLOT");
    for (std::size_t i = 0; i < candidates.size(); ++i)
        std::cout << i + 1 << ". " << candidates[i]->getName() << " (" << candidates[i]->getRollNumber() << ")\n";
    int choice = inputInt("Choose a candidate: ", 1, static_cast<int>(candidates.size()));
    const Student* candidate = candidates[choice - 1];
    if (confirm("Confirm vote for " + candidate->getName() + "?")) {
        VoteRecord vote{student.getRollNumber(), "CLASS_EC", batch, group, candidate->getRollNumber()};
        if (FileHandler::appendVote(vote)) { votes.push_back(vote); success("Vote cast successfully."); }
        else error("Vote could not be saved.");
    } else info("Vote cancelled.");
    pauseScreen();
}

void VotingSystem::voteBatchRepresentative(Student& student) {
    int batch = student.getBatch();
    if (phaseFor(batch) != ElectionPhase::BATCH_REP_RUNNING) {
        error("The Batch Representative election is locked until the EC election is completed."); pauseScreen(); return;
    }
    if (hasVoted(student.getRollNumber(), "BATCH_REP", batch, "ALL")) { error("You have already voted in this election."); pauseScreen(); return; }
    std::vector<const ElectionResult*> candidates;
    for (const ElectionResult& result : classResults) if (result.batch == batch) candidates.push_back(&result);
    if (candidates.empty()) { error("No EC winners are available as candidates."); pauseScreen(); return; }
    heading("BATCH REPRESENTATIVE BALLOT");
    for (std::size_t i = 0; i < candidates.size(); ++i)
        std::cout << i + 1 << ". " << candidates[i]->winnerName << " - " << candidates[i]->group << " EC\n";
    int choice = inputInt("Choose a candidate: ", 1, static_cast<int>(candidates.size()));
    const ElectionResult* candidate = candidates[choice - 1];
    if (confirm("Confirm vote for " + candidate->winnerName + "?")) {
        VoteRecord vote{student.getRollNumber(), "BATCH_REP", batch, "ALL", candidate->winnerRoll};
        if (FileHandler::appendVote(vote)) { votes.push_back(vote); success("Vote cast successfully."); }
        else error("Vote could not be saved.");
    } else info("Vote cancelled.");
    pauseScreen();
}

void VotingSystem::showVotingStatus(const Student& student) const {
    heading("MY VOTING STATUS");
    bool classVote = hasVoted(student.getRollNumber(), "CLASS_EC", student.getBatch(), student.getClassOrMajor());
    bool batchVote = hasVoted(student.getRollNumber(), "BATCH_REP", student.getBatch(), "ALL");
    std::cout << "Class/Major EC       : " << (classVote ? "Voted" : "Not voted")
              << "\nBatch Representative : " << (batchVote ? "Voted" : "Not voted") << '\n';
}

void VotingSystem::resultsMenu() const {
    bool back = false;
    while (!back) {
        heading("ELECTION RESULTS");
        std::cout << "1. Batch 13\n2. Batch 12\n3. Batch 11\n4. Batch 10\n5. Batch 9\n6. All Results\n7. Back\n\n";
        int choice = inputInt("Enter choice: ", 1, 7);
        if (choice >= 1 && choice <= 5) { showBatchResults(14 - choice); pauseScreen(); }
        else if (choice == 6) { showAllResults(); pauseScreen(); }
        else back = true;
    }
}

void VotingSystem::showBatchResults(int batch) const {
    heading("BATCH " + std::to_string(batch) + " RESULTS");
    std::cout << "Election phase: " << phaseName(phaseFor(batch)) << "\n\n" << MAGENTA << "CLASS / MAJOR EC RESULTS\n" << RESET;
    bool found = false;
    for (const ElectionResult& result : classResults) if (result.batch == batch) {
        found = true;
        std::cout << result.group << ": " << result.winnerName << " (" << result.votes << " votes)\n";
    }
    if (!found) std::cout << "Not available yet.\n";
    std::cout << MAGENTA << "\nBATCH REPRESENTATIVE\n" << RESET;
    found = false;
    for (const ElectionResult& result : batchResults) if (result.batch == batch) {
        found = true; std::cout << "WINNER: " << result.winnerName << " (" << result.votes << " votes)\n";
    }
    if (!found) std::cout << "Not available yet.\n";
}

void VotingSystem::showAllResults() const {
    for (int batch = 13; batch >= 9; --batch) {
        std::cout << "\n--- Batch " << batch << " ---\n";
        bool found = false;
        for (const ElectionResult& result : classResults) if (result.batch == batch) {
            found = true; std::cout << result.group << " EC: " << result.winnerName << '\n';
        }
        for (const ElectionResult& result : batchResults) if (result.batch == batch) {
            found = true; std::cout << "Batch Representative: " << result.winnerName << '\n';
        }
        if (!found) std::cout << "No results yet.\n";
    }
}

void VotingSystem::showStatistics(int batch) const {
    heading("VOTING STATISTICS - BATCH " + std::to_string(batch));
    int totalStudents = 0, classVotes = 0, batchVotes = 0;
    for (const Student& student : students) if (student.getBatch() == batch) ++totalStudents;
    for (const VoteRecord& vote : votes) if (vote.batch == batch) {
        if (vote.electionType == "CLASS_EC") ++classVotes;
        else if (vote.electionType == "BATCH_REP") ++batchVotes;
    }
    double classTurnout = totalStudents ? 100.0 * classVotes / totalStudents : 0.0;
    double batchTurnout = totalStudents ? 100.0 * batchVotes / totalStudents : 0.0;
    std::cout << std::fixed << std::setprecision(1)
              << "Total Students            : " << totalStudents
              << "\nClass/Major EC Votes      : " << classVotes
              << "\nClass/Major EC Turnout    : " << classTurnout << "%"
              << "\nBatch Representative Votes: " << batchVotes
              << "\nBatch Turnout             : " << batchTurnout << "%\n";
}

void VotingSystem::showAbout() const {
    heading("ABOUT");
    std::cout << "University of Information Technology\n\nUIT Student Election Management System\n"
              << "A C++ semester-end group project.\n\nDeveloped by: Group 5\n";
    pauseScreen();
}

Student* VotingSystem::findStudent(const std::string& roll) {
    for (Student& student : students) if (student.getRollNumber() == roll) return &student;
    return nullptr;
}
const Student* VotingSystem::findStudent(const std::string& roll) const {
    for (const Student& student : students) if (student.getRollNumber() == roll) return &student;
    return nullptr;
}
bool VotingSystem::hasVoted(const std::string& roll, const std::string& type,
                            int batch, const std::string& group) const {
    for (const VoteRecord& vote : votes)
        if (vote.studentRoll == roll && vote.electionType == type && vote.batch == batch && vote.group == group) return true;
    return false;
}
int VotingSystem::voteCount(const std::string& type, int batch, const std::string& group,
                            const std::string& candidateRoll) const {
    int count = 0;
    for (const VoteRecord& vote : votes)
        if (vote.electionType == type && vote.batch == batch && vote.group == group && vote.candidateRoll == candidateRoll) ++count;
    return count;
}
ElectionPhase VotingSystem::phaseFor(int batch) const {
    if (batch < 9 || batch > 13 || phases.size() < 5) return ElectionPhase::NOT_STARTED;
    return phases[batch - 9];
}
void VotingSystem::setPhase(int batch, ElectionPhase phase) {
    if (phases.size() < 5) phases.assign(5, ElectionPhase::NOT_STARTED);
    phases[batch - 9] = phase;
    FileHandler::savePhases(phases);
}
