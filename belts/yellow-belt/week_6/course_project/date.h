#pragma once
#include <sstream>
#include <iomanip>
#include <iostream>

using namespace std;

class Day {
public:
	Day(int day);

	int value_;
};

class Month {
public:
	Month(int month);

	int value_;
};

class Year {
public:
	Year(int year);

	int value_;
};

class Date {
public:
	Date(const Year& year, const Month& month, const Day& day);
	inline int GetDay() const
	{
		return this->day_;
	}

	inline int GetMonth() const
	{
		return this->month_;
	}

	inline int GetYear() const
	{
		return this->year_;
	}

protected:
	int year_;
	int month_;
	int day_;
};

Date ParseDate(istream& is);

ostream& operator<<(ostream& os, const Date& date);

inline bool operator<(const Date& lhs, const Date& rhs)
{
	return lhs.GetYear() != rhs.GetYear() ? lhs.GetYear() < rhs.GetYear() :
		lhs.GetMonth() != rhs.GetMonth() ? lhs.GetMonth() < rhs.GetMonth() :
		lhs.GetDay() < rhs.GetDay();
}

inline bool operator>(const Date& lhs, const Date& rhs)
{
	return rhs < lhs;
}

inline bool operator<=(const Date& lhs, const Date& rhs)
{
	return !(lhs > rhs);
}

inline bool operator>=(const Date& lhs, const Date& rhs)
{
	return !(lhs < rhs);
}

inline bool operator==(const Date& lhs, const Date& rhs)
{
	return lhs.GetYear() == rhs.GetYear()
		&& lhs.GetMonth() == rhs.GetMonth()
		&& lhs.GetDay() == rhs.GetDay();
}

inline bool operator!=(const Date& lhs, const Date& rhs)
{
	return !(lhs == rhs);
}