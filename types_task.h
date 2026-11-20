#pragma once
#include <string>
#include <vector>

enum class Faculty { IT, Economics, Law, Other };
struct Date { int day, month, year; };
struct Student {
    int id;
    std::string name;
    Date birthDate;
    Faculty faculty;
    double avgGrade;
};
using StudentList = std::vector<Student>;
std::string toString(Faculty faculty);
void studentsMenu();
