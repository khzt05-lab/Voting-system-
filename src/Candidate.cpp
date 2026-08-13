// ============================================================
//  Candidate.cpp  —  Candidate class implementation
// ============================================================
#include "Candidate.h"
#include "colors.h"
#include <sstream>
#include <iomanip>
#include <iostream>

// ── Constructors ──────────────────────────────────────────────
Candidate::Candidate()
    : id(0), name(""), party(""), voteCount(0) {}

Candidate::Candidate(int id, const std::string& name, const std::string& party)
    : id(id), name(name), party(party), voteCount(0) {}

// ── Getters ───────────────────────────────────────────────────
int         Candidate::getId()        const { return id; }
std::string Candidate::getName()      const { return name; }
std::string Candidate::getParty()     const { return party; }
int         Candidate::getVoteCount() const { return voteCount; }

// ── Setters ───────────────────────────────────────────────────
void Candidate::setName(const std::string& n)  { name = n; }
void Candidate::setParty(const std::string& p) { party = p; }
void Candidate::setId(int i)                   { id = i; }
void Candidate::setVoteCount(int count)        { voteCount = count; }

// ── Actions ───────────────────────────────────────────────────
void Candidate::addVote()   { ++voteCount; }
void Candidate::resetVotes(){ voteCount = 0; }

// ── Display (detail view) ─────────────────────────────────────
void Candidate::display() const {
    std::cout << C_BR_YELLOW "  Candidate ID  : " C_RESET << id        << "\n"
              << C_BR_CYAN   "  Name          : " C_RESET << name      << "\n"
              << C_MAGENTA   "  Party         : " C_RESET << party     << "\n"
              << C_BR_GREEN  "  Votes         : " C_RESET << voteCount << "\n";
}

// ── Serialization ─────────────────────────────────────────────
// Format:  id|name|party|voteCount
std::string Candidate::serialize() const {
    return std::to_string(id) + "|" + name + "|" + party + "|" + std::to_string(voteCount);
}

Candidate Candidate::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, '|')) tokens.push_back(token);

    Candidate c;
    if (tokens.size() >= 4) {
        c.id        = std::stoi(tokens[0]);
        c.name      = tokens[1];
        c.party     = tokens[2];
        c.voteCount = std::stoi(tokens[3]);
    }
    return c;
}

// ── Operators ─────────────────────────────────────────────────
bool Candidate::operator>(const Candidate& o) const { return voteCount > o.voteCount; }
bool Candidate::operator<(const Candidate& o) const { return voteCount < o.voteCount; }
bool Candidate::operator==(const Candidate& o) const { return id == o.id; }

std::ostream& operator<<(std::ostream& os, const Candidate& c) {
    os << "Candidate #" << c.id << ": " << c.name
       << " [" << c.party << "] — " << c.voteCount << " votes";
    return os;
}
