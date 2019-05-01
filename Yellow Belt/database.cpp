#include "database.h"

void Database::Add(const Date& date, const string& event) {
	auto it = this->db_.equal_range(date);
	bool flag = true;
	for (auto it2 = it.first; it2 != it.second; ++it2) {
		if (it2->second == event) {
			flag = false;
			break;
		}
	}
	if (flag) {
		this->db_.insert({ date,event });
	}
}

void Database::Print(ostream& os) const {
	for (const auto& record : this->db_) {
		os << record.first << ' ' << record.second << endl;
	}
}

string Database::Last(const Date& date) const
{
	auto it = this->db_.upper_bound(date);
	if (it == begin(this->db_)) {
		throw invalid_argument("");
	}
	auto it2 = prev(it);
	ostringstream os;
	os << it2->first << ' ' << it2->second;
	return os.str();
}

vector<pair<Date, string>> Database::FindIf(const Predicate& predicate) const
{
	vector<pair<Date, string>> res;
	copy_if(begin(this->db_), end(this->db_), back_inserter(res),
		[&](const pair<Date, string> & p) {return predicate(p.first, p.second); });
	return res;
}

int Database::RemoveIf(const Predicate& predicate)
{
	int res = 0;
	for (auto it = begin(this->db_); it != end(this->db_);) {
		if (predicate(it->first, it->second)) {
			it = this->db_.erase(it);
			++res;
		}
		else {
			++it;
		}
	}
	return res;
}
