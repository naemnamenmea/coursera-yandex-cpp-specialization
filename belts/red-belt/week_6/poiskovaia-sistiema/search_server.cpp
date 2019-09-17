#include "search_server.h"
#include "iterator_range.h"
#include "auxiliary.h"

#include <algorithm>
#include <iterator>
#include <string_view>
#include <sstream>
#include <iostream>

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    new_index.Add(move(current_document));
  }

  index = move(new_index);
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
  TotalDuration _FormResults("Total FormResults");
  TotalDuration _Sort("Total Sort");
  TotalDuration _VectorResaultsForm("Total Vector Resaults Form");
  TotalDuration _IndexAccessing("Total IndexAccessing");
  TotalDuration _SplitIntoWords("Total SplitIntoWords");

  for (string current_query; getline(query_input, current_query); ) {
    /*const*/ vector<string_view> words;
    { // * Разбиение на слова
      ADD_DURATION(_SplitIntoWords);
      words = SplitIntoWords(current_query);
    }

    map<size_t, size_t> docid_count;
    { // * Обращение к индексу
      ADD_DURATION(_IndexAccessing);
      for (const auto& word : words) {
        for (const size_t docid : index.Lookup(word)) {
          docid_count[docid]++;
        }
      }
    }

    vector<pair<size_t, size_t>> search_results;
    { // * Формирование вектора результатов
      ADD_DURATION(_VectorResaultsForm);
      search_results = vector<pair<size_t, size_t>>(docid_count.begin(), docid_count.end());
    }

    { // * Сортировка документов
      ADD_DURATION(_Sort);
      partial_sort(
        begin(search_results),
        Head(search_results, top_result_count).end(),
        end(search_results),
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
      ADD_DURATION(_FormResults);
      search_results_output << current_query << ':';
      for (auto [docid, hitcount] : Head(search_results, 5)) {
        search_results_output << " {"
          << "docid: " << docid << ", "
          << "hitcount: " << hitcount << '}';
      }
      search_results_output << endl;
    }
  }
}

void InvertedIndex::Add(const string& document) {
  docs.push_back(document);

  const size_t docid = docs.size() - 1;
  for (const auto& word : SplitIntoWords(document)) {
    index[word].push_back(docid);
  }
}

list<size_t> InvertedIndex::Lookup(string_view word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return {};
  }
}
