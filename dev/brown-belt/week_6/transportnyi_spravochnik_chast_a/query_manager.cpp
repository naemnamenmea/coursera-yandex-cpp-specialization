#include "requests.hpp"
#include "query_manager.hpp"
#include "auxiliary.hpp"
#include <string>
#include <sstream>
#include <cmath>

using namespace std;

namespace
{

}

double QueryManager::EARTH_R = 6371 * 1000;


double QueryManager::CalcDistance2(double x1, double y1, double x2, double y2)
{
  const double dy = y2 - y1;
  const double num = sqrt(pow(cos(x2) * sin(dy), 2) + pow(cos(x1) * sin(x2) - sin(x1) * cos(x2) * cos(dy), 2));
  const double denum = sin(x1) * sin(x2) + cos(x1) * cos(x2) * cos(dy);
  const double v = atan(num / denum);
  return EARTH_R * v;
}

double QueryManager::CalcDistance(double latitude1, double longitude1, double latitude2, double longitude2)
{
  return EARTH_R * std::acos(std::sin(latitude1) * std::sin(latitude2) + std::cos(latitude1) * std::cos(latitude2) * std::cos(longitude2 - longitude1));
}

void QueryManager::AddRoute(int busNumber, const std::vector<std::string_view>& route_, bool isCyclic)
{
  QueryManager::Route route;
  for_each(route_.begin(), route_.end(), [&](const string_view stopName) {
    auto [it, _] = m_stops->emplace(stopName, nullopt);

    route.push_back(it->first.c_str());
    });

  auto [g, _] = m_routes.try_emplace(busNumber, m_stops, std::move(route), isCyclic);
}

pair<QueryManager::Stops::iterator, bool> QueryManager::AddStop(const std::string_view stopName, optional<StopData> stopData)
{
  auto [it, isInsertionHappend] = m_stops->emplace(stopName, move(stopData));
  if (!isInsertionHappend && stopData.has_value())
  {
    it->second = move(stopData);
  }

  return { it, isInsertionHappend };
}

void QueryManager::ProcessQuery()
{
  string request_str;
  getline(m_is, request_str);

  if (auto request = ParseRequest(request_str)) {
    if (request->type == Request::Type::READ) {
      m_os << static_cast<const ReadRequest*>(request.get())->Process(*this)->ToString() << endl;
    }
    else if (request->type == Request::Type::READ_AND_MODIFY)
    {
      m_os << static_cast<const ModifyAndReadRequest*>(request.get())->Process(*this)->ToString() << endl;
    }
    else {
      static_cast<const ModifyRequest*>(request.get())->Process(*this);
    }
  }
}

void Run(std::istream& is, std::ostream& os)
{
  QueryManager queryManager(is, os);
  int queriesNumber = aux::ReadNumberOnLine<int>(is);

  for (int i = 0; i < queriesNumber; ++i)
  {
    queryManager.ProcessQuery();
  }

  queriesNumber = aux::ReadNumberOnLine<int>(is);

  for (int i = 0; i < queriesNumber; ++i)
  {
    queryManager.ProcessQuery();
  }
}
