#include "../include/money.h"

void Money::normalize() {
    if (!digits_ || size_ <= 2)
        return;

    while (size_ > 2 && digits_[size_ - 1] == 0)
        --size_;
}

void Money::allocateMemory(size_t size) {
    digits_ = new unsigned char[size];
    size_ = size;
}

void Money::deallocateMemory() {
    delete[] digits_;
    digits_ = nullptr;
    size_ = 0;
}

void Money::parseString(const std::string& amount) {
    size_t dot = amount.find('.');

    if (dot == std::string::npos)
        throw std::invalid_argument("Missing point in number.");

    std::string rubles = amount.substr(0, dot);
    std::string kopecks = amount.substr(dot + 1);

    if (kopecks.size() != 2)
        throw std::invalid_argument("Number must have exactly two kopecks digits.");

    std::string digits = rubles + kopecks;

    for (char c : digits)
        if (!std::isdigit(c))
            throw std::invalid_argument("Number must contain only digits.");

    size_t size = digits.size();

    deallocateMemory();
    allocateMemory(size);

    for (size_t i = 0; i < size; ++i)
        digits_[i] = digits[size - i - 1] - '0';

    normalize();
}

void Money::parseDouble(double amount) {
    if (amount < 0)
        throw std::invalid_argument("Number mast be positive.");

    unsigned long long number = static_cast<unsigned long long>(std::round(amount * 100));
    unsigned long long temp = number;

    size_t size = 0;
    do {
        ++size;
        temp /= 10;
    } while (temp > 0);

    deallocateMemory();
    allocateMemory(size);

    for (size_t i = 0; i < size; ++i) {
        digits_[i] = number % 10;
        number /= 10;
    }

    normalize();
}

Money::Money() : digits_(nullptr), size_(0) {
    allocateMemory(1);
    digits_[0] = 0;
}

Money::Money(const std::string& amount) : digits_(nullptr), size_(0) {
    parseString(amount);
}

Money::Money(double amount) : digits_(nullptr), size_(0) {
    parseDouble(amount);
}

Money::Money(const Money& other) : digits_(nullptr), size_(0) {
    *this = other;
}

Money::Money(Money&& other) noexcept : digits_(nullptr), size_(0) {
    *this = std::move(other);
}

Money::~Money() {
    deallocateMemory();
}

Money& Money::operator=(const Money& other) {
    if (this == &other)
        return *this;

    deallocateMemory();
    allocateMemory(other.size_);

    for (size_t i = 0; i < other.size_; ++i)
        digits_[i] = other.digits_[i];

    return *this;
}

Money& Money::operator=(Money&& other) noexcept {
    if (this != &other) {
        deallocateMemory();

        digits_ = other.digits_;
        size_ = other.size_;

        other.digits_ = nullptr;
        other.size_ = 0;
    }

    return *this;
}

Money Money::operator+(const Money& other) const {
    return Money(*this) += other;
}

Money Money::operator-(const Money& other) const {
    return Money(*this) -= other;
}

Money Money::operator*(const Money& other) const {
    return Money(*this) *= other;
}
Money Money::operator/(const Money& other) const {
    return Money(*this) /= other;
}

Money& Money::operator+=(const Money& other) {
    size_t size = std::max(size_, other.size_) + 1;
    unsigned char* digits = new unsigned char[size]{};
    unsigned carry = 0;

    for (size_t i = 0; i < size - 1; ++i) {
        unsigned value = carry;

        if (i < size_)
            value += digits_[i];

        if (i < other.size_)
            value += other.digits_[i];

        digits[i] = value % 10;
        carry = value / 10;
    }

    digits[size - 1] = carry;

    deallocateMemory();

    digits_ = digits;
    size_ = size;

    normalize();

    return *this;
}

Money& Money::operator-=(const Money& other) {
    if (*this < other)
        throw std::invalid_argument("Result number can not be negative.");

    unsigned carry = 0;

    for (size_t i = 0; i < size_; ++i) {
        int diff = digits_[i] - (i < other.size_ ? other.digits_[i] : 0) - carry;
        carry = diff < 0;
        digits_[i] = static_cast<unsigned char>(diff + (carry ? 10 : 0));
    }

    normalize();

    return *this;
}

Money& Money::operator*=(const Money& other) {
    size_t size = size_ + other.size_;
    unsigned char* digits = new unsigned char[size]{};

    for (size_t i = 0; i < size_; ++i) {
        unsigned carry = 0;

        for (size_t j = 0; j < other.size_; ++j) {
            unsigned value = digits_[i] * other.digits_[j] + digits[i + j] + carry;
            digits[i + j] = value % 10;
            carry = value / 10;
        }

        digits[i + other.size_] += carry;
    }

    deallocateMemory();

    digits_ = digits;
    size_ = size;

    normalize();

    return *this;
}

Money& Money::operator/=(const Money& other) {
    if (other == Money("0"))
        throw std::invalid_argument("Can not divide by zero.");

    return *this = Money(this->toDouble() / other.toDouble());
}

bool Money::operator==(const Money& other) const {
    if (size_ != other.size_)
        return false;

    for (size_t i = 0; i < size_; ++i) {
        if (digits_[i] != other.digits_[i])
            return false;
    }
    return true;
}

bool Money::operator!=(const Money& other) const {
    return !(*this == other);
}

bool Money::operator>=(const Money& other) const {
    return !(*this < other);
}

bool Money::operator<=(const Money& other) const {
    return !(*this > other);
}

bool Money::operator>(const Money& other) const {
    if (size_ != other.size_)
        return size_ > other.size_;

    for (size_t i = size_; i > 0; --i) {
        if (digits_[i - 1] != other.digits_[i - 1])
            return digits_[i - 1] > other.digits_[i - 1];
    }

    return false;
}

bool Money::operator<(const Money& other) const {
    return other > *this;
}

std::ostream& operator<<(std::ostream& os, const Money& money) {
    return os << money.toString('.');
}

std::istream& operator>>(std::istream& is, Money& money) {
    std::string str;
    return (is >> str) ? (money.parseString(str), is) : is;
}

size_t Money::getSize() const {
    return size_;
}

const unsigned char* Money::getDigits() const {
    return digits_;
}

std::string Money::toString(char sep) const {
    std::string result;

    if (size_ <= 2)
        result = "0";
    else
        for (int i = static_cast<int>(size_) - 1; i >= 2; --i)
            result += '0' + digits_[i];

    result += sep;
    result += '0' + (size_ >= 2 ? digits_[1] : 0);
    result += '0' + digits_[0];

    return result;
}

double Money::toDouble() const {
    double result = 0.0;
    double factor = 1.0;

    for (size_t i = 0; i < size_; ++i) {
        result += digits_[i] * factor;
        factor *= 10.0;
    }

    if (size_ >= 2)
        result /= 100.0;

    return result;
}
