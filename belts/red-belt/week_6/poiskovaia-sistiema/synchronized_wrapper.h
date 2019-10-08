#pragma once

#include <shared_mutex>
using namespace std;

template <typename T>
class Synchronized {
public:
  explicit Synchronized(T initial = T()) : value(move(initial)) {}

  struct Access {
    Access(T& ref, unique_lock<shared_mutex>&& lock)
      : ref_to_value{ ref }, lock{ move(lock) } {}

    T& ref_to_value;
  private:
    unique_lock<shared_mutex> lock;
  };

  struct ConstAccess {
    ConstAccess(const T& ref, shared_lock<shared_mutex>&& lock)
      : ref_to_value{ ref }, lock{ move(lock) } {}

    const T& ref_to_value;
  private:
    shared_lock<shared_mutex> lock;
  };

  Access GetAccess() {
    return { value, unique_lock(m) };
  }

  ConstAccess GetAccess() const {
    return { value, shared_lock(m) };
  }

private:
  T value;
  mutable shared_mutex m;
};

template <typename T>
class SynchronizedReference {
public:
  explicit SynchronizedReference(T& initial) : value(initial) {}

  struct Access {
    Access(T& ref, unique_lock<shared_mutex>&& lock)
      : ref_to_value{ ref }, lock{ move(lock) } {}

    T& ref_to_value;
  private:
    unique_lock<shared_mutex> lock;
  };

  struct ConstAccess {
    ConstAccess(const T& ref, shared_lock<shared_mutex>&& lock)
      : ref_to_value{ ref }, lock{ move(lock) } {}

    const T& ref_to_value;
  private:
    shared_lock<shared_mutex> lock;
  };

  Access GetAccess() {
    return { value, unique_lock(m) };
  }

  ConstAccess GetAccess() const {
    return { value, shared_lock(m) };
  }

private:
  T& value;
  mutable shared_mutex m;
};
