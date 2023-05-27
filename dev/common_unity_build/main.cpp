#include <string>
#include <sstream>
#include <tuple>
#include <ctime>

struct Day
{
  explicit Day(const int value) : value_(value) {}
  operator int() const { return value_; }

  int value_;
};

struct Month
{
  explicit Month(const int value) : value_(value) {}
  operator int() const { return value_; }

  int value_;
};

struct Year
{
  explicit Year(const int value) : value_(value) {}
  operator int() const { return value_; }

  int value_;
};

class Date
{
public:
  explicit Date(Day day, Month month, Year year)
    :day_(day), month_(month), year_(year) {}

  static Date CreateDate(const tm* const t)
  {
    return Date(Day(t->tm_mday), Month(t->tm_mon + 1), Year(t->tm_year + 1900));
  }

  time_t AsTimestamp() const
  {
    tm t = {};
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = day_;
    t.tm_mon = month_ - 1;
    t.tm_year = year_ - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

  std::string ToString(const char d = '/') const
  {
    std::ostringstream oss;
    oss << day_ << d << month_ << d << year_;
    return oss.str();
  }

  bool operator<(const Date& other) const
  {
    return std::tie(year_, month_, day_)
      < std::tie(other.year_, other.month_, other.day_);
  }

  bool operator<=(const Date& other) const
  {
    return std::tie(year_, month_, day_)
      <= std::tie(other.year_, other.month_, other.day_);
  }

  bool operator==(const Date& other) const
  {
    return std::tie(this->year_, this->month_, this->day_)
      == std::tie(other.year_, other.month_, other.day_);
  }

private:
  Day day_;
  Month month_;
  Year year_;
};

class DateRange
{
public:
  DateRange(Date fromInc, Date toExc) : m_fromInc(std::move(fromInc)), m_toExc(std::move(toExc)) {
    Validate();
  }

  const Date& GetBegin() const 
  {
    return m_fromInc;
  }

  const Date& GetEnd() const 
  {
    return m_toExc;
  }

  void SetBegin(Date fromInc)
  {
    m_fromInc = std::move(fromInc);
    Validate();
  }

  void SetEnd(Date toExc)
  {
    m_toExc = std::move(toExc);
    Validate();
  }
private:
  void Validate() const
  {
    if (m_toExc < m_fromInc)
      throw std::range_error("inverse range provided");
  }

  Date m_fromInc;
  Date m_toExc;
};

std::ostream& operator<<(std::ostream& os, const Date& date);

class DateHelper
{
public:
  static Date ParseDate(std::istream& is);

  static int ComputeDiffInDays(const Date& date_from, const Date& date_to);

  static Date AddDaysToDate(const Date& date, const int daysToAdd);

private:
  static const time_t SECONDS_IN_DAY = 60 * 60 * 24;
};

#include <cmath>
#include <limits>
#ifdef _MSC_VER
#include <immintrin.h>
#endif

inline unsigned int MostSignificantBitIdx(unsigned long long x)
{
  if (x == 0) return static_cast<unsigned int>(-1);
#ifdef _MSC_VER
  return static_cast<unsigned int>(8 * sizeof(unsigned long long) - _lzcnt_u64(x) - 1);
#else
  return static_cast<unsigned int>(8 * sizeof(unsigned long long) - __builtin_clzll(x) - 1);
#endif
}

namespace mathdef
{
  const double MATH_TOL = 1e-12;
  const float MATH_TOL_FLOAT = 1e-6f;
  const double MATH_TOL_LD = 1e-16;

  constexpr double MAX_DOUBLE = std::numeric_limits<double>::max();

  inline bool is_within(
    const size_t value,
    const size_t from,
    const size_t to,
    const bool isIncludeBegin = true,
    const bool isIncludeEnd = true
  )
  {
    if (!(isIncludeBegin ? from <= value : from < value))
      return false;
    if (!(isIncludeEnd ? value <= to : value < to))
      return false;
    return true;
  }

  inline double math_tol(const double&)
  {
    return MATH_TOL;
  }

  inline bool is_eq(const double& a, const double& b)
  {
    return std::abs(a - b) < MATH_TOL;
  }

  inline const bool is_neq(const double& val1, const double& val2)
  {
    return !is_eq(val1, val2);
  }

  inline bool is_eq(const long double& a, const long double& b)
  {
    return std::abs(a - b) < MATH_TOL_LD;
  }

  inline bool is_eq(const double& a, const double& b, const double& tol)
  {
    return std::abs(a - b) < tol;
  }

  inline const bool is_lt(const double& toCompare, const double& source)
  {
    return toCompare < (source - MATH_TOL);
  }

  inline const bool is_gt(const double& toCompare, const double& source)
  {
    return toCompare > (source + MATH_TOL);
  }

  inline const bool is_lte(const double& toCompare, const double& source)
  {
    return toCompare <= (source + MATH_TOL);
  }

  inline const bool is_gte(const double& toCompare, const double& source)
  {
    return toCompare >= (source - MATH_TOL);
  }

  inline const bool is_lte(const long double& toCompare, const long double& source)
  {
    return toCompare <= (source + MATH_TOL);
  }

  inline const bool is_zero(const long double& value)
  {
    return std::abs(value) <= MATH_TOL_LD;
  }

  inline const bool is_not_zero(const long double& value)
  {
    return !is_zero(value);
  }
}  // namespace mathdef

#include <vector>
#include <stdexcept>
#include <tuple>
#include <functional>

namespace SegmentTree
{
  template<typename TNode>
  class SegmentTree
  {
  public:
    typedef typename TNode::T T;
    typedef typename TNode::CalcData CalcData;
    typedef typename TNode::ModifyOp ModifyOp;

    SegmentTree() : m_size((size_t)-1), m_realSize((size_t)-1) {}

    void Build(size_t n, const T& initValue)
    {
      Init(n);
      Build(n, initValue, 0, 0, m_size);
    }

    void Build(const std::vector<T>& arr)
    {
      Init(arr.size());
      Build(arr, 0, 0, m_size);
    }

    void Modify(size_t i, const ModifyOp& modifyOp)
    {
      Modify(i, i + 1, modifyOp);
    }

    void Modify(size_t l, size_t r, const ModifyOp& modifyOp)
    {
      ValidateBounds(l, r);

      InternalNode updateNode;
      updateNode.m_modifyOp = modifyOp;
      updateNode.m_isModifyOpPresent = true;
      Modify(l, r, updateNode, 0, 0, m_size);
    }

    CalcData Get(size_t i)
    {
      return Calculate(i, i + 1);
    }

    CalcData Calculate(size_t l, size_t r)
    {
      ValidateBounds(l, r);

      return Calculate(l, r, 0, 0, m_size);
    }

  private:
    struct InternalNode
    {
      InternalNode() : m_isModifyOpPresent(false), m_isCalcOnSegmentDataPresent(false) {}

      CalcData m_calcOnSegmentData;
      ModifyOp m_modifyOp;
      bool m_isModifyOpPresent;
      bool m_isCalcOnSegmentDataPresent;
    };

    virtual void UpdateModifyOp(ModifyOp& lhs, const ModifyOp& rhs) = 0;

    virtual void UpdateCalcData(CalcData& nodeCalcData, const ModifyOp& modifyOp, const size_t, const size_t) = 0;

    virtual CalcData CombineCalcData(const CalcData& leftChild, const CalcData& rightChild) = 0;

    void Init(size_t n)
    {
      m_realSize = n;
      m_size = 1;
      while (m_size < n) m_size *= 2;

      m_tree.assign(2 * m_size - 1, InternalNode{});
    }

    //bool IsDummyNode(size_t nodeNum) const
    //{
    //  size_t treeMaxLvl = MostSignificantBitIdx(m_size);
    //  size_t nodeLvl = MostSignificantBitIdx(nodeNum + 1);
    //  size_t minForbiddenIdxOnLastLayer = (1 << treeMaxLvl) - 1 + m_realSize;

    //  return nodeNum >= minForbiddenIdxOnLastLayer / (size_t(1) << (treeMaxLvl - nodeLvl));
    //}

    void ValidateBounds(size_t l, size_t r)
    {
      if (!mathdef::is_within(l, 0, r, true, false) || !mathdef::is_within(r, l, m_realSize, false, true))
      {
        const std::string err_msg = "l=" + std::to_string(l) + " or r=" + std::to_string(r) + " is outside of [0;" + std::to_string(m_realSize) + "] or l>=r";
        throw std::domain_error(err_msg.c_str());
      }
    }

    static InternalNode BuildOneElement(const T& value)
    {
      InternalNode res;
      res.m_calcOnSegmentData = CalcData(value);
      res.m_isCalcOnSegmentDataPresent = true;
      return res;
    }

    void Propagate(size_t x, size_t lx, size_t rx)
    {
      if (!m_tree[x].m_isModifyOpPresent || rx - lx == 1) return;

      size_t m = (lx + rx) / 2;

      UpdateModifyOp(m_tree[2 * x + 1], m_tree[x]);
      UpdateCalcData(m_tree[2 * x + 1], m_tree[x], lx, m);
      UpdateModifyOp(m_tree[2 * x + 2], m_tree[x]);
      UpdateCalcData(m_tree[2 * x + 2], m_tree[x], m, rx);
      m_tree[x].m_isModifyOpPresent = false;
    }

    void Build(size_t n, const T& initValue, size_t x, size_t lx, size_t rx)
    {
      if (rx - lx == 1)
      {
        if (lx < n)
          m_tree[x] = BuildOneElement(initValue);
        return;
      }

      size_t m = (lx + rx) / 2;
      Build(n, initValue, 2 * x + 1, lx, m);
      Build(n, initValue, 2 * x + 2, m, rx);
      std::tie(m_tree[x].m_isCalcOnSegmentDataPresent, m_tree[x].m_calcOnSegmentData) =
        _CombineCalcData(
          m_tree[2 * x + 1].m_isCalcOnSegmentDataPresent, m_tree[2 * x + 1].m_calcOnSegmentData,
          m_tree[2 * x + 2].m_isCalcOnSegmentDataPresent, m_tree[2 * x + 2].m_calcOnSegmentData);
    }

    void Build(const std::vector<T>& arr, size_t x, size_t lx, size_t rx)
    {
      if (rx - lx == 1)
      {
        if (lx < arr.size())
          m_tree[x] = BuildOneElement(arr[lx]);
        return;
      }

      size_t m = (lx + rx) / 2;
      Build(arr, 2 * x + 1, lx, m);
      Build(arr, 2 * x + 2, m, rx);
      std::tie(m_tree[x].m_isCalcOnSegmentDataPresent, m_tree[x].m_calcOnSegmentData) =
        _CombineCalcData(
          m_tree[2 * x + 1].m_isCalcOnSegmentDataPresent, m_tree[2 * x + 1].m_calcOnSegmentData,
          m_tree[2 * x + 2].m_isCalcOnSegmentDataPresent, m_tree[2 * x + 2].m_calcOnSegmentData);
    }

    void UpdateCalcData(InternalNode& lhs, const InternalNode& rhs, size_t lx, size_t rx)
    {
      if (!rhs.m_isModifyOpPresent || !lhs.m_isCalcOnSegmentDataPresent)
        return;

      UpdateCalcData(lhs.m_calcOnSegmentData, rhs.m_modifyOp, lx, rx);
    }

    void UpdateModifyOp(InternalNode& lhs, const InternalNode& rhs)
    {
      if (!rhs.m_isModifyOpPresent)
        return;

      if (!lhs.m_isModifyOpPresent)
      {
        lhs.m_modifyOp = rhs.m_modifyOp;
        lhs.m_isModifyOpPresent = true;
      }
      else
      {
        UpdateModifyOp(lhs.m_modifyOp, rhs.m_modifyOp);
      }
    }

    void Modify(size_t l, size_t r, const InternalNode& updateNode, size_t x, size_t lx, size_t rx)
    {
      Propagate(x, lx, rx);

      if (rx <= l || r <= lx)
        return;

      if (l <= lx && rx <= r)
      {
        UpdateModifyOp(m_tree[x], updateNode);
        UpdateCalcData(m_tree[x], updateNode, lx, rx);
        return;
      }

      size_t m = (lx + rx) / 2;
      Modify(l, r, updateNode, 2 * x + 1, lx, m);
      Modify(l, r, updateNode, 2 * x + 2, m, rx);
      std::tie(m_tree[x].m_isCalcOnSegmentDataPresent, m_tree[x].m_calcOnSegmentData) =
        _CombineCalcData(
          m_tree[2 * x + 1].m_isCalcOnSegmentDataPresent, m_tree[2 * x + 1].m_calcOnSegmentData,
          m_tree[2 * x + 2].m_isCalcOnSegmentDataPresent, m_tree[2 * x + 2].m_calcOnSegmentData);
    }

    CalcData Calculate(size_t l, size_t r, size_t x, size_t lx, size_t rx)
    {
      return std::move(_Calculate(l, r, x, lx, rx).second);
    }

    std::pair<bool, CalcData> _Calculate(size_t l, size_t r, size_t x, size_t lx, size_t rx)
    {
      Propagate(x, lx, rx);

      if (rx <= l || r <= lx)
        return { false, CalcData{} };

      if (l <= lx && rx <= r)
        return { true, m_tree[x].m_calcOnSegmentData };

      const size_t m = (lx + rx) / 2;
      const auto [lhsIsExist, lhsCalcData] = _Calculate(l, r, 2 * x + 1, lx, m);
      const auto [rhsIsExist, rhsCalcData] = _Calculate(l, r, 2 * x + 2, m, rx);

      return _CombineCalcData(lhsIsExist, lhsCalcData, rhsIsExist, rhsCalcData);
    }

    std::pair<bool, CalcData> _CombineCalcData(const bool lhsIsExist, const CalcData& lhsCalcData, const bool rhsIsExist, const CalcData& rhsCalcData)
    {
      return { lhsIsExist || rhsIsExist, !lhsIsExist ? rhsCalcData : rhsIsExist ? CombineCalcData(lhsCalcData, rhsCalcData) : lhsCalcData };
    }

  private:
    std::vector<InternalNode> m_tree;
    size_t m_size;
    size_t m_realSize;
  };
}
#include <vector>
#include <iomanip>

class QueryManager
{
public:
  QueryManager(std::ostream& os, const Date& lowerLimit, const Date& upperLimit) : m_ostream(os), m_lowerLimit(lowerLimit), m_upperLimit(upperLimit) {
    m_income.Build(DateHelper::ComputeDiffInDays(lowerLimit, upperLimit), 0);
    m_expenses.Build(DateHelper::ComputeDiffInDays(lowerLimit, upperLimit), 0);
    m_ostream << std::fixed << std::setprecision(25);
  }

  void ProcessQuery(std::istream& is);

private:
  struct MyNode
  {
    typedef double T;

    struct CalcData
    {
      CalcData() = default;
      CalcData(const double value) : m_sum(value) {}

      double m_sum;
    };

    struct ModifyOp
    {
      ModifyOp() = default;
      ModifyOp(const double k, const double x) : m_k(k), m_x(x) {}

      double m_k;
      double m_x;
    };
  };

  class MySegmentTree : public SegmentTree::SegmentTree<MyNode>
  {
    void UpdateModifyOp(ModifyOp& lhs, const ModifyOp& rhs) override
    {
      lhs.m_k *= rhs.m_k;
      lhs.m_x = lhs.m_x * rhs.m_k + rhs.m_x;
    }

    void UpdateCalcData(CalcData& nodeCalcData, const ModifyOp& modifyOp, const size_t lx, const size_t rx) override
    {
      nodeCalcData.m_sum = nodeCalcData.m_sum * modifyOp.m_k + modifyOp.m_x * (rx - lx);
    }

    CalcData CombineCalcData(const CalcData& leftChild, const CalcData& rightChild) override
    {
      CalcData res;
      res.m_sum = leftChild.m_sum + rightChild.m_sum;
      return res;
    }
  };

  std::pair<size_t, size_t> DateLimitsToIndices(const Date& dateFromInc, const Date& dateToExc) const;

  void Earn(const Date& dateFromInc, const Date& dateToExc, double totalIncome);
  void PayTax(const Date& dateFromInc, const Date& dateToExc, const double percentage);
  void Spend(const Date& dateFromInc, const Date& dateToExc, const double totalExpenses);
  double ComputeIncome(const Date& dateFromInc, const Date& dateToExc);

  std::ostream& m_ostream;
  const Date m_upperLimit;
  const Date m_lowerLimit;
  MySegmentTree m_income;
  MySegmentTree m_expenses;
};

void RunAllTests();

void BasicTest();

#include <ctime>
#include <string>
#include <map>
#include <tuple>
#include <stdexcept>
#include <fstream>
#include <string_view>
#include <sstream>
#include <iostream>
#include <iomanip>

int main()
{
#ifdef LOCAL_LAUNCH
  RunAllTests();
#endif

  Date lowerLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 1999 });
  Date upperLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 2100 });
  QueryManager queryManager(std::cout, lowerLimit, upperLimit);
  int queriesNumber;

  std::cin >> queriesNumber;

  for (size_t q = 0; q < queriesNumber; ++q)
  {
    queryManager.ProcessQuery(std::cin);
  }

  return 0;
}

std::ostream& operator<<(std::ostream& os, const Date& date)
{
  return os << date.ToString();
}

Date DateHelper::ParseDate(std::istream& is)
{
  int day, month, year;
  is >> year;
  is.ignore(1);
  is >> month;
  is.ignore(1);
  is >> day;
  return Date(Day(day), Month(month), Year(year));
}

int DateHelper::ComputeDiffInDays(const Date& date_from, const Date& date_to) {
  const time_t timestamp_from = date_from.AsTimestamp();
  const time_t timestamp_to = date_to.AsTimestamp();
  return static_cast<int>((timestamp_to - timestamp_from) / SECONDS_IN_DAY);
}

Date DateHelper::AddDaysToDate(const Date& date, const int daysToAdd)
{
  time_t timeInSec = date.AsTimestamp() + daysToAdd * SECONDS_IN_DAY;
#pragma warning(suppress: 4996)
  tm* t = localtime(&timeInSec);
  return Date::CreateDate(t);
}

using namespace std;

void QueryManager::ProcessQuery(std::istream& is)
{
  std::string query;
  is >> query;

  if (query == "ComputeIncome")
  {
    Date dateFromInc = DateHelper::ParseDate(is);
    Date dateToExc = DateHelper::AddDaysToDate(DateHelper::ParseDate(is), 1);

    m_ostream << ComputeIncome(dateFromInc, dateToExc) << std::endl;
  }
  else if (query == "Earn")
  {
    Date dateFromInc = DateHelper::ParseDate(is);
    Date dateToExc = DateHelper::AddDaysToDate(DateHelper::ParseDate(is), 1);
    double totalIncome;
    is >> totalIncome;

    Earn(dateFromInc, dateToExc, totalIncome);
  }
  else if (query == "PayTax")
  {
    Date dateFromInc = DateHelper::ParseDate(is);
    Date dateToExc = DateHelper::AddDaysToDate(DateHelper::ParseDate(is), 1);
    double percentage;
    is >> percentage;

    PayTax(dateFromInc, dateToExc, percentage);
  }
  else if (query == "Spend")
  {
    Date dateFromInc = DateHelper::ParseDate(is);
    Date dateToExc = DateHelper::AddDaysToDate(DateHelper::ParseDate(is), 1);
    double totalExpenses;
    is >> totalExpenses;

    Spend(dateFromInc, dateToExc, totalExpenses);
  }
  else
  {
    std::ostringstream exMsg;
    exMsg << "Unexpected query: \"" << query << "\"";
    throw std::runtime_error(exMsg.str());
  }
}

std::pair<size_t, size_t> QueryManager::DateLimitsToIndices(const Date& dateFromInc, const Date& dateToExc) const
{
  const int l = DateHelper::ComputeDiffInDays(m_lowerLimit, dateFromInc);
  const int r = DateHelper::ComputeDiffInDays(m_lowerLimit, dateToExc);

  return { l, r };
}

void QueryManager::Spend(const Date& dateFromInc, const Date& dateToExc, const double totalExpenses)
{
  const int daysTotal = DateHelper::ComputeDiffInDays(dateFromInc, dateToExc);
  const double expensesPerDay = totalExpenses / daysTotal;
  auto [l, r] = DateLimitsToIndices(dateFromInc, dateToExc);

  m_expenses.Modify(l, r, MyNode::ModifyOp{ 1, expensesPerDay });
}

void QueryManager::Earn(const Date& dateFromInc, const Date& dateToExc, double income)
{
  const int daysTotal = DateHelper::ComputeDiffInDays(dateFromInc, dateToExc);
  const double incomePerDay = income / daysTotal;
  auto [l, r] = DateLimitsToIndices(dateFromInc, dateToExc);

  m_income.Modify(l, r, MyNode::ModifyOp{1, incomePerDay});
}

void QueryManager::PayTax(const Date& dateFromInc, const Date& dateToExc, const double percentage)
{
  const double coeff = 1. - percentage / 100.;

  auto [l, r] = DateLimitsToIndices(dateFromInc, dateToExc);

  m_income.Modify(l, r, MyNode::ModifyOp{ coeff, 0});
}

double QueryManager::ComputeIncome(const Date& dateFromInc, const Date& dateToExc)
{
  auto [l, r] = DateLimitsToIndices(dateFromInc, dateToExc);

  return m_income.Calculate(l, r).m_sum - m_expenses.Calculate(l, r).m_sum;
}

#include <algorithm>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

template <class Container>
std::string Join(const Container& container, char delimiter);

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
	return os << '[' << Join(v, ',') << ']';
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m)
{
	return os << '{' << Join(m, ',') << '}';
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& m)
{
	return os << '{' << Join(m, ',') << '}';
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_multimap<K, V>& m)
{
	return os << '{' << Join(m, ',') << '}';
}

template <typename L, typename R>
std::ostream& operator<<(std::ostream& os, const std::pair<L, R>& p)
{
	return os << '(' << p.first << ',' << p.second << ')';
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s)
{
	return os << '{' << Join(s, ',') << '}';
}

template <typename T, class Comp>
std::ostream& operator<<(std::ostream& os, const std::multiset<T, Comp>& s)
{
	return os << '{' << Join(s, ',') << '}';
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::list<T>& s)
{
	return os << '(' << Join(s, ',') << ')';
}

template <typename Container>
std::string Join(const Container& container, char delimiter)
{
	std::stringstream ss;
	bool flag = false;

	for (const auto& el : container)
	{
		if (flag)
		{
			ss << delimiter;
		}

		flag = true;
		ss << el;
	}

	return ss.str();
}

#include <sstream>
#include <string>
#include <iostream>

#if defined(linux) && __GNUC__ >= 4
#define THROW_MESSAGE(errorText) \
	throw tMessage(std::string(__func__) + _T(": ") + std::string(errorText))
#else
#define THROW_MESSAGE(errorText) \
	throw tMessage(std::string(_T(__FUNCTION__)) + _T(": ") + std::string(errorText))
#endif

class tMessage
{
public:
	tMessage(const char* newMessage_) : m_data(newMessage_)
	{
	}
	tMessage(const std::string& newMessage_) : m_data(newMessage_)
	{
	}
	tMessage& operator=(const tMessage& o_)
	{
		m_data = o_.m_data;
		return *this;
	}
	tMessage& operator<<(const std::string& str_)
	{
		m_data += str_;
		return *this;
	}
	tMessage& operator<<(const char* str_)
	{
		m_data += str_;
		return *this;
	}
	tMessage& operator<<(char ch_)
	{
		m_buffer[0] = ch_;
		m_buffer[1] = '\0';
		m_data += m_buffer;
		return *this;
	}
	template <typename T>
	tMessage& operator<<(const T&);
	operator const char*() const
	{
		return m_data.c_str();
	}
	operator char*() const
	{
		m_data.copy(m_buffer, m_data.length());
		return m_buffer;
	}

	std::string m_data;

private:
	static char m_buffer[];
};

inline std::ostream& operator<<(std::ostream& out, const tMessage& msg)
{
	out << msg.m_data;
	return out;
}

inline void Assert(bool assertion_, const tMessage& msg_)
{
	if (!assertion_)
		throw msg_;
}

template <typename T>
inline tMessage& tMessage::operator<<(const T& v_)
{
	/*#if !defined __BORLANDC__  ||  __BORLANDC__ >= 0x550
	 std::
	#endif*/
	/* ostrstream(Buffer,4) << v_ << '\0';
	 Data += Buffer;*/
	std::ostringstream os;
	os << v_;
	m_data += os.str();
	return *this;
}

#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

template <class T, class U, class Comparator = std::equal_to<T>>
void AssertEqual(
	const T& t, const U& u, const std::string& hint = {}, Comparator cmp = Comparator())
{
#pragma warning(push)
#pragma warning(disable : 4389)
	if (!cmp(t, u))
#pragma warning(pop)
	{
		std::stringstream os;
		os << "Assertion failed: " << t << " != " << u;
		if (!hint.empty())
		{
			os << " hint: " << hint;
		}
		throw std::runtime_error(os.str());
	}
}

inline void Assert(bool b, const std::string& hint)
{
	AssertEqual(b, true, hint, std::equal_to<bool>());
}

class TestRunner
{
public:
	template <class TestFunc, class... Args>
	void RunTest(TestFunc func, std::string_view test_name, Args... args)
	{
		try
		{
			func(args...);
			std::cerr << test_name << " OK" << std::endl;
		}
		catch (const tMessage& e)
		{
			++m_failCount;
			std::cerr << test_name << " fail: " << e << std::endl;
		}
		catch (std::exception& e)
		{
			++m_failCount;
			std::cerr << test_name << " fail: " << e.what() << std::endl;
		}
		catch (...)
		{
			++m_failCount;
			std::cerr << test_name << " fail: Unknown exception caught" << std::endl;
		}
	}

	~TestRunner()
	{
		if (m_failCount > 0)
		{
			std::cerr << m_failCount << " unit tests failed. Terminate" << std::endl;
			exit(1);
		}
		else
		{
			std::cerr << "All tests passed." << std::endl;
		}
	}

private:
	int m_failCount = 0;
};

#define ASSERT_EQUAL_CMP(x, y, comparator)                                                      \
	{                                                                                           \
		std::stringstream __assert_equal_private_os;                                            \
		__assert_equal_private_os << #x << " != " << #y << ", " << __FILE__ << ":" << __LINE__; \
		AssertEqual(x, y, __assert_equal_private_os.str(), comparator);                         \
	}

#define ASSERT_EQUAL(x, y)                                                                      \
	{                                                                                           \
		std::stringstream __assert_equal_private_os;                                            \
		__assert_equal_private_os << #x << " != " << #y << ", " << __FILE__ << ":" << __LINE__; \
		AssertEqual(x, y, __assert_equal_private_os.str());                                     \
	}

#define ASSERT(x)                                                                        \
	{                                                                                    \
		std::stringstream __assert_equal_private_os;                                     \
		__assert_equal_private_os << #x << " is false, " << __FILE__ << ":" << __LINE__; \
		Assert(x, __assert_equal_private_os.str());                                      \
	}

#define RUN_TEST(tr, func, ...) tr.RunTest(func, #func, ##__VA_ARGS__)

#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

const fs::path GetTestPath()
{
  return fs::path("tests_data");
}

void RunAllTests()
{
  TestRunner tr;
  RUN_TEST(tr, BasicTest);
}

namespace
{
  std::vector<double> ReadOutput(std::istream& is)
  {
    std::vector<double> res;
    std::string str;

    while (std::getline(is, str))
    {
      res.push_back(std::stod(str));
    }

    return res;
  }

  void CompareRes(const std::vector<double>& res, const std::vector<double>& expected)
  {
    ASSERT(res.size() == expected.size());

    for (size_t i = 0; i < res.size(); ++i)
    {
      ASSERT(mathdef::is_eq(res[i], expected[i]));
    }
  }
}

void BasicTest()
{
  const fs::path dataDir = GetTestPath() / "BasicTest";
  const fs::path inputFilename1 = dataDir / "test1_in.txt";
  std::ifstream ifs(inputFilename1);
  if (!ifs.is_open())
  {
    throw std::runtime_error(std::string("file \"") + inputFilename1.string() + "\" not found");
  }

  std::stringstream ss;
  Date lowerLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 1999 });
  Date upperLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 2100 });
  QueryManager queryManager(ss, lowerLimit, upperLimit);
  int queriesNumber;

  ifs >> queriesNumber;

  for (size_t q = 0; q < queriesNumber; ++q)
  {
    queryManager.ProcessQuery(ifs);
  }

  const fs::path inputFilename2 = dataDir / "test1_out.txt";
  std::ifstream ifs_expected(inputFilename2);
  if (!ifs_expected.is_open())
  {
    throw std::runtime_error(std::string("file \"") + inputFilename2.string() + "\" not found");
  }
  std::stringstream expectedSS;
  expectedSS << ifs_expected.rdbuf();

  auto res = ReadOutput(ss);
  auto expected = ReadOutput(expectedSS);
  CompareRes(res, expected);
}

char tMessage::m_buffer[300];
