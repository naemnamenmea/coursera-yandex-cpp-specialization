#include "query_manager.hpp"

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
