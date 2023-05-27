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
  std::vector<double> ReadOutput(std::istream& is)
  {
    std::vector<double> res;
    std::string str;

    while (std::getline(is, str))
    {
      res.push_back(std::stod(str));
    }

    return res;
  }

  void CompareRes(const std::vector<double>& res, const std::vector<double>& expected)
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
  std::ifstream ifs(inputFilename1);
  if (!ifs.is_open())
  {
    throw std::runtime_error(std::string("file \"") + inputFilename1.string() + "\" not found");
  }

  std::stringstream ss;
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
  std::ifstream ifs_expected(inputFilename2);
  if (!ifs_expected.is_open())
  {
    throw std::runtime_error(std::string("file \"") + inputFilename2.string() + "\" not found");
  }
  std::stringstream expectedSS;
  expectedSS << ifs_expected.rdbuf();

  auto res = ReadOutput(ss);
  auto expected = ReadOutput(expectedSS);
  CompareRes(res, expected);
}
