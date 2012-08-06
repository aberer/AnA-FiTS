#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>            // for min/max
#include <ctime>
#include <cmath>                // pow

#include <RandomLib/MPFRRandom.hpp>
#include <RandomLib/MPFRExponential.hpp>

int main(int argc, char* argv[]) {
  static const int bits = 1;
  if (argc != 4) {
    std::cerr << "Usage: MPFRExample (u|d|n) prec count\n"
              << "to test MPRF{Uniform,Exponential,Normal{,S}}\n";
    return 1;
  }
  mpfr_rnd_t round = MPFR_RNDN;
  mpfr_prec_t prec = 53;
  long long count = 1000000;
  int emin = -8;
  int emax = 3;
  {
    std::string arg;
    arg = std::string(argv[1]);
    if (arg.length() > 0) {
      if (arg[0] == 'u')
        round = MPFR_RNDU;
      else if (arg[0] == 'd')
        round = MPFR_RNDD;
      else if (arg[0] == 'n')
        round = MPFR_RNDN;
      else if (arg[0] == 'z')
        round = MPFR_RNDZ;
      else if (arg[0] == 'a')
        round = MPFR_RNDA;
      // default to N
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
  gmp_randstate_t r;
  gmp_randinit_mt(r);
  time_t t0 = std::time(0);
  gmp_randseed_ui(r, t0);
  mpfr_t z;
  mpfr_init2(z, prec);
  RandomLib::MPFRExponential<bits> edist;
  RandomLib::MPFRRandom<bits> n;

  static const int nbins = (1 << (prec-1)) * (emax - emin + 1) + 2;
  std::vector<long long> bins(nbins, 0);
  std::vector<long long> flags(3, 0);
  std::vector<double> p(nbins);
  {
    // numbers are of form 2^e * m*2^(-prec) with e in in [emin, emax] and m is
    // in [2^(prec-1), 2^prec).
    double l, u;
    l = 0;
    switch (round) {
    case MPFR_RNDD:
    case MPFR_RNDZ:
      u = std::pow(2.0, emin - 1);
      break;
    case MPFR_RNDU:
    case MPFR_RNDA:
      u = std::pow(2.0, emin - 1) * (1 - std::pow(0.5, prec));
      break;
    case MPFR_RNDN:
    case MPFR_RNDF:
    case MPFR_RNDNA:
      u = std::pow(2.0, emin - 1) * (1 - std::pow(0.5, prec)/2);
      break;
    }
    int i = 0;
    // std::cout << i << " " << l << " " << u << "\n";
    p[i] = std::exp(-l) - std::exp(-u);
    for (int e = emin; e <= emax; ++e)
      for (int m = (1 << (prec-1)); m < (1 << prec); ++m) {
        ++i;
        l = u;
        switch (round) {
        case MPFR_RNDD:
        case MPFR_RNDZ:
          u = std::pow(2.0, e) * (m + 1) * std::pow(0.5, prec);
          break;
        case MPFR_RNDU:
        case MPFR_RNDA:
          u = std::pow(2.0, e) * (m + 0) * std::pow(0.5, prec);
          break;
        case MPFR_RNDN:
        case MPFR_RNDF:
        case MPFR_RNDNA:
          u = std::pow(2.0, e) * (m + 0.5) * std::pow(0.5, prec);
          break;
        }
        // std::cout << i << " " << l << " " << u << "\n";
        p[i] = std::exp(-l) - std::exp(-u);
      }
    ++i;
    l = u;
    // std::cout << i << " " << l << " inf\n";
    p[i] = std::exp(-l);
  }
  double x;
  int f, e, m;
  for (long long i = 0; i < count; ++i) {
     f = edist(z, r, round);
     x = mpfr_get_d(z, round);
     m = int((std::frexp(x, &e) - 0.5) * std::pow(2.0, prec));
     int i = m + (e - emin) * (1 << (prec-1));
     i = std::max(0, std::min(nbins - 1, i + 1));
     ++bins[i];
     ++flags[f+1];
  }
  double v = 0;
  for (int i = 0; i < nbins; ++i) {
    double x = bins[i] - count * p[i];
    v += x * x / (count * p[i]);
  }
  std::cout << "degree = " << nbins - 1 << " V = " << v << " flags = ["
            << flags[0] << "," << flags[1] << "," << flags[2] << "]\n";
  return 0;

  for (long long i = 0; i < count; ++i) {
    edist(n, r);
    f = n(z, MPFR_RNDD);
    x = mpfr_get_d(z, round);
    std::cout << "[" << x << "(" << f << ")";
    f = n(z, MPFR_RNDN);
    x = mpfr_get_d(z, round);
    std::cout << "," << x << "(" << f << ")";
    f = n(z, MPFR_RNDU);
    x = mpfr_get_d(z, round);
    std::cout << "," << x << "(" << f << ")";
    f = n(z, r, round);
    x = mpfr_get_d(z, round);
    std::cout << "] = " << x << "(" << f << ")\n";
  }
  // Clean up
  mpfr_clear(z);
  mpfr_free_cache();
  gmp_randclear(r);
}
