#include "query_manager.hpp"
#include "auxiliary.hpp"
#include "tests.hpp"
#include <iostream>

using namespace std;

int main()
{
#ifdef LOCAL_LAUNCH
  RunAllTests();
#endif

  Run();

  return 0;
}