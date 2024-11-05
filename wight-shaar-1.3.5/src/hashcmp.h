// -*-c++-*-
#ifndef HASHCMP_H
#define HASHCMP_H

#include <ext/hash_map>

//NOTE! This is NOT my own code!

/*
struct eqstr {
  bool operator() (const char* s1, const char* s2) const {
    return strcmp(s1, s2) == 0;
  }
};
*/

struct eqstr {
  bool operator() (string s1, string s2) const {
    return s1==s2;
  }
};

namespace __gnu_cxx
{
  template<> struct hash< std::string >
  {
    size_t operator()( const std::string& x ) const
    {
      return hash< const char* >()( x.c_str() );
    }
  };
}

#endif
