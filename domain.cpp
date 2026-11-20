#include "domain.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>

namespace {
void validText(const std::string& s) {
    if (s.empty() || s.find_first_of(";\r\n") != std::string::npos)
        throw DomainError("Текст порожній або містить заборонений символ ;");
}
std::string common(const LibraryItem& x) {
    std::ostringstream out;
    out << ';' << x.title() << ';' << x.year() << ';' << x.borrowCount();
    return out.str();
}
}
int LibraryItem::nextId_ = 0;
LibraryItem::LibraryItem(std::string title, int year) : id_(++nextId_) {
    setTitle(title);
    setYear(year);
}
LibraryItem::LibraryItem(const LibraryItem& other)
    : id_(++nextId_), title_(other.title_), year_(other.year_),
      extraDays_(other.extraDays_), borrowCount_(other.borrowCount_), renewalCount_(other.renewalCount_) {}
void LibraryItem::setTitle(const std::string& title) { validText(title); title_ = title; }
void LibraryItem::setYear(int year) {
    if (year < 1450 || year > 2026) throw DomainError("Рік має бути в межах 1450..2026");
    year_ = year;
}
void LibraryItem::addExtraDays(int days) {
    if (days < 0) throw DomainError("Дні подовження не можуть бути від'ємними");
    extraDays_ += days;
}
void LibraryItem::setRenewalState(int extraDays, int renewals) {
    if (extraDays < 0 || renewals < 0 || renewals > 2)
        throw DomainError("Некоректний стан подовжень");
    extraDays_ = extraDays;
    renewalCount_ = renewals;
}
void LibraryItem::restoreStats(int count) {
    if (count < 0) throw DomainError("Некоректна кількість видач");
    borrowCount_ = count;
}
double LibraryItem::lateFee(int daysHeld) const {
    if (daysHeld < 0) throw DomainError("Кількість днів не може бути від'ємною");
    return std::max(0, daysHeld - loanDays() - extraDays_) * finePerDay();
}
void LibraryItem::print(std::ostream& os) const {
    os << "ID=" << id_ << " | " << kindName() << " | " << title_
       << " (" << year_ << ") | строк=" << loanDays() + extraDays_
       << " дн. | видач=" << borrowCount_;
}
bool LibraryItem::operator<(const LibraryItem& other) const { return title_ < other.title_; }
bool LibraryItem::operator==(const LibraryItem& other) const { return id_ == other.id_; }
std::ostream& operator<<(std::ostream& os, const LibraryItem& item) { item.print(os); return os; }

Book::Book(std::string title, int year, std::string author, int pages)
    : LibraryItem(std::move(title), year), author_(std::move(author)), pages_(pages) {
    validText(author_);
    if (pages_ <= 0) throw DomainError("Кількість сторінок має бути > 0");
}
void Book::renew() {
    if (renewalsLeft() == 0) throw DomainError("Ліміт подовжень вичерпано");
    addExtraDays(loanDays() / 2);
    ++renewalCount_;
}
void Book::restoreRenewals(int count) {
    if (count < 0 || count > 2) throw DomainError("Некоректна кількість подовжень");
    setRenewalState(count * (loanDays() / 2), count);
}
std::string Book::toLine() const {
    return typeTag() + common(*this) + ';' + author_ + ';' + std::to_string(pages_) + ';' + std::to_string(renewalCount_);
}
void Book::print(std::ostream& os) const { LibraryItem::print(os); os << " | " << author_ << " | " << pages_ << " стор."; }
std::unique_ptr<LibraryItem> Book::clone() const { return std::make_unique<Book>(*this); }

Magazine::Magazine(std::string title, int year, int issueNumber)
    : LibraryItem(std::move(title), year), issueNumber_(issueNumber) {
    if (issueNumber_ <= 0) throw DomainError("Номер журналу має бути > 0");
}
void Magazine::renew() {
    if (renewalsLeft() == 0) throw DomainError("Ліміт подовжень вичерпано");
    addExtraDays(loanDays() / 2);
    ++renewalCount_;
}
void Magazine::restoreRenewals(int count) {
    if (count < 0 || count > 2) throw DomainError("Некоректна кількість подовжень");
    setRenewalState(count * (loanDays() / 2), count);
}
std::string Magazine::toLine() const {
    return typeTag() + common(*this) + ';' + std::to_string(issueNumber_) + ';' + std::to_string(renewalCount_);
}
void Magazine::print(std::ostream& os) const { LibraryItem::print(os); os << " | випуск №" << issueNumber_; }
std::unique_ptr<LibraryItem> Magazine::clone() const { return std::make_unique<Magazine>(*this); }

DVD::DVD(std::string title, int year, int minutes)
    : LibraryItem(std::move(title), year), minutes_(minutes) {
    if (minutes_ <= 0) throw DomainError("Тривалість має бути > 0");
}
std::string DVD::toLine() const { return typeTag() + common(*this) + ';' + std::to_string(minutes_); }
void DVD::print(std::ostream& os) const { LibraryItem::print(os); os << " | " << minutes_ << " хв."; }
std::unique_ptr<LibraryItem> DVD::clone() const { return std::make_unique<DVD>(*this); }

EBook::EBook(std::string title, int year, std::string author, int pages, double fileSizeMB)
    : Book(std::move(title), year, std::move(author), pages), fileSizeMB_(fileSizeMB) {
    if (!std::isfinite(fileSizeMB_) || fileSizeMB_ <= 0) throw DomainError("Розмір файлу має бути > 0");
}
void EBook::renew() { throw DomainError("Електронну книгу подовжувати не можна"); }
std::string EBook::toLine() const {
    std::ostringstream out;
    out << typeTag() << common(*this) << ';' << author() << ';' << pages() << ';' << fileSizeMB_;
    return out.str();
}
void EBook::print(std::ostream& os) const { Book::print(os); os << " | " << fileSizeMB_ << " МБ (автоповернення)"; }
std::unique_ptr<LibraryItem> EBook::clone() const { return std::make_unique<EBook>(*this); }
