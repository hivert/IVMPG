#include "perm16.hpp"
#include <iostream>

std::ostream & operator<<(std::ostream & stream, Perm16 const &term) {
  stream << "[" << unsigned(term[0]);
  for (unsigned i=1; i < 16; i++) stream << "," << unsigned(term[i]);
  stream << "]";
  return stream;
}
