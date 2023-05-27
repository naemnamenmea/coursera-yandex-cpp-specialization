#include "test_runner.h"
#include <algorithm>
#include <memory>
#include <utility>
#include <tuple>
#include <vector>

using namespace std;

template <typename RandomIt>
void MergeSort(RandomIt range_begin, RandomIt range_end) {
	size_t size = distance(range_begin, range_end);
	if (size < 2) {
		return;
	}
	using T = typename RandomIt::value_type;
	size_t part_size = size / 3;
	vector<T> v(
		make_move_iterator(range_begin),
		make_move_iterator(range_end));

	auto part1 = pair( begin(v), begin(v) + part_size );
	auto part2 = pair{ begin(v) + part_size, begin(v) + part_size * 2 };
	auto part3 = pair{ begin(v) + part_size * 2, end(v) };

	MergeSort(part1.first, part1.second);
	MergeSort(part2.first, part2.second);
	MergeSort(part3.first, part3.second);

	vector<T> tmp;
	merge(
		make_move_iterator(part1.first), make_move_iterator(part1.second),
		make_move_iterator(part2.first), make_move_iterator(part2.second),
		back_inserter(tmp));
	merge(
		make_move_iterator(begin(tmp)), make_move_iterator(end(tmp)),
		make_move_iterator(part3.first), make_move_iterator(part3.second),
		range_begin);
}

struct NoncopyableInt {
	int value;

	NoncopyableInt() = default;

	NoncopyableInt(const NoncopyableInt&) = delete;
	NoncopyableInt& operator=(const NoncopyableInt&) = delete;

	NoncopyableInt(NoncopyableInt&&) = default;
	NoncopyableInt& operator=(NoncopyableInt&&) = default;
};

bool operator < (const NoncopyableInt& lhs, const NoncopyableInt& rhs) {
	return lhs.value < rhs.value;
}

void TestIntVector() {
	vector<NoncopyableInt> numbers;
	numbers.push_back({ 6 });
	numbers.push_back({ 1 });
	numbers.push_back({ 3 });
	numbers.push_back({ 9 });
	numbers.push_back({ 1 });
	numbers.push_back({ 9 });
	numbers.push_back({ 8 });
	numbers.push_back({ 12 });
	numbers.push_back({ 1 });
	MergeSort(begin(numbers), end(numbers));
	ASSERT(is_sorted(begin(numbers), end(numbers)));
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestIntVector);
	
	return 0;
}
