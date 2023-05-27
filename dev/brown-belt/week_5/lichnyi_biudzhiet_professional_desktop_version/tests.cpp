#include "tests.hpp"
#include "query_manager.hpp"
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
  vector<double> ReadOutput(istream& is)
  {
    vector<double> res;
    string str;

    while (getline(is, str))
    {
      res.push_back(stod(str));
    }

    return res;
  }

  void CompareRes(const vector<double>& res, const vector<double>& expected)
  {
    ASSERT(res.size() == expected.size());

    for (size_t i = 0; i < res.size(); ++i)
    {
      ASSERT(mathdef::is_eq(res[i], expected[i]));
    }
  }
}

void BasicTest()
{
  const fs::path dataDir = GetTestPath() / "BasicTest";
  const fs::path inputFilename1 = dataDir / "test1_in.txt";
  ifstream ifs(inputFilename1);
  if (!ifs.is_open())
  {
    throw runtime_error(string("file \"") + inputFilename1.string() + "\" not found");
  }

  stringstream ss;
  Date lowerLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 1999 });
  Date upperLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 2100 });
  QueryManager queryManager(ss, lowerLimit, upperLimit);
  int queriesNumber;

  ifs >> queriesNumber;

  for (size_t q = 0; q < queriesNumber; ++q)
  {
    queryManager.ProcessQuery(ifs);
  }

  const fs::path inputFilename2 = dataDir / "test1_out.txt";
  ifstream ifs_expected(inputFilename2);
  if (!ifs_expected.is_open())
  {
    throw runtime_error(string("file \"") + inputFilename2.string() + "\" not found");
  }
  stringstream expectedSS;
  expectedSS << ifs_expected.rdbuf();

  auto res = ReadOutput(ss);
  auto expected = ReadOutput(expectedSS);
  CompareRes(res, expected);
}
