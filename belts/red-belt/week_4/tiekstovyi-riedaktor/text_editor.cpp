#define NOMINMAX

#include <typeinfo>
#include <list>
#include <cassert>
#include <algorithm>
#include <string>
#include "test_runner.h"
using namespace std;

class Editor {
public:
	Editor()
		: pos(data.end()), right_el_count(0)
	{

	}
	void Left() {
		if (pos != data.begin()) {
			pos = prev(pos);
			++right_el_count;
		}
	}
	void Right() {
		if (pos != data.end()) {
			pos = next(pos);
			--right_el_count;
		}
	}
	void Insert(char token) {
		data.insert(pos, token);
	}
	void Cut(size_t tokens = 1) {
		size_t val = min(right_el_count, tokens);
		buffer.clear();
		auto it = next(pos, val);
		buffer.splice(buffer.begin(), data, pos, it);
		pos = it;
		right_el_count -= val;
	}
	void Copy(size_t tokens = 1) {
		size_t val = min(right_el_count, tokens);
		buffer.assign(pos, next(pos, val));
	}
	void Paste() {
		data.insert(pos, begin(buffer), end(buffer));
	}
	string GetText() const {
		return { begin(data), end(data) };
	}

private:
	list<char> data;
	list<char> buffer;
	list<char>::iterator pos;
	size_t right_el_count;
};

void TypeText(Editor& editor, const string& text) {
	for (char c : text) {
		editor.Insert(c);
	}
}

void TestEditing() {
	{
		Editor editor;

		const size_t text_len = 12;
		const size_t first_part_len = 7;
		TypeText(editor, "hello, world");
		for (size_t i = 0; i < text_len; ++i) {
			editor.Left();
		}
		editor.Cut(first_part_len);
		for (size_t i = 0; i < text_len - first_part_len; ++i) {
			editor.Right();
		}
		TypeText(editor, ", ");
		editor.Paste();
		editor.Left();
		editor.Left();
		editor.Cut(3);

		ASSERT_EQUAL(editor.GetText(), "world, hello");
	}
	{
		Editor editor;

		TypeText(editor, "misprnit");
		editor.Left();
		editor.Left();
		editor.Left();
		editor.Cut(1);
		editor.Right();
		editor.Paste();

		ASSERT_EQUAL(editor.GetText(), "misprint");
	}
}

void TestReverse() {
	Editor editor;

	const string text = "esreveR";
	for (char c : text) {
		editor.Insert(c);
		editor.Left();
	}

	ASSERT_EQUAL(editor.GetText(), "Reverse");
}

void TestNoText() {
	Editor editor;
	ASSERT_EQUAL(editor.GetText(), "");

	editor.Left();
	editor.Left();
	editor.Right();
	editor.Right();
	editor.Copy(0);
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "");
}

void TestEmptyBuffer() {
	Editor editor;

	editor.Paste();
	TypeText(editor, "example");
	editor.Left();
	editor.Left();
	editor.Paste();
	editor.Right();
	editor.Paste();
	editor.Copy(0);
	editor.Paste();
	editor.Left();
	editor.Cut(0);
	editor.Paste();

	ASSERT_EQUAL(editor.GetText(), "example");
}

void MultiplePaste() {
	Editor editor;
	TypeText(editor, "text");
	editor.Left();
	editor.Copy(1);
	for (int i = 0; i < 3; ++i) {
		editor.Paste();
	}
	ASSERT_EQUAL(editor.GetText(), "textttt");
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestEditing);
	RUN_TEST(tr, TestReverse);
	RUN_TEST(tr, TestNoText);
	RUN_TEST(tr, TestEmptyBuffer);
	RUN_TEST(tr, MultiplePaste);
	return 0;
}