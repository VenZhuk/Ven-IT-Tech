#include "menu_utils.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string readText(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    if (!std::getline(std::cin, s)) throw std::runtime_error("Ввід завершено");
    return s;
}
int readInt(const std::string& prompt) {
    while (true) {
        std::string s = readText(prompt);
        std::istringstream in(s);
        int x;
        char extra;
        if (in >> x && !(in >> extra)) return x;
        std::cout << "Помилка: введіть ціле число.\n";
    }
}
double readDouble(const std::string& prompt) {
    while (true) {
        std::string s = readText(prompt);
        std::istringstream in(s);
        double x;
        char extra;
        if (in >> x && !(in >> extra) && std::isfinite(x)) return x;
        std::cout << "Помилка: введіть число (крапка для дробової частини).\n";
    }
}
