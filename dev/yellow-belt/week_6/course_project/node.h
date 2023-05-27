#pragma once

#include <string>
#include <exception>
#include "date.h"
#include "condition_parser.h"

enum class LogicalOperation;
enum class Comparison;

template<class T>
bool Compare(const T& left, const T& right, const Comparison cmp);

class Node {
public:
	virtual bool Evaluate(const Date& date, const string& event) = 0;
};

class EmptyNode : public Node {
public:
	bool Evaluate(const Date& date, const string& event);
};

class DateComparisonNode : public Node {
public:
	DateComparisonNode(const Comparison& cmp, const Date& date);
	bool Evaluate(const Date& date, const string& event) override;
private:
	Comparison cmp_;
	Date date_;
};

class EventComparisonNode : public Node {
public:
	EventComparisonNode(const Comparison& cmp, const string& event);
	bool Evaluate(const Date& date, const string& event) override;
private:
	Comparison cmp_;
	string event_;
};

class LogicalOperationNode : public Node {
public:
	LogicalOperationNode(const LogicalOperation logical_op, shared_ptr<Node> left, shared_ptr<Node> right);

	bool Evaluate(const Date& date, const string& event) override;

	LogicalOperation logical_op_;
	shared_ptr<Node> left_;
	shared_ptr<Node> right_;
};