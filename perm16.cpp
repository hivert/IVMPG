#include "perm16.hpp"
#include <iostream>

// Definition since previously *only* declared
const constexpr size_t Vect16::N;

std::ostream & operator<<(std::ostream & stream, Vect16 const &term) {
  stream << "[" << unsigned(term[0]);
  for (unsigned i=1; i < 16; i++) stream << "," << unsigned(term[i]);
  stream << "]";
  return stream;
}
