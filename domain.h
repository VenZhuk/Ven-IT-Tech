#pragma once
#include "errors.h"
#include <iostream>
#include <memory>
#include <string>

class IRenewable {
public:
    virtual ~IRenewable() = default;
    virtual void renew() = 0;
    virtual int renewalsLeft() const = 0;
};

/** Abstract library publication; owns its unique inventory identifier. */
class LibraryItem {
    static int nextId_;
    int id_;
    std::string title_;
    int year_;
    int extraDays_ = 0;
    int borrowCount_ = 0;
protected:
    LibraryItem(const LibraryItem& other);
    void addExtraDays(int days);
    void setRenewalState(int extraDays, int renewals);
    int renewalCount_ = 0;
public:
    LibraryItem(std::string title, int year);
    virtual ~LibraryItem() = default;
    int id() const { return id_; }
    const std::string& title() const { return title_; }
    int year() const { return year_; }
    int extraDays() const { return extraDays_; }
    int borrowCount() const { return borrowCount_; }
    void setTitle(const std::string& title);
    void setYear(int year);
    void markBorrowed() { ++borrowCount_; }
    void restoreStats(int borrowCount);
    double lateFee(int daysHeld) const;
    virtual int loanDays() const = 0;
    virtual double finePerDay() const = 0;
    virtual std::string kindName() const = 0;
    virtual std::string typeTag() const = 0;
    virtual std::string toLine() const = 0;
    virtual void print(std::ostream& os) const;
    virtual std::unique_ptr<LibraryItem> clone() const = 0;
    bool operator<(const LibraryItem& other) const;
    bool operator==(const LibraryItem& other) const;
};
std::ostream& operator<<(std::ostream& os, const LibraryItem& item);

class Book : public LibraryItem, public IRenewable {
    std::string author_;
    int pages_;
public:
    Book(std::string title, int year, std::string author, int pages);
    const std::string& author() const { return author_; }
    int pages() const { return pages_; }
    int loanDays() const override { return 21; }
    double finePerDay() const override { return 2.0; }
    std::string kindName() const override { return "Книга"; }
    std::string typeTag() const override { return "Book"; }
    void renew() override;
    int renewalsLeft() const override { return 2 - renewalCount_; }
    std::string toLine() const override;
    void print(std::ostream& os) const override;
    std::unique_ptr<LibraryItem> clone() const override;
    void restoreRenewals(int count);
};

class Magazine : public LibraryItem, public IRenewable {
    int issueNumber_;
public:
    Magazine(std::string title, int year, int issueNumber);
    int issueNumber() const { return issueNumber_; }
    int loanDays() const override { return 7; }
    double finePerDay() const override { return 1.0; }
    std::string kindName() const override { return "Журнал"; }
    std::string typeTag() const override { return "Magazine"; }
    void renew() override;
    int renewalsLeft() const override { return 2 - renewalCount_; }
    std::string toLine() const override;
    void print(std::ostream& os) const override;
    std::unique_ptr<LibraryItem> clone() const override;
    void restoreRenewals(int count);
};

class DVD : public LibraryItem {
    int minutes_;
public:
    DVD(std::string title, int year, int minutes);
    int loanDays() const override { return 3; }
    double finePerDay() const override { return 5.0; }
    std::string kindName() const override { return "DVD"; }
    std::string typeTag() const override { return "DVD"; }
    int minutes() const { return minutes_; }
    std::string toLine() const override;
    void print(std::ostream& os) const override;
    std::unique_ptr<LibraryItem> clone() const override;
};

class EBook : public Book {
    double fileSizeMB_;
public:
    EBook(std::string title, int year, std::string author, int pages, double fileSizeMB);
    int loanDays() const override { return 14; }
    double finePerDay() const override { return 0.0; }
    std::string kindName() const override { return "Електронна книга"; }
    std::string typeTag() const override { return "EBook"; }
    void renew() override; // EBook cannot be renewed, despite inheriting Book's interface
    int renewalsLeft() const override { return 0; }
    double fileSizeMB() const { return fileSizeMB_; }
    std::string toLine() const override;
    void print(std::ostream& os) const override;
    std::unique_ptr<LibraryItem> clone() const override;
};
