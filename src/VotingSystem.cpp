// ============================================================
//  VotingSystem.cpp  —  Core controller: all 10 sections
// ============================================================
#include "VotingSystem.h"
#include "ui.h"
#include "colors.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>

// ════════════════════════════════════════════════════════════
//  SECTION 10: Constructor & Entry Point (Final Integration)
// ════════════════════════════════════════════════════════════

VotingSystem::VotingSystem()
    : admin("admin", "admin123"),
      isRunning(true),
      nextCandidateId(1)
{
    enableANSI();
    loadData();

    // Recalculate next unique candidate ID after load
    for (const auto& c : candidates)
        if (c.getId() >= nextCandidateId)
            nextCandidateId = c.getId() + 1;
}

void VotingSystem::run() {
    showMainMenu();
}

// ── Persistence Helpers ───────────────────────────────────────
void VotingSystem::loadData() {
    FileHandler::loadCandidates(candidates);
    FileHandler::loadStudents(students);
}

void VotingSystem::saveData() {
    FileHandler::saveCandidates(candidates);
    FileHandler::saveStudents(students);
}

// ── Lookup Helpers ────────────────────────────────────────────
Candidate* VotingSystem::findCandidateById(int id) {
    for (auto& c : candidates)
        if (c.getId() == id) return &c;
    return nullptr;
}

Student* VotingSystem::findStudentById(const std::string& id) {
    for (auto& s : students)
        if (s.getStudentID() == id) return &s;
    return nullptr;
}

bool VotingSystem::candidateExists(int id) const {
    for (const auto& c : candidates)
        if (c.getId() == id) return true;
    return false;
}

bool VotingSystem::studentExists(const std::string& id) const {
    for (const auto& s : students)
        if (s.getStudentID() == id) return true;
    return false;
}

int VotingSystem::getTotalVotes() const {
    int total = 0;
    for (const auto& c : candidates) total += c.getVoteCount();
    return total;
}

// ════════════════════════════════════════════════════════════
//  SECTION 1: Main Menu
// ════════════════════════════════════════════════════════════

void VotingSystem::showMainMenu() {
    while (isRunning) {
        printBanner();

        drawTitleBox("  MAIN MENU  ");

        std::vector<std::string> opts = {
            "Admin Login / Panel",
            "Student Registration",
            "Voting Booth",
            "View Live Results",
            "Calculate Winner",
            "Save & Exit"
        };

        drawMenu(opts);

        // Status bar
        drawMidBorder();
        std::string sb1 = std::string(C_DIM C_WHITE) + "  Candidates: " + C_RESET
                        + C_BR_YELLOW + std::to_string(candidates.size()) + C_RESET;
        std::string sb2 = std::string(C_DIM C_WHITE) + "  Students: " + C_RESET
                        + C_BR_CYAN + std::to_string(students.size()) + C_RESET;
        std::string sb3 = std::string(C_DIM C_WHITE) + "  Votes Cast: " + C_RESET
                        + C_BR_GREEN + std::to_string(getTotalVotes()) + C_RESET;
        int sbVis = 14 + (int)std::to_string(candidates.size()).size()
                  + 12 + (int)std::to_string(students.size()).size()
                  + 14 + (int)std::to_string(getTotalVotes()).size();
        drawBoxRow(sb1 + "   " + sb2 + "   " + sb3, sbVis + 6);
        drawBottomBorder();

        std::cout << "\n";
        int choice = getIntInput("Select an option [1-6]: ", 1, 6);

        switch (choice) {
            case 1: adminLogin();          break;
            case 2: studentRegistration(); break;
            case 3: votingBooth();         break;
            case 4: showResults();         break;
            case 5: calculateWinner();     break;
            case 6:
                saveData();
                clearScreen();
                std::cout << "\n\n";
                drawTitleBox("  GOODBYE  ");
                drawEmptyRow();
                {
                    std::string m1 = "Thank you for using the Voting System!";
                    std::string m2 = "All data has been saved successfully.";
                    drawBoxRow(C_BOLD C_BR_GREEN + m1 + C_RESET, (int)m1.size(), W, "center");
                    drawEmptyRow();
                    drawBoxRow(C_BR_CYAN + m2 + C_RESET, (int)m2.size(), W, "center");
                }
                drawEmptyRow();
                drawBottomBorder();
                std::cout << "\n";
                isRunning = false;
                break;
        }
    }
}

// ════════════════════════════════════════════════════════════
//  SECTION 2: Admin Login
// ════════════════════════════════════════════════════════════

void VotingSystem::adminLogin() {
    if (admin.isAuthenticated()) {
        adminPanel();
        return;
    }

    if (admin.isLocked()) {
        clearScreen();
        drawTitleBox("  ADMIN LOGIN  ");
        drawEmptyRow();
        drawBoxRow(C_BR_RED C_BOLD "  ACCOUNT LOCKED" C_RESET, 16, W, "center");
        drawEmptyRow();
        std::string msg = "Too many failed attempts. Restart the application.";
        drawBoxRow(C_RED + msg + C_RESET, (int)msg.size(), W, "center");
        drawEmptyRow();
        drawBottomBorder();
        pause();
        return;
    }

    clearScreen();
    drawTitleBox("  ADMIN LOGIN  ");
    drawEmptyRow();
    {
        std::string hint = "Default credentials:  admin / admin123";
        drawBoxRow(C_DIM C_WHITE + hint + C_RESET, (int)hint.size());
    }
    drawEmptyRow();
    drawMidBorder();
    drawEmptyRow();

    std::string user = getStringInput("Username: ");
    std::string pass = getMaskedInput("Password: ");

    drawEmptyRow();

    if (admin.login(user, pass)) {
        printSuccess("Login successful!  Welcome, " + admin.getUsername() + ".");
        pause();
        adminPanel();
    } else {
        if (admin.isLocked()) {
            printError("Account LOCKED after " + std::to_string(admin.getFailedAttempts())
                       + " failed attempts.");
        } else {
            printError("Invalid credentials!  "
                       + std::to_string(admin.getRemainingAttempts())
                       + " attempt(s) remaining.");
        }
        drawBottomBorder();
        pause();
    }
}

// ════════════════════════════════════════════════════════════
//  SECTION 3: Admin Panel  (Admin-only gateway)
// ════════════════════════════════════════════════════════════

void VotingSystem::adminPanel() {
    bool inPanel = true;
    while (inPanel) {
        clearScreen();
        drawTitleBox("  ADMIN PANEL  ");

        std::string welc = "Logged in as: " + admin.getUsername();
        drawBoxRow(C_DIM C_GREEN + welc + C_RESET, (int)welc.size());
        drawMidBorder();

        std::vector<std::string> opts = {
            "Candidate Management",
            "View All Students",
            "Reset All Votes",
            "Save Data Now",
            "Logout"
        };
        drawMenu(opts);
        drawBottomBorder();

        std::cout << "\n";
        int c = getIntInput("Select an option [1-5]: ", 1, 5);

        switch (c) {
            case 1: candidateManagement(); break;
            case 2: {
                // View all students
                clearScreen();
                drawTitleBox("  REGISTERED STUDENTS  ");

                // Table header
                std::string hdr = C_BOLD C_BR_CYAN
                    "  Student ID        Name                  Department          Voted?" C_RESET;
                drawBoxRow(hdr, 62);
                drawMidBorder();

                if (students.empty()) {
                    std::string nm = "No students registered yet.";
                    drawBoxRow(C_BR_YELLOW + nm + C_RESET, (int)nm.size(), W, "center");
                } else {
                    for (const auto& s : students) {
                        std::string idCol   = s.getStudentID();
                        std::string nmCol   = s.getName();
                        std::string deptCol = s.getDepartment();

                        while ((int)idCol.size()   < 18) idCol   += ' ';
                        while ((int)nmCol.size()   < 22) nmCol   += ' ';
                        while ((int)deptCol.size() < 20) deptCol += ' ';

                        std::string vCol = s.getHasVoted()
                            ? C_BR_GREEN "  Yes" C_RESET
                            : C_BR_RED   "  No " C_RESET;

                        std::string row = C_BR_YELLOW "  " + idCol + C_RESET
                                        + C_BR_WHITE  + nmCol + C_RESET
                                        + C_MAGENTA   + deptCol + C_RESET
                                        + vCol;
                        drawBoxRow(row, 2 + 18 + 22 + 20 + 5);
                    }
                }
                drawBottomBorder();
                pause();
                break;
            }
            case 3: resetAllVotes(); break;
            case 4:
                saveData();
                printSuccess("Data saved to disk.");
                pause();
                break;
            case 5:
                admin.logout();
                printInfo("Logged out.");
                pause();
                inPanel = false;
                break;
        }
    }
}

// ════════════════════════════════════════════════════════════
//  SECTION 4: Candidate Management
// ════════════════════════════════════════════════════════════

void VotingSystem::candidateManagement() {
    bool inMenu = true;
    while (inMenu) {
        clearScreen();
        drawTitleBox("  CANDIDATE MANAGEMENT  ");

        std::vector<std::string> opts = {
            "Add Candidate",
            "Edit Candidate",
            "Remove Candidate",
            "List All Candidates",
            "Back to Admin Panel"
        };
        drawMenu(opts);
        drawBottomBorder();

        std::cout << "\n";
        int c = getIntInput("Select an option [1-5]: ", 1, 5);

        switch (c) {
            case 1: addCandidate();    break;
            case 2: editCandidate();   break;
            case 3: removeCandidate(); break;
            case 4: listCandidates();  break;
            case 5: inMenu = false;    break;
        }
    }
}

void VotingSystem::addCandidate() {
    clearScreen();
    drawTitleBox("  ADD CANDIDATE  ");
    drawEmptyRow();

    std::string name  = getStringInput("Candidate Name  : ");
    std::string party = getStringInput("Party / Affiliation: ");

    drawEmptyRow();

    // Confirm
    std::cout << C_BOLD C_BR_CYAN "  Confirm adding:\n" C_RESET;
    std::cout << C_BR_WHITE "    Name : " C_RESET << name  << "\n";
    std::cout << C_MAGENTA  "    Party: " C_RESET << party << "\n\n";

    std::cout << C_BR_YELLOW "  Confirm? [y/n]: " C_RESET;
    std::string yn;
    std::getline(std::cin, yn);

    if (yn == "y" || yn == "Y") {
        candidates.emplace_back(nextCandidateId++, name, party);
        printSuccess("Candidate \"" + name + "\" added with ID #"
                     + std::to_string(nextCandidateId - 1) + ".");
    } else {
        printWarning("Addition cancelled.");
    }
    drawBottomBorder();
    pause();
}

void VotingSystem::editCandidate() {
    clearScreen();
    drawTitleBox("  EDIT CANDIDATE  ");
    drawEmptyRow();

    if (candidates.empty()) {
        printWarning("No candidates to edit.");
        drawBottomBorder();
        pause();
        return;
    }

    listCandidates();

    int id = getIntInput("Enter Candidate ID to edit: ", 1, 9999);
    Candidate* c = findCandidateById(id);
    if (!c) {
        printError("Candidate ID #" + std::to_string(id) + " not found.");
        pause();
        return;
    }

    std::cout << "\n" C_DIM "  (Leave blank to keep current value)\n" C_RESET;
    std::cout << C_BR_CYAN "  -> " C_RESET "New Name  [" << c->getName()  << "]: ";
    std::string newName; std::getline(std::cin, newName);
    std::cout << C_BR_CYAN "  -> " C_RESET "New Party [" << c->getParty() << "]: ";
    std::string newParty; std::getline(std::cin, newParty);

    if (!newName.empty())  c->setName(newName);
    if (!newParty.empty()) c->setParty(newParty);

    printSuccess("Candidate #" + std::to_string(id) + " updated.");
    pause();
}

void VotingSystem::removeCandidate() {
    clearScreen();
    drawTitleBox("  REMOVE CANDIDATE  ");
    drawEmptyRow();

    if (candidates.empty()) {
        printWarning("No candidates to remove.");
        drawBottomBorder();
        pause();
        return;
    }

    listCandidates();

    int id = getIntInput("Enter Candidate ID to remove: ", 1, 9999);
    auto it = std::find_if(candidates.begin(), candidates.end(),
                           [id](const Candidate& c){ return c.getId() == id; });

    if (it == candidates.end()) {
        printError("Candidate ID #" + std::to_string(id) + " not found.");
        pause();
        return;
    }

    std::cout << C_BR_RED "\n  Remove \"" C_RESET << it->getName()
              << C_BR_RED "\"? [y/n]: " C_RESET;
    std::string yn; std::getline(std::cin, yn);

    if (yn == "y" || yn == "Y") {
        std::string nm = it->getName();
        candidates.erase(it);
        printSuccess("Candidate \"" + nm + "\" removed.");
    } else {
        printWarning("Removal cancelled.");
    }
    pause();
}

void VotingSystem::listCandidates() {
    clearScreen();
    drawTitleBox("  CANDIDATE LIST  ");

    // Table header row  (ID=6, Name=22, Party=22, Votes=8)
    std::string hdr = C_BOLD C_BR_CYAN
        "  #     Name                   Party                  Votes" C_RESET;
    drawBoxRow(hdr, 58);
    drawMidBorder();

    if (candidates.empty()) {
        std::string nm = "No candidates registered yet.";
        drawEmptyRow();
        drawBoxRow(C_BR_YELLOW + nm + C_RESET, (int)nm.size(), W, "center");
        drawEmptyRow();
    } else {
        for (const auto& c : candidates) {
            std::string idCol   = std::to_string(c.getId());
            std::string nmCol   = c.getName();
            std::string parCol  = c.getParty();
            std::string vtCol   = std::to_string(c.getVoteCount());

            while ((int)idCol.size()  <  4) idCol  += ' ';
            while ((int)nmCol.size()  < 22) nmCol  += ' ';
            while ((int)parCol.size() < 22) parCol += ' ';
            while ((int)vtCol.size()  <  5) vtCol  += ' ';

            std::string row = C_BR_YELLOW "  " + idCol + C_RESET + "  "
                            + C_BR_WHITE  + nmCol + C_RESET + " "
                            + C_MAGENTA   + parCol + C_RESET + " "
                            + C_BR_GREEN  + vtCol + C_RESET;
            drawBoxRow(row, 2 + 4 + 2 + 22 + 1 + 22 + 1 + 5);
        }
    }
    drawBottomBorder();
    pause();
}

void VotingSystem::resetAllVotes() {
    clearScreen();
    drawTitleBox("  RESET ALL VOTES  ");
    drawEmptyRow();
    drawBoxRow(C_BOLD C_BR_RED "  WARNING: This will erase ALL votes!" C_RESET, 38, W, "center");
    drawEmptyRow();
    std::cout << C_BR_RED "  Are you absolutely sure? [yes/no]: " C_RESET;
    std::string yn; std::getline(std::cin, yn);

    if (yn == "yes") {
        for (auto& c : candidates) c.resetVotes();
        for (auto& s : students)   s.markVoted(-1); // will set hasVoted=false in a helper

        // Mark students un-voted
        // (Student::markVoted(-1) still sets hasVoted=true — fix with a dedicated reset)
        // Re-create students with same data but un-voted
        for (auto& s : students) {
            // Hack: deserialize a fresh version
            Student fresh(s.getStudentID(), s.getName(), s.getDepartment());
            s = fresh;
        }
        printSuccess("All votes have been reset.");
    } else {
        printWarning("Reset cancelled.");
    }
    pause();
}

// ════════════════════════════════════════════════════════════
//  SECTION 5: Student Registration
// ════════════════════════════════════════════════════════════

void VotingSystem::studentRegistration() {
    clearScreen();
    drawTitleBox("  STUDENT REGISTRATION  ");
    drawEmptyRow();

    // Prompt
    std::string id   = getStringInput("Student ID   : ");
    if (studentExists(id)) {
        printError("Student ID \"" + id + "\" is already registered.");
        drawBottomBorder();
        pause();
        return;
    }

    std::string name = getStringInput("Full Name    : ");
    std::string dept = getStringInput("Department   : ");

    drawEmptyRow();
    std::cout << C_BOLD C_BR_CYAN "  Confirm registration:\n" C_RESET
              << C_BR_YELLOW "    ID     : " C_RESET << id   << "\n"
              << C_BR_WHITE  "    Name   : " C_RESET << name << "\n"
              << C_MAGENTA   "    Dept   : " C_RESET << dept << "\n\n";

    std::cout << C_BR_YELLOW "  Confirm? [y/n]: " C_RESET;
    std::string yn; std::getline(std::cin, yn);

    if (yn == "y" || yn == "Y") {
        students.emplace_back(id, name, dept);
        printSuccess("Student \"" + name + "\" registered successfully!");
    } else {
        printWarning("Registration cancelled.");
    }
    drawBottomBorder();
    pause();
}

// ════════════════════════════════════════════════════════════
//  SECTION 6: Voting Booth
// ════════════════════════════════════════════════════════════

void VotingSystem::votingBooth() {
    clearScreen();
    drawTitleBox("  VOTING BOOTH  ");
    drawEmptyRow();

    if (candidates.empty()) {
        printWarning("No candidates are registered yet. Please ask the admin to add candidates.");
        drawBottomBorder();
        pause();
        return;
    }

    // Step 1: Identify voter
    std::string sid = getStringInput("Enter your Student ID: ");
    Student* voter  = findStudentById(sid);

    if (!voter) {
        printError("Student ID \"" + sid + "\" is not registered.");
        printInfo("Please register first via option 2 from the Main Menu.");
        drawBottomBorder();
        pause();
        return;
    }

    if (voter->getHasVoted()) {
        printWarning("You have already cast your vote! Each student may vote only once.");
        drawBottomBorder();
        pause();
        return;
    }

    // Step 2: Show ballot
    drawEmptyRow();
    std::string welcome = "Welcome, " + voter->getName() + "!";
    drawBoxRow(C_BOLD C_BR_CYAN + welcome + C_RESET, (int)welcome.size());
    drawEmptyRow();

    drawMidBorder();
    {
        std::string title = "BALLOT — Select your candidate:";
        drawBoxRow(C_BOLD C_BR_YELLOW + title + C_RESET, (int)title.size());
    }
    drawMidBorder();

    // Candidate list as ballot
    for (size_t i = 0; i < candidates.size(); ++i) {
        std::string num  = "  [" + std::to_string(candidates[i].getId()) + "]  ";
        std::string line = C_BR_CYAN + num + C_RESET
                         + C_BR_WHITE + candidates[i].getName() + C_RESET
                         + C_DIM " — " C_RESET
                         + C_MAGENTA + candidates[i].getParty() + C_RESET;
        int vl = (int)num.size() + (int)candidates[i].getName().size()
                 + 3 + (int)candidates[i].getParty().size();
        drawBoxRow(line, vl);
    }
    drawEmptyRow();
    drawBottomBorder();

    // Step 3: Cast vote
    std::cout << "\n";
    int chosenId = getIntInput("Enter Candidate ID to vote for: ", 1, 9999);
    Candidate* chosen = findCandidateById(chosenId);

    if (!chosen) {
        printError("Candidate ID #" + std::to_string(chosenId) + " does not exist.");
        pause();
        return;
    }

    // Confirm
    std::cout << C_BR_YELLOW "\n  You are voting for: " C_RESET
              << C_BOLD C_BR_WHITE << chosen->getName() << C_RESET
              << C_MAGENTA " (" << chosen->getParty() << ")" C_RESET "\n";
    std::cout << C_BR_YELLOW "  Confirm vote? [y/n]: " C_RESET;
    std::string yn; std::getline(std::cin, yn);

    if (yn == "y" || yn == "Y") {
        voter->markVoted(chosenId);
        chosen->addVote();
        saveData();   // Immediate save after each vote for safety

        std::cout << "\n";
        drawTopBorder();
        {
            std::string msg = "VOTE CAST SUCCESSFULLY!";
            drawBoxRow(C_BOLD C_BR_GREEN + msg + C_RESET, (int)msg.size(), W, "center");
        }
        drawMidBorder();
        {
            std::string msg2 = "Thank you, " + voter->getName() + ", for participating!";
            drawBoxRow(C_BR_CYAN + msg2 + C_RESET, (int)msg2.size(), W, "center");
        }
        drawEmptyRow();
        drawBottomBorder();
    } else {
        printWarning("Vote cancelled. You may vote again.");
    }
    pause();
}

// ════════════════════════════════════════════════════════════
//  SECTION 7 & 8: Results + Winner Calculation
// ════════════════════════════════════════════════════════════

void VotingSystem::showResults() {
    clearScreen();
    drawTitleBox("  LIVE VOTE RESULTS  ");

    int total = getTotalVotes();

    if (candidates.empty()) {
        printWarning("No candidates registered.");
        drawBottomBorder();
        pause();
        return;
    }

    // Sort a copy descending by votes
    std::vector<Candidate> sorted = candidates;
    std::sort(sorted.begin(), sorted.end(),
              [](const Candidate& a, const Candidate& b){ return a > b; });

    int maxVotes = sorted.empty() ? 1 : std::max(sorted[0].getVoteCount(), 1);

    {
        std::string ts = "Total votes cast: " + std::to_string(total)
                       + "   |   Registered students: " + std::to_string(students.size());
        drawBoxRow(C_DIM C_WHITE + ts + C_RESET, (int)ts.size());
    }
    drawMidBorder();
    drawEmptyRow();

    for (size_t i = 0; i < sorted.size(); ++i) {
        const auto& c = sorted[i];
        double pct = (total > 0)
            ? (100.0 * c.getVoteCount() / total) : 0.0;

        // Rank badge
        std::string badge = (i == 0 && c.getVoteCount() > 0)
            ? std::string(C_BR_YELLOW) + "#1 " + C_RESET
            : std::string(C_DIM C_WHITE) + "#" + std::to_string(i+1) + " " C_RESET;

        std::cout << "  " << badge;
        printBarRow(c.getName(), c.getVoteCount(), maxVotes, pct);
        std::cout << "     " C_DIM C_WHITE << std::left << std::setw(22)
                  << (" " + c.getParty()) << C_RESET "\n\n";
    }

    drawBottomBorder();
    pause();
}

void VotingSystem::calculateWinner() {
    clearScreen();
    drawTitleBox("  WINNER CALCULATION  ");

    int total = getTotalVotes();

    if (candidates.empty()) {
        printWarning("No candidates registered.");
        drawBottomBorder();
        pause();
        return;
    }
    if (total == 0) {
        printWarning("No votes have been cast yet.");
        drawBottomBorder();
        pause();
        return;
    }

    // Find maximum votes
    int maxVotes = 0;
    for (const auto& c : candidates)
        if (c.getVoteCount() > maxVotes) maxVotes = c.getVoteCount();

    // Collect all candidates with max votes (tie detection)
    std::vector<const Candidate*> winners;
    for (const auto& c : candidates)
        if (c.getVoteCount() == maxVotes) winners.push_back(&c);

    drawEmptyRow();

    if (winners.size() > 1) {
        // ── TIE ───────────────────────────────────────────────
        std::string tieMsg = "TIE DETECTED!  " + std::to_string(winners.size()) + " candidates are tied.";
        drawBoxRow(C_BOLD C_BR_YELLOW + tieMsg + C_RESET, (int)tieMsg.size(), W, "center");
        drawEmptyRow();
        drawMidBorder();
        for (const auto* w : winners) {
            std::string row = std::string(C_BOLD C_BR_WHITE) + "  -> " + std::string(C_RESET)
                            + std::string(C_BR_YELLOW) + w->getName() + std::string(C_RESET)
                            + " (" + w->getParty() + ")"
                            + std::string(C_BR_GREEN) + "  -  " + std::to_string(w->getVoteCount()) + " votes" + std::string(C_RESET);

            int rv = 5 + (int)w->getName().size() + 2 + (int)w->getParty().size() + 1
                   + 5 + (int)std::to_string(w->getVoteCount()).size() + 6;
            drawBoxRow(row, rv);
        }
    } else {
        // ── WINNER ────────────────────────────────────────────
        const Candidate* w = winners[0];
        double pct = 100.0 * w->getVoteCount() / total;

        char pctBuf[16];
        snprintf(pctBuf, sizeof(pctBuf), "%.1f%%", pct);

        // Trophy row
        std::string trophy = "\xE2\x98\x85 \xE2\x98\x85 \xE2\x98\x85  WINNER  \xE2\x98\x85 \xE2\x98\x85 \xE2\x98\x85";
        drawBoxRow(C_BOLD C_BR_YELLOW + trophy + C_RESET, 21, W, "center");
        drawMidBorder();
        drawEmptyRow();

        std::string nameRow  = w->getName();
        std::string partyRow = w->getParty();
        std::string voteRow  = std::to_string(w->getVoteCount())
                             + " votes  (" + pctBuf + ")";

        drawBoxRow(C_BOLD C_BR_WHITE + nameRow  + C_RESET, (int)nameRow.size(), W, "center");
        drawBoxRow(C_MAGENTA         + partyRow + C_RESET, (int)partyRow.size(), W, "center");
        drawEmptyRow();
        drawBoxRow(C_BOLD C_BR_GREEN + voteRow  + C_RESET, (int)voteRow.size(), W, "center");
        drawEmptyRow();

        // Export to file
        FileHandler::exportResults(candidates, w->getName(), total);
        std::string saved = "Results saved to: data/results.txt";
        drawBoxRow(C_DIM C_CYAN + saved + C_RESET, (int)saved.size(), W, "center");
    }

    drawEmptyRow();
    drawBottomBorder();
    pause();
}
