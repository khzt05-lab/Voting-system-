// ============================================================
//  Student.cpp  —  Student class implementation
// ============================================================
#include "Student.h"
#include "colors.h"
#include <sstream>
#include <vector>
#include <iostream>

// ── Constructors ──────────────────────────────────────────────
Student::Student()
    : studentID(""), name(""), department(""), hasVoted(false), votedForID(-1) {}

Student::Student(const std::string& id, const std::string& name, const std::string& dept)
    : studentID(id), name(name), department(dept), hasVoted(false), votedForID(-1) {}

// ── Getters ───────────────────────────────────────────────────
std::string Student::getStudentID()   const { return studentID; }
std::string Student::getName()        const { return name; }
std::string Student::getDepartment()  const { return department; }
bool        Student::getHasVoted()    const { return hasVoted; }
int         Student::getVotedForID()  const { return votedForID; }

// ── Setters ───────────────────────────────────────────────────
void Student::setStudentID(const std::string& id)   { studentID = id; }
void Student::setName(const std::string& n)          { name = n; }
void Student::setDepartment(const std::string& dept) { department = dept; }

// ── Actions ───────────────────────────────────────────────────
void Student::markVoted(int candidateId) {
    hasVoted   = true;
    votedForID = candidateId;
}

// ── Display (detail view) ─────────────────────────────────────
void Student::display() const {
    std::string votedStatus = hasVoted
        ? std::string(C_BR_GREEN "Yes (Candidate #") + std::to_string(votedForID) + ")" C_RESET
        : std::string(C_BR_RED   "No" C_RESET);

    std::cout << C_BR_YELLOW "  Student ID    : " C_RESET << studentID  << "\n"
              << C_BR_CYAN   "  Name          : " C_RESET << name       << "\n"
              << C_MAGENTA   "  Department    : " C_RESET << department << "\n"
              << C_BR_WHITE  "  Has Voted     : " C_RESET << votedStatus << "\n";
}

// ── Serialization ─────────────────────────────────────────────
// Format:  studentID|name|department|hasVoted|votedForID
std::string Student::serialize() const {
    return studentID + "|"
         + name + "|"
         + department + "|"
         + (hasVoted ? "1" : "0") + "|"
         + std::to_string(votedForID);
}

Student Student::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, '|')) tokens.push_back(token);

    Student s;
    if (tokens.size() >= 5) {
        s.studentID  = tokens[0];
        s.name       = tokens[1];
        s.department = tokens[2];
        s.hasVoted   = (tokens[3] == "1");
        s.votedForID = std::stoi(tokens[4]);
    }
    return s;
}

// ── Operators ─────────────────────────────────────────────────
bool Student::operator==(const Student& o) const { return studentID == o.studentID; }

std::ostream& operator<<(std::ostream& os, const Student& s) {
    os << "Student [" << s.studentID << "]: " << s.name
       << " | " << s.department
       << " | Voted: " << (s.hasVoted ? "Yes" : "No");
    return os;
}
