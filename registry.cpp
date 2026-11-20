#include "registry.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

namespace {
std::vector<std::string> split(const std::string& line, char sep = ';') {
    std::vector<std::string> v;
    std::istringstream in(line);
    std::string s;
    while (std::getline(in, s, sep)) v.push_back(s);
    return v;
}
int number(const std::string& s) {
    size_t pos = 0;
    int n = std::stoi(s, &pos);
    if (pos != s.size()) throw DomainError("Некоректне ціле число");
    return n;
}
double decimal(const std::string& s) {
    size_t pos = 0;
    double x = std::stod(s, &pos);
    if (pos != s.size() || !std::isfinite(x)) throw DomainError("Некоректне число");
    return x;
}
void fields(const std::vector<std::string>& f, size_t n) {
    if (f.size() != n) throw DomainError("Неправильна кількість полів");
}
}
Reader::Reader(int id, std::string name, double debt) : id_(id), name_(std::move(name)), debt_(debt) {
    if (id_ <= 0 || name_.empty() || name_.find_first_of(";\r\n") != std::string::npos ||
        !std::isfinite(debt_) || debt_ < 0) throw DomainError("Некоректні дані читача");
}
void Reader::borrow(LibraryItem& item) {
    if (debt_ > 0.00001) throw DomainError("Читач має непогашений штраф");
    if (borrowed_.size() >= 5) throw DomainError("Ліміт 5 видань вичерпано");
    borrowed_.push_back(&item);
    item.markBorrowed();
}
void Reader::giveBack(LibraryItem& item, int daysHeld) {
    auto it = std::find(borrowed_.begin(), borrowed_.end(), &item);
    if (it == borrowed_.end()) throw DomainError("Це видання не належить читачу");
    double fine = item.lateFee(daysHeld);
    borrowed_.erase(it);
    debt_ += fine;
}
void Reader::restoreLoan(LibraryItem& item) {
    if (borrowed_.size() >= 5) throw DomainError("Ліміт 5 видань");
    borrowed_.push_back(&item);
}
void Reader::pay(double amount) {
    if (!std::isfinite(amount) || amount <= 0 || amount > debt_ + 0.00001)
        throw DomainError("Некоректна сума оплати");
    debt_ = std::max(0.0, debt_ - amount);
}
Library::Library() {
    creators_["Book"] = [](const std::vector<std::string>& f) -> std::unique_ptr<LibraryItem> {
        fields(f, 7);
        auto p = std::make_unique<Book>(f[1], number(f[2]), f[4], number(f[5]));
        p->restoreStats(number(f[3])); p->restoreRenewals(number(f[6])); return p;
    };
    creators_["Magazine"] = [](const std::vector<std::string>& f) -> std::unique_ptr<LibraryItem> {
        fields(f, 6);
        auto p = std::make_unique<Magazine>(f[1], number(f[2]), number(f[4]));
        p->restoreStats(number(f[3])); p->restoreRenewals(number(f[5])); return p;
    };
    creators_["DVD"] = [](const std::vector<std::string>& f) -> std::unique_ptr<LibraryItem> {
        fields(f, 5);
        auto p = std::make_unique<DVD>(f[1], number(f[2]), number(f[4]));
        p->restoreStats(number(f[3])); return p;
    };
    creators_["EBook"] = [](const std::vector<std::string>& f) -> std::unique_ptr<LibraryItem> {
        fields(f, 7);
        auto p = std::make_unique<EBook>(f[1], number(f[2]), f[4], number(f[5]), decimal(f[6]));
        p->restoreStats(number(f[3])); return p;
    };
}
void Library::add(std::unique_ptr<LibraryItem> item) {
    if (!item) throw DomainError("Порожній об'єкт");
    items_.push_back(std::move(item));
}
LibraryItem* Library::find(int id) {
    auto it = std::find_if(items_.begin(), items_.end(), [id](const auto& p){ return p->id() == id; });
    return it == items_.end() ? nullptr : it->get();
}
const LibraryItem* Library::find(int id) const {
    auto it = std::find_if(items_.begin(), items_.end(), [id](const auto& p){ return p->id() == id; });
    return it == items_.end() ? nullptr : it->get();
}
Reader* Library::findReader(int id) {
    auto it = std::find_if(readers_.begin(), readers_.end(), [id](const Reader& r){ return r.id() == id; });
    return it == readers_.end() ? nullptr : &*it;
}
bool Library::remove(int id) {
    LibraryItem* item = find(id);
    if (!item) return false;
    for (const auto& reader : readers_)
        if (std::find(reader.borrowed().begin(), reader.borrowed().end(), item) != reader.borrowed().end())
            throw DomainError("Не можна видалити видане читачу видання");
    items_.erase(std::remove_if(items_.begin(), items_.end(), [id](const auto& p){ return p->id() == id; }), items_.end());
    return true;
}
int Library::addReader(const std::string& name) {
    readers_.emplace_back(nextReaderId_ + 1, name);
    return ++nextReaderId_;
}
void Library::borrow(int readerId, int itemId) {
    Reader* r = findReader(readerId);
    LibraryItem* p = find(itemId);
    if (!r || !p) throw DomainError("Читача або видання не знайдено");
    for (const auto& reader : readers_)
        if (std::find(reader.borrowed().begin(), reader.borrowed().end(), p) != reader.borrowed().end())
            throw DomainError("Видання вже видано іншому читачу");
    r->borrow(*p);
}
double Library::giveBack(int readerId, int itemId, int daysHeld) {
    Reader* r = findReader(readerId);
    LibraryItem* p = find(itemId);
    if (!r || !p) throw DomainError("Читача або видання не знайдено");
    double fee = p->lateFee(daysHeld);
    r->giveBack(*p, daysHeld);
    return fee;
}
void Library::renew(int readerId, int itemId) {
    Reader* r = findReader(readerId);
    LibraryItem* p = find(itemId);
    if (!r || !p) throw DomainError("Читача або видання не знайдено");
    if (std::find(r->borrowed().begin(), r->borrowed().end(), p) == r->borrowed().end())
        throw DomainError("Подовжувати можна лише видання цього читача");
    if (dynamic_cast<EBook*>(p)) throw DomainError("Електронні книги не подовжуються");
    auto* renewable = dynamic_cast<IRenewable*>(p);
    if (!renewable) throw DomainError("Цей тип видання не подовжується");
    renewable->renew();
}
void Library::payDebt(int readerId, double amount) {
    Reader* r = findReader(readerId);
    if (!r) throw DomainError("Читача не знайдено");
    r->pay(amount);
}
void Library::duplicate(int id) {
    auto* p = find(id);
    if (!p) throw DomainError("Видання не знайдено");
    add(p->clone());
}
void Library::show() const {
    if (items_.empty()) { std::cout << "Каталог порожній.\n"; return; }
    for (const auto& p : items_) {
        std::cout << *p;
        bool taken = false;
        for (const auto& r : readers_)
            if (std::find(r.borrowed().begin(), r.borrowed().end(), p.get()) != r.borrowed().end()) taken = true;
        std::cout << (taken ? " | ВИДАНО" : " | доступно") << '\n';
    }
}
void Library::showReaders() const {
    if (readers_.empty()) std::cout << "Читачів немає.\n";
    for (const auto& r : readers_) {
        std::cout << "Читач #" << r.id() << " " << r.name() << " | борг " << r.debt() << " грн | видано: ";
        for (auto* p : r.borrowed()) std::cout << p->id() << " (" << p->title() << ") ";
        std::cout << '\n';
    }
}
void Library::debtors() const {
    bool any = false;
    for (const auto& r : readers_) if (r.debt() > 0.00001) {
        std::cout << r.id() << " " << r.name() << ": " << r.debt() << " грн\n";
        any = true;
    }
    if (!any) std::cout << "Боржників немає.\n";
}
void Library::top3() const {
    std::vector<const LibraryItem*> list;
    for (const auto& p : items_) list.push_back(p.get());
    std::sort(list.begin(), list.end(), [](const auto* a, const auto* b) {
        if (a->borrowCount() != b->borrowCount()) return a->borrowCount() > b->borrowCount();
        return a->title() < b->title();
    });
    for (size_t i = 0; i < std::min<size_t>(3, list.size()); ++i)
        std::cout << i + 1 << ". " << list[i]->kindName() << ": " << list[i]->title()
                  << " — " << list[i]->borrowCount() << " видач\n";
}
void Library::search(const std::string& query) const {
    std::vector<const LibraryItem*> candidates, found;
    for (const auto& p : items_) candidates.push_back(p.get());
    std::copy_if(candidates.begin(), candidates.end(), std::back_inserter(found),
        [&query](const LibraryItem* p) { return p->title().find(query) != std::string::npos || p->kindName().find(query) != std::string::npos; });
    for (const auto* p : found) std::cout << *p << '\n';
    if (found.empty()) std::cout << "Збігів немає.\n";
}
void Library::sortBy(int criterion) {
    if (criterion == 1) std::sort(items_.begin(), items_.end(), [](const auto& a, const auto& b){ return *a < *b; });
    else if (criterion == 2) std::sort(items_.begin(), items_.end(), [](const auto& a, const auto& b){ return a->year() < b->year(); });
    else if (criterion == 3) std::sort(items_.begin(), items_.end(), [](const auto& a, const auto& b){ return a->borrowCount() > b->borrowCount(); });
    else throw DomainError("Невідомий критерій");
}
bool Library::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    // V2: IDs of items are runtime IDs; persisted loans use the item's index in catalog.
    out << "LIBRARY_V2\n";
    for (const auto& p : items_) out << "ITEM;" << p->toLine() << '\n';
    for (const auto& r : readers_) {
        out << "READER;" << r.id() << ';' << r.name() << ';' << std::setprecision(17) << r.debt();
        for (auto* p : r.borrowed()) {
            auto it = std::find_if(items_.begin(), items_.end(), [p](const auto& x){ return x.get() == p; });
            if (it != items_.end()) out << ';' << std::distance(items_.begin(), it);
        }
        out << '\n';
    }
    return static_cast<bool>(out);
}
int Library::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw DomainError("Не вдалося відкрити файл");
    std::string line;
    if (!std::getline(in, line) || line != "LIBRARY_V2") throw DomainError("Невідомий формат файлу");
    // Build a temporary library; commit only after reading everything.
    Library loaded;
    std::vector<std::vector<std::string>> readerRows;
    int lineNo = 1, count = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        if (line.empty()) continue;
        try {
            auto f = split(line);
            if (f.empty()) throw DomainError("Порожній запис");
            if (f[0] == "ITEM") {
                f.erase(f.begin());
                if (f.empty()) throw DomainError("Немає тегу типу");
                auto it = loaded.creators_.find(f[0]);
                if (it == loaded.creators_.end()) throw DomainError("Невідомий тип: " + f[0]);
                loaded.add(it->second(f));
                ++count;
            } else if (f[0] == "READER") {
                if (f.size() < 4) throw DomainError("Неповний запис читача");
                readerRows.push_back(std::move(f));
            } else throw DomainError("Невідомий тег запису");
        } catch (const std::exception& e) {
            std::cerr << "Рядок " << lineNo << " пропущено: " << e.what() << '\n';
        }
    }
    for (const auto& f : readerRows) {
        try {
            int id = number(f[1]);
            if (loaded.findReader(id)) throw DomainError("Повторний ID читача");
            Reader r(id, f[2], decimal(f[3]));
            for (size_t j = 4; j < f.size(); ++j) {
                int idx = number(f[j]);
                if (idx < 0 || static_cast<size_t>(idx) >= loaded.items_.size()) throw DomainError("Невірний індекс видання");
                LibraryItem* item = loaded.items_[idx].get();
                for (const auto& existing : loaded.readers_)
                    if (std::find(existing.borrowed().begin(), existing.borrowed().end(), item) != existing.borrowed().end())
                        throw DomainError("Видання вже належить іншому читачу");
                // Restoration of loans must not increase historical borrow counts or reject existing debt.
                r.restoreLoan(*item);
            }
            loaded.nextReaderId_ = std::max(loaded.nextReaderId_, id);
            loaded.readers_.push_back(std::move(r));
        } catch (const std::exception& e) {
            std::cerr << "Читача пропущено: " << e.what() << '\n';
        }
    }
    items_ = std::move(loaded.items_);
    readers_ = std::move(loaded.readers_);
    nextReaderId_ = loaded.nextReaderId_;
    return count;
}
void Library::seed() {
    add(std::make_unique<Book>("Кобзар", 1840, "Тарас Шевченко", 115));
    add(std::make_unique<Magazine>("Наука і техніка", 2025, 12));
    add(std::make_unique<DVD>("Документальний фільм", 2012, 120));
    add(std::make_unique<EBook>("Програмування C++", 2024, "Автор навчального посібника", 350, 8.5));
    addReader("Олена Коваль");
    addReader("Андрій Петренко");
}
