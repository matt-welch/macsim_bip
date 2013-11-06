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

#include <assert.h>
#include "reliability.h"

RAMP::RAMP() : num_risks(5)
{
  constants.EM.Ea = 0.9; // 0.9eV
  constants.EM.n = 1.1;
  constants.EM.coeff = 1.0;
  constants.SM.Ea = 0.9; // 0.9eV
  constants.SM.n = 2.5;
  constants.SM.coeff = 1.0;
  constants.TDDB.a = 78.0;
  constants.TDDB.b = 0.081;
  constants.TDDB.X = 0.759;
  constants.TDDB.Y = 66.8;
  constants.TDDB.Z = 8.37e-4;
  constants.TDDB.coeff = 1.0;
  constants.TC.q = 2.35;
  constants.TC.coeff = 1.0;
  constants.NBTI.A = 1.6328;
  constants.NBTI.B = 0.07377;
  constants.NBTI.C = 0.01;
  constants.NBTI.D = 0.06852;
  constants.NBTI.beta = 0.3;
  constants.NBTI.coeff = 1.0;

  constants.T_sf = 500.0; // 500K
  constants.T_ref = 345.0; // 345K
  constants.T_ambient = 300.0; // 300K
  constants.MTTF_ref = (double)30/*years*/*24*365; // MTTF at T_ref

  constants.k = 8.617385e-5; // Boltzmann's constant
}

void RAMP::MTTF_target(double feature_size, double clk, double voltage, double hours)
{
  technology_node = (int)(feature_size*1e9);
  clock_frequency = clk;
  Vdd = voltage;
  constants.MTTF_ref = hours;

  // Constants based on the ITRS 2010 guideline
  // Max current density of the interconnect
  switch(technology_node)
  {
    case 65: 
      constants.EM.J = (2.0e-10*Vdd)/(1.8*pow(130,2)*1e-14);
      break;
    case 45:
      constants.EM.J = (2.0e-10*Vdd)/(1.8*pow(102,2)*1e-14);
      break;
    case 32:
      constants.EM.J = (1.8e-10*Vdd)/(1.9*pow(72,2)*1e-14);
      break;
    case 22:
      constants.EM.J = (1.7e-10*Vdd)/(1.9*pow(51,2)*1e-14);
      break;
    case 16:
      constants.EM.J = (1.5e-10*Vdd)/(2.0*pow(36,2)*1e-14);
      break;
    case 11:
      constants.EM.J = (1.4e-10*Vdd)/(2.0*pow(28.5,2)*1e-14);
      break;
    default:
      break;
  }

  constants.EM.coeff = num_risks*constants.MTTF_ref/MTTF_EM(constants.T_ref,1.0);
  constants.SM.coeff = num_risks*constants.MTTF_ref/MTTF_SM(constants.T_ref);
  constants.TDDB.coeff = num_risks*constants.MTTF_ref/MTTF_TDDB(constants.T_ref);
  constants.TC.coeff = num_risks*constants.MTTF_ref/MTTF_TC(constants.T_ref);
  constants.NBTI.coeff = num_risks*constants.MTTF_ref/MTTF_NBTI(constants.T_ref);
}

double RAMP::MTTF_EM(double temperature, double activity_factor)
{
  return constants.EM.coeff\
         *pow(constants.EM.J*activity_factor*clock_frequency,-constants.EM.n)\
         *exp(constants.EM.Ea/(constants.k*temperature));
}

double RAMP::MTTF_SM(double temperature)
{
  return constants.SM.coeff\
         *pow(constants.T_sf-temperature,-constants.SM.n)\
         *exp(constants.SM.Ea/(constants.k*temperature));
}

double RAMP::MTTF_TDDB(double temperature) 
{
  return constants.TDDB.coeff\
         *pow(1.0/Vdd,1.0/(constants.TDDB.a-constants.TDDB.b*temperature))\
         *exp((constants.TDDB.X+constants.TDDB.Y/temperature+constants.TDDB.Z*temperature)\
              /(constants.k*temperature));
}

double RAMP::MTTF_TC(double temperature)
{
  assert(temperature > constants.T_ambient);
  return constants.TC.coeff\
         *pow(1.0/(temperature-constants.T_ambient),constants.TC.q);
}

double RAMP::MTTF_NBTI(double temperature)
{
  return constants.NBTI.coeff\
         *pow((log(constants.NBTI.A/(1.0+2.0*exp(constants.NBTI.B/(constants.k*temperature))))\
               -log(constants.NBTI.A/(1.0+2.0*exp(constants.NBTI.B/(constants.k*temperature)))-constants.NBTI.C))\
              *temperature*exp(constants.NBTI.D/(constants.k*temperature)),1.0/constants.NBTI.beta);
}
        
double RAMP::MTTF(double temperature, double activity_factor)
{
  return 1.0/(1.0/MTTF_EM(temperature,activity_factor)+1.0/MTTF_SM(temperature)+1.0/MTTF_TDDB(temperature)+1.0/MTTF_TC(temperature)+1.0/MTTF_NBTI(temperature));
}
