#pragma once
#include "domain.h"
#include <functional>
#include <map>
#include <vector>

/** Reader aggregates (does not own) borrowed LibraryItem objects. */
class Reader {
    int id_;
    std::string name_;
    double debt_ = 0;
    std::vector<LibraryItem*> borrowed_;
public:
    Reader(int id, std::string name, double debt = 0);
    int id() const { return id_; }
    const std::string& name() const { return name_; }
    double debt() const { return debt_; }
    const std::vector<LibraryItem*>& borrowed() const { return borrowed_; }
    void borrow(LibraryItem& item);
    void giveBack(LibraryItem& item, int daysHeld);
    void pay(double amount);
    void clearBorrowed() { borrowed_.clear(); }
    void restoreLoan(LibraryItem& item);
};

class Library {
public:
    using Creator = std::function<std::unique_ptr<LibraryItem>(const std::vector<std::string>&)>;
private:
    std::vector<std::unique_ptr<LibraryItem>> items_;
    std::vector<Reader> readers_;
    std::map<std::string, Creator> creators_;
    int nextReaderId_ = 0;
public:
    Library();
    void add(std::unique_ptr<LibraryItem> item);
    bool remove(int itemId);
    LibraryItem* find(int id);
    const LibraryItem* find(int id) const;
    Reader* findReader(int id);
    int addReader(const std::string& name);
    void borrow(int readerId, int itemId);
    double giveBack(int readerId, int itemId, int daysHeld);
    void renew(int readerId, int itemId);
    void payDebt(int readerId, double amount);
    void duplicate(int itemId);
    void show() const;
    void showReaders() const;
    void debtors() const;
    void top3() const;
    void search(const std::string& query) const;
    void sortBy(int criterion);
    bool save(const std::string& path) const;
    int load(const std::string& path);
    void seed();
    size_t size() const { return items_.size(); }
    const std::vector<std::unique_ptr<LibraryItem>>& items() const { return items_; }
};
