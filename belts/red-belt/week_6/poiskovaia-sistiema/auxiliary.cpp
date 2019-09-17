#include "auxiliary.h"

#include <sstream>
#include <iterator>

vector<string_view> SplitIntoWords(string_view line) {
  vector<string_view> res;

  while (true) {
    size_t pos = line.find_first_not_of(' ');
    line.remove_prefix(pos);

    if (pos == line.npos) {
      break;
    }

    size_t to = line.find(' ');

    res.push_back(line.substr(0, to));

    if (to == line.npos) {
      break;
    }

    line.remove_prefix(to + 1);
  }

  return res;
}

// --

//vector<string_view> SplitIntoWords(string_view line) {
//  vector<string_view> res;
//  res.reserve(line.size() / 2);
//
//  while (!line.empty()) {
//    auto const word_start = line.find_first_not_of(' ');
//    if (word_start == line.npos) {
//      break;
//    }
//    auto const word_end = line.find(' ', word_start + 1);
//    res.push_back(line.substr(word_start, word_end - word_start));
//    line.remove_prefix(min({ word_end, line.size() }));
//  }
//  return res;
//}

// --

//void LeftStrip(string_view& sv) {
//  // удаляем лишние пробелы слева
//  while (!sv.empty() && isspace(sv[0])) {
//    sv.remove_prefix(1);
//  }
//}
//string_view ReadToken(string_view& sv) {
//  LeftStrip(sv);
//  // ищем пробел
//  // т.е. позицию конца слова - мы ведь сначала удалили пробелы
//  auto pos = sv.find(' ');
//  // создаем слово
//  auto word = sv.substr(0, pos);
//  // удаляем слово и
//  sv.remove_prefix(pos != sv.npos ? pos : sv.size());
//  return word;
//}
//
//vector<string_view> SplitIntoWords(string_view line) {
//  vector<string_view> value;
//  /* читаем строку(содержимое файла
//   * за одну итерацию читаем слово
//   * добавляем слово в вектор
//   * */
//  for (string_view word = ReadToken(line); !word.empty(); word = ReadToken(line)) {
//    value.push_back(word);
//  }
//  return value;
//}

// --

istream& ReadLine(istream& input, string& s, TotalDuration& dest) {
  ADD_DURATION(dest);
  return getline(input, s);
}