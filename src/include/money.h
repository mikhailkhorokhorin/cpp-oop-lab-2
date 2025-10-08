#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

class Money {
   private:
    unsigned char* digits_;
    size_t size_;

    void normalize();

    void allocateMemory(size_t size);
    void deallocateMemory();

    void parseString(const std::string& amount);
    void parseDouble(double amount);

   public:
    Money();

    explicit Money(const std::string& amount);
    explicit Money(double amount);

    Money(const Money& other);
    Money(Money&& other) noexcept;

    ~Money();

    Money& operator=(const Money& other);
    Money& operator=(Money&& other) noexcept;

    Money operator+(const Money& other) const;
    Money operator-(const Money& other) const;
    Money operator*(const Money& other) const;
    Money operator/(const Money& other) const;

    Money& operator+=(const Money& other);
    Money& operator-=(const Money& other);
    Money& operator*=(const Money& other);
    Money& operator/=(const Money& other);

    bool operator==(const Money& other) const;
    bool operator!=(const Money& other) const;
    bool operator>=(const Money& other) const;
    bool operator<=(const Money& other) const;
    bool operator>(const Money& other) const;
    bool operator<(const Money& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Money& money);
    friend std::istream& operator>>(std::istream& is, Money& money);

    size_t getSize() const;
    const unsigned char* getDigits() const;

    std::string toString(char sep = '.') const;
    double toDouble() const;
};
