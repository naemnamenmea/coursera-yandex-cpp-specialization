#include "query_manager.hpp"
#include "tests.hpp"
#include <ctime>
#include <string>
#include <map>
#include <tuple>
#include <stdexcept>
#include <fstream>
#include <string_view>
#include <sstream>
#include <iostream>
#include <iomanip>

int main()
{
#ifdef LOCAL_LAUNCH
  RunAllTests();
#endif

  Date lowerLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 1999 });
  Date upperLimit = Date(Day{ 1 }, Month{ 1 }, Year{ 2100 });
  QueryManager queryManager(std::cout, lowerLimit, upperLimit);
  int queriesNumber;

  std::cin >> queriesNumber;

  for (size_t q = 0; q < queriesNumber; ++q)
  {
    queryManager.ProcessQuery(std::cin);
  }

  return 0;
}
