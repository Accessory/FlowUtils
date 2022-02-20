#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace FlowVParser {
using namespace std;

inline std::vector<unsigned char>::iterator
findLastData(std::vector<unsigned char>::iterator pos) {
  --pos;
  if (*pos == '-')
    --pos;
  if (*pos == '-')
    --pos;
  if (*pos == '\n')
    --pos;
  if (*pos == '\r')
    --pos;
  ++pos;

  return pos;
}

inline std::vector<unsigned char>::iterator
findLastData(std::vector<unsigned char>::iterator pos,
             const std::vector<unsigned char>::iterator begin) {
  if (begin == pos)
    return pos;
  --pos;
  if (*pos == '-') {
    if (begin == pos)
      return pos;
    --pos;
  }
  if (*pos == '-') {
    if (begin == pos)
      return pos;
    --pos;
  }
  if (*pos == '\n') {
    if (begin == pos)
      return pos;
    --pos;
  }
  if (*pos == '\r') {
    if (begin == pos)
      return pos;
    --pos;
  }
  ++pos;

  return pos;
}

inline std::vector<unsigned char>::iterator
find_first(const std::vector<unsigned char> &data, const char &c,
           std::vector<unsigned char>::iterator pos) {
  while (pos != data.end()) {
    if (*pos == c)
      return pos;
    ++pos;
  }
  return pos;
}

inline std::vector<unsigned char>::iterator
find_first_of(const std::vector<unsigned char> &data, const std::string &oneOf,
              std::vector<unsigned char>::iterator pos) {
  while (pos != data.end()) {
    for (char c : oneOf) {
      if (*pos == c)
        return pos;
    }
    ++pos;
  }
  return pos;
}

inline std::string goToNextLine(const std::vector<unsigned char> &data,
                                std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  if (*pos == '\n' || (*pos == '\r' && *(++pos) == '\n'))
    ++pos;
  return std::string(start, pos);
}

inline std::vector<unsigned char>::iterator
find_first_not_of(const std::vector<unsigned char> &data,
                  const std::string &notOf,
                  std::vector<unsigned char>::iterator pos) {
  while (pos != data.end()) {
    bool notOneOf = true;
    for (char c : notOf) {
      if (*pos == c) {
        notOneOf = false;
        break;
      }
    }
    if (notOneOf)
      return pos;

    ++pos;
  }
  return pos;
}

inline std::string goTo(std::vector<unsigned char> &data,
                        const std::string &toGoTo,
                        std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  pos = search(pos, data.end(), toGoTo.begin(), toGoTo.end());
  return std::string(start, pos);
}

inline std::string gotoNextNonWhite(const std::vector<unsigned char> &data,
                                    std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  pos = find_first_not_of(data, " \n\r", pos);
  return std::string(start, pos);
}

inline std::string gotoNextNonAlpha(const std::vector<unsigned char> &data,
                                    std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  pos = find_first_of(data, " \n\r", pos);
  return std::string(start, pos);
}

inline std::string goToOne(const std::vector<unsigned char> &data,
                           const std::string &goToOne,
                           std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  pos = find_first_of(data, goToOne, pos);
  return std::string(start, pos);
}

inline bool isOneOf(const std::vector<unsigned char>::iterator &pos,
                    const std::vector<std::string> &toCheck) {
  for (const auto &check : toCheck) {
    auto cpos = pos;
    for (size_t i = 0; i <= check.size(); ++i) {
      if (i == check.size()) {
        return true;
      }
      if (*cpos != check.at(i)) {
        break;
      }
      ++cpos;
    }
  }
  return false;
}

inline bool isDoubleNewLine(std::vector<unsigned char>::iterator &pos) {
  return (*pos == '\n' && *(++pos) == '\n') ||
         (*pos == '\r' && *(++pos) == '\n' && *(++pos) == '\r' &&
          *(++pos) == '\n');
}

inline std::string goToEnd(const std::vector<unsigned char> &data,
                           size_t &pos) {
  size_t start = pos;
  pos = std::string::npos;
  return std::string(start, pos);
}

inline std::string goToPrevLine(const std::vector<unsigned char> &data,
                                std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  if (*(--pos) == '\n' && *(--pos) == '\r')
    --pos;
  return std::string(pos, start);
}

inline std::string goToNewLine(const std::vector<unsigned char> &data,
                               std::vector<unsigned char>::iterator &pos) {
  auto start = pos;
  pos = find_first_of(data, "\n\r", pos);
  return std::string(start, pos);
}

inline bool containsAt(const std::vector<unsigned char> &data,
                      const std::string &toCheck,
                      std::vector<unsigned char>::iterator &pos) {
  if (pos + toCheck.size() > data.end())
    return false;
  size_t i = 0;
  for (pos; i < toCheck.size() && pos != data.end(); ++pos) {
    if (*pos != toCheck.at(i++))
      return false;
  }
  return true;
}

}; // namespace FlowVParser
