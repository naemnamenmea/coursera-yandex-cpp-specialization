#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <numeric>
#include <vector>

#include "search_server.h"
#include "iterator_range.h"

using namespace std;

void SearchServer::UpdateDocumentBase(istream& document_input) {
  index = InvertedIndex{ document_input };
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
  // ������ �� ��� ����������(������)
  const auto& documents = index.GetDocument();
  // ����� ������ - Id �����, � �������� - �������
  vector<size_t> docid_count(documents.size());
  vector <int64_t> docids(documents.size());

  for (string current_query; getline(query_input, current_query); ) {
    // SplitIntoWords ��������� vector<string_view>
    vector<string_view> words = SplitIntoWords(current_query);
    docid_count.assign(docid_count.size(), 0);

    for (const auto& word : words) {
      // ������� �++17
      for (const auto& [docid, rating] : index.Lookup(word)) {
        // �� ���� �������� ������� � ������ � docid_count
        docid_count[docid] += rating;
      }
    }
    // ��������� �� 0 �� documents.size()
    iota(docids.begin(), docids.end(), 0);
    // ��������� id ���������� ������ ��� ���� �������� � docid_count
    partial_sort(
      begin(docids),
      Head(docids, 5).end(),
      end(docids),
      [&docid_count](int64_t lhs, int64_t rhs) {
        return make_pair(docid_count[lhs], -lhs) > make_pair(docid_count[rhs], -rhs);
      }
    );
    // ������� ���������
    search_results_output << current_query << ':';
    for (size_t docid : Head(docids, 5)) {
      // �������
      const size_t hit_count = docid_count[docid];
      if (hit_count == 0) {
        break;
      }
      search_results_output << " {"
        << "docid: " << docid << ", "
        << "hitcount: " << hit_count << '}';
    }
    search_results_output << '\n';
  }
}

