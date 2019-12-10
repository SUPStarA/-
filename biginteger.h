#pragma once
#ifndef __BIGINT
#define __BIGINT

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <algorithm>

enum SIGN {
    LESS,
    EQUALS,
    MORE
};

struct BigInteger : private std::vector<unsigned int> {
public:
    const static int BASE = 1e9;

    std::string toString() const;

    BigInteger operator-() const;

    BigInteger &operator++();

    BigInteger &operator--();

    BigInteger operator++(int);

    BigInteger operator--(int);

    BigInteger(int x = 0);

    BigInteger(const std::string &s);

    explicit operator bool() const;

    BigInteger &operator*=(const BigInteger &other);

    BigInteger &operator/=(const BigInteger &other);

    BigInteger &operator%=(const BigInteger &other);

    SIGN Compare(const BigInteger &other, bool changeMysign = false, bool changeOthersign = false) const;

    BigInteger &operator+=(const BigInteger &other);

    BigInteger &operator-=(const BigInteger &other);

    friend std::pair<BigInteger, BigInteger>
    div(const BigInteger &a, const BigInteger &b, std::pair<BigInteger, BigInteger> &ans);

private:
    bool _sign;

    BigInteger &_removeLeadingZeros();

    BigInteger &_addToModule(const BigInteger &other);

    BigInteger &_subLessModule(const BigInteger &other);

    friend BigInteger silly_mult(const BigInteger &a, const BigInteger &other);

    friend BigInteger multPowOfBase(const BigInteger &a, int deg);
};

// Outside Declaration

// Declaration comparators

bool operator<(const BigInteger &a, const BigInteger &b);

bool operator>(const BigInteger &a, const BigInteger &b);

bool operator==(const BigInteger &a, const BigInteger &b);

bool operator!=(const BigInteger &a, const BigInteger &b);

bool operator<=(const BigInteger &a, const BigInteger &b);

bool operator>=(const BigInteger &a, const BigInteger &b);

// Declaration arithmetics

BigInteger operator+(const BigInteger &a, const BigInteger &b);

BigInteger operator-(const BigInteger &a, const BigInteger &b);

BigInteger operator*(const BigInteger &a, const BigInteger &b);

BigInteger operator/(const BigInteger &a, const BigInteger &b);

BigInteger operator%(const BigInteger &a, const BigInteger &b);

BigInteger abs(const BigInteger &x);

// Declaration I/O operators

std::ostream &operator<<(std::ostream &out, const BigInteger &a);

std::istream &operator>>(std::istream &in, BigInteger &b);

// DEFINITION

BigInteger &BigInteger::_removeLeadingZeros() {
    while (size() > 1 && back() == 0) {
        pop_back();
    }
    if (size() == 1 && (*this)[0] == 0) {
        _sign = 1;
    }
    return *this;
}

SIGN BigInteger::Compare(const BigInteger &other, bool changeMysign, bool changeOthersign) const {
    if ((_sign ^ changeMysign) == 0 && (other._sign ^ changeOthersign) == 1) return SIGN::LESS;
    if ((_sign ^ changeMysign) == 1 && (other._sign ^ changeOthersign) == 0) return SIGN::MORE;
    if ((size() < other.size()) ^ (!_sign)) return SIGN::LESS;
    if ((size() > other.size()) ^ (!_sign)) return SIGN::MORE;
    int i = int(size()) - 1;
    while (i > -1 && (*this)[i] == other[i]) --i;
    if (i > -1) {
        if ((*this)[i] < other[i]) {
            return (_sign ^ changeMysign) == 1 ? SIGN::LESS : SIGN::MORE;
        } else {
            return (_sign ^ changeMysign) == 1 ? SIGN::MORE : SIGN::LESS;
        }
    }
    return SIGN::EQUALS;
}

// Constructors

BigInteger::BigInteger(int a) {
    if (a >= 0) {
        _sign = 1;
    } else {
        a = -a;
        _sign = 0;
    }
    if (a == 0) {
        push_back(0);
        return;
    }
    while (a > 0) {
        push_back(a % BASE);
        a /= BASE;
    }
    _removeLeadingZeros();
}

BigInteger::BigInteger(const std::string &s) {
    if (s == "0") {
        *this = BigInteger(0);
        return;
    }
    _sign = 1;
    clear();
    long long now = 0;
    long long st = 1;
    // bad
    for (int i = s.size() - 1; i > -1; i--) {
        if (s[i] == '-')
            continue;
        now += st * (s[i] - '0');
        st *= 10;
        if (st >= BigInteger::BASE) {
            push_back(now % BigInteger::BASE);
            now /= BigInteger::BASE;
            st = 1;
        }
    }
    if (s[0] == '-') {
        _sign = 0;
    }
    if (now > 0) {
        push_back(now % BigInteger::BASE);
    }
    _removeLeadingZeros();
}

// Compare operatos

bool operator<(const BigInteger &a, const BigInteger &b) {
    return a.Compare(b) == SIGN::LESS;
}

bool operator>(const BigInteger &a, const BigInteger &b) {
    return a.Compare(b) == SIGN::MORE;
}

bool operator==(const BigInteger &a, const BigInteger &b) {
    return a.Compare(b) == SIGN::EQUALS;
}

bool operator!=(const BigInteger &a, const BigInteger &b) {
    return a.Compare(b) != SIGN::EQUALS;
}

bool operator<=(const BigInteger &a, const BigInteger &b) {
    SIGN tmp = a.Compare(b);
    return tmp == SIGN::EQUALS || tmp == SIGN::LESS;
}

bool operator>=(const BigInteger &a, const BigInteger &b) {
    SIGN tmp = a.Compare(b);
    return tmp == SIGN::EQUALS || tmp == SIGN::MORE;
}

// Casting operators

BigInteger::operator bool() const {
    return (*this) != 0;
}

// Arithmetic operators

BigInteger BigInteger::operator-() const {
    if (size() == 1 && (*this)[0] == 0) return 0;
    BigInteger ans = *this;
    ans._sign ^= 1;
    ans._removeLeadingZeros();
    return ans;
}

BigInteger operator+(const BigInteger &a, const BigInteger &b) {
    BigInteger ans = a;
    ans += b;
    return ans;
}

BigInteger operator-(const BigInteger &a, const BigInteger &b) {
    BigInteger ans = a;
    ans -= b;
    return ans;
}

BigInteger &BigInteger::operator++() {
    return *this += 1;
}

BigInteger &BigInteger::operator--() {
    return *this -= 1;
}

BigInteger BigInteger::operator++(int) {
    BigInteger copy = *this;
    ++(*this);
    return copy;
}

BigInteger BigInteger::operator--(int) {
    BigInteger copy = *this;
    --(*this);
    return copy;
}

BigInteger silly_mult(const BigInteger &a, const BigInteger &other) {
    BigInteger ans;
    ans.clear();
    bool sgn = !(a._sign ^ other._sign);
    for (size_t i = 0; i < a.size(); i++) {
        BigInteger tmp;
        tmp.clear();
        tmp.resize(i);
        unsigned int add = 0;
        for (size_t j = 0; j < other.size(); j++) {
            tmp.push_back((1ull * a[i] * other[j] + add) % BigInteger::BASE);
            add = (1ull * a[i] * other[j] + add) / BigInteger::BASE;
        }
        if (add > 0) {
            tmp.push_back(add);
        }
        ans += tmp;
    }
    ans._sign = sgn;
    return ans._removeLeadingZeros();
}

BigInteger multPowOfBase(const BigInteger &a, int deg) {
    BigInteger ans;
    ans.clear();
    for (int i = 0; i < deg; i++) {
        ans.push_back(0);
    }
    for (size_t i = 0; i < a.size(); i++) {
        ans.push_back(a[i]);
    }
    ans._sign = a._sign;
    return ans._removeLeadingZeros();
}

BigInteger &BigInteger::operator*=(const BigInteger &other) {
    bool sgn = !(_sign ^ other._sign);
    BigInteger ans;
    size_t n = std::max(size(), other.size());
    if (n < 33) {
        return (*this) = silly_mult((*this), other);
    }
    BigInteger a, b;
    a.clear();
    b.clear();
    BigInteger c, d;
    c.clear();
    d.clear();
    for (size_t i = 0; i < n; ++i) {
        if (i < n / 2) {
            if (i < size())
                b.push_back((*this)[i]);
            if (i < other.size())
                d.push_back(other[i]);
        } else {
            if (i < size())
                a.push_back((*this)[i]);
            if (i < other.size())
                c.push_back(other[i]);
        }
    }
    if (a.size() == 0) {
        a = 0;
    }
    if (c.size() == 0) {
        c = 0;
    }
    a._removeLeadingZeros();
    b._removeLeadingZeros();
    c._removeLeadingZeros();
    d._removeLeadingZeros();
    BigInteger ac = a * c;
    BigInteger bd = b * d;
    BigInteger aplusbcplusd = (a + b) * (c + d);
    BigInteger tmp = multPowOfBase(ac, n / 2 * 2) + multPowOfBase(aplusbcplusd - ac - bd, n / 2) + bd;
    (*this) = tmp;
    _sign = sgn;
    return (*this)._removeLeadingZeros();
}

BigInteger operator*(const BigInteger &a, const BigInteger &b) {
    BigInteger tmp = a;
    tmp *= b;
    return tmp;
}

std::pair<BigInteger, BigInteger>
div(const BigInteger &a, const BigInteger &b, std::pair<BigInteger, BigInteger> &ans) {
    bool sgn = !(a._sign ^ b._sign);
    ans.first.clear();
    ans.second = 0;
    for (int i = a.size() - 1; i > -1; --i) {
        ans.second *= BigInteger::BASE;
        ans.second += a[i];
        unsigned int l = 0;
        unsigned int r = BigInteger::BASE;
        while (r - l > 1) {
            unsigned int m = (r + l) >> 1;
            BigInteger tmp = abs(b) * m;
            if (tmp > ans.second) {
                r = m;
            } else {
                l = m;
            }
        }
        ans.first.push_back(l);
        ans.second -= abs(b) * l;
    }
    std::reverse(ans.first.begin(), ans.first.end());
    ans.first._sign = sgn;
    ans.second._sign = sgn;
    ans.first._removeLeadingZeros();
    ans.second._removeLeadingZeros();
    return ans;
}

BigInteger &BigInteger::operator/=(const BigInteger &other) {
    std::pair<BigInteger, BigInteger> tmp;
    div((*this), other, tmp);
    return (*this) = tmp.first;
}

BigInteger &BigInteger::operator%=(const BigInteger &other) {
    std::pair<BigInteger, BigInteger> tmp;
    div((*this), other, tmp);
    return (*this) = tmp.second;
}

BigInteger operator/(const BigInteger &a, const BigInteger &b) {
    std::pair<BigInteger, BigInteger> tmp;
    div(a, b, tmp);
    return tmp.first;
}

BigInteger operator%(const BigInteger &a, const BigInteger &b) {
    std::pair<BigInteger, BigInteger> tmp;
    div(a, b, tmp);
    return tmp.second;
}

BigInteger abs(const BigInteger &x) {
    if (x < 0) return -x;
    return x;
}

// I/O Stream operators

std::ostream &operator<<(std::ostream &out, const BigInteger &A) {
    return out << A.toString();
}

std::istream &operator>>(std::istream &in, BigInteger &b) {
    std::string s;
    in >> s;
    b = s;
    return in;
}

// std::string converting

std::string BigInteger::toString() const {
    char *buf = new char[size() * 9 + !_sign + 1];
    int beg;
    if (_sign) {
        beg = 0;
    } else {
        beg = 1;
        buf[0] = '-';
    }
    beg += sprintf(buf + beg, "%u", back());
    for (int i = int(size()) - 2; i > -1; --i) {
        beg += sprintf(buf + beg, "%09u", (*this)[i]);
    }
    std::string ans = buf;
    delete[] buf;
    return ans;
}

BigInteger &BigInteger::_addToModule(const BigInteger &other) {
    int l;
    int r;
    int add = 0;
    for (size_t i = 0; i < std::max(size(), other.size()); ++i) {
        if (i >= size()) l = 0;
        else l = (*this)[i];
        if (i >= other.size()) r = 0;
        else r = other[i];
        int temp = (l + r + add);
        if (temp >= BigInteger::BASE) {
            temp -= BigInteger::BASE;
        }
        if (i < size()) {
            (*this)[i] = temp;
        } else {
            push_back(temp);
        }
        add = (l + r + add >= BigInteger::BASE);
    }
    if (add > 0) {
        push_back(add);
    }
    _removeLeadingZeros();
    return (*this);
}

BigInteger &BigInteger::_subLessModule(const BigInteger &other) {
    unsigned int remainder = 0;
    for (size_t i = 0; i < size(); i++) {
        int r;
        if (i >= other.size()) r = 0;
        else r = other[i];
        int temp = static_cast<int>((*this)[i]) - r - remainder;
        if ((*this)[i] < r + remainder) {
            (*this)[i] = temp + BigInteger::BASE;
            remainder = 1;
        } else {
            (*this)[i] = temp;
            remainder = 0;
        }
    }
    _removeLeadingZeros();
    return (*this);
}

BigInteger &BigInteger::operator+=(const BigInteger &other) {
    if (_sign == other._sign) {
        _addToModule(other);
        return (*this);
    }
    // example : 1 + (-2) = -(-1 - (-2))
    _sign ^= true;
    (*this) -= (other);
    _sign ^= true;
    _removeLeadingZeros();
    return *this;
}

BigInteger &BigInteger::operator-=(const BigInteger &other) {
    if (_sign != other._sign) {
        // example : 1 - (-2) = -(-1 + (-2))
        _sign ^= true;
        (*this) += other;
        _sign ^= true;
        return (*this);
    }
    if ((!_sign) ^ ((*this) >= other)) {
        // example : 20 - 1 or -20 - (-1)
        _subLessModule(other);
        return (*this);
    }
    // example : 1 - 20 or (-1) - (-20)
    BigInteger tmp = other;
    (*this) = (tmp -= (*this));
    _sign ^= true;
    _removeLeadingZeros();
    return (*this);
}

#endif