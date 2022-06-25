#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>

void redirect(std::istream &is, std::stringstream &os) {
    is >> std::noskipws;
    std::istream_iterator<char> begin(is);
    std::istream_iterator<char> end;
    std::ostream_iterator<char> out(os);
    std::copy(begin, end, out);
}
