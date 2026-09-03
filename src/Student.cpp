#include "Student.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <vector>

Student::Student() : batch(0), year(0) {}

Student::Student(const std::string& roll, const std::string& studentName,
                 int studentBatch, int studentYear, const std::string& group)
    : rollNumber(roll), name(studentName), batch(studentBatch), year(studentYear),
      classOrMajor(group) {}

std::string Student::getRollNumber() const { return rollNumber; }
std::string Student::getName() const { return name; }
int Student::getBatch() const { return batch; }
int Student::getYear() const { return year; }
std::string Student::getClassOrMajor() const { return classOrMajor; }
void Student::setName(const std::string& studentName) { name = studentName; }
void Student::setClassOrMajor(const std::string& group) { classOrMajor = group; }

std::string Student::serialize() const {
    return rollNumber + "|" + name + "|" + std::to_string(batch) + "|" +
           std::to_string(year) + "|" + classOrMajor;
}

Student Student::deserialize(const std::string& line) {
    std::stringstream stream(line);
    std::vector<std::string> fields;
    std::string field;
    while (std::getline(stream, field, '|')) fields.push_back(field);
    if (fields.size() != 5) return Student();
    try {
        return Student(fields[0], fields[1], std::stoi(fields[2]),
                       std::stoi(fields[3]), fields[4]);
    } catch (...) {
        return Student();
    }
}

std::string normalizeRollNumber(const std::string& input) {
    std::string compact;
    for (char ch : input) {
        if (!std::isspace(static_cast<unsigned char>(ch)) && ch != '-') {
            compact += static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        }
    }
    if (compact.size() != 7 || compact.substr(0, 3) != "TNT") return "";
    for (std::size_t i = 3; i < compact.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(compact[i]))) return "";
    }
    return "TNT - " + compact.substr(3);
}

int extractRollNumber(const std::string& input) {
    std::string normalized = normalizeRollNumber(input);
    if (normalized.empty()) return -1;
    return std::stoi(normalized.substr(6));
}

int determineBatch(int number) {
    if (number >= 1600 && number <= 1799) return 9;
    if (number >= 1800 && number <= 1999) return 10;
    if (number >= 2000 && number <= 2199) return 11;
    if (number >= 2200 && number <= 2399) return 12;
    if (number >= 2400 && number <= 2600) return 13;
    return 0;
}

int determineYear(int batch) {
    if (batch >= 9 && batch <= 13) return 14 - batch;
    return 0;
}

bool validateRollNumber(const std::string& input) {
    return determineBatch(extractRollNumber(input)) != 0;
}

std::string yearLabel(int year) {
    if (year == 1) return "1st Year";
    if (year == 2) return "2nd Year";
    if (year == 3) return "3rd Year";
    if (year == 4) return "4th Year";
    if (year == 5) return "5th Year";
    return "Unknown Year";
}

static std::string upperTrimmed(std::string value) {
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.front()))) value.erase(value.begin());
    while (!value.empty() && std::isspace(static_cast<unsigned char>(value.back()))) value.pop_back();
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return std::toupper(c); });
    return value;
}

std::string normalizeGroup(int year, const std::string& group) {
    std::string value = upperTrimmed(group);
    if (year <= 3) {
        if (value.size() == 1 && value[0] >= 'A' && value[0] <= 'E') return value;
        if (value.size() == 7 && value.substr(0, 6) == "CLASS " && value[6] >= 'A' && value[6] <= 'E')
            return value.substr(6);
        return "";
    }

    if (value == "SE" || value == "SOFTWARE ENGINEERING") return "Software Engineering";
    if (value == "BIS" || value == "BUSINESS INFORMATION SYSTEMS") return "Business Information Systems";
    if (value == "KE" || value == "KNOWLEDGE ENGINEERING") return "Knowledge Engineering";
    if (value == "HPC" || value == "HIGH PERFORMANCE COMPUTING") return "High Performance Computing";
    if (value == "ES" || value == "EMBEDDED SYSTEMS") return "Embedded Systems";
    if (value == "CS" || value == "CYBER SECURITY") return "Cyber Security";
    if (value == "CN" || value == "COMMUNICATION AND NETWORKING") return "Communication and Networking";
    return "";
}

bool isValidGroup(int year, const std::string& group) {
    return !normalizeGroup(year, group).empty();
}

