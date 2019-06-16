#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <map>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <experimental/filesystem>

using namespace std;

typedef long long ll;

namespace fs = std::experimental::filesystem;

typedef int hotel_data;

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

	void ProcessRequest(ll last_time, string hotel, int user_id, int room_count) {
		HotelData& new_hotel = _hotel_data[hotel];
		_events.push_front({ hotel,last_time,user_id,room_count });
		new_hotel.rooms += room_count;
		++new_hotel.user_requests[user_id];
		auto it = _events.begin();
		while (it != _events.end()) {
			auto it_next = next(it);
			auto& cur_hotel = _hotel_data[it->hotel];
			if (last_time - 86400 >= it->time) {
				cur_hotel.rooms -= it->room_count;
				auto it2 = cur_hotel.user_requests.find(it->user_id);
				if (it2->second == 1) {
					cur_hotel.user_requests.erase(it2);
				}
				else {
					--it2->second;
				}
			}

			it = it_next;
		}
	}

	int OccupiedRooms(string hotel) const {
		if (_hotel_data.count(hotel) == 0) {
			return 0;
		}

		return _hotel_data.at(hotel).rooms;
	}

	int UniqueUsers(string hotel)  const {
		if (_hotel_data.count(hotel) == 0) {
			return 0;
		}

		return _hotel_data.at(hotel).user_requests.size();
	}
private:
	deque<Event> _events;
	unordered_map<string, HotelData> _hotel_data;
};

int main() {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	const fs::path ROOT = fs::path(__FILE__).parent_path();
	ifstream fis(ROOT / INPUT_FILENAME); cin.rdbuf(fis.rdbuf());

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