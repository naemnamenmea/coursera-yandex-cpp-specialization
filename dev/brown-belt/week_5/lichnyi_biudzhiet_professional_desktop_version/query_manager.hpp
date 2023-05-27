#pragma once

#include "dateUtils.hpp"
#include "segment_tree.hpp"
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
