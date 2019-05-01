#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <set>
#include <windows.h>

using namespace std;

template <typename Collection>
string Join(const Collection & c, char d) {
	stringstream ss;
	bool flag = false;
	for (const auto& el : c) {
		if (flag) {
			ss << d;
		}
		flag = true;
		ss << el;
	}
	return ss.str();
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
	return os << '[' << Join(v, ',') << ']';
}

template <typename L, typename R>
ostream& operator<<(ostream& os, const pair<L, R>& p) {
	return os << p.first << ' ' << p.second;
}

template <typename K, typename V>
ostream& operator<<(ostream& os, const map<K, V>& m) {
	return os << '{' << Join(m, ',') << '}';
}

template <typename K, typename V>
ostream& operator<<(ostream& os, const multimap<K, V>& m) {
	return os << '{' << Join(m, ',') << '}';
}

template <typename T>
ostream& operator<<(ostream& os, const set<T>& s) {
	return os << '{' << Join(s, ',') << '}';
}

template <typename T>
ostream& operator<<(ostream& os, const multiset<T>& s) {
	return os << '{' << Join(s, ',') << '}';
}

template<class T, class U>
void AssertEqual(const T& t, const U& u,
	const string& hint)
{
	if (!(t == u)) {
		ostringstream os;
		os << "Assertion failed: " << t << " != " << u
			<< " hint: " << hint;
		throw runtime_error(os.str());
	}
}

inline void Assert(bool b, const string& hint) {
	AssertEqual(b, true, hint);
}

class TestRunner {
public:
	TestRunner() {
	}

	template <class TestFunc>
	void RunTest(TestFunc func, const string& test_name) {
		try {
			func();
			cerr << test_name << " OK" << endl;
		}
		catch (runtime_error & e) {
			++fail_count;
			cerr << test_name << " fail: " << e.what() << endl;
		}
	}

	~TestRunner() {
		if (fail_count > 0) {
			cerr << fail_count << " unit tests failed. Terminate" << endl;
			exit(1);
		}
		else {
			cerr << "All tests passed." << endl;
		}
	}

private:
	int fail_count = 0;
};