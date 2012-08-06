/**
 * \file RandomMPFR.hpp
 * \brief OBSOLETE: Random number distributions for MPFR
 *
 * Sample MPFR reals from a unit uniform, unit exponential, or unit normal
 * distribution.  These routines are transcriptions of the %RandomLib classes
 * RandomLib::RandomNumber, RandomLib::ExactExponential, and
 * RandomLib::ExactNormal.  The changes are
 * - Use GMP's mpz_urandomb as a source for random numbers.
 * - Accumulate random fraction in a GMP integer, mpz_t, instead of a
 *   std::vector of bytes.  This leads to a slight inefficiency at high
 *   precision, because adding the bytes to the mpz_t requires shifting the
 *   existing number to the left.  This results in a quadratic scaling; however
 *   this only becomes apparent when the precision is about 2^16.  It would be
 *   straightforward to eliminate this problem (and revert to ideal linear
 *   scaling) by writing the random bytes directly to a mpfr_t object.
 * - The output is written to a mpfr_t object.  The results then adher to the
 *   MPFR paradigm; they are equivalent to sampling exactly and then rounding
 *   according to the rounding mode to the a mpfr_t.
 *
 * Further notes:
 * - This is a header-only implementation.  Test programs are available in
 *   MPFRTest.cpp (a simple interface illustration) and MPFRCheck.cpp (a
 *   chi-squared and timing check).
 * - RandomFraction includes the machinery to generate the mpfr_t.  Rounding,
 *   underflow, and overflow are handled explicitly here.  Probably this can be
 *   converted to better code by letting MPFR do this.
 * - UnitUniform duplicates the functionality of mpfr_urandom.  It's included
 *   here to help exercise the generation of a mpfr_t by RandomFraction.
 * - UnitExponential uses von Neumann's algorithm for the exponential
 *   distribution with an optimization from RandomLib::ExactExponential
 *   version 1.4 (not released yet).  Von Neumann's algorithm samples a uniform
 *   deviate from [0,1) and accepts it with probability exp(-x); the
 *   exponential sample is then k + x, where k is the number of rejected
 *   samples.  The optimization samples a uniform deviate from [0,1), and
 *   accepts it with probability (x < 1/2 ? exp(-x) : 0); the exponential
 *   sample is then k/2 + x, where k is the number of rejected samples.  This
 *   saves 1.774 bits per sample.
 * - UnitNormal uses the algorithm from RandomLib::ExactNormal version 1.3
 *   (2012-01-21).  This more-or-less duplicates the functionality of
 *   mpfr_grandom, except that:
 *   - It generates one normal deviate per call instead of two.
 *   - It's not obvious (to me!) that mpfr_grandom produces results which are
 *     equivalent to sampling from a true normal and rounding to a
 *     representable number.  In particular, although mpfr_grandom does a
 *     rigorous rejection test for the two uniform deviates, it appears that's
 *     no guarantee that the resulting normal deviates are computed with
 *     sufficient precision to guarantee accuracy.  UnitNormal does yield
 *     accurate results (barring coding blunders).
 *   - UnitNormal is much faster than mpfr_grandom (even it's used to produce
 *     two results per call) for a wide range of precisions.  The following
 *     table gives the time per sample in microsecs on an Intel Xeon, x86_42,
 *     2.66GHz computer with g++ version 4.6.1 (-O3).  The columns are
 *     -# RandomLib::NormalDistribution (using the ratio method)
 *     -# RandomLib::ExactNormal (same algorithm as UnitNormal)
 *     -# UnitNormal
 *     -# UnitNormalRatio, the ratio method implemented in MPFR
 *     -# mpfr_grandom (MPFR version 3.1.0)
 *     .
\verbatim
    prec    i     ii     iii      iv     v     iii   iv
      8                  1.1     0.6     3
     24   0.034   0.5    1.1     0.6     3.4
     53   0.038   0.6    1.1     1       3.7
     64   0.042   0.63   1.1     1.4     4.0         1.1
    2^8                  1.4     4.2     8.3   1.2   1.7
    2^10                 2.2      18      27   1.4   3.8
    2^12                 6.2     110     160   2.2    24
    2^14                  32    1050    1550   5.4   208
    2^16                 290   13000   15000    18  1800
    2^18                                        76 14500
    2^20                                       290
\endverbatim
 *   (The last two columns are using faster versions of the algorithms which
 *   minimize bit shuffline.)  It's possible that this timing comparison is a
 *   little unfair to mpfr_grandom because mpfr_grandom initializes and clears
 *   the temporary mpz_t and mpfr_t variables it needs on each call, while
 *   UnitNormal keeps the temporaries it needs across calls.  I doubt this
 *   accounts for the timing differences, however.
 * - This file contains rather sparse documentation.  See the equivalent
 *   %RandomLib classes for more information.
 *
 * Copyright (c) Charles Karney (2012) <charles@karney.com> and licensed under
 * the MIT/X11 License.  For more information, see
 * http://randomlib.sourceforge.net/
 **********************************************************************/

#if !defined(RANDOMMPFR_HPP)
#define RANDOMMPFR_HPP "$Id$"

#include <algorithm>            // swap, max/min
#include <cmath>                // pow
#include <mpfr.h>

/// \cond SKIP

// An object containing a mpz integer f and an exponent e.  We have e >= 0 and
// 0 <= f < b^e, and b = 2^GMP_LIMB_BITS.  This represents the number x = f *
// b^-e, with x in [0, 1).
class RandomFraction {
  static const int bits_ = GMP_LIMB_BITS; // How many bits in a digit
  mutable mpz_t _tt;            // A temporary
  mpz_t _f;                     // The fraction
  mp_size_t _e;                 // Count of digits
  void AddDigit(gmp_randstate_t r) { // Add another digit
    mpz_mul_2exp(_f, _f, bits_);
    mpz_urandomb(_tt, r, bits_);
    mpz_add(_f, _f, _tt);
    ++_e;
  }
  void ExpandTo(gmp_randstate_t r, mp_size_t k)
  { while (_e <= k) AddDigit(r); }
  // return k'th limb counting k = 0 as most significant
  mp_limb_t Digit(gmp_randstate_t r, mp_size_t k) {
    ExpandTo(r, k);             // Now e > k
    return mpz_getlimbn(_f, _e - 1 - k);
  }
  // Return index [0..32] of highest bit set.  Return 0 if x = 0, 32 is if x =
  // ~0.  (From Algorithms for programmers by Joerg Arndt.)
  static int highest_bit_idx(unsigned x) {
    if (x == 0) return 0;
    int r = 1;
    if (x & 0xffff0000U) { x >>= 16; r += 16; }
    if (x & 0x0000ff00U) { x >>=  8; r +=  8; }
    if (x & 0x000000f0U) { x >>=  4; r +=  4; }
    if (x & 0x0000000cU) { x >>=  2; r +=  2; }
    if (x & 0x00000002U) {           r +=  1; }
    return r;
  }
public:
  RandomFraction() : _e(0u) { mpz_init(_f); mpz_init(_tt); }
  ~RandomFraction() { mpz_clear(_f); mpz_clear(_tt); }
  void Init() { mpz_set_ui(_f, 0u); _e = 0; }
  int LessThan(gmp_randstate_t r, RandomFraction& t) {
    for (mp_size_t k = 0; ; ++k) {
      mp_limb_t x = Digit(r, k);
      mp_limb_t y = t.Digit(r, k);
      if (x != y)
        return x < y;
    }
  }
  void swap(RandomFraction& t) {
    std::swap(_e, t._e);
    mpz_swap(_f, t._f);           // Don't swap _tt
  }
  void SetHighBit(gmp_randstate_t r) { // Set the msb to 1
    ExpandTo(r, 0);               // Generate msb if necessary
    mpz_setbit(_f, bits_ * _e  - 1);
  }
  int TestHighBit(gmp_randstate_t r) { // test the msb of f
    ExpandTo(r, 0);               // Generate msb if necessary
    return mpz_tstbit(_f, bits_ * _e  - 1);
  }
  // Position of leading bit 1/2 -> 0, 1/4 -> -1, etc.  If result is less than
  // bmin, then bmin may be returned.
  mp_size_t LeadingBit(gmp_randstate_t r,  mp_size_t bmin) {
    while (1) {
      int sgn = mpz_sgn(_f);
      if (sgn > 0)
        return mp_size_t(mpz_sizeinbase(_f, 2)) - mp_size_t(bits_ * _e);
      if (-mp_size_t(bits_ * _e) < bmin)
        return bmin;
      AddDigit(r);
    }
  }
  int operator()(gmp_randstate_t r, unsigned n, int s,
                 mpfr_t val, mpfr_rnd_t round) {
    // The value is constructed as a positive quantity, so adjust rounding mode
    // to account for this.
    switch (round) {
    case MPFR_RNDZ:
      round = s < 0 ? MPFR_RNDU : MPFR_RNDD;
      break;
    case MPFR_RNDA:
      round = s < 0 ? MPFR_RNDD : MPFR_RNDU;
      break;
    default:
      break;
    } // Now round is one of MPFR_RND{D,N,U}
    mpfr_exp_t
      emin = mpfr_get_emin (),
      emax = mpfr_get_emax ();
    mp_size_t lead = n > 0 ? highest_bit_idx(n) : LeadingBit(r, emin);
    if (lead < emin) lead = emin;
    mpfr_prec_t prec = mpfr_get_prec (val);
    mp_size_t trail = lead - prec; // position one past trailing bit
    mp_size_t guard = trail + (round == MPFR_RNDN ? 0 : 1); // guard bit pos
    if (guard < 0) ExpandTo(r, (-guard)/bits_); // Generate the bits needed.
    mpz_set_ui(_tt, n);                         // The integer part
    mpz_mul_2exp(_tt, _tt, bits_ * _e);         // Shift to allow for fraction
    mpz_add(_tt, _tt, _f);                      // Add fraction
    mpz_tdiv_q_2exp(_tt, _tt, bits_ * _e + trail); // Shift to leave prec digits
    int flag;
    if (round == MPFR_RNDU ||
        (round == MPFR_RNDN && (guard > 0
                                ? n & (1 << (guard-1)) // guard in integer part
                                : mpz_tstbit(_f, bits_ * _e  - 1 + guard)))) {
      // Round result up
      mpz_add_ui(_tt, _tt, 1u);
      // Check if exponent needs adjusting
      if (mpz_tstbit(_tt, prec)) { mpz_tdiv_q_2exp(_tt, _tt, 1); ++lead; }
      flag = +1;                // result is greater that exact result
    } else
      flag = -1;                // result is smaller that exact result
    mpfr_clear_flags();
    if ((lead <= emax && lead > emin) ||
        (lead == emin && mpz_tstbit(_tt, prec - 1))) {
      mpfr_set_z_2exp(val, _tt, trail, MPFR_RNDN); // exact
      if (s < 0)
        mpfr_neg (val, val, MPFR_RNDN);
    } else if (lead > emax) {   // overflow
      mpfr_set_inf(val, s); mpfr_set_overflow(); flag = +1;
    } else {                    // underflow
      mpfr_set_zero(val, s); mpfr_set_underflow(); flag = -1;
    }
    mpfr_set_inexflag();        // all results are inexact
    return s < 0 ? -flag : flag;
  }
};

class UnitUniform {
public:
  int operator()(mpfr_t val, gmp_randstate_t r, mpfr_rnd_t round) const {
    _x.Init();
    return _x(r, 0, 1, val, round);
  }
private:
  mutable RandomFraction _x;
};

// This is a transcription of RandomLib::ExactExponential (version 1.4).
class UnitExponential {
public:
  int operator()(mpfr_t val, gmp_randstate_t r, mpfr_rnd_t round) const {
    _x.Init();
    unsigned k = 0;
    while (!ExpFraction(r, _x)) { ++k; _x.Init(); }
    if (k & 1) _x.SetHighBit(r);
    return _x(r, k >> 1, 1, val, round);
  }
private:
  int ExpFraction(gmp_randstate_t r, RandomFraction& p) const {
    // The early bale out
    if (p.TestHighBit(r)) return 0;
    // Implement the von Neumann algorithm
    _w.Init();
    if (!_w.LessThan(r, p)) return 1;
    while (1) {
      _v.Init(); if (!_v.LessThan(r, _w)) return 0;
      _w.Init(); if (!_w.LessThan(r, _v)) return 1;
    }
  }
  mutable RandomFraction _x;
  mutable RandomFraction _v;
  mutable RandomFraction _w;
};

// This is a transcription of RandomLib::ExactNormal (version 1.3)
class UnitNormal {
public:
  UnitNormal() { mpz_init(_tt); }
  ~UnitNormal() { mpz_clear(_tt); }
  int operator()(mpfr_t val, gmp_randstate_t r, mpfr_rnd_t round) const {
    while (1) {
      unsigned k = ExpProbN(r); // the integer part of the result.
      if (ExpProb(r, (k - 1) * k)) {
        _x.Init();
        unsigned s = 1;
        for (unsigned j = 0; j <= k; ++j) { // execute k + 1 times
          int first;
          for (s = 1, first = 1; ; s ^= 1, first = 0) {
            if (k == 0 && Boolean(r)) break;
            _q.Init(); if (!_q.LessThan(r, first ? _x : _p)) break;
            int y = k == 0 ? 0 : Choose(r, k);
            if (y < 0)
              break;
            else if (y == 0) {
              _p.Init(); if (!_p.LessThan(r, _x)) break;
            }
            _p.swap(_q);        // a fast way of doing p = q
          }
          if (s == 0) break;
        }
        if (s != 0)
          return _x(r, k, Boolean(r) ? -1 : 1, val, round);
      }
    }
  }
private:
  mutable mpz_t _tt;            // A temporary
  // True with prob exp(-1/2)
  int ExpProbH(gmp_randstate_t r) const {
    _p.Init(); if (_p.TestHighBit(r)) return 1;
    // von Neumann rejection
    while (1) {
      _q.Init(); if (!_q.LessThan(r, _p)) return 0;
      _p.Init(); if (!_p.LessThan(r, _q)) return 1;
    }
  }
  // True with prob exp(-n/2)
  int ExpProb(gmp_randstate_t r, unsigned n) const {
    while (n--) { if (!ExpProbH(r)) return 0; }
    return 1;
  }
  // n with prob (1-exp(-1/2)) * exp(-n/2)
  unsigned ExpProbN(gmp_randstate_t r) const {
    unsigned n = 0;
    while (ExpProbH(r)) ++n;
    return n;
  }
  // A coin toss
  int Boolean(gmp_randstate_t r) const {
    mpz_urandomb(_tt, r, 1);
    return mpz_tstbit(_tt, 0);
  }
  // Return:
  //  1 with prob 2k/(2k + 2)
  //  0 with prob  1/(2k + 2)
  // -1 with prob  1/(2k + 2)
  int Choose(gmp_randstate_t r, int k) const {
    const int b = 15;           // To avoid integer overflow on multiplication
    const int m = 2 * k + 2;
    int n1 = m - 2, n2 = m - 1;
    while (1) {
      mpz_urandomb(_tt, r, b);
      int d = int( mpz_get_ui(_tt) ) * m;
      n1 = std::max((n1 << b) - d, 0);
      if (n1 >= m) return 1;
      n2 = std::min((n2 << b) - d, m);
      if (n2 <= 0) return -1;
      if (n1 == 0 && n2 == m) return 0;
    }
  }
  mutable RandomFraction _x;
  mutable RandomFraction _p;
  mutable RandomFraction _q;
};

/// \endcond

#endif  // RANDOMMPFR_HPP
