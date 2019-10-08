#pragma once

struct Constraints {
  static const size_t max_documents = 50'000; // 50'000
  static const size_t max_queries = 500'000; // 500'000
  static const size_t max_unique_words = 10'000; // 10'000
  static const size_t min_word_length = 3;
  static const size_t max_words_per_query = 10; // 10
  static const size_t max_word_length = 100; // 100
  static const size_t max_words_per_doc = 1'000; // 1'000
};
