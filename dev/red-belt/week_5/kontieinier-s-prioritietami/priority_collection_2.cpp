#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
public:
	using Id = size_t;
	using Id2 = multiset<pair<int,Id>>::iterator;

	// Добавить объект с нулевым приоритетом
	// с помощью перемещения и вернуть его идентификатор
	Id Add(T object) {
		Id id = data.size();
		auto it = m.insert({ 0,id });
		data.push_back({ move(object), it});
		return id;
	}

	// Добавить все элементы диапазона [range_begin, range_end)
	// с помощью перемещения, записав выданные им идентификаторы
	// в диапазон [ids_begin, ...)
	template <typename ObjInputIt, typename IdOutputIt>
	void Add(ObjInputIt range_begin, ObjInputIt range_end, IdOutputIt ids_begin) {
		while (range_begin != range_end) {
			Id id = data.size();
			auto it = m.insert({ 0,id });
			data.push_back({ move(*range_begin), it });
			*ids_begin++ = id;
			++range_begin;
		}
	}

	// Определить, принадлежит ли идентификатор какому-либо
	// хранящемуся в контейнере объекту
	bool IsValid(Id id) const {
		return id < data.size() && data[id].second != end(m);
	}

	// Получить объект по идентификатору
	const T& Get(Id id) const {
		return data[id].first;
	}

	// Увеличить приоритет объекта на 1
	void Promote(Id id) {
		auto it = data[id].second;
		int rate = it->first;
		m.erase(it);
		data[id].second = m.insert({rate + 1, id});
	}

	// Получить объект с максимальным приоритетом и его приоритет
	pair<const T&, int> GetMax() const {
		auto it = prev(end(m));
		return {data[it->second].first,it->first};
	}

	// Аналогично GetMax, но удаляет элемент из контейнера
	pair<T, int> PopMax() {
		auto it = prev(end(m));
		Id id = it->second;
		int rate = it->first;
		m.erase(it);
		data[id].second = end(m);
		return { move(data[id]).first,rate };
	}

private:
	vector<pair<T, Id2>> data;
	multiset<pair<int, Id>> m;
};


class StringNonCopyable : public string {
public:
	using string::string;  // Позволяет использовать конструкторы строки
	StringNonCopyable(const StringNonCopyable&) = delete;
	StringNonCopyable(StringNonCopyable&&) = default;
	StringNonCopyable& operator=(const StringNonCopyable&) = delete;
	StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
	PriorityCollection<StringNonCopyable> strings;
	const auto white_id = strings.Add("white");
	const auto yellow_id = strings.Add("yellow");
	const auto red_id = strings.Add("red");

	strings.Promote(yellow_id);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(red_id);
	}
	strings.Promote(yellow_id);
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 0);
	}
}

void TestAddRange() {
	PriorityCollection<StringNonCopyable> strings;
	vector<StringNonCopyable> data;
	data.push_back({ "white" });
	data.push_back({ "yellow" });
	data.push_back({ "red" });
	vector<size_t> res(data.size());
	strings.Add(begin(data), end(data), begin(res));

	strings.Promote(res[1]);
	for (int i = 0; i < 2; ++i) {
		strings.Promote(res[2]);
	}
	strings.Promote(res[1]);
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "red");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "yellow");
		ASSERT_EQUAL(item.second, 2);
	}
	{
		const auto item = strings.PopMax();
		ASSERT_EQUAL(item.first, "white");
		ASSERT_EQUAL(item.second, 0);
	}
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestNoCopy);
	RUN_TEST(tr, TestAddRange);
	return 0;
}
