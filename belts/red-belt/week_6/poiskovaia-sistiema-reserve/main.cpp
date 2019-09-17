#define WIN32_LEAN_AND_MEAN

#include "search_server.h"
#include "parse.h"
#include "profile.h"
#include "test_runner.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <cctype>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>

#ifdef LOCAL_KONTROL
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;
#endif

using namespace std;

void TestFunctionality(
  const vector<string>& docs,
  const vector<string>& queries,
  const vector<string>& expected
) {
  istringstream docs_input(Join('\n', docs));
  istringstream queries_input(Join('\n', queries));

  SearchServer srv;
  srv.UpdateDocumentBase(docs_input);
  ostringstream queries_output;
  srv.AddQueriesStream(queries_input, queries_output);

  const string result = queries_output.str();
  const auto lines = SplitBy(Strip(result), '\n');
  ASSERT_EQUAL(lines.size(), expected.size());
  for (size_t i = 0; i < lines.size(); ++i) {
    ASSERT_EQUAL(lines[i], expected[i]);
  }
}

void TestSerpFormat() {
  const vector<string> docs = {
    "london is the capital of great britain",
    "i am travelling down the river"
  };
  const vector<string> queries = { "london", "the" };
  const vector<string> expected = {
    "london: {docid: 0, hitcount: 1}",
    Join(' ', vector{
    "the:",
    "{docid: 0, hitcount: 1}",
    "{docid: 1, hitcount: 1}"
    })
  };

  TestFunctionality(docs, queries, expected);
}

void TestTop5() {
  const vector<string> docs = {
    "milk a",
    "milk b",
    "milk c",
    "milk d",
    "milk e",
    "milk f",
    "milk g",
    "water a",
    "water b",
    "fire and earth"
  };

  const vector<string> queries = { "milk", "water", "rock" };
  const vector<string> expected = {
    Join(' ', vector{
    "milk:",
    "{docid: 0, hitcount: 1}",
    "{docid: 1, hitcount: 1}",
    "{docid: 2, hitcount: 1}",
    "{docid: 3, hitcount: 1}",
    "{docid: 4, hitcount: 1}"
    }),
    Join(' ', vector{
    "water:",
    "{docid: 7, hitcount: 1}",
    "{docid: 8, hitcount: 1}",
    }),
    "rock:",
  };
  TestFunctionality(docs, queries, expected);
}

void TestHitcount() {
  const vector<string> docs = {
    "the river goes through the entire city there is a house near it",
    "the wall",
    "walle",
    "is is is is",
  };
  const vector<string> queries = { "the", "wall", "all", "is", "the is" };
  const vector<string> expected = {
    Join(' ', vector{
    "the:",
    "{docid: 0, hitcount: 2}",
    "{docid: 1, hitcount: 1}",
    }),
    "wall: {docid: 1, hitcount: 1}",
    "all:",
    Join(' ', vector{
    "is:",
    "{docid: 3, hitcount: 4}",
    "{docid: 0, hitcount: 1}",
    }),
    Join(' ', vector{
    "the is:",
    "{docid: 3, hitcount: 4}",
    "{docid: 0, hitcount: 3}",
    "{docid: 1, hitcount: 1}",
    }),
  };
  TestFunctionality(docs, queries, expected);
}

void TestRanking() {
  const vector<string> docs = {
    "london is the capital of great britain",
    "paris is the capital of france",
    "berlin is the capital of germany",
    "rome is the capital of italy",
    "madrid is the capital of spain",
    "lisboa is the capital of portugal",
    "bern is the capital of switzerland",
    "moscow is the capital of russia",
    "kiev is the capital of ukraine",
    "minsk is the capital of belarus",
    "astana is the capital of kazakhstan",
    "beijing is the capital of china",
    "tokyo is the capital of japan",
    "bangkok is the capital of thailand",
    "welcome to moscow the capital of russia the third rome",
    "amsterdam is the capital of netherlands",
    "helsinki is the capital of finland",
    "oslo is the capital of norway",
    "stockgolm is the capital of sweden",
    "riga is the capital of latvia",
    "tallin is the capital of estonia",
    "warsaw is the capital of poland",
  };

  const vector<string> queries = { "moscow is the capital of russia" };
  const vector<string> expected = {
    Join(' ', vector{
    "moscow is the capital of russia:",
    "{docid: 7, hitcount: 6}",
    "{docid: 14, hitcount: 6}",
    "{docid: 0, hitcount: 4}",
    "{docid: 1, hitcount: 4}",
    "{docid: 2, hitcount: 4}",
    })
  };
  TestFunctionality(docs, queries, expected);
}

void TestBasicSearch() {
  const vector<string> docs = {
    "we are ready to go",
    "come on everybody shake you hands",
    "i love this game",
    "just like exception safety is not about writing try catch everywhere in your code move semantics are not about typing double ampersand everywhere in your code",
    "daddy daddy daddy dad dad dad",
    "tell me the meaning of being lonely",
    "just keep track of it",
    "how hard could it be",
    "it is going to be legen wait for it dary legendary",
    "we dont need no education"
  };

  const vector<string> queries = {
    "we need some help",
    "it",
    "i love this game",
    "tell me why",
    "dislike",
    "about"
  };

  const vector<string> expected = {
    Join(' ', vector{
    "we need some help:",
    "{docid: 9, hitcount: 2}",
    "{docid: 0, hitcount: 1}"
    }),
    Join(' ', vector{
    "it:",
    "{docid: 8, hitcount: 2}",
    "{docid: 6, hitcount: 1}",
    "{docid: 7, hitcount: 1}",
    }),
    "i love this game: {docid: 2, hitcount: 4}",
    "tell me why: {docid: 5, hitcount: 2}",
    "dislike:",
    "about: {docid: 3, hitcount: 2}",
  };
  TestFunctionality(docs, queries, expected);
}

#ifdef LOCAL_KONTROL
void TestStress() {
  fs::path appRoot = fs::path(__FILE__).parent_path();
  fs::path filename = appRoot.append("../../../../text_files/The Hobbit.txt");
  ifstream ifs(filename);

  vector<string> docs;
  vector<string> query_input;
  size_t max_documents = 50'000;
  size_t max_queries = 500'000;
  docs.reserve(max_documents);
  query_input.reserve(max_queries);
  size_t max_unique_words = 10'000;
  size_t min_word_length = 3;
  size_t max_word_length = 100;
  size_t max_words_per_doc = 20; // 1'000
  set<string> unique_words;

  size_t words_count = max_words_per_doc;
  for (string input; getline(ifs, input) && docs.size() < max_documents; ) {
    transform(begin(input), end(input), begin(input), [](unsigned char c) {
      return isalpha(c) ? tolower(c) : ' ';
      });
    for (auto& word : SplitIntoWords(input)) {
      if (word.size() < min_word_length || word.size() > max_word_length)
        continue;
      if (unique_words.size() > max_unique_words && unique_words.count(word) == 0)
        continue;

      unique_words.insert(word);

      if (words_count == max_words_per_doc) {
        docs.push_back(move(word));
        words_count = 1;
      }
      else {
        size_t index = docs.size() - 1;
        docs[index].append(" " + word);
        ++words_count;
      }
    }
  }

  ifs.close();

  /*const*/ vector<string> queries;
  const vector<string> queries_chunk = {
    "hobbit the my favourite dragon insure fire late chase middle",
    "dwarf coin flip tree fish train juice mountain sun raise",
    "dragon for my glory pain freedom spider elfish quite milk",
    "smaug tree domestic greedy leathery hard tough fluid business mordor",
    "there and back again mistic chase shop goblin eagle bear" };
  const vector<string> expected = {
    Join(' ', vector{
    "hobbit:",
    "{docid: 1989, hitcount: 3}",
    "{docid: 0, hitcount: 2}",
    "{docid: 51, hitcount: 2}",
    "{docid: 77, hitcount: 2}",
    "{docid: 1523, hitcount: 2}",
    }),
     Join(' ', vector{
    "dwarf:",
    "{docid: 2094, hitcount: 2}",
    "{docid: 3901, hitcount: 2}",
    "{docid: 23, hitcount: 1}",
    "{docid: 32, hitcount: 1}",
    "{docid: 36, hitcount: 1}",
    }),
     Join(' ', vector{
    "dragon:",
    "{docid: 252, hitcount: 2}",
    "{docid: 0, hitcount: 1}",
    "{docid: 2, hitcount: 1}",
    "{docid: 216, hitcount: 1}",
    "{docid: 220, hitcount: 1}",
    }),
    Join(' ', vector{
    "smaug:",
    "{docid: 2879, hitcount: 2}",
    "{docid: 2, hitcount: 1}",
    "{docid: 289, hitcount: 1}",
    "{docid: 291, hitcount: 1}",
    "{docid: 335, hitcount: 1}",
    }),
    Join(' ', vector{
    "there and back again:",
    "{docid: 3766, hitcount: 8}",
    "{docid: 177, hitcount: 7}",
    "{docid: 313, hitcount: 7}",
    "{docid: 3179, hitcount: 7}",
    "{docid: 172, hitcount: 6}",
    }),
  };

  for (int i = 0; i < 200; ++i) {
    copy(begin(queries_chunk), end(queries_chunk), back_inserter(queries));
  }

  {
    LOG_DURATION("Stress test: THE HOBBIT");

    istringstream docs_input(Join('\n', docs));
    istringstream queries_input(Join('\n', queries));

    SearchServer srv;
    {
      LOG_DURATION("UpdateDocumentBase");
      srv.UpdateDocumentBase(docs_input);
    }
    ostringstream queries_output;
    {
      LOG_DURATION("AddQueriesStream");
      srv.AddQueriesStream(queries_input, queries_output);
    }
  }
}
#endif

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSerpFormat);
  RUN_TEST(tr, TestTop5);
  RUN_TEST(tr, TestHitcount);
  RUN_TEST(tr, TestRanking);
  RUN_TEST(tr, TestBasicSearch);
#ifdef LOCAL_KONTROL
  RUN_TEST(tr, TestStress);
#endif
}
