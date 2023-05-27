#define WIN32_LEAN_AND_MEAN

#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "constraints.h"

#ifdef LOCAL_LAUNCH
#include "profile.h"
#include <experimental/filesystem>
#endif

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
using namespace std;

#ifdef LOCAL_LAUNCH
namespace fs = std::experimental::filesystem;
#endif

//void TestFunctionality(
//  const vector<string>& docs,
//  const vector<string>& queries,
//  const vector<string>& expected
//) {
//  SearchServer srv;
//  {
//    LOG_DURATION("UpdateDocumentBase 100000 times: ");
//    for (int i = 0; i < 10'000; ++i)
//    {
//      istringstream docs_input(Join('\n', docs));
//      srv.UpdateDocumentBase(docs_input);
//    }
//  }
//  ostringstream queries_output;
//  {
//    LOG_DURATION("AddQuiriesToStream 100000 times: ");
//    for (int i = 0; i < 10'000; ++i)
//    {
//      ostringstream local_output;
//      istringstream queries_input(Join('\n', queries));
//      srv.AddQueriesStream(queries_input, local_output);
//      queries_output = move(local_output);
//    }
//  }
//  const string result = queries_output.str();
//  const auto lines = SplitBy(Strip(result), '\n');
//  ASSERT_EQUAL(lines.size(), expected.size());
//  for (size_t i = 0; i < lines.size(); ++i) {
//    ASSERT_EQUAL(lines[i], expected[i]);
//  }
//}

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

#ifdef LOCAL_LAUNCH
void TestStress() {
  fs::path appRoot = fs::path(__FILE__).parent_path();
  //fs::path filename = appRoot.append("../../../../text_files/The Hobbit.txt");
  fs::path filename = appRoot.append("../../../../text_files/The Lord of the Rings.txt");
  ifstream ifs(filename);

  vector<string> docs(Constraints::max_documents);
  set<string> unique_words_set;

  for (string line; unique_words_set.size() < Constraints::max_unique_words && getline(ifs, line);) {
    transform(begin(line), end(line), begin(line), [](unsigned char c) {
      return isalpha(c) ? tolower(c) : ' ';
      });
    for (const auto& word_view : SplitBy(line, ' ')) {
      if (unique_words_set.size() >= Constraints::max_unique_words)
        break;
      if (word_view.size() < Constraints::min_word_length || word_view.size() > Constraints::max_word_length)
        continue;
      unique_words_set.insert(string(word_view));
    }
  }

  ifs.close();
  ASSERT_EQUAL(unique_words_set.size(), Constraints::max_unique_words);

  vector<string> unique_words_vector(
    make_move_iterator(begin(unique_words_set)),
    make_move_iterator(end(unique_words_set))
  );

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<size_t> dis(0, Constraints::max_unique_words - 1);
  for (size_t i = 0; i < Constraints::max_documents; ++i) {

    docs[i].reserve(Constraints::max_words_per_doc * 
      (Constraints::min_word_length + Constraints::max_word_length) / 2);

    for (size_t j = 0; j < Constraints::max_words_per_doc; ++j) {
      size_t index = dis(gen);
      (docs[i] += ' ') += unique_words_vector[index];
    }
  }

  vector<string> queries(Constraints::max_queries);
  for (size_t i = 0; i < Constraints::max_queries; ++i) {
    for (size_t j = 0; j < Constraints::max_words_per_query; ++j) {
      size_t index = dis(gen);
      (queries[i] += ' ') += unique_words_vector[index];
    }
  }

  const vector<string> expected;
  {
    istringstream docs_input(Join('\n', docs));
    istringstream queries_input(Join('\n', queries));

    LOG_DURATION("Stress test ( The Hobbit )");

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
  RUN_TEST(tr, TestSerpFormat); cout << endl;
  RUN_TEST(tr, TestTop5); cout << endl;
  RUN_TEST(tr, TestHitcount); cout << endl;
  RUN_TEST(tr, TestRanking); cout << endl;
  RUN_TEST(tr, TestBasicSearch); cout << endl;
#ifdef LOCAL_LAUNCH
  //RUN_TEST(tr, TestStress); cout << endl;
#endif
}
