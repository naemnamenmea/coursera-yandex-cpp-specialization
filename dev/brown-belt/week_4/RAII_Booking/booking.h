#pragma once
#include <memory>

namespace RAII
{

  template <class Provider>
  class Booking
  {
  public:
    class ProviderWrapper
    {
    private:
      Provider* p;
    public:
      ProviderWrapper(Provider* pp) : p(pp) {}
      ~ProviderWrapper() { if (p != nullptr) p->counter--; }
    };

    Booking(Provider* prov, int)
      : ptr(new ProviderWrapper(prov))
    {
    }

    Booking() = delete;

    Booking(const Booking& other) = delete;
    Booking(Booking&& other) noexcept
    {
      ptr.reset(other.ptr.release());
    }

    Booking& operator=(const Booking& other) = delete;
    Booking& operator=(Booking&& other) noexcept
    {
      ptr.reset(other.ptr.release());
      return *this;
    }

    ~Booking()
    {
    }

  private:
    std::unique_ptr<ProviderWrapper> ptr;
  };
}