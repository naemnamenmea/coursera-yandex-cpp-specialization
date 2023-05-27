#pragma once

#include "http_request.h"
#include "stats.h"

#include <string_view>
#include <map>
#include <set>
using namespace std;

const map<string_view, int> exp_method_count = {
  {"GET", 0},
  {"PUT", 0},
  {"POST", 0},
  {"DELETE", 0},
  {"UNKNOWN", 0}
};

const map<string_view, int> exp_url_count = {
  {"/", 0},
  {"/order", 0},
  {"/product", 0},
  {"/basket", 0},
  {"/help", 0},
  {"unknown", 0}
};

Stats::Stats()
	:expected_method_count(exp_method_count), expected_url_count(exp_url_count)
{}

void Stats::AddMethod(string_view method) {
	auto it = expected_method_count.find(method);
	if (it == expected_method_count.end() || next(it) == expected_method_count.end()) {
		++expected_method_count["UNKNOWN"];
	}
	else {
		++it->second;
	}
}

void Stats::AddUri(string_view uri) {
	auto it = expected_url_count.find(uri);
	if (it == expected_url_count.end() || next(it) == expected_url_count.end()) {
		++expected_url_count["unknown"];
	}
	else {
		++it->second;
	}
}

const map<string_view, int>& Stats::GetMethodStats() const {
	return expected_method_count;
}

const map<string_view, int>& Stats::GetUriStats() const {
	return expected_url_count;
}

HttpRequest ParseRequest(string_view line) {
	HttpRequest res;
	size_t pos, next;
	pos = line.find_first_not_of(' ');
	next = line.find_first_of(' ', pos);
	res.method = line.substr(pos, next - pos);
	pos = next;
	next = line.find_first_of(' ', pos + 1);
	res.uri = line.substr(pos + 1, next - pos - 1);
	pos = next;
	next = line.find_first_of(' ', pos + 1);
	res.protocol = line.substr(pos + 1, next - pos - 1);

	return res;
}
