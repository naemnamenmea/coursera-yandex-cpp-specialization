#include "Common.h"
#include <unordered_map>
#include <shared_mutex>
#include <list>

using namespace std;

class LruCache : public ICache {
public:
  LruCache(
      shared_ptr<IBooksUnpacker> books_unpacker,
      const Settings& settings
  ) : m_booksUnpacker(move(books_unpacker)), m_settings(settings), m_occupiedMemory(0){
    // реализуйте метод
  }

  BookPtr GetBook(const string& book_name) override {
    lock_guard lock(m_mutex);
    auto it = m_data.find(book_name);

    const bool isBookExist = it != m_data.end();
    BookPtr res = isBookExist ? it->second.book : m_booksUnpacker->UnpackBook(book_name);
    
    if (isBookExist)
    {
      UpdateFreqData(it->second);
    }
    else
    {
      size_t contentSize = GetBookSizeInBytes(res);

      while (m_occupiedMemory + contentSize > m_settings.max_memory && !m_freq.empty())
      {
        auto it = m_freq.begin();

        auto it2 = m_data.find(*it);
        m_occupiedMemory -= GetBookSizeInBytes(it2->second.book);
        m_data.erase(it2);
        m_freq.erase(it);
      }

      if (contentSize <= m_settings.max_memory)
      {
        AddBookRecord(res);
        m_occupiedMemory += contentSize;
      }
    }

    return res;
  }

private:
  struct BookWrapper
  {
    list<const char*>::const_iterator freqIt;
    shared_ptr<const IBook> book;
  };

  size_t GetBookSizeInBytes(const BookPtr& book)
  {
    return book->GetContent().empty() ? 0 : sizeof(book->GetContent()[0]) * book->GetContent().size();
  }

  void AddBookRecord(BookPtr& book)
  {
    BookWrapper bookWrapper;
    bookWrapper.book = book;
    auto [it, inserted] = m_data.emplace(book->GetName(), bookWrapper);
    it->second.freqIt = m_freq.insert(m_freq.end(), it->first.c_str());
  }

  void UpdateFreqData(BookWrapper& bookWrapper)
  {
    m_freq.erase(bookWrapper.freqIt);
    bookWrapper.freqIt = m_freq.insert(m_freq.end(), bookWrapper.book->GetName().c_str());
  }

  Settings m_settings;
  size_t m_occupiedMemory;
  shared_ptr<IBooksUnpacker> m_booksUnpacker;
  unordered_map<string, BookWrapper> m_data;
  list<const char*> m_freq;
  mutex m_mutex;
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings& settings
) {
  return make_unique<LruCache>(move(books_unpacker), settings);
}
