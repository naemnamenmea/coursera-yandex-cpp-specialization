#pragma once
#include "iohelpder.hpp"
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <memory>
#include <functional>
#include <variant>
#include <optional>

class QueryManager
{
public:
  typedef std::list<const char*> Route;

  struct Coord
  {
    Coord(const double latitudeInRad_, const double longitudeInRad_) : latitudeInRad(latitudeInRad_), longitudeInRad(longitudeInRad_) {}

    double latitudeInRad;
    double longitudeInRad;
  };

  typedef Coord StopData;
  typedef std::unordered_map<std::string, std::optional<StopData>> Stops;

  class RouteData
  {
  public:
    RouteData(std::shared_ptr<const Stops> stops, Route route, bool isCyclic) : m_route(std::move(route)), m_stops(std::move(stops)) , m_isCyclic(isCyclic), m_isRouteChanged(true), m_routeLength(-1) {
      ObtainUniqueStops();
    }

    size_t UniqueStopsNumber() const { return m_unique_stops.size(); }

    const Route& GetRoute() const
    {
      return m_route;
    }

    double GetRouteLength()
    {
      if (m_isRouteChanged)
      {
        m_routeLength = CalcLength();
      }

      return m_routeLength;
    }

    size_t GetStopsCount() const
    {
      return m_isCyclic ? m_route.size() : m_route.size() * 2 - 1;
    }

  private:
    void ObtainUniqueStops()
    {
      m_unique_stops.clear();

      for (auto it = m_route.begin(); it != m_route.end(); ++it)
      {
        ++m_unique_stops[*it];
      }
    }

    double CalcLength() const
    {
      auto prev_ = m_route.begin();
      auto it = std::next(prev_);
      double res = 0;

      for (; it != m_route.end(); ++it) {
        const auto& from = m_stops->find(*prev_)->second;
        const auto& to = m_stops->find(*it)->second;

        if (!from.has_value() || !to.has_value())
          throw std::runtime_error("stop data was not set");

        res += QueryManager::CalcDistance(from->latitudeInRad, from->longitudeInRad, to->latitudeInRad, to->longitudeInRad);
        prev_ = it;
      }

      if (!m_isCyclic) res *= 2;

      return res;
    }

    const std::shared_ptr<const Stops> m_stops;
    bool m_isCyclic;
    std::unordered_map<const char*, unsigned int> m_unique_stops;
    Route m_route;

    bool m_isRouteChanged;
    // out data
    double m_routeLength;
  };

  QueryManager(std::istream& is = std::cin, std::ostream& os = std::cout) : m_is(is), m_os(os), m_stops(std::make_shared<Stops>()) {
    m_os << std::fixed << std::setprecision(6);
  }

  void ProcessQuery();

  void AddRoute(int busNumber, const std::vector<std::string_view>& route, bool isCyclic);
  std::pair<Stops::iterator, bool> AddStop(const std::string_view stopName, std::optional<StopData> stopData);

  std::optional<std::reference_wrapper<RouteData>> GetRouteData(int routeNumber)
  {
    auto it = m_routes.find(routeNumber);
    if (it == m_routes.end())
    {
      return std::nullopt;
    }
    else
    {
      return it->second;
    }
  }

private:
  static double EARTH_R;

  static double CalcDistance2(double latitude1, double longitude1, double latitude2, double longitude2);
  static double CalcDistance(double latitude1, double longitude1, double latitude2, double longitude2);

  std::shared_ptr<Stops> m_stops;
  std::unordered_map<size_t, RouteData> m_routes;

  std::istream& m_is;
  std::ostream& m_os;
};

void Run(std::istream& is = std::cin, std::ostream& os = std::cout);

