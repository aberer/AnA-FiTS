/**
 * \file MPFRTest.cpp
 * \brief OBSOLETE: Interface illustration for RandomMPFR.hpp
 *
 * Compile, link, and run with
 * g++ -O2 -g -o MPFRTest MPFRTest.cpp -lmpfr -lgmp
 * ./MPFRTest
 *
 * $Id$
 *
 * Copyright (c) Charles Karney (2012) <charles@karney.com> and licensed under
 * the MIT/X11 License.  For more information, see
 * http://randomlib.sourceforge.net/
 **********************************************************************/

#include <cstdio>
#include <iostream>
#include <ctime>                // for time()
#include "RandomMPFR.hpp"

int main() {
  mpfr_prec_t prec = 24;
  mpfr_rnd_t round = MPFR_RNDN;
  int count = 10, base = 16;
  gmp_randstate_t r;
  gmp_randinit_mt(r);
  gmp_randseed_ui(r, std::time(0));
  mpfr_t z;
  mpfr_init2(z, prec);
  {
    std::cout << "Sample from uniform distribution...\n";
    UnitUniform dist;
    for (int i = 0; i < count; ++i) {
      dist(z, r, round);
      mpfr_out_str(stdout, base, 0, z, round);
      std::cout << "\n";
    }
  }
  {
    std::cout << "Sample from exponential distribution...\n";
    UnitExponential dist;
    for (int i = 0; i < count; ++i) {
      dist(z, r, round);
      mpfr_out_str(stdout, base, 0, z, round);
      std::cout << "\n";
    }
  }
  {
    std::cout << "Sample from normal distribution...\n";
    UnitNormal dist;
    for (int i = 0; i < count; ++i) {
      dist(z, r, round);
      mpfr_out_str(NULL, base, 0, z, round);
      std::cout << "\n";
    }
  }
  mpfr_clear(z);
  gmp_randclear(r);
}
