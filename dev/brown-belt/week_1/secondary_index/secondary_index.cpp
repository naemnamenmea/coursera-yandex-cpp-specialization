#if not defined LOCAL_LAUNCH or defined SAHDSAFGSHDFASSD

#include "test_runner.h"

#include <iostream>
#include <map>
#include <list>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
public:
  typedef list<Record>::iterator StorageIt;

  struct DDT
  {
    multimap<int, StorageIt>::iterator it1;
    multimap<int, StorageIt>::iterator it2;
    multimap<string, StorageIt>::iterator it3;
  };

  typedef unordered_map<string, pair<StorageIt,DDT>> Index;
  typedef string Id;

  bool Put(const Record& record)
  {
    if (m_primaryIndex.find(record.id) != m_primaryIndex.end())
      return false;
    
    auto it = m_list.insert(m_list.end(), record);
    DDT fff;
    fff.it1 = SecondaryIndexTimestamp.emplace(it->timestamp, it);
    fff.it2 = SecondaryIndexKarma.emplace(it->karma, it);
    fff.it3 = SecondaryIndexUser.emplace(it->user, it);

    m_primaryIndex.emplace( it->id, make_pair( it, fff ) );

    return true;
  }

  const Record* GetById(const Id& id) const
  {
    auto it = m_primaryIndex.find(id);

    if (it == m_primaryIndex.end())
      return nullptr;

    return &*m_primaryIndex.at(id).first;
  }

  bool Erase(const string& id)
  {
    auto it = m_primaryIndex.find(id);

    if (it == m_primaryIndex.end())
      return false;

    auto& item = it->second;
    SecondaryIndexTimestamp.erase(item.second.it1);
    SecondaryIndexKarma.erase(item.second.it2);
    SecondaryIndexUser.erase(item.second.it3);
    auto it2 = item.first;
    m_primaryIndex.erase(it);
    m_list.erase(it2);

    return true;
  }

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const
  {
    ApplyFunctor(SecondaryIndexTimestamp, low, high, callback);
  }

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const
  {
    ApplyFunctor(SecondaryIndexKarma, low, high, callback);
  }

  template <typename Callback>
  void AllByUser(const string user, Callback callback) const
  {
    ApplyFunctor(SecondaryIndexUser, user, callback);
  }

protected:
  template<typename K, typename V, typename Callback>
  void ApplyFunctor(const multimap<K, V>& table, const K& value, Callback callback) const
  {
    auto [from, to] = table.equal_range(value);

    for (auto it = from; it != to; ++it)
    {
      const auto& item = *it->second;

      if (!callback(item))
        break;
    }
  }

  template<typename K, typename V, typename Callback>
  void ApplyFunctor(const multimap<K, V>& table, const K& low, const K& high, Callback callback) const
  {
    auto from = table.lower_bound(low);
    auto to = table.upper_bound(high);

    for (auto it = from; it != to; ++it)
    {
      const auto& item = *it->second;

      if (!callback(item))
        break;
    }
  }

private:
  multimap<int, StorageIt> SecondaryIndexTimestamp;
  multimap<int, StorageIt> SecondaryIndexKarma;
  multimap<string, StorageIt> SecondaryIndexUser;
  Index m_primaryIndex;
  list<Record> m_list;
};

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({ "id1", "Hello there", "master", 1536107260, good_karma });
  db.Put({ "id2", "O>>-<", "general2", 1536107260, bad_karma });

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
    });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({ "id1", "Don't sell", "master", 1536107260, 1000 });
  db.Put({ "id2", "Rethink life", "master", 1536107260, 2000 });

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
    });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({ "id", "Have a hand", "not-master", 1536107260, 10 });
  db.Erase("id");
  db.Put({ "id", final_body, "not-master", 1536107260, -10 });

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}

#endif
