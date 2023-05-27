#include "test_runner.h"
#include "profile.h"

#include <future>
#include <shared_mutex>
#include <unordered_map>
#include <numeric>
#include <vector>
#include <utility>
#include <algorithm>
#include <random>

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentMap {
public:
  using MapType = std::unordered_map<K, V, Hash>;

private:
  struct InternalData
  {
    MapType m_bucket;
    mutable std::shared_mutex m_Mutex;

    InternalData() {}
    InternalData(const InternalData& other)
      : m_bucket(other.m_bucket) {}

    InternalData& operator=(const InternalData& other)
    {
      m_bucket = other.m_bucket;
    }
  };

public:
  struct WriteAccess {
    std::shared_mutex& autoLock;
    V& ref_to_value;
        
    WriteAccess(V& value, std::shared_mutex& m) : ref_to_value(value), autoLock(m) {
      bool res = autoLock.try_lock();
      res;
    }
    ~WriteAccess() { autoLock.unlock(); }
  };

  struct ReadAccess {
    std::shared_lock<std::shared_mutex> autoLock;
    const V& ref_to_value;

    ReadAccess(const V& value, std::shared_mutex& m) : ref_to_value(value), autoLock(m) {}
  };

  explicit ConcurrentMap(size_t bucket_count)
    :m_bucketPull(bucket_count) {}

  WriteAccess operator[](const K& key)
  {
    InternalData& bucket = m_bucketPull[GetBucketNumber(key)];
    
    bucket.m_Mutex.lock();
    if (bucket.m_bucket.find(key) == bucket.m_bucket.end())
    {
      bucket.m_bucket[key];
    }

    return WriteAccess(bucket.m_bucket[key], bucket.m_Mutex);
  }

  ReadAccess At(const K& key) const
  {
    const InternalData& bucket = m_bucketPull[GetBucketNumber(key)];
    
    std::shared_lock lock(bucket.m_Mutex);
    if (bucket.m_bucket.find(key) == bucket.m_bucket.end())
    {
      throw std::out_of_range("");
    }

    return ReadAccess( bucket.m_bucket.at(key), bucket.m_Mutex );
  }

  bool Has(const K& key) const
  {
    const auto& bucket = m_bucketPull[GetBucketNumber(key)];
    std::shared_lock sharedLock(bucket.m_Mutex);

    return bucket.m_bucket.find(key) != bucket.m_bucket.end();
  }

  MapType BuildOrdinaryMap() const
  {
    MapType res;

    for (size_t i = 0; i < m_bucketPull.size(); ++i)
    {
      const InternalData& internalData = m_bucketPull[i];
      std::shared_lock sharedLock(internalData.m_Mutex);

      const MapType& currentBucket = internalData.m_bucket;
      res.insert(currentBucket.begin(), currentBucket.end());
    }

    return res;
  }

private:
  size_t GetBucketNumber(const K& key) const
  {
    return m_hasher(key) % m_bucketPull.size();
  }

  Hash m_hasher;
  std::vector<InternalData> m_bucketPull;
};

void RunConcurrentUpdates(
  ConcurrentMap<int, int>& cm, size_t thread_count, int key_count
) {
  auto kernel = [&cm, key_count](int seed) {
    std::vector<int> updates(key_count);
    iota(begin(updates), end(updates), -key_count / 2);
    shuffle(begin(updates), end(updates), std::default_random_engine(seed));

    for (int i = 0; i < 2; ++i) {
      for (auto key : updates) {
        cm[key].ref_to_value++;
      }
    }
  };

  std::vector<std::future<void>> futures;
  for (size_t i = 0; i < thread_count; ++i) {
    futures.push_back(std::async(kernel, static_cast<int>(i)));
  }
}

void TestConcurrentUpdate() {
  const size_t thread_count = 3;
  const size_t key_count = 50000;

  ConcurrentMap<int, int> cm(thread_count);
  RunConcurrentUpdates(cm, thread_count, key_count);

  const auto result = std::as_const(cm).BuildOrdinaryMap();
  ASSERT_EQUAL(result.size(), key_count);
  for (auto& [k, v] : result) {
    AssertEqual(v, 6, "Key = " + std::to_string(k));
  }
}

void TestReadAndWrite() {
  ConcurrentMap<size_t, std::string> cm(5);

  auto updater = [&cm] {
    for (size_t i = 0; i < 50000; ++i) {
      cm[i].ref_to_value += 'a';
    }
  };
  auto reader = [&cm] {
    std::vector<std::string> result(50000);
    for (size_t i = 0; i < result.size(); ++i) {
      result[i] = cm[i].ref_to_value;
    }
    return result;
  };

  auto u1 = std::async(updater);
  auto r1 = std::async(reader);
  auto u2 = std::async(updater);
  auto r2 = std::async(reader);

  u1.get();
  u2.get();

  for (auto f : { &r1, &r2 }) {
    auto result = f->get();
    ASSERT(std::all_of(result.begin(), result.end(), [](const std::string& s) {
      return s.empty() || s == "a" || s == "aa";
      }));
  }
}

void TestSpeedup() {
  {
    ConcurrentMap<int, int> single_lock(1);

    LOG_DURATION("Single lock");
    RunConcurrentUpdates(single_lock, 4, 50000);
  }
  {
    ConcurrentMap<int, int> many_locks(100);

    LOG_DURATION("100 locks");
    RunConcurrentUpdates(many_locks, 4, 50000);
  }
}

void TestConstAccess() {
  const std::unordered_map<int, std::string> expected = {
    {1, "one"},
    {2, "two"},
    {3, "three"},
    {31, "thirty one"},
    {127, "one hundred and twenty seven"},
    {1598, "fifteen hundred and ninety eight"}
  };

  const ConcurrentMap<int, std::string> cm = [&expected] {
    ConcurrentMap<int, std::string> result(3);
    for (const auto& [k, v] : expected) {
      result[k].ref_to_value = v;
    }
    return result;
  }();

  std::vector<std::future<std::string>> futures;
  for (int i = 0; i < 10; ++i) {
    futures.push_back(std::async([&cm, i] {
      try {
        return cm.At(i).ref_to_value;
      }
      catch (std::exception&) {
        return std::string();
      }
      }));
  }
  futures.clear();

  ASSERT_EQUAL(cm.BuildOrdinaryMap(), expected);
}

void TestStringKeys() {
  const std::unordered_map<std::string, std::string> expected = {
    {"one", "ONE"},
    {"two", "TWO"},
    {"three", "THREE"},
    {"thirty one", "THIRTY ONE"},
  };

  const ConcurrentMap<std::string, std::string> cm = [&expected] {
    ConcurrentMap<std::string, std::string> result(2);
    for (const auto& [k, v] : expected) {
      result[k].ref_to_value = v;
    }
    return result;
  }();

  ASSERT_EQUAL(cm.BuildOrdinaryMap(), expected);
}

struct Point {
  int x, y;
};

struct PointHash {
  size_t operator()(Point p) const {
    std::hash<int> h;
    return h(p.x) * 3571 + h(p.y);
  }
};

bool operator==(Point lhs, Point rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

void TestUserType() {
  ConcurrentMap<Point, size_t, PointHash> point_weight(5);

  std::vector<std::future<void>> futures;
  for (int i = 0; i < 1000; ++i) {
    futures.push_back(std::async([&point_weight, i] {
      point_weight[Point{ i, i }].ref_to_value = i;
      }));
  }

  futures.clear();

  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQUAL(point_weight.At(Point{ i, i }).ref_to_value, i);
  }

  const auto weights = point_weight.BuildOrdinaryMap();
  for (int i = 0; i < 1000; ++i) {
    ASSERT_EQUAL(weights.at(Point{ i, i }), i);
  }
}

void TestHas() {
  ConcurrentMap<int, int> cm(2);
  cm[1].ref_to_value = 100;
  cm[2].ref_to_value = 200;

  const auto& const_map = std::as_const(cm);
  ASSERT(const_map.Has(1));
  ASSERT(const_map.Has(2));
  ASSERT(!const_map.Has(3));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestConcurrentUpdate);
  RUN_TEST(tr, TestReadAndWrite);
  RUN_TEST(tr, TestSpeedup);
  RUN_TEST(tr, TestConstAccess);
  RUN_TEST(tr, TestStringKeys);
  RUN_TEST(tr, TestUserType);
  RUN_TEST(tr, TestHas);
}
