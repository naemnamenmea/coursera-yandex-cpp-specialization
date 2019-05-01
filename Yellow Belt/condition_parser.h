#pragma once

#include <memory>
#include <iostream>
#include <map>
#include <exception>

#include "node.h"
#include "token.h"

using namespace std;

class Node;

enum class Comparison {
	Less,
	LessOrEqual,
	Greater,
	GreaterOrEqual,
	Equal,
	NotEqual
};

enum class LogicalOperation {
	Or,
	And
};

template<class T>
bool Compare(const T& left, const T& right, const Comparison cmp)
{
	switch (cmp)
	{
	case Comparison::Less:
		return left < right;
	case Comparison::LessOrEqual:
		return left <= right;
	case Comparison::Greater:
		return left > right;
	case Comparison::GreaterOrEqual:
		return left >= right;
	case Comparison::Equal:
		return left == right;
	case Comparison::NotEqual:
		return left != right;
	}
}

shared_ptr<Node> ParseCondition(istream& is);

void TestParseCondition();