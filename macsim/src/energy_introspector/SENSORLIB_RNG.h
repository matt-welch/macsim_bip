#ifndef RANDTHERM_H
#define RANDTHERM_H

#include <assert.h>
#include "energy_introspector.h"

#include "SENSORLIB_RNG/rngs.h"
#include "SENSORLIB_RNG/rvgs.h"

class RNG_t 
{
 public:
  virtual double get_random(void) = 0;
};

class Bernoulli_t : public RNG_t 
{
 public:
  Bernoulli_t (double P) : p(P) { assert(p > 0); }
  double p;
  virtual double get_random(void) { return Bernoulli(p); }
};

class Binomial_t : public RNG_t 
{
 public:
  Binomial_t (long N, double P) : n(N), p(P) { assert(n > 0); assert(p > 0); }
  long n;
  double p;
  virtual double get_random(void) { return Binomial(n,p); }
};

class UniformINT_t : public RNG_t 
{
 public:
  UniformINT_t (long A, long B) : a(A), b(B) {}
  long a, b;
  virtual double get_random(void) { return Equilikely(a,b); }
};

class Geometric_t : public RNG_t
{
 public:
  Geometric_t (double P) : p(P) { assert(p > 0); }
  double p;
  virtual double get_random(void) { return Geometric(p); }
};

class Pascal_t : public RNG_t
{
 public:
  Pascal_t (long N, double P) : n(N), p(P) { assert(n > 0); assert(p > 0); }
  long n;
  double p;
  virtual double get_random(void) { return Pascal(n,p); }
};

class Poisson_t : public RNG_t
{
 public:
  Poisson_t (long N) : n(N) { assert(n > 0); }
  long n;
  virtual double get_random(void) { return Poisson(n); }
};

class Uniform_t : public RNG_t
{
 public:
  Uniform_t (long A, long B) : a(A), b(B) {}
  long a, b;
  virtual double get_random(void) { return Uniform(a,b); }
};

class Exponential_t : public RNG_t
{
 public:
  Exponential_t (double LAMBDA) : lambda(LAMBDA) { assert(lambda > 0); }
  double lambda;
  virtual double get_random(void) { return Exponential(lambda); }
};

class Erlang_t : public RNG_t
{
 public:
  Erlang_t (long N, double LAMBDA) : n(N), lambda(LAMBDA) { assert(n > 0); assert(lambda > 0); }
  long n;
  double lambda;
  virtual double get_random(void) { return Erlang(n,lambda); }
};

class Normal_t : public RNG_t
{
 public:
  Normal_t (double MEAN, double STD) : mean(MEAN), std(STD) { assert(std > 0); }
  double mean, std;
  virtual double get_random(void) { return Normal(mean,std); }
};

class Lognormal_t : public RNG_t
{
 public:
  Lognormal_t (double MEAN, double STD) :  mean(MEAN), std(STD) { assert(std > 0); }
  double mean, std;
  virtual double get_random(void) { return Lognormal(mean,std); }
};

class Chisquare_t : public RNG_t
{
 public:
  Chisquare_t (long N) : n(N) { assert(n > 0); }
  long n;
  virtual double get_random(void) { return Chisquare(n); }
};

class SENSORLIB_RNG : public sensor_library_t
{
 public:
  SENSORLIB_RNG(parameters_sensor_t &p_sensor, parameters_technology_t &p_technology,\
                energy_introspector_t *ei);

  virtual void read_data(double time_tick, void *data);
	
 private:
  // thermal sensor location
  double x, y;
  int layer;

  // noise and delay model
  int noise_model, delay_model;

  // sensor ID
  string sensorID;

  RNG_t *noise;
  RNG_t *delay;

  enum {
   BERNOULLI, BINOMIAL, 
   CHISQUARE, ERLANG, 
   EXPONENTIAL, GEOMETRIC,
   LOGNORMAL, NORMAL,
   PASCAL, POISSON, 
   UNIFORM, UNIFORM_INT
  }RNG;
};

#endif
