#pragma once

#include "profile.h"

#include <iostream>
#include <string_view>
#include <string>
#include <vector>

using namespace std;

vector<string_view> SplitIntoWords(string_view line);

istream& ReadLine(istream& input, string& s, TotalDuration& dest);