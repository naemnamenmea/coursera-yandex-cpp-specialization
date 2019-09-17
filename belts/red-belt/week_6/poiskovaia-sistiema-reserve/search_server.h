#pragma once

#include <istream>
#include <sstream>
#include <ostream>
#include <set>
#include <deque>
#include <iterator>
#include <list>
#include <vector>
#include <map>
#include <string>
using namespace std;

vector<string> SplitIntoWords(const string& line);

class InvertedIndex {
public:
  void Add(string document);
  list<size_t> Lookup(const string& word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  map<string, list<size_t>> index;
  deque<string> docs;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex index;
  static constexpr int top_result = 5;
  static constexpr size_t max_documents = 50'000;
};
