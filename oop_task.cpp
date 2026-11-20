#include "oop_task.h"
#include "menu_utils.h"
#include "registry.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

namespace {
template<class Range, class Key>
const typename Range::value_type* maxBy(const Range& range, Key key) {
    if (range.empty()) return nullptr;
    const auto* best = &range.front();
    for (const auto& value : range) if (key(value) > key(*best)) best = &value;
    return best;
}
void test(const std::string& name, bool pass) {
    std::cout << (pass ? "[PASS] " : "[FAIL] ") << name << '\n';
}
bool near(double a, double b) { return std::fabs(a - b) < 1e-8; }
void selfTest() {
    Book b("Тестова книга", 2020, "Автор", 100);
    DVD d("Тестовий DVD", 2020, 90);
    Magazine m("Тестовий журнал", 2020, 1);
    EBook e("Тестова EBook", 2020, "Автор", 100, 3.5);
    test("Book 30 днів -> 18 грн", near(b.lateFee(30), 18));
    test("DVD 5 днів -> 10 грн", near(d.lateFee(5), 10));
    test("Magazine 7 днів -> 0 грн", near(m.lateFee(7), 0));
    test("EBook 40 днів -> 0 грн", near(e.lateFee(40), 0));
    b.renew();
    test("Book +10 днів, 30 днів -> 0 грн", near(b.lateFee(30), 0));
    test("Book після подовження -> 1 залишилось", b.renewalsLeft() == 1);
    test("EBook не подовжується", e.renewalsLeft() == 0);
    Library lib;
    lib.add(std::make_unique<Book>("Один", 2020, "Автор", 100));
    int reader = lib.addReader("Тестовий читач");
    int id = lib.items().front()->id();
    lib.borrow(reader, id);
    bool blocked = false;
    try { lib.borrow(reader, id); } catch (const DomainError&) { blocked = true; }
    test("Подвійна видача заборонена", blocked);
    test("Повернення та штраф", near(lib.giveBack(reader, id, 30), 18));
    blocked = false;
    try { lib.borrow(reader, id); } catch (const DomainError&) { blocked = true; }
    test("Читач із боргом не може позичати", blocked);
    lib.payDebt(reader, 18);
    lib.borrow(reader, id);
    test("Після сплати штрафу видача дозволена", lib.findReader(reader)->borrowed().size() == 1);
    // Bonus template: generic maxBy used twice with different criteria.
    std::vector<int> values{2, 10, 7};
    const int* maxNumber = maxBy(values, [](int x){ return x; });
    const int* maxNegative = maxBy(values, [](int x){ return -x; });
    test("Шаблон maxBy: максимум", maxNumber && *maxNumber == 10);
    test("Шаблон maxBy: мінімум через критерій", maxNegative && *maxNegative == 2);
}
std::unique_ptr<LibraryItem> makeItem() {
    std::cout << "1. Книга  2. Журнал  3. DVD  4. Електронна книга\n";
    int type = readInt("Тип: ");
    std::string title = readText("Назва: ");
    int year = readInt("Рік: ");
    switch (type) {
        case 1: { auto author = readText("Автор: "); int pages = readInt("Сторінок: "); return std::make_unique<Book>(title, year, author, pages); }
        case 2: { int issue = readInt("Номер випуску: "); return std::make_unique<Magazine>(title, year, issue); }
        case 3: { int minutes = readInt("Тривалість, хв: "); return std::make_unique<DVD>(title, year, minutes); }
        case 4: { auto author = readText("Автор: "); int pages = readInt("Сторінок: "); double mb = readDouble("Розмір, МБ: "); return std::make_unique<EBook>(title, year, author, pages, mb); }
        default: throw DomainError("Невідомий тип");
    }
}
void operations(Library& lib) {
    std::cout << "\n1. Додати читача\n2. Список читачів\n3. Видати видання\n"
              << "4. Повернути видання\n5. Подовжити строк\n6. Боржники\n"
              << "7. Топ-3 видань\n8. Сплатити штраф\n0. Назад\n";
    int choice = readInt("Дія: ");
    if (choice == 1) std::cout << "Створено читача ID=" << lib.addReader(readText("Ім'я: ")) << '\n';
    else if (choice == 2) lib.showReaders();
    else if (choice == 3) { int r = readInt("ID читача: "); int i = readInt("ID видання: "); lib.borrow(r, i); std::cout << "Видано.\n"; }
    else if (choice == 4) { int r = readInt("ID читача: "); int i = readInt("ID видання: "); int days = readInt("Днів тримали: "); std::cout << "Штраф: " << lib.giveBack(r, i, days) << " грн\n"; }
    else if (choice == 5) { int r = readInt("ID читача: "); int i = readInt("ID видання: "); lib.renew(r, i); std::cout << "Строк подовжено.\n"; }
    else if (choice == 6) lib.debtors();
    else if (choice == 7) lib.top3();
    else if (choice == 8) { int r = readInt("ID читача: "); double x = readDouble("Сума: "); lib.payDebt(r, x); std::cout << "Оплачено.\n"; }
}
}
void oopMenu() {
    Library lib;
    std::cout << std::fixed << std::setprecision(2);
    while (true) {
        std::cout << "\n=== ДЕНЬ 5. ООП: БІБЛІОТЕКА (ВАРІАНТ 3) ===\n"
                  << "1. Додати видання\n2. Показати всі\n3. Видалити за ID\n"
                  << "4. Дублювати (clone)\n5. Пошук / фільтр\n6. Сортування\n"
                  << "7. Операції бібліотеки\n8. Зберегти у файл\n9. Завантажити з файлу\n"
                  << "10. Самотестування\n11. Додати тестові дані\n0. Назад\n";
        try {
            int c = readInt("Ваш вибір: ");
            if (c == 0) return;
            switch (c) {
                case 1: lib.add(makeItem()); std::cout << "Додано.\n"; break;
                case 2: lib.show(); break;
                case 3: std::cout << (lib.remove(readInt("ID: ")) ? "Видалено.\n" : "Не знайдено.\n"); break;
                case 4: lib.duplicate(readInt("ID: ")); std::cout << "Дубль отримав новий ID.\n"; break;
                case 5: lib.search(readText("Пошук за назвою або типом: ")); break;
                case 6: std::cout << "1. Назва  2. Рік  3. Кількість видач\n"; lib.sortBy(readInt("Критерій: ")); lib.show(); break;
                case 7: operations(lib); break;
                case 8: { auto p = readText("Шлях [Enter = data/oop_library.txt]: "); if (p.empty()) p = "data/oop_library.txt"; std::cout << (lib.save(p) ? "Збережено.\n" : "Помилка збереження (перевірте папку).\n"); break; }
                case 9: { auto p = readText("Шлях [Enter = data/oop_library.txt]: "); if (p.empty()) p = "data/oop_library.txt"; std::cout << "Завантажено видань: " << lib.load(p) << '\n'; break; }
                case 10: selfTest(); break;
                case 11: lib.seed(); std::cout << "Додано тестові дані.\n"; break;
                default: std::cout << "Невідомий пункт.\n";
            }
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Ввід завершено") return;
            std::cout << "Помилка: " << e.what() << '\n';
        } catch (const std::exception& e) {
            std::cout << "Помилка: " << e.what() << '\n';
        }
    }
}
