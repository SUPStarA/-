#pragma once
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


struct BigInteger {
public:

	explicit operator bool() {
	    this->removeLeadingZeros();
		if (a.size() == 1 && a[0] == 0) return 0;
		return 1;
	}

	BigInteger(int a = 0) {
		if (a >= 0) {
			sign = 1;
		}
		else {
			a = -a;
			sign = 0;
		}
		if (a == 0) {
			this->a.push_back(0);
			return;
		}
		while (a > 0) {
			this->a.push_back(a % BLO);
			a /= BLO;
		}
		removeLeadingZeros();
	}

	friend istream& operator >> (istream &cin, BigInteger &b){
		string s;
		cin >> s;
		if (s == "0") {
			b = BigInteger(0);
			return cin;
		}
		b.sign = 1;
		b.a.clear();
		long long now = 0, st = 1;
		for (int i = s.size() - 1; i > 0; i--) {
			now += st * (s[i] - '0');
			st *= 10;
			if (st >= BLO) {
				b.push_back(now % BLO);
				now /= BLO;
				st = 1;
			}
		}
		if (s[0] == '-') {
			b.sign = 0;
		}
		else {
			now += st * (s[0] - '0');
			st *= 10;
			if (st >= BLO) {
				b.push_back(now % BLO);
				now /= BLO;
				st = 1;
			}
		}
		if (now > 0) {
			b.push_back(now % BLO);
		}
		b.removeLeadingZeros();
		return cin;
	}

	friend ostream& operator << (ostream &cout, const BigInteger &b){
		if (!b.sign) cout << "-";
		cout << b.a.back();
		for (int i = b.size() - 2; i > -1; i--) {
			cout << std::setfill('0') << std::setw(9) << b.a[i] << std::ends << endl;
		}
		return cout;
	}

	friend bool operator == (const BigInteger &a, const BigInteger &b) {
		if (a.size() == 1 && a[0] == 0) {
			if (b.size() == 1 && b[0] == 0) {
				return true;
			}
			return false;
		}
		return (a.sign == b.sign && a.a == b.a);
	}

	friend bool operator != (const BigInteger &a, const BigInteger &b) {
		return !(a == b);
	}

	friend bool operator < (const BigInteger &a, const BigInteger &b) {
		if (a == b) {
			return false;
		}
		if (a.sign < b.sign) return true;
		if (a.sign > b.sign) return false;
		if (a.size() < b.size()) {
			return a.sign;
		}
		if (a.size() > b.size()) {
			return !a.sign;
		}
		for (int i = a.size() - 1; i > -1; i--) {
			if (a[i] != b[i]) {
				return (a[i] > b[i]) ^ a.sign;
			}
		}
		return false;
	}



	friend bool operator > (const BigInteger &a, const BigInteger &b) {
		return b < a;
	}

	friend bool operator >= (const BigInteger &a, const BigInteger &b) {
		return a > b || a == b;
	}

	friend bool operator <= (const BigInteger &a, const BigInteger &b) {
		return a < b || a == b;
	}

	BigInteger operator -() const {
		BigInteger tmp = *this;
		tmp.sign ^= 1;
		return tmp;
	}

	friend BigInteger operator + (const BigInteger &a, const BigInteger &b) {
		if (!a.sign) {  
			// a is negative
			return b - (-a);
		}
		// a is positive
		if (!b.sign) {   
			// b is negative
			return a - (-b);
		}
		// b is positive
		long long add = 0;
		BigInteger ans;
		ans.a.clear();
		for (int i = 0; i < std::max(a.size(), b.size()); ++i) {
			ans.push_back((a[i] + b[i] + add) % BLO);
			add = (a[i] + b[i] + add) / BLO;
		}
		if (add > 0) {
			ans.push_back(add);
		}
		return ans.removeLeadingZeros();
	}

	friend BigInteger& operator += (BigInteger &a, const BigInteger &b) {
		return a = a + b;
	}

	friend BigInteger operator - (const BigInteger &a, const BigInteger &b) {
		if (!a.sign) { 
			// a is negative
			return -((-a) - (-b));
		}
		// a is positive
		if (!b.sign) { 
			// b is negative
			return a + (-b);
		}
		// b is positive
		if (b > a) {
			//6 - 10 = -4   10 - 6 = 4
			return -(b - a);
		}
		long long reminder = 0;
		BigInteger ans;
		ans.a.clear();
		for (int i = 0; i < a.size(); i++) {
			if (a[i] - b[i] - reminder < 0) {
				ans.push_back(a[i] - b[i] + BLO - reminder);
				reminder = 1;
			}
			else {
				ans.push_back(a[i] - b[i] - reminder);
				reminder = 0;
			}
		}
		return ans.removeLeadingZeros();
	}

	friend BigInteger& operator -= (BigInteger &a, const BigInteger &b) {
		return a = a - b;
	}

	friend BigInteger operator * (const BigInteger &a, const BigInteger &b) {
		BigInteger ans;
		ans.a.clear();
		bool sgn = !(a.sign ^ b.sign);
		/*a.sign = 1;
		b.sign = 1;*/
		BigInteger st;
		st.a.clear();
		for (int i = 0; i < a.size(); i++) {
			BigInteger tmp = st;
			long long add = 0;
			for (int j = 0; j < b.size(); j++) {
				tmp.push_back((a[i] * b[j] + add) % BLO);
				add = (a[i] * b[j] + add) / BLO;
			}
			if (add > 0) {
				tmp.push_back(add);
			}
			ans = ans + tmp;
			st.push_back(0);
		}
		ans.sign = sgn;
		return ans.removeLeadingZeros();
	}

	friend BigInteger& operator *= (BigInteger &a, const BigInteger &b) {
		return a = a * b;
	}

	friend BigInteger operator / (const BigInteger &a, const BigInteger &b) {
		return div(a, b).first;
	}

	friend BigInteger& operator /= (BigInteger &a, const BigInteger &b) {
		return a = a / b;
	}

	friend BigInteger operator % (const BigInteger &a, const BigInteger &b) {
		return div(a, b).second;
	}

	friend BigInteger& operator %= (BigInteger &a, const BigInteger &b) {
		return a = a % b;
	}

	BigInteger& operator ++ ()
	{
		*this += 1;
		return *this;
	}

	BigInteger operator ++ (int)
	{
		BigInteger tmp = *this;
		*this += 1;
		return tmp;
	}

	BigInteger& operator -- ()
	{
		*this -= 1;
		return *this;
	}
	BigInteger operator -- (int)
	{
		BigInteger tmp = *this;
		*this -= 1;
		return tmp;
	}

	string toString() {
		string ans;
		int tmp = a[0];
		while (tmp > 0) {
			ans += tmp % 10 + '0';
			tmp /= 10;
		}
		if (!sign) ans += "-";
		std::reverse(ans.begin(), ans.end());
		for (int i = 1; i < size(); i++) {
			for (int x = BLO / 10; x > 0; x /= 10) {
				ans += a[i] / x % 10 + '0';
			}
		}
		return ans;
	}

	friend BigInteger abs(const BigInteger &a) {
		return (a.sign ? a: -a);
	}

private:
	vector < long long > a;
	char sign;
	static const int BLO = 1e9; // power of 10


	void push_back(long long x) {
		a.push_back(x);
	}

	void pop_back() {
		assert(a.size() > 0);
		//if (a.size() <= 0) exit(0);
		a.pop_back();
	}

	long long operator [](int x) const {
		assert(x >= 0);
		//if (x < 0) exit(0);
		//if (x >= this->size()) return 0;
		return a[x];
	}

	long long back() const {
		assert(a.size() > 0);
		//if (a.size() <= 0) exit(0);
		return a.back();
	}

	int size() const {
		return a.size();
	}

	BigInteger& removeLeadingZeros() {
		while (size() > 1 && back() == 0) {
			pop_back();
		}
		if (size() == 1 && a[0] == 0) {
			sign = 1;
		}
		return *this;
	}

	long long & operator [](const int &x) {
		assert(x < this->size() && x >= 0);
		//if (x >= size()) exit(0);
		//if (x < 0) exit(0);
		assert(x >= 0);
		return a[x];
	}

	friend pair < BigInteger, BigInteger > div(const BigInteger &a, const BigInteger &b) {
		assert(b != 0);
		bool sgn = !(a.sign ^ b.sign);
		BigInteger now(0);
		vector < long long > ans;
		for (int i = a.size() - 1; i > -1; i--) {
			now = now * BigInteger(BLO);
			now = now + BigInteger(a[i]);
			long long l = 0;
			long long r = BLO;
			while (r - l > 1) {
				long long m = (r + l) >> 1;
				if (abs(b) * BigInteger(m) > now) {
					r = m;
				}
				else {
					l = m;
				}
			}
			ans.push_back(l);
			now = now - abs(b) * BigInteger(l);
		}
		reverse(ans.begin(), ans.end());
		BigInteger Ans;
		Ans.a = ans;
		Ans.sign = sgn;
		now.sign = sgn;
		return std::make_pair(Ans.removeLeadingZeros(), now.removeLeadingZeros());
	}
};