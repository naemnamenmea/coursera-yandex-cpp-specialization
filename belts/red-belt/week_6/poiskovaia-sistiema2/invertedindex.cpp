#include "invertedindex.h"

InvertedIndex::InvertedIndex(istream& stream) {
  // ������ ����� ������ � current_document
  for (string current_document; getline(stream, current_document); ) {
    docs.push_back(move(current_document));
    // �������� ������������� ����� �����: ��������� �� ������� ���������� � 0
    size_t docid = docs.size() - 1;
    /* ���� ������ � ���� � � ��� ���� ��� id - ������ �� ���� �����,
     * � ������� SplitIntoWords - ��� ������ ��������� �������� � ����(��� ��� �� ���
     * ������ ��� ��������) � ��������� vector<string_view>
     * (������ �� ��� - ������ ����������� �������)
     * */
    for (string_view word : SplitIntoWords(docs.back())) {
      /* ����� ���� ��� SplitIntoWords ������� ��� �� ��������� ������ ����
       * ��������� ���� �� ��������� ������� �����
       * */
       /*
        * �������� ������ �� ������ �������� ��� �����
        * */

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
  // ����� ������ ???
  static vector<InvertedIndex::Entry> result;
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  }
  else {
    return result;
  }
}
