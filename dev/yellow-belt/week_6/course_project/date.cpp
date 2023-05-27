#include "date.h"

Day::Day(int day) 
	:value_{ day } 
{}

Month::Month(int month) 
	: value_{ month } 
{}

Year::Year(int year) 
	: value_{ year } 
{}

Date::Date(const Year& year, const Month& month, const Day& day)
	: year_{ year.value_ }, month_{ month.value_ }, day_{ day.value_ } 
{}

Date ParseDate(istream & is) {
	int day, month, year;
	is >> year;
	is.ignore(1);
	is >> month;
	is.ignore(1);
	is >> day;
	return { year,month,day };
}

ostream& operator<<(ostream& os, const Date& date) {
	return os << setfill('0') << setw(4) << date.GetYear()
		<< '-' << setw(2) << date.GetMonth()
		<< '-' << setw(2) << date.GetDay();
}