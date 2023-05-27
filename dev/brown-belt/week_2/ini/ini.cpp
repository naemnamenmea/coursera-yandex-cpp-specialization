#include "ini.h"
#include "string_utils.h"
#include <istream>
#include <stdexcept>

namespace Ini
{
  Section& Ini::Document::AddSection(string name)
  {
    return sections[move(name)];
  }

  Section& Document::GetSection(const string& name)
  {
    return sections.at(name);
  }

  const Section& Document::GetSection(const string& name) const
  {
    return sections.at(name);
  }

  size_t Document::SectionCount() const
  {
    return sections.size();
  }

  Document Load(istream& input)
  {
    Document res;
    string line;
    std::string sectionName;

    while (getline(input, line))
    {
      line = Trim(line, " ");

      if (line.empty())
        continue;

      if (line.size() < 3)
      {
        throw std::invalid_argument("Wrong section name");
      }

      if (line[0] == '[' && line[line.size() - 1] == ']')
      {
        sectionName = line.substr(1, line.size() - 2);
        res.AddSection(sectionName);
        continue;
      }

      if (sectionName.empty())
        continue;

      size_t equalSignPos = line.find('=');

      string key = line.substr(0, equalSignPos);
      string value = line.substr(equalSignPos + 1);

      res.GetSection(sectionName).emplace(move(key), move(value));
    }

    return res;
  }
}
