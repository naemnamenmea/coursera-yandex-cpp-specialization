#include "parse.h"

string_view Strip(string_view s) {
  while (!s.empty() && isspace(s.front())) {
    s.remove_prefix(1);
  }
  while (!s.empty() && isspace(s.back())) {
    s.remove_suffix(1);
  }
  return s;
}

string_view StripLeft(string_view sv, char sep) {
  size_t pos = sv.find_first_not_of(sep);
  sv.remove_prefix(pos != sv.npos ? pos : sv.size());
  return sv;
}

string_view ReadToken(string_view& sv, char sep) {
  sv = StripLeft(sv,sep);
  size_t pos = sv.find(sep);
  string_view word = sv.substr(0, pos);
  sv.remove_prefix(pos != sv.npos ? pos + 1 : sv.size());
  return word;
}

vector<string_view> SplitBy(string_view line, char sep) {
  vector<string_view> res;

  while (true) {
    string_view word = ReadToken(line, sep);
    if (word.empty())
      break;
    res.push_back(word);
  }
  return res;
}

#ifdef LOCAL_LAUNCH
istream& ReadLine(istream& input, string& s, TotalDuration& dest) {
  ADD_DURATION(dest);
  return getline(input, s);
}
#endif