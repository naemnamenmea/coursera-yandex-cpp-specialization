#include "query_manager.hpp"
#include "tests.hpp"
#include "auxiliary.hpp"
#include "test_runner.h"
#include "stl_containers_write.hpp"
#include "math_constants.hpp"
#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

const fs::path GetTestPath()
{
  return fs::path("tests_data");
}

void RunAllTests()
{
  TestRunner tr;
  RUN_TEST(tr, BasicTest);
}

namespace
{
  const double CHECK_TOLERANCE = 1e-4;

  struct BusQueryData
  {
    unsigned int busNumber;
    unsigned int stopsOnRoute;
    unsigned int uniqueStops;
    double routeLength;
  };

  optional<BusQueryData> ReadAndParseBusQuery(string_view sv) {
    optional<BusQueryData> res = make_optional<BusQueryData>();
    aux::ReadToken(sv);
    string_view numStr = aux::ReadToken(sv, ": ");
    res->busNumber = aux::ConvertToNumber<unsigned int>(numStr);

    try
    {
      res->stopsOnRoute = aux::ConvertToNumber<int>(aux::ReadToken(sv));
      aux::ReadToken(sv);
      aux::ReadToken(sv);
      aux::ReadToken(sv);

      res->uniqueStops = aux::ConvertToNumber<int>(aux::ReadToken(sv));
      aux::ReadToken(sv);
      aux::ReadToken(sv);
      res->routeLength = aux::ConvertToNumber<double>(aux::ReadToken(sv));
      return res;
    }
    catch (...)
    {
      return {};
    }
  }

  void CompareRes(const optional<BusQueryData>& result, const optional < BusQueryData>& expected) {
    ASSERT(bool(result) == bool(expected));

    if (!result && !expected)
      return;

    ASSERT_EQUAL(result->busNumber, expected->busNumber);
    ASSERT_EQUAL(result->stopsOnRoute, expected->stopsOnRoute);
    ASSERT_EQUAL(result->uniqueStops, expected->uniqueStops);
    ASSERT(mathdef::is_eq(result->routeLength, expected->routeLength, CHECK_TOLERANCE));
  }
}

void BasicTest()
{
  const fs::path dataDir = GetTestPath();
  const fs::path inputFilename1 = dataDir / "test1_in.txt";
  ifstream ifs(inputFilename1, ios::binary);
  if (!ifs.is_open())
  {
    throw runtime_error(string("file \"") + inputFilename1.string() + "\" not found");
  }

  stringstream oss;
  Run(ifs, oss);

  const fs::path inputFilename2 = dataDir / "test1_out.txt";
  ifstream ifs_expected(inputFilename2);
  if (!ifs_expected.is_open())
  {
    throw runtime_error(string("file \"") + inputFilename2.string() + "\" not found");
  }

  string outputStr;
  string expectedStr;
  while(getline(oss, outputStr) && getline(ifs_expected, expectedStr)) {
    auto res = ReadAndParseBusQuery(outputStr);
    auto expected = ReadAndParseBusQuery(expectedStr);
    CompareRes(res, expected);
  }
}
