#include <iostream>
#include <string>

#include "profile.h"

using namespace std;

istream& ReadLine(istream& input, string& s, TotalDuration& dest) {
  ADD_DURATION(dest);
  return getline(input, s);
}