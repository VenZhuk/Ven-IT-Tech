#include "types_task.h"
#include "menu_utils.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <array>
#include <stdexcept>

std::string toString(Faculty f) {
    switch (f) {
        case Faculty::IT: return "IT";
        case Faculty::Economics: return "Economics";
        case Faculty::Law: return "Law";
        default: return "Other";
    }
}
static bool leap(int y) { return y % 400 == 0 || (y % 4 == 0 && y % 100 != 0); }
static bool validDate(Date d) {
    if (d.year < 1900 || d.year > 2100 || d.month < 1 || d.month > 12) return false;
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (leap(d.year)) days[1] = 29;
    return d.day >= 1 && d.day <= days[d.month-1];
}
static Date readDate() {
    while (true) {
        Date d;
        d.day = readInt("День (1-31): ", 1, 31);
        d.month = readInt("Місяць (1-12): ", 1, 12);
        d.year = readInt("Рік (1900-2100): ", 1900, 2100);
        if (validDate(d)) return d;
        std::cout << "Такої календарної дати не існує. Спробуйте знову.\n";
    }
}
static Faculty readFaculty() {
    std::cout << "Факультет: 1 - IT, 2 - Economics, 3 - Law, 4 - Other\n";
    return static_cast<Faculty>(readInt("Номер: ", 1, 4) - 1);
}
static Student* findId(StudentList& a, int id) {
    auto it = std::find_if(a.begin(), a.end(), [id](const Student& s) { return s.id == id; });
    return it == a.end() ? nullptr : &*it;
}
static std::string lowerAscii(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}
static std::string surname(const std::string& name) {
    auto pos = name.find_first_of(" \t");
    return lowerAscii(name.substr(0, pos)); // Прізвище — перше слово ПІБ
}
static void header() {
    std::cout << std::left << std::setw(7) << "ID" << std::setw(32) << "ПІБ"
              << std::setw(14) << "Народження" << std::setw(14) << "Факультет"
              << "Сер. бал\n" << std::string(80, '-') << '\n';
}
static void printStudent(const Student& s) {
    std::string date = std::to_string(s.birthDate.day) + "." + std::to_string(s.birthDate.month) + "." + std::to_string(s.birthDate.year);
    // setw вимірює байти UTF-8, тому кирилиця може мати нерівні відступи у консолі.
    std::cout << std::left << std::setw(7) << s.id << std::setw(32) << s.name.substr(0, 30)
              << std::setw(14) << date << std::setw(14) << toString(s.faculty)
              << std::fixed << std::setprecision(2) << s.avgGrade << '\n';
}
static void showAll(const StudentList& a) {
    if (a.empty()) { std::cout << "Список студентів порожній.\n"; return; }
    header();
    for (const auto& s : a) printStudent(s);
}
static void add(StudentList& a) {
    int id = readInt("ID (1-1000000): ", 1, 1000000);
    if (findId(a, id)) { std::cout << "Такий ID вже існує.\n"; return; }
    Student s;
    s.id = id;
    s.name = readNonEmpty("Прізвище Ім'я: ");
    s.birthDate = readDate();
    s.faculty = readFaculty();
    s.avgGrade = readDouble("Середній бал (0-5): ", 0, 5);
    a.push_back(s);
    std::cout << "Студента додано.\n";
}
static void search(const StudentList& a) {
    std::string q = lowerAscii(readNonEmpty("Частина прізвища: "));
    bool found = false;
    for (const auto& s : a) {
        if (surname(s.name).find(q) != std::string::npos) {
            if (!found) header();
            printStudent(s); found = true;
        }
    }
    if (!found) std::cout << "Збігів немає.\n";
}
static void sortStudents(StudentList& a) {
    if (a.empty()) { std::cout << "Список порожній.\n"; return; }
    std::cout << "1 - За прізвищем (А-Я)\n2 - За середнім балом (спадання)\n3 - За датою народження (старші спочатку)\n";
    int mode = readInt("Сортування: ", 1, 3);
    std::sort(a.begin(), a.end(), [mode](const Student& x, const Student& y) {
        if (mode == 1) return surname(x.name) < surname(y.name);
        if (mode == 2) return x.avgGrade == y.avgGrade ? x.id < y.id : x.avgGrade > y.avgGrade;
        if (x.birthDate.year != y.birthDate.year) return x.birthDate.year < y.birthDate.year;
        if (x.birthDate.month != y.birthDate.month) return x.birthDate.month < y.birthDate.month;
        return x.birthDate.day < y.birthDate.day;
    });
    showAll(a);
}
static void removeStudent(StudentList& a) {
    int id = readInt("ID для видалення: ", 1, 1000000);
    auto old = a.size();
    a.erase(std::remove_if(a.begin(), a.end(), [id](const Student& s) { return s.id == id; }), a.end());
    std::cout << (a.size() != old ? "Запис видалено.\n" : "ID не знайдено.\n");
}
static void statistics(const StudentList& a) {
    std::array<int, 4> counts{};
    std::array<double, 4> sums{};
    for (const auto& s : a) {
        int index = static_cast<int>(s.faculty);
        ++counts[index]; sums[index] += s.avgGrade;
    }
    for (int i = 0; i < 4; ++i) {
        std::cout << toString(static_cast<Faculty>(i)) << ": кількість = " << counts[i]
                  << ", середній бал = " << std::fixed << std::setprecision(2)
                  << (counts[i] ? sums[i] / counts[i] : 0.0) << '\n';
    }
}
static void edit(StudentList& a) {
    int id = readInt("ID для редагування: ", 1, 1000000);
    Student* s = findId(a, id);
    if (!s) { std::cout << "ID не знайдено.\n"; return; }
    std::cout << "Введіть нові дані (ID не змінюється).\n";
    s->name = readNonEmpty("Прізвище Ім'я: ");
    s->birthDate = readDate();
    s->faculty = readFaculty();
    s->avgGrade = readDouble("Середній бал (0-5): ", 0, 5);
    std::cout << "Запис оновлено.\n";
}
void studentsMenu() {
    StudentList students;
    try {
        int choice;
        do {
            std::cout << "\n=== ДЕНЬ 2. ОБЛІК СТУДЕНТІВ ===\n"
                         "1. Додати студента\n2. Показати всіх\n3. Пошук за прізвищем\n"
                         "4. Сортування\n5. Видалити за ID\n6. Статистика факультетів\n"
                         "7. Редагувати запис\n0. Вихід\n";
            choice = readInt("Ваш вибір: ", 0, 7);
            switch (choice) {
                case 1: add(students); break;
                case 2: showAll(students); break;
                case 3: search(students); break;
                case 4: sortStudents(students); break;
                case 5: removeStudent(students); break;
                case 6: statistics(students); break;
                case 7: edit(students); break;
                case 0: std::cout << "Завершення роботи.\n"; break;
            }
        } while (choice != 0);
    } catch (const std::runtime_error&) { std::cout << "\nВвід завершено.\n"; }
}
