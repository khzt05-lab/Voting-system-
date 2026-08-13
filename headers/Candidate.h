// ============================================================
//  Candidate.h  —  Candidate class declaration
// ============================================================
#pragma once
#include <string>
#include <iostream>
#include <vector>

class Candidate {
private:
    int         id;
    std::string name;
    std::string party;
    int         voteCount;

public:
    // ── Constructors ──────────────────────────────────────────
    Candidate();
    Candidate(int id, const std::string& name, const std::string& party);

    // ── Getters ───────────────────────────────────────────────
    int         getId()        const;
    std::string getName()      const;
    std::string getParty()     const;
    int         getVoteCount() const;

    // ── Setters ───────────────────────────────────────────────
    void setName(const std::string& name);
    void setParty(const std::string& party);
    void setId(int id);
    void setVoteCount(int count);

    // ── Actions ───────────────────────────────────────────────
    void addVote();
    void resetVotes();

    // ── Display ───────────────────────────────────────────────
    void display() const;       // simple info dump (used in detail view)

    // ── Serialization for file I/O ────────────────────────────
    std::string serialize()                          const;
    static Candidate deserialize(const std::string& line);

    // ── Operators ─────────────────────────────────────────────
    bool operator>(const Candidate& other) const;
    bool operator<(const Candidate& other) const;
    bool operator==(const Candidate& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Candidate& c);
};
