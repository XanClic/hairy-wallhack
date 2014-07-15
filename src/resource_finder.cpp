#include <cstdio>
#include <stdexcept>

#include "resource_finder.hpp"


std::string find_resource_file(const std::string &basename)
{
  std::string fname("res/" + basename);

  for (int i = 0; i < 5; i++) {
    // could've used access(), but that probably won't work on Windows
    FILE *fp = fopen(fname.c_str(), "rb");
    if (fp) {
      fclose(fp);
      return fname;
    }

    fname = "../" + fname;
  }

  throw std::invalid_argument("Could not find resource " + basename);
}
