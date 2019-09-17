#include "test_runner.h"

#include <vector>

using namespace std;

// ��������� Sentence<Token> ��� ������������� ���� Token
// ��������� vector<Token>.
// ��������� ����� � �������� ������������� ��������
// ������� ����� ������� �� ������������ ������ ��������,
// � ������ ����������� � vector<Sentence<Token>>.
template <typename Token>
using Sentence = vector<Token>;

// ����� Token ����� ����� bool IsEndSentencePunctuation() const
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
	vector<Sentence<Token>> res;

	bool flag = false;
	Sentence<Token> tmp;
	for (auto it = tokens.begin(); it != tokens.end(); ) {
		if (!it->IsEndSentencePunctuation()) {
			if (flag) {
				flag = false;
				res.push_back(move(tmp));
			}
		}
		else {
			flag = true;
		}
		tmp.push_back(move(*it));

		if (++it == tokens.end()) {
			res.push_back(move(tmp));
		}
	}
	return res;
}


struct TestToken {
	string data;
	bool is_end_sentence_punctuation = false;

	TestToken() = default;
	TestToken(const TestToken& token) = default;
	TestToken& operator=(const TestToken& token) = default;
	TestToken(TestToken&& token) = default;
	TestToken& operator=(TestToken&& token) = default;

	bool IsEndSentencePunctuation() const {
		return is_end_sentence_punctuation;
	}
	bool operator==(const TestToken& other) const {
		return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
	}
};

ostream& operator<<(ostream& stream, const TestToken& token) {
	return stream << token.data;
}

// ���� �������� ����������� �������� ������ TestToken.
// ��� �������� ���������� ����������� � ������� SplitIntoSentences
// ���������� �������� ��������� ����.
void TestSplitting() {
	ASSERT_EQUAL(
		SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"sentences"}, {"!"} })),
		vector<Sentence<TestToken>>({
			{{"Split"}, {"into"}, {"sentences"}, {"!"}}
			})
	);

	ASSERT_EQUAL(
		SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"sentences"}, {"!", true} })),
		vector<Sentence<TestToken>>({
			{{"Split"}, {"into"}, {"sentences"}, {"!", true}}
			})
	);

	ASSERT_EQUAL(
		SplitIntoSentences(vector<TestToken>({ {"!", true},{"!", true},{"!", true} })),
		vector<Sentence<TestToken>>({
			{{"!", true},{"!", true},{"!", true}}
			})
	);

	ASSERT_EQUAL(
		SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"!", true}, {"sentences"} })),
		vector<Sentence<TestToken>>({
			{{"Split"}, {"into"}, {"!", true}},
			{{"sentences"}}
			})
	);

	ASSERT_EQUAL(
		SplitIntoSentences(vector<TestToken>({ {"Split"}, {"into"}, {"sentences"}, {"!", true}, {"!", true}, {"Without"}, {"copies"}, {".", true} })),
		vector<Sentence<TestToken>>({
			{{"Split"}, {"into"}, {"sentences"}, {"!", true}, {"!", true}},
			{{"Without"}, {"copies"}, {".", true}},
			})
			);
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSplitting);
	return 0;
}
