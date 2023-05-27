#pragma once
#include "query_manager.hpp"
#include "auxiliary.hpp"
#include "math_constants.hpp"
#include <optional>
#include <string_view>
#include <algorithm>
#include <memory>
#include <unordered_set>

struct Request;
using RequestHolder = std::unique_ptr<Request>;

struct RequestReturnDataBase
{
  typedef std::unique_ptr<RequestReturnDataBase> Ptr;
  virtual std::string ToString() const = 0;
  virtual ~RequestReturnDataBase() = default;
protected:
  RequestReturnDataBase() {}
};

struct Request {
  enum class Type {
    READ,
    MODIFY,
    READ_AND_MODIFY
  };

  enum class QueryType
  {
    ADD_STOP,
    ADD_ROUTE,
    ROUTE_INFO
  };

  Request(Type type) : type(type) {}
  static RequestHolder Create(Request::QueryType type);
  virtual void ParseFrom(std::string_view input) = 0;
  virtual ~Request() = default;

  const Type type;
};

std::optional<Request::QueryType> ConvertRequestTypeFromString(std::string_view type_str, std::string_view request_str) {
  if (type_str == "Stop") return Request::QueryType::ADD_STOP;
  else if (type_str == "Bus") {
    const char* last = request_str.data() + request_str.size();
    unsigned int dummy{};
    auto [pos, ec] {std::from_chars(request_str.data(), last, dummy)};
    if (pos != last)
    {
      return Request::QueryType::ADD_ROUTE;
    }
    else
    {
      return Request::QueryType::ROUTE_INFO;
    }
  }
  else {
    return std::nullopt;
  }
}

RequestHolder ParseRequest(std::string_view request_str) {
  const std::string_view type_str = aux::ReadToken(request_str);
  const auto request_type = ConvertRequestTypeFromString(type_str, request_str);
  if (!request_type) {
    return nullptr;
  }
  RequestHolder request = Request::Create(*request_type);
  if (request) {
    request->ParseFrom(request_str);
  };
  return request;
}

struct ReadRequest : public Request {
  using Request::Request;
  ReadRequest() : Request(Request::Type::READ) {}
  virtual RequestReturnDataBase::Ptr Process(const QueryManager& manager) const = 0;
};

struct ModifyRequest : public Request {
  using Request::Request;
  ModifyRequest() : Request(Request::Type::MODIFY) {}
  virtual void Process(QueryManager& manager) const = 0;
};

struct ModifyAndReadRequest : public Request {
  using Request::Request;
  ModifyAndReadRequest() : Request(Request::Type::READ_AND_MODIFY) {}
  virtual RequestReturnDataBase::Ptr Process(QueryManager& manager) const = 0;
};

class GetRouteInfoRequest : public ModifyAndReadRequest
{
public:
  GetRouteInfoRequest() : m_RouteNumber(-1) {}

  struct RouteInfoData : public RequestReturnDataBase
  {
    RouteInfoData() : m_RouteNumber(-1) {}

    struct Data
    {
      size_t m_stopsOnRoute;
      size_t m_uniqueStops;
      double m_routeLength;
    };

    std::string ToString() const override
    {
      std::ostringstream os;
      os << "Bus " << m_RouteNumber << ": ";
      if (m_data.has_value())
      {
        os << m_data->m_stopsOnRoute << " stops on route, " << m_data->m_uniqueStops << " unique stops, " << m_data->m_routeLength << " route length";
      }
      else
      {
        os << "not found";
      }
      return os.str();
    }

    int m_RouteNumber;
    std::optional<Data> m_data;
  };
  void ParseFrom(std::string_view input) override
  {
    m_RouteNumber = aux::ConvertToNumber<int>(aux::ReadToken(input));
  }

  RequestReturnDataBase::Ptr Process(QueryManager& manager) const override
  {
    std::unique_ptr<RouteInfoData> res = std::make_unique<RouteInfoData>();
    res->m_RouteNumber = m_RouteNumber;
    auto routeData = manager.GetRouteData(m_RouteNumber);
    if (routeData.has_value())
    {
      RouteInfoData::Data data;
      data.m_routeLength = routeData->get().GetRouteLength();
      data.m_stopsOnRoute = routeData->get().GetStopsCount();
      data.m_uniqueStops = routeData->get().UniqueStopsNumber();
      res->m_data = std::move(data);
    }

    return res;
  }

  int m_RouteNumber;
};

class AddRouteRequest : public ModifyRequest
{
public:
  AddRouteRequest() : m_busNumber(-1), m_isCyclic{} {}

  struct RouteInfoData
  {

  };

  void ParseFrom(std::string_view input) override
  {
    std::string_view busNumberSV = aux::ReadToken(input, ": ");
    m_busNumber = aux::ConvertToNumber<int>(busNumberSV);

    m_isCyclic = input.at(input.find_first_of(">-")) == '>';

    while (!input.empty())
    {
      std::string_view stop = aux::ReadToken(input, m_isCyclic ? " > " : " - ");
      m_route.push_back(stop);
    }
  }

  void Process(QueryManager& manager) const override
  {
    manager.AddRoute(m_busNumber, m_route, m_isCyclic);
  }

private:
  int m_busNumber;
  std::vector<std::string_view> m_route;
  bool m_isCyclic;
};

class AddStopRequest : public ModifyRequest
{
public:
  AddStopRequest() : m_latitudeInDeg(-1), m_longitudeInDeg(-1) {}
  struct RouteInfoData
  {

  };

  void ParseFrom(std::string_view input) override
  {
    m_stopName = aux::ReadToken(input, ": ");

    m_latitudeInDeg = aux::ConvertToNumber<double>(aux::ReadToken(input, ", "));
    m_longitudeInDeg = aux::ConvertToNumber<double>(aux::ReadToken(input));
  }

  void Process(QueryManager& manager) const override
  {
    QueryManager::Coord coord(m_latitudeInDeg * mathdef::D2R, m_longitudeInDeg * mathdef::D2R);
    manager.AddStop(m_stopName, std::move(coord));
  }

private:
  std::string_view m_stopName;
  double m_latitudeInDeg;
  double m_longitudeInDeg;
};

RequestHolder Request::Create(Request::QueryType type) {
  switch (type) {
  case Request::QueryType::ADD_ROUTE:
    return std::make_unique<AddRouteRequest>();
  case Request::QueryType::ADD_STOP:
    return std::make_unique<AddStopRequest>();
  case Request::QueryType::ROUTE_INFO:
    return std::make_unique<GetRouteInfoRequest>();
  default:
    return nullptr;
  }
}