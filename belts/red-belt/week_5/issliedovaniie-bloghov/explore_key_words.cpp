#include "test_runner.h"
#include "profile.h"

#include <sstream>
#include <set>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include <iterator>
#include <execution>
#include <map>
#include <algorithm>
#include <future>
#include <functional>
#include <string>
#include <experimental/filesystem>

using namespace std;
namespace fs = std::experimental::filesystem;

struct Stats {
	map<string, int> word_frequences;

	void operator += (const Stats& other) {
		for (const auto& [word, frequency] : other.word_frequences)
		{
			word_frequences[word] += frequency;
		}
	}
};

Stats ExploreLine(const set<string>& key_words, const string& line)
{
	Stats res;
	size_t from = 0;
	while (from != string::npos) {
		size_t to = line.find(' ', from);
		string word = line.substr(from, to - from);
		if (key_words.count(word)) {
			res.word_frequences[move(word)]++;
		}
		if (to != string::npos) {
			from = to + 1;
		}
		else {
			from = to;
		}
	}
	return res;
}

Stats ExploreKeyWordsSingleThread(
	const set<string>& key_words, istream& input)
{
	Stats result;
	for (string line; getline(input, line); ) {
		result += ExploreLine(key_words, line);
	}
	return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input)
{
	vector<future<Stats>> stats;
	const size_t CHUNK_SIZE = 10'000;

	string words;
	while (!input.eof())
	{
		string word;
		input >> word;
		words += move(word);

		bool finish = input.rdbuf()->in_avail() == 0;
		if (words.size() > CHUNK_SIZE || finish) {
			stats.push_back(async(ExploreLine, ref(key_words), move(words)));
		}
		else {
			words += ' ';
		}

		if (finish) {
			break;
		}
	}

	Stats res;
	for (auto& stat : stats) {
		res += stat.get();
	}
	return res;
}

void TestBasic(function<Stats(const set<string>&, istream&)> func)
{
	const set<string> key_words = { "yangle", "rocks", "sucks", "all" };

	stringstream ss;
	ss << "this new yangle service really rocks\n";
	ss << "It sucks when yangle isn't available\n";
	ss << "10 reasons why yangle is the best IT company\n";
	ss << "yangle rocks others suck\n";
	ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

	const auto stats = func(key_words, ss);
	const map<string, int> expected = {
	  {"yangle", 6},
	  {"rocks", 2},
	  {"sucks", 1}
	};
	ASSERT_EQUAL(stats.word_frequences, expected);
}

void TestBigText(function<Stats(const set<string>&, istream&)> func)
{
	const set<string> key_words = { "hobbit", "hole", "dragon", "food", "dwarf" };

	const fs::path root = fs::path(__FILE__).parent_path();
	ifstream ifs(root / "The Hobbit.txt");
	const auto stats = func(key_words, ifs);

	//cerr << "There and back again..." << endl;
	//for (auto& [word, frequency] : stats.word_frequences) {
	//	cerr << setw(10) << word << " " << frequency << endl;
	//}

	ifs.close();
}

int main()
{
	cin.tie(nullptr);
	ios::sync_with_stdio(false);

	TestRunner tr;
	{
		LOG_DURATION("TestBasicSingleThread");
		RUN_TEST(tr, TestBasic, ExploreKeyWordsSingleThread);
	}
	{
		LOG_DURATION("TestBasicMultiThreaded");
		RUN_TEST(tr, TestBasic, ExploreKeyWords);
	}
	{
		LOG_DURATION("TestBigTextSingleThread");
		RUN_TEST(tr, TestBigText, ExploreKeyWordsSingleThread);
	}
	{
		LOG_DURATION("TestBigTextMultiThreaded");
		RUN_TEST(tr, TestBigText, ExploreKeyWords);
	}
}