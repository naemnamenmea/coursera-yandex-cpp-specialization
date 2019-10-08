#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <set>

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

#ifdef LOCAL_LAUNCH
class TestRunner {
public:
  template <class TestFunc, class... Args>
  void RunTest(TestFunc func, string_view test_name, Args... args) {
    try {
      func(args...);
      cerr << test_name << " OK" << endl;
    }
    catch (exception& e) {
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
#else
class TestRunner
{
public:
  template <class TestFunc>
  void RunTest(TestFunc func, const string& test_name)
  {
    try
    {
      func();
      cerr << test_name << " OK" << endl;
    }
    catch (exception& e)
    {
      ++fail_count;
      cerr << test_name << " fail: " << e.what() << endl;
    }
    catch (...)
    {
      ++fail_count;
      cerr << "Unknown exception caught" << endl;
    }
  }

  ~TestRunner()
  {
    if (fail_count > 0)
    {
      cerr << fail_count << " unit tests failed. Terminate" << endl;
      exit(1);
    }
  }

private:
  int fail_count = 0;
};
#endif

#define ASSERT_EQUAL(x, y) {							  \
  ostringstream __assert_equal_private_os;                \
  __assert_equal_private_os << #x << " != " << #y << ", " \
    << __FILE__ << ":" << __LINE__;						  \
  AssertEqual(x, y, __assert_equal_private_os.str());     \
}

#define ASSERT(x) {								   \
  ostringstream __assert_equal_private_os;		   \
  __assert_equal_private_os << #x << " is false, " \
    << __FILE__ << ":" << __LINE__;				   \
  Assert(x, __assert_equal_private_os.str());      \
}

#ifdef LOCAL_LAUNCH
#define RUN_TEST(tr, func, ...) \
  tr.RunTest(func, #func, __VA_ARGS__)
#else
#define RUN_TEST(tr, func) \
	tr.RunTest(func, #func)
#endif