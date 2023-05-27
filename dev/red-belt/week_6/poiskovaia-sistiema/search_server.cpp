#include "search_server.h"
#include "iterator_range.h"
#include "paginator.h"
#include "parse.h"
#include "synchronized_wrapper.h"
#include "constraints.h"

#include <algorithm>
#include <future>
#include <iterator>
#include <algorithm>
#include <string_view>
#include <deque>
#include <string>
#include <sstream>
#include <iostream>

SearchServer::SearchServer() : init_launch(true) {}

SearchServer::SearchServer(istream& document_input) 
  : init_launch(false)
{
  syncIndex.GetAccess().ref_to_value = InvertedIndex{ document_input };
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  if (init_launch) {
    syncIndex.GetAccess().ref_to_value = InvertedIndex{ document_input };
  }
  else {
    //newIndex = async(launch::async, [&] {
    //  return InvertedIndex{ document_input };
    //  });
  }
}

template<class Iterator>
void SearchServer::AddQueriesStreamSingleThread(
  IteratorRange<Iterator> query_input, SynchronizedReference<ostream&>& search_results_output
) {
  const auto& constSyncIndex = syncIndex;

  //TotalDuration _FormResults("Total FormResults");
  //TotalDuration _Sort("Total Sort");
  //TotalDuration _IndexAccessing("Total IndexAccessing");
  //TotalDuration _SplitIntoWords("Total SplitIntoWords");

  vector<pair<size_t, size_t>> docid_count;
  docid_count.reserve(Constraints::max_documents);

  for (auto& current_query : query_input) {
    if (newIndex.valid()) {
      auto lock = syncIndex.GetAccess();
      if (newIndex.valid()) {
        lock.ref_to_value = newIndex.get();
      }
    }

    auto lock = constSyncIndex.GetAccess();
    auto& __syncIndex = lock.ref_to_value;

    if (__syncIndex.DocsCount() != docid_count.size()) {
      docid_count.resize(__syncIndex.DocsCount());
    }

    for (size_t i = 0; i < docid_count.size(); ++i) {
      docid_count[i].first = i;
      docid_count[i].second = 0;
    }

    vector<string_view> words;
    { // * Разбиение на слова
      //ADD_DURATION(_SplitIntoWords);
      words = SplitBy(current_query, ' ');
    }

    { // * Обращение к индексу
      //ADD_DURATION(_IndexAccessing);
      for (const auto& word : words) {
        for (const auto [docid, rating] : __syncIndex.Lookup(word)) {
          docid_count[docid].second += rating;
        }
      }
    }

    { // * Сортировка документов
      //ADD_DURATION(_Sort);
      partial_sort(
        begin(docid_count),
        Head(docid_count, top_result_count).end(),
        end(docid_count),
        [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
          int64_t lhs_docid = lhs.first;
          auto lhs_hit_count = lhs.second;
          int64_t rhs_docid = rhs.first;
          auto rhs_hit_count = rhs.second;
          return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
        }
      );
    }

    { // * Формирование результата
      //ADD_DURATION(_FormResults);
      auto lock = search_results_output.GetAccess();
      auto& syncOut = lock.ref_to_value;

      syncOut << current_query << ':';
      for (auto [docid, hitcount] : Head(docid_count, top_result_count)) {
        if (hitcount == 0) {
          break;
        }

        syncOut << " {"
          << "docid: " << docid << ", "
          << "hitcount: " << hitcount << '}';
      }
      syncOut << endl;
    }
  }
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output)
{
  queries_results.push_back(async(launch::async, [&, it = this]() {
    deque<string> queries;
    for (string current_query; getline(query_input, current_query); ) {
      queries.push_back(move(current_query));
    }

    SynchronizedReference<ostream&> syncOStream(search_results_output);
    it->AddQueriesStreamSingleThread(IteratorRange{begin(queries), end(queries)}, syncOStream);
    //size_t chunks = 1;
    //size_t page_size = max(queries.size() / chunks, static_cast<size_t>(10'000));

    //vector<future<void>> futures;
    //for (auto page : Paginate(queries, page_size)) {
    //  futures.push_back(async(launch::async, [it, page, &syncOStream]() {
    //    it->AddQueriesStreamSingleThread(page, syncOStream);
    //    }));
    //}

    //for (auto& _future : futures) {
    //  _future.get();
    //}
    }));
}

InvertedIndex::InvertedIndex(istream& stream) {

  for (string current_document; getline(stream, current_document); ) {
    const size_t docid = docs.size();
    docs.push_back(move(current_document));

    for (const string_view word : SplitBy(docs.back(), ' ')) {
      auto& docids = index[word];

      if (!docids.empty() && docids.back().docid == docid) {
        docids.back().rating++;
      }
      else {
        docids.push_back({ docid, 1 });
      }
    }
  }
}

const vector<InvertedIndex::Entry>& InvertedIndex::Lookup(string_view word) const {
  static vector<InvertedIndex::Entry> empty_list;
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  }
  else {
    return empty_list;
  }
}
