#pragma once

#include "http_request.h"

#include <string_view>
#include <map>
#include <set>
using namespace std;

extern const map<string_view, int> exp_method_count;
extern const map<string_view, int> exp_url_count;

class Stats {
public:
	Stats();
	void AddMethod(string_view method);
	void AddUri(string_view uri);
	const map<string_view, int>& GetMethodStats() const;
	const map<string_view, int>& GetUriStats() const;
private:
	map<string_view, int> expected_method_count;
	map<string_view, int> expected_url_count;
};

HttpRequest ParseRequest(string_view line);