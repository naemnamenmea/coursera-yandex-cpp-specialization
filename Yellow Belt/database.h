#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include <exception>
#include <set>
#include <algorithm>
#include <map>

#include "date.h"

using namespace std;

using Predicate = function<bool(const Date& date, const string& event)>;


class Database {
public:
	void Add(const Date& date, const string& event);
	void Print(ostream& os) const;
	string Last(const Date& date) const;
	vector<pair<Date, string>> FindIf(const Predicate& predicate) const;
	int RemoveIf(const Predicate& predicate);

private:
	multimap<Date, string> db_;
	set<pair<Date, string>> db2_;
};