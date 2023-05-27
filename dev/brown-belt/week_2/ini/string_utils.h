#pragma once

#include <string>
#include <string_view>
#include <algorithm>

std::string_view TrimLeft(std::string_view str, const std::string& delimiters)
{
  while (!str.empty() && std::find_first_of(str.begin(), str.end(), delimiters.begin(), delimiters.end()) != str.end())
  {
    str.remove_prefix(1);
  }

  return str;
}

std::string_view TrimRight(std::string_view str, const std::string& delimiters)
{
  while (!str.empty() && std::find_first_of(str.rbegin(), str.rend(), delimiters.begin(), delimiters.end()) != str.rend())
  {
    str.remove_suffix(1);
  }

  return str;
}

std::string_view Trim(std::string_view str, const std::string& delimiters)
{
  TrimLeft(str, delimiters);
  TrimRight(str, delimiters);

  return str;
}
