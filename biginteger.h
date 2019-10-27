#pragma once
#ifndef __BIGINT
#define __BIGINT

//#define _DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <iomanip>
#include <algorithm>



using std::cin;
using std::cout;
using std::endl;

using std::istream;
using std::ostream;
using std::vector;
using std::string;
using std::pair;

enum SIGN {
	LESS,
	EQUALS,
	MORE
};

struct BigInteger : private vector < unsigned int > {
public:
/*#ifndef _DEBUG
	const static int BASE = 1e9;
#endif
#ifdef _DEBUG*/
	const static int BASE = 1e9;
//#endif

	std::string toString() const;
	BigInteger operator -() const;
	BigInteger& operator++();
	BigInteger& operator--();
	const BigInteger operator++(int);
	const BigInteger operator--(int);
	BigInteger(int x = 0);
	BigInteger(const std::string &s);
	explicit operator bool() const;
	//friend BigInteger operator *(const BigInteger& left, const BigInteger& right);
	BigInteger& operator *= (const BigInteger &other);
	BigInteger& operator /= (const BigInteger &other);
	BigInteger& operator %= (const BigInteger &other);
	//friend BigInteger operator /(const BigInteger& left, const BigInteger& right);
	//friend BigInteger sdv_x(const BigInteger& a, int x);

	SIGN Compare(const BigInteger &other, bool changeMysign = false, bool changeOthersign = false) const;

	BigInteger& operator += (const BigInteger &other);

	BigInteger& operator -= (const BigInteger &other);

	friend istream& operator >> (istream &cin, BigInteger &b);

	friend BigInteger abs(const BigInteger &x);

private:
	bool sign;

	friend void sum(const BigInteger &a, const BigInteger &b, BigInteger &ans, bool changeAsign, bool changeBsign);
	friend void sub(const BigInteger &a, const BigInteger &b, BigInteger &ans, bool changeAsign, bool changeBsign);

	friend pair < BigInteger, BigInteger > div(const BigInteger &a, const BigInteger &b, pair < BigInteger, BigInteger > &ans);

	BigInteger& removeLeadingZeros();
};

// Utility

void sum(const BigInteger &a, const BigInteger &b, BigInteger &ans, bool changeAsign = false, bool changeBsign = false) {
	if (!(a.sign ^ changeAsign)) {
		// a is negative
		sub(b, a, ans, changeBsign, changeAsign ^ 1);
		return;
	}
	// a.sign ^ changeAsign == true
	if (!(b.sign ^ changeBsign)) {
		// b is negative
		sub(a, b, ans, changeAsign, changeBsign ^ 1);
		return;
		//return a - (-b);
	}
	// a.sign ^ changeBsign == true
	unsigned int add = 0;
	//BigInteger ans;
	if (&a != &ans && &b != &ans) {
		ans.clear();
	}
	ans.resize(std::max(a.size(), b.size()));
	for (size_t i = 0; i < std::max(a.size(), b.size()); ++i) {
		int l, r;
		if (i >= a.size()) l = 0;
		else l = a[i];
		if (i >= b.size()) r = 0;
		else r = b[i];
		ans[i] = (l + r + add) % BigInteger::BASE;
		add = (l + r + add) / BigInteger::BASE;
	}
	if (add > 0) {
		ans.push_back(add);
	}
	ans.removeLeadingZeros();
	ans.sign = 1;
	return;
}

void sub(const BigInteger &a, const BigInteger &b, BigInteger &ans, bool changeAsign = false, bool changeBsign = false) {
	if (!(a.sign ^ changeAsign)) {
		// a is negative
		sub(a, b, ans, changeAsign ^ 1, changeBsign ^ 1);
		ans.sign ^= 1;
		ans.removeLeadingZeros();
		return;
	}
	// a is positive
	if (!(b.sign ^ changeBsign)) {
		// b is negative
		sum(a, b, ans, changeAsign, changeBsign ^ 1);
		return;
	}
	// b is positive
	if (b.Compare(a, changeBsign, changeAsign) == SIGN::MORE) {
		//6 - 10 = -4   10 - 6 = 4
		sub(b, a, ans, changeBsign, changeAsign);
		ans.sign ^= 1;
		ans.removeLeadingZeros();
		return;
	}
	unsigned int reminder = 0;
	//BigInteger ans;
	ans.sign = 1;
	if (&a != &ans && &b != &ans) {
		ans.clear();
	}
	ans.resize(std::max(a.size(), b.size()));
	for (size_t i = 0; i < a.size(); i++) {
		int r;
		if (i >= b.size()) r = 0;
		else r = b[i];
		if (a[i] < r + reminder) {
			ans[i] = a[i] + BigInteger::BASE - r - reminder;
			reminder = 1;
		}
		else {
			ans[i] = a[i] - r - reminder;
			reminder = 0;
		}
	}
	ans.removeLeadingZeros();
	return;
}

BigInteger& BigInteger::removeLeadingZeros() {
	while (size() > 1 && back() == 0) {
		pop_back();
	}
	if (size() == 1 && (*this)[0] == 0) {
		sign = 1;
	}
	return *this;
}

SIGN BigInteger::Compare(const BigInteger &other, bool changeMysign, bool changeOthersign) const {
	if ((sign ^ changeMysign) == 0 && (other.sign ^ changeOthersign) == 1) return SIGN::LESS;
	if ((sign ^ changeMysign) == 1 && (other.sign ^ changeOthersign) == 0) return SIGN::MORE;
	if (size() < other.size()) return SIGN::LESS;
	if (size() > other.size()) return SIGN::MORE;
	int i = int(size()) - 1;
	while (i > -1 && (*this)[i] == other[i]) --i;
	if (i > -1) {
		if ((*this)[i] < other[i]) {
			return (sign ^ changeMysign) == 1 ? SIGN::LESS : SIGN::MORE;
		}
		else {
			return (sign ^ changeMysign) == 1 ? SIGN::MORE : SIGN::LESS;
		}
	}
	return SIGN::EQUALS;
}

// Constructors

BigInteger::BigInteger(int a) {
	if (a >= 0) {
		sign = 1;
	}
	else {
		a = -a;
		sign = 0;
	}
	if (a == 0) {
		push_back(0);
		return;
	}
	while (a > 0) {
		push_back(a % BASE);
		a /= BASE;
	}
	removeLeadingZeros();
}

BigInteger::BigInteger(const std::string &s) {
	if (s == "0") {
		*this = BigInteger(0);
		return;
	}
	sign = 1;
	clear();
	long long now = 0;
	long long st = 1;
	for (int i = s.size() - 1; i > 0; i--) {
		now += st * (s[i] - '0');
		st *= 10;
		if (st >= BigInteger::BASE) {
			push_back(now % BigInteger::BASE);
			now /= BigInteger::BASE;
			st = 1;
		}
	}
	if (s[0] == '-') {
		sign = 0;
	}
	else {
		now += st * (s[0] - '0');
		st *= 10;
		if (st >= BigInteger::BASE) {
			push_back(now % BigInteger::BASE);
			now /= BigInteger::BASE;
			st = 1;
		}
	}
	if (now > 0) {
		push_back(now % BigInteger::BASE);
	}
	removeLeadingZeros();
}

// Compare operatos

bool operator <(const BigInteger &a, const BigInteger &b) {
	return a.Compare(b) == SIGN::LESS;
}

bool operator >(const BigInteger &a, const BigInteger &b) {
	return a.Compare(b) == SIGN::MORE;
}

bool operator ==(const BigInteger &a, const BigInteger &b) {
	return a.Compare(b) == SIGN::EQUALS;
}

bool operator !=(const BigInteger &a, const BigInteger &b) {
	return a.Compare(b) != SIGN::EQUALS;
}

bool operator <=(const BigInteger &a, const BigInteger &b) {
	SIGN tmp = a.Compare(b);
	return tmp == SIGN::EQUALS || tmp == SIGN::LESS;
}

bool operator >=(const BigInteger &a, const BigInteger &b) {
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
	ans.sign ^= 1;
	return ans;
}

BigInteger operator + (const BigInteger &a, const BigInteger &b) {
	//exit(0);
	BigInteger ans;
	sum(a, b, ans);
	return ans;
}

BigInteger operator - (const BigInteger &a, const BigInteger &b) {
	//exit(0);
	BigInteger ans;
	sub(a, b, ans);
	return ans;
}

BigInteger& BigInteger::operator += (const BigInteger &other) {
	//exit(0);
	sum(*this, other, *this);
	return *this;
}

BigInteger& BigInteger::operator -= (const BigInteger &other) {
	//exit(0);
	sub(*this, other, *this);
	return *this;
}

BigInteger& BigInteger::operator++() {
	sum(*this, 1, *this);
	return *this;
}

BigInteger& BigInteger::operator--() {
	sub(*this, 1, *this);
	return *this;
}

const BigInteger BigInteger::operator++(int) {
	BigInteger copy = *this;
	++(*this);
	return copy;
}

const BigInteger BigInteger::operator--(int) {
	BigInteger copy = *this;
	--(*this);
	return copy;
}

BigInteger& BigInteger::operator *= (const BigInteger &other) {
	BigInteger ans;
	ans.clear();
	bool sgn = !(sign ^ other.sign);
	for (size_t i = 0; i < size(); i++) {
		BigInteger tmp;
		tmp.clear();
		tmp.resize(i);
		unsigned int add = 0;
		for (size_t j = 0; j < other.size(); j++) {
			tmp.push_back((1ull * (*this)[i] * other[j] + add) % BASE);
			add = (1ull * (*this)[i] * other[j] + add) / BASE;
		}
		if (add > 0) {
			tmp.push_back(add);
		}
		ans += tmp;
	}
	ans.sign = sgn;
	return (*this) = ans.removeLeadingZeros();
}

BigInteger operator * (const BigInteger &a, const BigInteger &b) {
	BigInteger tmp = a;
	tmp *= b;
	return tmp;
}

pair < BigInteger, BigInteger > div(const BigInteger &a, const BigInteger &b, pair < BigInteger, BigInteger > &ans) {
	//assert(b != 0);
	bool sgn = !(a.sign ^ b.sign);
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
			}
			else {
				l = m;
			}
		}
		ans.first.push_back(l);
		ans.second -= abs(b) * l;
	}
	reverse(ans.first.begin(), ans.first.end());
	ans.first.sign = sgn;
	ans.second.sign = sgn;
	ans.first.removeLeadingZeros();
	ans.second.removeLeadingZeros();
	return ans;
}

BigInteger& BigInteger::operator/= (const BigInteger &other) {
	pair < BigInteger, BigInteger > tmp;
	div((*this), other, tmp);
	return (*this) = tmp.first;
}

BigInteger& BigInteger::operator%= (const BigInteger &other) {
	pair < BigInteger, BigInteger > tmp;
	div((*this), other, tmp);
	return (*this) = tmp.second;
}

BigInteger operator / (const BigInteger &a, const BigInteger &b) {
	pair < BigInteger, BigInteger > tmp;
	div(a, b, tmp);
	return tmp.first;
}

BigInteger operator % (const BigInteger &a, const BigInteger &b) {
	pair < BigInteger, BigInteger > tmp;
	div(a, b, tmp);
	return tmp.second;
}

BigInteger abs(const BigInteger &x) {
	BigInteger ans = x;
	ans.sign = 1;
	return ans;
}

// I/O Stream operators

std::ostream& operator<<(std::ostream &out, const BigInteger &A) {
	return out << A.toString();
}

istream& operator >> (istream &cin, BigInteger &b) {
	string s;
	cin >> s;
	b = s;
	return cin;
}

// String converting

string BigInteger::toString() const {
	char *buf = new char[size() * 9 + !sign + 1];
	int beg;
	if (sign) {
		beg = 0;
	}
	else {
		beg = 1;
		buf[0] = '-';
	}
	beg += sprintf(buf + beg, "%u", back());
	for (int i = int(size()) - 2; i > -1; --i) {
		beg += sprintf(buf + beg, "%01u", (*this)[i]);
	}
	string  ans = buf;
	delete[] buf;
	return ans;
}

#endif 