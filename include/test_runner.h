#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <set>
#include <windows.h>

using namespace std;

template <typename Collection>
string Join(const Collection& c, char d) {
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
	return os << '(' << p.first << ',' << p.second << ')';
}

template <typename K, typename V>
ostream& operator<<(ostream& os, const map<K, V>& m) {
	return os << '{' << Join(m, ',') << '}';
}

template <typename T>
ostream& operator<<(ostream& os, const set<T>& s) {
	return os << '{' << Join(s, ',') << '}';
}

template<class T, class U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
	if (!(t == u)) {
		ostringstream os;
		os << "Assertion failed: " << t << " != " << u;
		if (!hint.empty()) {
			os << " hint: " << hint;
		}
		throw runtime_error(os.str());
	}
}

inline void Assert(bool b, const string& hint) {
	AssertEqual(b, true, hint);
}

class TestRunner {
public:
	template <class TestFunc>
	void RunTest(TestFunc func, const string& test_name) {
		try {
			func();
			cerr << test_name << " OK" << endl;
		}
		catch (exception & e) {
			++fail_count;
			cerr << test_name << " fail: " << e.what() << endl;
		}
		catch (...) {
			++fail_count;
			cerr << "Unknown exception caught" << endl;
		}
	}

	~TestRunner() {
		if (fail_count > 0) {
			cerr << fail_count << " unit tests failed. Terminate" << endl;
			exit(1);
		}
	}

private:
	int fail_count = 0;
};

#define ASSERT_EQUAL(x, y) {            \
  ostringstream os;                     \
  os << #x << " != " << #y << ", "      \
    << __FILE__ << ":" << __LINE__;     \
  AssertEqual(x, y, os.str());          \
}

#define ASSERT(x) {                     \
  ostringstream os;                     \
  os << #x << " is false, "             \
    << __FILE__ << ":" << __LINE__;     \
  Assert(x, os.str());                  \
}

#define RUN_TEST(tr, func) \
  tr.RunTest(func, #func)