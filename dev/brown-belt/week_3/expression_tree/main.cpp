#include "Common.h"
#include "test_runner.h"

#include <sstream>
#include <unordered_set>

using namespace std;

inline string WrapInBrackets(string value)
{
  return '(' + move(value) + ')';
}

class ValueExpression : public Expression
{
public:
  ValueExpression(int value) : m_value(value) {}

  int Evaluate() const override
  {
    return m_value;
  }

  string ToString() const override
  {
    return to_string(m_value);
  }

private:
  int m_value;
};

class BinaryOp : public Expression
{
public:
  BinaryOp(ExpressionPtr lhs, ExpressionPtr rhs, char op)
    :m_lhs(move(lhs)), m_rhs(move(rhs)), m_op(op) {
    if (m_supportedOperations.count(op) == 0)
    {
      throw runtime_error("unsupported binary op \'" + to_string(op) + "\'");
    }
  }

  int Evaluate() const override
  {
    switch (m_op)
    {
    case '+':
      return m_lhs->Evaluate() + m_rhs->Evaluate();
    case '*':
      return m_lhs->Evaluate() * m_rhs->Evaluate();
    default:
      throw runtime_error("unsupported binary op \'" + to_string(m_op) + "\'");
    }
  }

  string ToString() const override
  {
    string res = WrapInBrackets(m_lhs->ToString()) + m_op + WrapInBrackets(m_rhs->ToString());
    return res;
  }

protected:
  static const unordered_set<char> m_supportedOperations;

private:
  char m_op;
  ExpressionPtr m_lhs;
  ExpressionPtr m_rhs;
};

const unordered_set<char> BinaryOp::m_supportedOperations = { '+', '*' };

ExpressionPtr Value(int value)
{
  return make_unique<ValueExpression>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right)
{
  return make_unique<BinaryOp>(move(left), move(right), '+');
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right)
{
  return make_unique<BinaryOp>(move(left), move(right), '*');
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ExpressionPtr e3 = Value(5);
  ASSERT_EQUAL(Print(e3.get()), "5 = 5");

  ExpressionPtr e4 = Sum(Value(2), Product(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e4.get()), "(2)+((3)*(4)) = 14");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}