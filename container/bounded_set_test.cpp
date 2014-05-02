#include <iostream>
#include <utility>
#include "bounded_set.hpp"

using namespace std;


int main() {
  bounded_set<int> s, t;

  cout << s.size() << " : " << s << endl;
  s.insert(4);
  cout << s.size() << " : " << s << endl;
  s.insert(4);
  cout << s.size() << " : " << s << endl;
  s.insert(2348);
  cout << s.size() << " : " << s << endl;
  s.insert(22348);
  cout << s.size() << " : " << s << endl;
  s.insert(12322348);
  cout << s.size() << " : " << s << endl;
  s.clear();
  cout << s.size() << " : " << s << endl;
  s.insert(22348);
  cout << s.size() << " : " << s << endl;
  s.insert(12322348);
  cout << s.size() << " : " << s << endl;
  cout << "Before Swap " << endl;
  cout << "s=" << s.size() << " : " << s << endl;
  cout << "t=" << t.size() << " : " << t << endl;
  std::swap(s, t);
  cout << "After Swap " << endl;
  cout << "s=" << s.size() << " : " << s << endl;
  cout << "t=" << t.size() << " : " << t << endl;
}
