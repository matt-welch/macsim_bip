// Copyright 2009-2010 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2010, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

#ifndef RAMP_H
#define RAMP_H


#include <stdio.h>			
#include <math.h>
#include <string>

using namespace std;

class RAMP {
 public:
  RAMP();
  ~RAMP() {}

  /* Constants */
  class {
   public:
    // Generic Constants
    double k; // Boltzmann's Constant
    double T_sf; // Stress Free Temperature
    double T_ref; // reference temperature
    double T_ambient; // ambient temperature
    double MTTF_ref; // reference MTTF

    // Electromigration Constants
    class {
     public:
      double J;
      double n;
      double Ea;
      double coeff;
    }EM;

    // Stress Migration Constants
    class {
     public:
      double n;
      double Ea;
      double coeff;
    }SM;

    // Time-dependent Dielectric Breakdown
    class {
     public:
      double a, b;
      double X, Y, Z;
      double coeff;
    }TDDB;

    // Thermal Cycling
    class {
     public:
      double q;
      double coeff;
    }TC;

    // Negative Bias Temperature Instability
    class {
     public:
      double A, B, C, D;
      double beta;
      double coeff;
    }NBTI;
  }constants;

  int technology_node;
  double Vdd;
  double clock_frequency;

  /* MTTF functions */
  void MTTF_target(double feature_size, double clk, double voltage, double hours = 262800/*0years*/);
  double MTTF_EM(double temperature, double activity_factor);
  double MTTF_SM(double temperature);
  double MTTF_TDDB(double temperature);
  double MTTF_TC(double temperature);
  double MTTF_NBTI(double temperature);
  double MTTF(double temperature, double activity_factor);

 private:
  int num_risks; // number of risk models
};

#endif


