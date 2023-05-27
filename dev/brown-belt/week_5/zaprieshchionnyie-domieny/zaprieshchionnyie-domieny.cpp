#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <cassert>
#include <sstream>

#ifdef LOCAL_LAUNCH
#include "test_runner.hpp"
#endif

using namespace std;

bool IsSubdomain(string_view subdomain, string_view domain) {
  size_t i = 0;
  size_t j = 0;
  while (i < subdomain.size() && j < domain.size()) {
    if (subdomain[i++] != domain[j++]) {
      return false;
    }
  }

  if (j == domain.size())
  {
    return i == subdomain.size() || subdomain[i] == '.';
  }
  else
  {
    return i == subdomain.size();
  }
}


vector<string> ReadDomains() {
  size_t count;
  cin >> count;

  vector<string> domains;
  for (size_t i = 0; i < count; ++i) {
    string domain;
    cin >> domain;
    domains.push_back(move(domain));
  }
  return domains;
}

vector<bool> AuthorsSolve(vector<string>& banned_domains, const vector<string>& domains_to_check)
{
  for (string& domain : banned_domains) {
    reverse(begin(domain), end(domain));
  }
  sort(begin(banned_domains), end(banned_domains));

  size_t insert_pos = 0;
  for (string& domain : banned_domains) {
    if (insert_pos == 0 || !IsSubdomain(domain, banned_domains[insert_pos - 1])) {
      swap(banned_domains[insert_pos++], domain);
    }
  }
  banned_domains.resize(insert_pos);

  vector<bool> res;
  for (const string& domain : domains_to_check) {
    const string d(domain.crbegin(), domain.crend());
    const auto it = upper_bound(begin(banned_domains), end(banned_domains), d);
    const bool isGood = it == begin(banned_domains) || !IsSubdomain(d, *prev(it));

    res.push_back(isGood);
  }

  return res;
}

bool IsGood(const set<string>& forbiddenDomaines, string_view sv)
{
  auto pred = [&sv](const string& str) -> bool {
    size_t pos = sv.find(str);

    if (pos == sv.npos)
      return false;

    return (pos == 0 || sv[pos - 1] == '.') && pos + str.length() == sv.length();
  };

  return !any_of(forbiddenDomaines.begin(), forbiddenDomaines.end(), pred);
}

vector<bool> Solve(vector<string>& forbiddenDomainesVec, const vector<string>& domainsToTest)
{
  vector<bool> res(domainsToTest.size());

  set<string> forbiddenDomainesSet(
    make_move_iterator(forbiddenDomainesVec.begin()),
    make_move_iterator(forbiddenDomainesVec.end()));

  for (size_t i = 0; i < domainsToTest.size(); ++i)
  {
    res[i] = IsGood(forbiddenDomainesSet, domainsToTest[i]);
  }

  return res;
}

#ifdef LOCAL_LAUNCH
template<typename Func>
void TestBasic1(Func func)
{
  vector<string> forbiddenDomaines = { "ya.ru", "maps.me", "m.ya.ru", "com" };
  const vector<string> domainsToTest = {
    "ya.ru", "ya.com", "m.maps.me" ,"moscow.m.ya.ru",
    "maps.com", "maps.ru", "ya.ya"
  };

  const vector<bool> expected = { false,false,false,false,false,true,true };

  assert(domainsToTest.size() == expected.size());

  vector<bool> result = func(forbiddenDomaines, domainsToTest);

  ASSERT_EQUAL(expected, result);
}

template<typename Func>
void TestBasic2(Func func)
{
  vector<string> forbiddenDomaines = { "com" };
  const vector<string> domainsToTest = { "com.ru", "ru.com.en", "ru.com" };

  const vector<bool> expected = { true, true, false };

  assert(domainsToTest.size() == expected.size());

  vector<bool> result = func(forbiddenDomaines, domainsToTest);

  ASSERT_EQUAL(expected, result);
}

template<typename Func>
void RunAllTests(Func func)
{
  TestRunner tr;
  RUN_TEST(tr, TestBasic1<Func>, func);
  RUN_TEST(tr, TestBasic2<Func>, func);
}
#endif

int main()
{
#ifdef LOCAL_LAUNCH
  RunAllTests(&AuthorsSolve);
#endif

#ifndef LOCAL_LAUNCH
  vector<string> banned_domains = ReadDomains();
  vector<string> domains_to_check = ReadDomains();

  //vector<bool> res = Solve(banned_domains, domains_to_check);
  vector<bool> res = AuthorsSolve(banned_domains, domains_to_check);
  for (size_t i = 0; i < res.size(); ++i)
  {
    cout << (res[i] ? "Good" : "Bad") << endl;
  }
#endif
  return 0;
}