#pragma once

#include <string>

class Student {
private:
    std::string rollNumber;
    std::string name;
    int batch;
    int year;
    std::string classOrMajor;

public:
    Student();
    Student(const std::string& roll, const std::string& studentName,
            int studentBatch, int studentYear, const std::string& group);

    std::string getRollNumber() const;
    std::string getName() const;
    int getBatch() const;
    int getYear() const;
    std::string getClassOrMajor() const;

    void setName(const std::string& studentName);
    void setClassOrMajor(const std::string& group);

    std::string serialize() const;
    static Student deserialize(const std::string& line);
};

std::string normalizeRollNumber(const std::string& input);
bool validateRollNumber(const std::string& input);
int extractRollNumber(const std::string& input);
int determineBatch(int number);
int determineYear(int batch);
std::string yearLabel(int year);
bool isValidGroup(int year, const std::string& group);
std::string normalizeGroup(int year, const std::string& group);

