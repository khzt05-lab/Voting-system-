// ============================================================
//  Student.h  —  Student (voter) class declaration
// ============================================================
#pragma once
#include <string>
#include <iostream>

class Student {
private:
    std::string studentID;
    std::string name;
    std::string department;
    bool        hasVoted;
    int         votedForID;   // -1 = hasn't voted

public:
    // ── Constructors ──────────────────────────────────────────
    Student();
    Student(const std::string& id,
            const std::string& name,
            const std::string& dept);

    // ── Getters ───────────────────────────────────────────────
    std::string getStudentID()   const;
    std::string getName()        const;
    std::string getDepartment()  const;
    bool        getHasVoted()    const;
    int         getVotedForID()  const;

    // ── Setters ───────────────────────────────────────────────
    void setStudentID(const std::string& id);
    void setName(const std::string& name);
    void setDepartment(const std::string& dept);

    // ── Actions ───────────────────────────────────────────────
    void markVoted(int candidateId);

    // ── Display ───────────────────────────────────────────────
    void display() const;

    // ── Serialization for file I/O ────────────────────────────
    std::string serialize()                         const;
    static Student deserialize(const std::string& line);

    // ── Operators ─────────────────────────────────────────────
    bool operator==(const Student& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Student& s);
};
