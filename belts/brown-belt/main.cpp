#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <execution>

using namespace std;

int main(int argc, char* argv[]) {

	mutex m;
	vector<int> v{ 1,2,3,4,5,6,7,8,9 };

	for_each(execution::par, begin(v), end(v), [&m](int x) {
		lock_guard guard(m);
		cout << x << endl;
		});

	return 0;
}