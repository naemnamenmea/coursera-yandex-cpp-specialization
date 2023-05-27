#pragma once

#include "synchronized_wrapper.h"
#include "iterator_range.h"

#include <iostream>
#include <set>
#include <list>
#include <deque>
#include <vector>
#include <future>
#include <map>
#include <string>
using namespace std;

class InvertedIndex {
public:
  InvertedIndex() = default;
  explicit InvertedIndex(istream& is);

  struct Entry {
    size_t docid;
    size_t rating;
  };

  const vector<Entry>& Lookup(string_view word) const;

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

  size_t DocsCount() const {
    return docs.size();
  }

private:
  map<string_view, vector<Entry>> index;
  deque<string> docs;
};

class SearchServer {
public:
  SearchServer();
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

  template <class Iterator>
  void AddQueriesStreamSingleThread(IteratorRange<Iterator> query_input, SynchronizedReference<ostream&>& search_results_output);
  
private:
  size_t top_result_count = 5;
  Synchronized<InvertedIndex> syncIndex;
  future<InvertedIndex> newIndex;
  vector<future<void>> queries_results;
  bool init_launch;
};