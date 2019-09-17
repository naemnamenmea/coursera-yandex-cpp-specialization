#include "search_server.h"
#include "profile.h"
#include "paginator.h"

#include <algorithm>
#include <iostream>

vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return { istream_iterator<string>(words_input), istream_iterator<string>() };
}

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
  TotalDuration _SplitIntoWords("Total SplitIntoWords");
  TotalDuration _Sort("Total Sort");
  TotalDuration _IndexAccessing("Total IndexAccessing");
  TotalDuration _FormResults("Total FormResults");
  TotalDuration _Other("Total Other");
  TotalDuration _MoveVector("Total MoveVector");

  vector<pair<size_t, size_t>> docid_count(SearchServer::max_documents);

  for (string current_query; getline(query_input, current_query); ) {

    { // * Другое
      ADD_DURATION(_Other);
      size_t _cnt = 0;
      for (auto& [pos, count] : docid_count) {
        pos = _cnt++;
        count = 0;
      }
    }

    /*const*/ vector<string> words;
    { // * Разбиение на слова
      ADD_DURATION(_SplitIntoWords);
      words = SplitIntoWords(current_query);
    }

    { // * Обращение к индексу
      ADD_DURATION(_IndexAccessing);
      for (const auto& word : words) {
        for (const size_t docid : index.Lookup(word)) {
          docid_count[docid].second++;
        }
      }
    }

    { // * Сортировка документов
      ADD_DURATION(_Sort);
      partial_sort(
        begin(docid_count),
        Head(docid_count, top_result).end(),
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
      ADD_DURATION(_FormResults);
      search_results_output << current_query << ':';
      for (auto [docid, hitcount] : Head(docid_count, top_result)) {
        if (hitcount == 0) break;
        search_results_output << " {"
          << "docid: " << docid << ", "
          << "hitcount: " << hitcount << '}';
      }
      search_results_output << endl;
    }
  }
}

void InvertedIndex::Add(string document) {
  const size_t docid = docs.size();
  for (auto& word : SplitIntoWords(document)) {
    index[move(word)].push_back(docid);
  }

  docs.push_back(move(document));
}

list<size_t> InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  }
  else {
    return {};
  }
}
