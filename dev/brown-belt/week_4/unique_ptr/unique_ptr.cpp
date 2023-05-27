#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;


template<class T>
class UniquePtr {
public:
  UniquePtr() :m_ptr(nullptr) {}
  UniquePtr(T* p) : m_ptr(p) {}
  UniquePtr(const UniquePtr<T>&) = delete;
  UniquePtr(UniquePtr&& other) noexcept
    : m_ptr(other.m_ptr)
  {
    other.m_ptr = nullptr;
  };
  UniquePtr& operator=(const UniquePtr<T>&) = delete;
  UniquePtr& operator = (nullptr_t)
  {
    Reset(nullptr);
    return *this;
  }
  UniquePtr& operator = (UniquePtr&& other) noexcept
  {
    if (this != &other)
    {
      Reset(other.m_ptr);
      other.m_ptr = nullptr;
    }
    return *this;
  }
  ~UniquePtr() {
    if (m_ptr)
      delete m_ptr;
  }

  T& operator * () const
  {
    return *m_ptr;
  }

  T* operator -> () const
  {
    return m_ptr;
  }

  T* Release()
  {
    T* res = m_ptr;
    m_ptr = nullptr;
    return res;
  }

  void Reset(T* ptr)
  {
    delete m_ptr;
    m_ptr = ptr;
  }

  void Swap(UniquePtr& other)
  {
    swap(m_ptr, other.m_ptr);
  }

  T* Get() const
  {
    return m_ptr;
  }

private:
  T* m_ptr;
};

template class UniquePtr<int>;

struct Item {
  static int counter;
  int value;
  Item(int v = 0) : value(v) {
    ++counter;
  }
  Item(const Item& other) : value(other.value) {
    ++counter;
  }
  ~Item() {
    --counter;
  }
};

int Item::counter = 0;


void TestLifetime() {
  Item::counter = 0;
  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    ptr.Reset(new Item);
    ASSERT_EQUAL(Item::counter, 1);
  }
  ASSERT_EQUAL(Item::counter, 0);

  {
    UniquePtr<Item> ptr(new Item);
    ASSERT_EQUAL(Item::counter, 1);

    auto rawPtr = ptr.Release();
    ASSERT_EQUAL(Item::counter, 1);

    delete rawPtr;
    ASSERT_EQUAL(Item::counter, 0);
  }
  ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters() {
  UniquePtr<Item> ptr(new Item(42));
  ASSERT_EQUAL(ptr.Get()->value, 42);
  ASSERT_EQUAL((*ptr).value, 42);
  ASSERT_EQUAL(ptr->value, 42);
}

void TestSwap()
{
  int* a = new int(7);
  int* b = new int(8);
  UniquePtr<int> pa(a);
  UniquePtr<int> pb(b);

  pa.Swap(pb);
  ASSERT_EQUAL(*pa, 8);
  ASSERT_EQUAL(*pb, 7);
}

void TestReset()
{
  int* a = new int(8);
  UniquePtr<int> ptr(a);
  int* b = ptr.Release();

  ASSERT(!ptr.Get());
  ASSERT_EQUAL(*b, 8);

  ptr = UniquePtr<int>(b);
  ASSERT_EQUAL(*ptr, 8);
}

void TestSelfAssignment()
{
  UniquePtr<int> a(new int(7));
  UniquePtr<int> b(move(a));

  ASSERT(!a.Get());
  ASSERT_EQUAL(*b, 7);
}

void TestNullptrAssignment()
{
  UniquePtr<int> p(new int(3));
  ASSERT_EQUAL(*p, 3);
  p = nullptr;
  ASSERT_EQUAL(p.Get(), static_cast<int*>(nullptr));
}

struct Dummy
{
  Dummy() { cout << "Dummy()" << endl; }
  ~Dummy() { cout << "~Dummy()" << endl; }
};

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestLifetime);
  RUN_TEST(tr, TestGetters);
  RUN_TEST(tr, TestSwap);
  RUN_TEST(tr, TestReset);
  RUN_TEST(tr, TestSelfAssignment);
  RUN_TEST(tr, TestNullptrAssignment);

  return 0;
}
