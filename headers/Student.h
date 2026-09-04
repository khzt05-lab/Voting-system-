#pragma once

#include <string>

// A registered UIT voter.  Batch and year are derived from the roll number;
// only the class/major is stored in the registry.
class Student {
private:
    std::string rollNumber;
    std::string name;
    std::string group;

public:
    Student();
    Student(const std::string& roll, const std::string& name,
            const std::string& classOrMajor);

    const std::string& getRollNumber() const;
    const std::string& getStudentID() const; // compatibility with the old UI
    const std::string& getName() const;
    const std::string& getGroup() const;
    std::string getDepartment() const;       // compatibility with old data
    int getBatch() const;
    int getYear() const;
    std::string getYearLabel() const;

    static std::string normalizeRoll(const std::string& roll);
    static bool isValidRoll(const std::string& roll);
    static int batchFromRoll(const std::string& roll);
    static int yearFromRoll(const std::string& roll);
    static bool isValidGroupForYear(const std::string& group, int year);

    std::string serialize() const;
    static Student deserialize(const std::string& line);
};
