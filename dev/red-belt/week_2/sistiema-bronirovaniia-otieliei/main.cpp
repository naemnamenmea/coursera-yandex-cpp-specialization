#define NOMINMAX

#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include <unordered_map>
#include <fstream>
#include <experimental/filesystem>
#include <list>
#include <limits>

#ifdef LOCAL_KONTROL
#include "test_runner.h"
#include "profile.h"
#include <random>
#endif

using namespace std;

typedef long long ll;

namespace fs = std::experimental::filesystem;

typedef int hotel_data;

#define MAX_Q 1e5
#define MAX_ROOM_COUNT 1e3
#define MAX_CLIENT_ID 1e9

struct Event {
	string hotel;
	ll time;
	int user_id;
	int room_count;
};

struct HotelData {
	map<int, int> user_requests;
	int rooms;
};

class HotelManager {
public:
	HotelManager() {}

	void ProcessRequest(ll last_time, const string& hotel, int user_id, int room_count) {

		HotelData& new_hotel = _hotel_data[hotel];
		_events.push_front({ hotel,last_time,user_id,room_count });
		new_hotel.rooms += room_count;
		++new_hotel.user_requests[user_id];
		while (!_events.empty()) {
			auto el = _events.back();
			if (last_time - 86400 < el.time)
				break;
			auto& cur_hotel = _hotel_data[el.hotel];
			cur_hotel.rooms -= el.room_count;
			auto it2 = cur_hotel.user_requests.find(el.user_id);
			if (it2->second == 1) {
				cur_hotel.user_requests.erase(it2);
			}
			else {
				--it2->second;
			}
			_events.pop_back();
		}
	}

	int OccupiedRooms(const string& hotel) const {
		if (_hotel_data.count(hotel) == 0) {
			return 0;
		}

		return _hotel_data.at(hotel).rooms;
	}

	int UniqueUsers(const string& hotel)  const {
		if (_hotel_data.count(hotel) == 0) {
			return 0;
		}

		return _hotel_data.at(hotel).user_requests.size();
	}
private:
	list<Event> _events;
	unordered_map<string, HotelData> _hotel_data;
};

#ifdef LOCAL_KONTROL
void TestAll() {
	default_random_engine gen(random_device{}());
	uniform_int_distribution<long long> dist(numeric_limits<long long>::min(), numeric_limits<long long>::max());
	vector<long long> time_stamp(MAX_Q);
	for (int i = 0; i < MAX_Q; ++i) {
		time_stamp[i] = dist(gen);
	}
	sort(begin(time_stamp), end(time_stamp));

	//cout << time_stamp[0] << endl;
	//cout << time_stamp[MAX_Q / 2.] << endl;
	//cout << time_stamp[MAX_Q - 1] << endl;

	{
		LOG_DURATION("Stress test");

		HotelManager hm;

		string hotel_name;
		int client_id = 1;
		int room_count = MAX_ROOM_COUNT;

		for (int q = 0; q < MAX_Q - 1; ++q) {
			hotel_name = to_string(q);
			client_id = q;
			hm.ProcessRequest(time_stamp[q], hotel_name, client_id, room_count);
		}
		hm.ProcessRequest(numeric_limits<long long>::max(), hotel_name, client_id, room_count);
	}
}
#endif

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

#ifdef LOCAL_KONTROL
	TestAll();

	const fs::path ROOT = fs::path(__FILE__).parent_path();
	ifstream fis(ROOT / INPUT_FILENAME); cin.rdbuf(fis.rdbuf());
#endif
	int query_count;
	cin >> query_count;

	HotelManager hm;

	for (int query_id = 0; query_id < query_count; ++query_id) {
		string query_type;
		string hotel_name;
		cin >> query_type;

		if (query_type == "BOOK") {
			ll time;
			int client_id, room_count;
			cin >> time >> hotel_name >> client_id >> room_count;
			hm.ProcessRequest(time, hotel_name, client_id, room_count);
		}
		else if (query_type == "CLIENTS") {
			cin >> hotel_name;
			cout << hm.UniqueUsers(hotel_name) << endl;
		}
		else if (query_type == "ROOMS") {
			cin >> hotel_name;
			cout << hm.OccupiedRooms(hotel_name) << endl;
		}
	}

	return 0;
}