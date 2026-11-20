#pragma once
#include <stdexcept>
#include <string>
class DomainError : public std::invalid_argument {
public:
    using std::invalid_argument::invalid_argument;
};
