#include "Student.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

namespace {
const std::vector<std::string> MAJORS = {
    "Software Engineering", "Business Information Systems",
    "Knowledge Engineering", "High Performance Computing",
    "Embedded Systems", "Cyber Security", "Communication and Networking"
};
}

Student::Student() = default;

Student::Student(const std::string& roll, const std::string& studentName,
                 const std::string& classOrMajor)
    : rollNumber(normalizeRoll(roll)), name(studentName), group(classOrMajor) {}

const std::string& Student::getRollNumber() const { return rollNumber; }
const std::string& Student::getStudentID() const { return rollNumber; }
const std::string& Student::getName() const { return name; }
const std::string& Student::getGroup() const { return group; }
std::string Student::getDepartment() const { return group; }

std::string Student::normalizeRoll(const std::string& value) {
    std::string compact;
    for (unsigned char ch : value) {
        if (!std::isspace(ch)) compact += static_cast<char>(std::toupper(ch));
    }
    if (compact.size() == 8 && compact.substr(0, 3) == "TNT" &&
        compact[3] == '-') {
        return compact.substr(0, 3) + " - " + compact.substr(4);
    }
    return compact;
}

bool Student::isValidRoll(const std::string& value) {
    const std::string roll = normalizeRoll(value);
    if (roll.size() != 10 || roll.substr(0, 6) != "TNT - ") return false;
    for (std::size_t i = 6; i < 10 && i < roll.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(roll[i]))) return false;
    const int number = std::stoi(roll.substr(6));
    return number >= 1600 && number <= 2600;
}

int Student::batchFromRoll(const std::string& value) {
    if (!isValidRoll(value)) return 0;
    const int n = std::stoi(normalizeRoll(value).substr(6));
    if (n <= 1799) return 9;
    if (n <= 1999) return 10;
    if (n <= 2199) return 11;
    if (n <= 2399) return 12;
    return 13;
}

int Student::yearFromRoll(const std::string& value) {
    const int batch = batchFromRoll(value);
    return batch == 0 ? 0 : 14 - batch;
}

int Student::getBatch() const { return batchFromRoll(rollNumber); }
int Student::getYear() const { return yearFromRoll(rollNumber); }
std::string Student::getYearLabel() const {
    static const char* labels[] = {"", "1st Year", "2nd Year", "3rd Year",
                                    "4th Year", "5th Year"};
    return getYear() >= 1 && getYear() <= 5 ? labels[getYear()] : "Unknown";
}

bool Student::isValidGroupForYear(const std::string& value, int year) {
    if (year >= 1 && year <= 3)
        return value.size() == 1 && value[0] >= 'A' && value[0] <= 'E';
    if (year == 4 || year == 5)
        return std::find(MAJORS.begin(), MAJORS.end(), value) != MAJORS.end();
    return false;
}

std::string Student::serialize() const {
    return rollNumber + "|" + name + "|" + group;
}

Student Student::deserialize(const std::string& line) {
    std::stringstream input(line);
    std::vector<std::string> fields;
    std::string field;
    while (std::getline(input, field, '|')) fields.push_back(field);
    if (fields.size() < 3) return {};
    return Student(fields[0], fields[1], fields[2]);
}
