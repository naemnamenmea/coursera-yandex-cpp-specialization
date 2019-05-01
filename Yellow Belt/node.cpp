#include "node.h"

bool Node::Evaluate(const Date& date, const string& event)
{
	return false;
}

LogicalOperationNode::LogicalOperationNode(const LogicalOperation logical_op, shared_ptr<Node> left, shared_ptr<Node> right)
	:logical_op_{ logical_op }, left_{ left }, right_{ right }
{
}

bool LogicalOperationNode::Evaluate(const Date& date, const string& event)
{
	switch (this->logical_op_) {
	case LogicalOperation::And:
		return this->left_->Evaluate(date, event) && this->right_->Evaluate(date, event);
	case  LogicalOperation::Or:
		return this->left_->Evaluate(date, event) || this->right_->Evaluate(date, event);		
	}
}

DateComparisonNode::DateComparisonNode(const Comparison& cmp, const Date& date)
	: cmp_{ cmp }, date_{ date }
{
}

bool DateComparisonNode::Evaluate(const Date& date, const string& event)
{
	return Compare(date, this->date_, this->cmp_);
}

EventComparisonNode::EventComparisonNode(const Comparison& cmp, const string& event)
	:cmp_{ cmp }, event_{ event }
{
}

bool EventComparisonNode::Evaluate(const Date& date, const string& event)
{
	return Compare(event, this->event_, this->cmp_);
}

bool EmptyNode::Evaluate(const Date& date, const string& event)
{
	return true;
}
