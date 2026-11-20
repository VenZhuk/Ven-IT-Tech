#include "oop_task.h"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // Source files are UTF-8; set Windows console input/output to UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    std::cout << "Навчальний комплекс — День 5\n";
    oopMenu();
}
