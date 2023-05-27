#include <iostream>
#include <set>
#include <stdexcept>
#include <fstream>
#include <experimental/filesystem>

#include "database.h"
#include "date.h"
#include "condition_parser.h"
#include "node.h"

#include "tests.h"
#include "test_runner.h"

namespace fs = std::experimental::filesystem;
using namespace std;

string ParseEvent(istream& is) {
	string s;
	is >> ws;
	getline(is, s);
	return s;
}

void TestAll();

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	//TestAll();
	const fs::path ROOT = fs::path(__FILE__).parent_path();
	ifstream ifs(ROOT / INPUT_FILENAME); cin.rdbuf(ifs.rdbuf());
	//ofstream ofs(ROOT / OUTPUT_FILENAME); cout.rdbuf(ofs.rdbuf());

	Database db;
	
	for (string line; getline(cin, line); ) {
		istringstream is(line);

		string command;
		is >> command;
		if (command == "Add") {
			const auto date = ParseDate(is);
			const auto event = ParseEvent(is);
			db.Add(date, event);			
		}
		else if (command == "Print") {
			db.Print(cout);
		}
		else if (command == "Del") {
			auto condition = ParseCondition(is);
			auto predicate = [condition](const Date& date, const string& event) {
				return condition->Evaluate(date, event);
			};
			int count = db.RemoveIf(predicate);
			cout << "Removed " << count << " entries" << endl;
		}
		else if (command == "Find") {
			auto condition = ParseCondition(is);
			auto predicate = [condition](const Date& date, const string& event) {
				return condition->Evaluate(date, event);
			};

			const auto entries = db.FindIf(predicate);
			for (const auto& entry : entries) {
				cout << entry.first << ' ' << entry.second << endl;
			}
			cout << "Found " << entries.size() << " entries" << endl;
		}
		else if (command == "Last") {
			try {
				cout << db.Last(ParseDate(is)) << endl;
			}
			catch (invalid_argument&) {
				cout << "No entries" << endl;
			}
		}
		else if (command.empty()) {
			continue;
		}
		else {
			throw logic_error("Unknown command: " + command);
		}
	}

	return 0;
}

void TestParseEvent() {
	{
		istringstream is("event");
		AssertEqual(ParseEvent(is), "event", "Parse event without leading spaces");
	}
	{
		istringstream is("   sport event ");
		AssertEqual(ParseEvent(is), "sport event ", "Parse event with leading spaces");
	}
	{
		istringstream is("  first event  \n  second event");
		vector<string> events;
		events.push_back(ParseEvent(is));
		events.push_back(ParseEvent(is));
		AssertEqual(events, vector<string>{"first event  ", "second event"}, "Parse multiple events");
	}
}

void TestAll() {
	TestRunner tr;
	tr.RunTest(CompDate, "CompDate");
	tr.RunTest(TestDataOutput, "TestDataOutput");
	tr.RunTest(TestLegalDataInput, "TestLegalDataInput");
	tr.RunTest(TestParseEvent, "TestParseEvent");
	tr.RunTest(TestParseCondition, "TestParseCondition");
	tr.RunTest(TestAdd, "TestAdd");
	tr.RunTest(TestPrint, "TestPrint");
	tr.RunTest(TestFindIf, "TestFindIf");
	tr.RunTest(TestDelIf, "TestDel");
	tr.RunTest(TestLast, "TestLast");
}