/**
 * \file MPFRCheck.cpp
 * \brief OBSOLETE: Chi-squared check for RandomMPFR.hpp
 *
 * Compile, link, and run with, e.g.,
 * g++ -O2 -g -o MPFRCheck MPFRCheck.cpp -lmpfr -lgmp
 * ./MPFRCheck normal 53 1000000
 *
 * $Id$
 *
 * Copyright (c) Charles Karney (2012) <charles@karney.com> and licensed under
 * the MIT/X11 License.  For more information, see
 * http://randomlib.sourceforge.net/
 **********************************************************************/

#include <iostream>
#include <ctime>                // for time()
#include <cmath>                // for abs
#include <algorithm>            // for max/min
#include <vector>
#include <string>
#include <sstream>
#include "RandomMPFR.hpp"

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: MPFRCheck (uniform|exponential|normal) prec count\n";
    return 1;
  }
  std::string dist("normal");
  mpfr_rnd_t round = MPFR_RNDN;
  mpfr_prec_t prec = 53;
  long long count = 1000000;
  {
    std::string arg;
    arg = std::string(argv[1]);
    if (arg.length() > 0) {
      if (arg[0] == 'u')
        dist = "uniform";
      else if (arg[0] == 'e')
        dist = "exponential";
      else if (arg[0] == 'g')
        dist = "grandom";
      // default to normal
    }
    arg = std::string(argv[2]);
    {
      std::istringstream str(arg);
      str >> prec;
      if (!(prec >= MPFR_PREC_MIN && prec <= MPFR_PREC_MAX))
        prec = 53;
    }
    arg = std::string(argv[3]);
    {
      std::istringstream str(arg);
      str >> count;
      if (!(count > 10)) count = 10;
    }
  }
  static const int nbins = 51;
  std::vector<long long> bins(nbins, 0);
  gmp_randstate_t r;
  gmp_randinit_mt(r);
  time_t t0 = std::time(0);
  gmp_randseed_ui(r, t0);
  mpfr_t z;
  mpfr_init2(z, prec);
  std::cout << "Chi-squared test:\n  dist = " << dist
            << ";\n  prec = " << prec
            << ";\n  count = " << count
            << ";\n  degrees of freedom = " << nbins - 1 << ".\n";
  double v = 0;
  if (dist == "uniform") {
    double binwidth = 1.0/nbins;
    UnitUniform dist;
    for (long long i = 0; i < count; ++i) {
      dist(z, r, round);
      double x = mpfr_get_d(z, round);
      ++bins[ std::min(nbins-1, std::max(0, int(std::floor(x/binwidth)))) ];
    }
    double p = binwidth;
    for (int i = 0; i < nbins; ++i) {
      double x = bins[i] - count * p;
      v += x * x / (count * p);
    }
  } else if (dist == "exponential") {
    double binwidth = 0.1;
    UnitExponential dist;
    for (long long i = 0; i < count; ++i) {
      dist(z, r, round);
      double x = mpfr_get_d(z, round);
      ++bins[ std::min(nbins-1, std::max(0, int(std::floor(x/binwidth)))) ];
    }
    double p[nbins];
    for (int i = 0; i < nbins - 1; ++i)
      p[i] = exp(- binwidth * i) - exp(- binwidth * (i + 1));
    p[nbins - 1] = exp(- binwidth * (nbins - 1));
    for (int i = 0; i < nbins; ++i) {
      double x = bins[i] - count * p[i];
      v += x * x / (count * p[i]);
    }
  }  else  {                      // normal
    double binwidth = 0.1;
    UnitNormal dist;
    for (long long i = 0; i < count; ++i) {
      dist(z, r, round);
      double x = std::abs(mpfr_get_d(z, round));
      ++bins[ std::min(nbins-1, std::max(0, int(std::floor(x/binwidth)))) ];
    }
    double p[nbins] = {
      7.96556745540579481e-02, 7.88637443241481106e-02, 7.73034254996992354e-02,
      7.50206388427429594e-02, 7.20814393273778720e-02, 6.85688419518266645e-02,
      6.45789310540012540e-02, 6.02165072793526823e-02, 5.55905464732742161e-02,
      5.08097428306049093e-02, 4.59783859701487496e-02, 4.11927814493489031e-02,
      3.65383712721957798e-02, 3.20876507036786107e-02, 2.78989159298258793e-02,
      2.40158191386001668e-02, 2.04676578820299493e-02, 1.72702872912344207e-02,
      1.44275185938480266e-02, 1.19328557356451759e-02, 9.77142277072531179e-03,
      7.92194609863590762e-03, 6.35867498364560760e-03, 5.05314819415935562e-03,
      3.97574119763997175e-03, 3.09695460411477108e-03, 2.38842844135616836e-03,
      1.82368694522547573e-03, 1.37863406008778598e-03, 1.03183053750788376e-03,
      7.64589636823479130e-04, 5.60930550605015857e-04, 4.07427591064141669e-04,
      2.92989753413793711e-04, 2.08600373282711302e-04, 1.47040977755982165e-04,
      1.02617713360291353e-04, 7.09033791045366715e-05, 4.85033998150344255e-05,
      3.28502043689655565e-05, 2.20274698411464552e-05, 1.46235157932807794e-05,
      9.61168708982905286e-06, 6.25472312657591533e-06, 4.02974156594759062e-06,
      2.57043684445443002e-06, 1.62329449717113033e-06, 1.01495860388337775e-06,
      6.28289750770543824e-07, 3.85063409422252025e-07, 5.73303143758389094e-07,
    };
    for (int i = 0; i < nbins; ++i) {
      double x = bins[i] - count * p[i];
      v += x * x / (count * p[i]);
    }
  }
  std::cout << "Results:\n  V = " << v
            << ";\n  time per sample = " << (std::time(0) - t0) * (1e6/count)
            << " us.\n";
  mpfr_clear(z);
  gmp_randclear(r);
}
