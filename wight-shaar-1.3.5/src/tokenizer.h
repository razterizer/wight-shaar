// -*-c++-*-
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

class Tokenizer {
  const char delim;
  unsigned i, pos1, pos2;
  std::string curr, rest;
public:
  Tokenizer(const std::string& str, const char delim=' ')
    : delim(delim), rest(str) {}
  std::string next(void) {
    for (i=0;i<rest.length();i++)
      if (rest[i]!=delim) break;
    pos1 = i;
    for (i=pos1;i<rest.length();i++)
      if (rest[i]==delim) break;
    pos2 = i;
    curr = rest.substr(pos1, pos2-pos1);
    rest = rest.substr(pos2, rest.length());
    return curr;
  }
  bool end(void) { return !rest.length();}
};


#endif
