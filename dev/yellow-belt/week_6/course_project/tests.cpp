#include <sstream>

#include "condition_parser.h"
#include "tests.h"

using namespace std;

void TestDataOutput() {
	{
		ostringstream os;
		Date date{ 208,6,7 };
		os << date;
		AssertEqual(os.str(), "0208-06-07", "Date output with leading zeros");
	}
	{
		ostringstream os;
		Date date{ 2008,6,17 };
		os << date;
		AssertEqual(os.str(), "2008-06-17", "Date output with leading zeros");
	}
}

void TestLegalDataInput() {
	{
		string date_str = "2015-05-03";
		istringstream is(date_str);
		Date date = ParseDate(is);
		Assert(is.rdbuf()->in_avail() == 0, "");
		ostringstream os;
		os << date;
		AssertEqual(os.str(), date_str, "");
	}
	{
		string date_str = "2015-5-3";
		istringstream is(date_str);
		Date date = ParseDate(is);
		Assert(is.rdbuf()->in_avail() == 0, "");
		ostringstream os;
		os << date;
		AssertEqual(os.str(), "2015-05-03", "");
	}
	{
		string date_str = "2015-5-13   ";
		istringstream is(date_str);
		Date date = ParseDate(is);
		Assert(is.rdbuf()->in_avail() != 0, "");
		ostringstream os;
		os << date;
		AssertEqual(os.str(), "2015-05-13", "");
	}
}

void CompDate() {
	{
		Assert(Date{ 2016,6,7 } < Date{ 2016,6,8 }, "");
		Assert(Date{ 2016,9,17 } < Date{ 2017,6,8 }, "");
		Assert(Date{ 2016,6,7 } < Date{ 2016,9,5 }, "");
		Assert(!(Date{ 2016,7,7 } < Date{ 2016,6,8 }), "");
		Assert(!(Date{ 2017,6,7 } < Date{ 2016,9,7 }), "");
		Assert(!(Date{ 2017,6,7 } < Date{ 2016,6,8 }), "");
	}
}

void TestAdd() {
	{
		Database db;
		db.Add({ 2017,1,1 }, "New Year");
		db.Add({ 2017,3,8 }, "Holiday");
		db.Add({ 2017,1,1 }, "Holiday");
		try {
			db.Last({ 2016,12,31 });
			throw runtime_error("");
		}
		catch (invalid_argument&) {}
		AssertEqual(db.Last({ 2017,1,1 }), "2017-01-01 Holiday", "");
		AssertEqual(db.Last({ 2017,6,1 }), "2017-03-08 Holiday", "");
		db.Add({ 2017,5,9 }, "Holiday");
	}
}

void TestPrint() {
	{
		Database db;
		db.Add({ 2017,1,1 }, "Holiday");
		db.Add({ 2017,3,8 }, "Holiday");
		db.Add({ 2017,1,1 }, "New Year");
		db.Add({ 2017,1,1 }, "New Year");

		string origin = "2017-01-01 Holiday\n2017-01-01 New Year\n2017-03-08 Holiday\n";
		ostringstream os;
		db.Print(os);
		AssertEqual(origin, os.str(), "");
	}
}

void TestFindIf() {
	{
		Database db;
		db.Add({ 2017,1,1 }, "Holiday");
		db.Add({ 2017,3,8 }, "Holiday");
		db.Add({ 2017,1,1 }, "New Year");

		istringstream is(R"(event != "working day")");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date & date, const string & event) {
			return condition->Evaluate(date, event);
		};

		vector<pair<Date, string>> origin = { {{2017,1,1},"Holiday"},{{2017,1,1},"New Year"},{{2017,3,8},"Holiday"} };
		const auto entries = db.FindIf(predicate);
		AssertEqual(entries, origin, "");

		db.Add({ 2017,5,9 }, "Holiday");
	}
}

void TestDelIf() {
	{
		Database db;
		db.Add({ 2017,6,1 }, "1st of June");
		db.Add({ 2017,7,8 }, "8th of July");
		db.Add({ 2017,7,8 }, "Someone's birthday");

		istringstream is(R"(date == 2017-07-08)");
		auto condition = ParseCondition(is);
		auto predicate = [condition](const Date & date, const string & event) {
			return condition->Evaluate(date, event);
		};

		string origin = "2017-06-01 1st of June\n";
		const auto entries = db.RemoveIf(predicate);
		ostringstream os;
		db.Print(os);
		AssertEqual(os.str(), origin, "");
	}
}

void TestLast() {
	{
		Database db;
		db.Add({ 2017,1,1 }, "New Year");
		db.Add({ 2017,3,8 }, "Holiday");
		db.Add({ 2017,1,1 }, "Holiday");
		try {
			db.Last({ 2016,12,31 });
			throw runtime_error("");
		}
		catch (invalid_argument&) {}
		AssertEqual(db.Last({ 2017,1,1 }), "2017-01-01 Holiday", "");
		AssertEqual(db.Last({ 2017,6,1 }), "2017-03-08 Holiday", "");
		db.Add({ 2017,5,9 }, "Holiday");
	}
}