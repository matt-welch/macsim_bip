#include "RELIABILITYLIB_RAMP.h"
#include "RELIABILITYLIB_RAMP.src"

RELIABILITYLIB_RAMP::RELIABILITYLIB_RAMP(parameters_package_t &p_package, parameters_partition_t &p_partition, \
                                         parameters_technology_t &p_technology, energy_introspector_t *ei) :
ramp(NULL)
{
  name = "ramp";
  energy_introspector = ei;

  ramp = new RAMP();

  if(!ramp)
  {
    fprintf(stdout,"EI ERROR (RAMP): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (RAMP): cannot create a reliability model\n");
    #endif
    exit(1);
  }

  double feature_size;
  set_variable<parameters_technology_t>(feature_size,p_technology,"feature_size",0.0);
  if(feature_size == 0.0)
    set_variable<parameters_partition_t>(feature_size,p_partition,"feature_size",0.0,true);

  set_variable<parameters_technology_t>(clock_frequency,p_technology,"baseline_clk",0.0);
  if(clock_frequency == 0.0)
    set_variable<parameters_partition_t>(clock_frequency,p_partition,"baseline_clk",0.0,true);

  double MTTF;
  set_variable<parameters_technology_t>(MTTF,p_technology,"baseline_MTTF",0.0);
  if(MTTF == 0.0)
    set_variable<parameters_partition_t>(MTTF,p_partition,"baseline_MTTF",(double)262800);

  double Vdd;
  set_variable<parameters_technology_t>(Vdd,p_technology,"baseline_Vdd",0.0);
  if(Vdd == 0.0)
    set_variable<parameters_partition_t>(Vdd,p_partition,"baseline_Vdd",0.0,true);
    
  set_variable<parameters_package_t>(ramp->constants.T_ambient,p_package,"ambient_temperature",ramp->constants.T_ambient);
  
/*
  map<string,pseudo_partition_t>::iterator partition_it = energy_introspector->partition.find(p_partition.ID);
  if(partition_it != energy_introspector->partition.end())
  {
    map<string,pseudo_package_t>::iterator package_it = energy_introspector->package.find(partition_it->second.package);
    if(package_it != energy_introspector->package.end())
      set_variable<parameters_package_t>(ramp->constants.T_ambient,p_package,"ambient_temperature",ramp->constants.T_ambient);
  }
*/
    
/*
 constants.EM.Ea = 0.9; // 0.9eV
 constants.EM.n = 1.1;
 constants.EM.coeff = 1.0;
 constants.SM.Ea = 0.9; // 0.9eV
 constants.SM.n = 2.5;
 constants.SM.coeff = 1.0;
 constants.TDDB.a = 78.0;
 constants.TDDB.b = -0.081;
 constants.TDDB.X = 0.759;
 constants.TDDB.Y = -66.8;
 constants.TDDB.Z = -8.37e-4;
 constants.TDDB.coeff = 1.0;
 constants.TC.q = 2.35;
 constants.TC.coeff = 1.0;
 constants.NBTI.A = 1.6328;
 constants.NBTI.B = 0.07377;
 constants.NBTI.C = 0.01;
 constants.NBTI.D = -0.06852;
 constants.NBTI.beta = 0.3;
 constants.NBTI.coeff = 1.0;
 
 constants.T_sf = 500.0; // 500K
 constants.T_ref = 345.0; // 345K
 constants.T_ambient = 300.0; // 300K
 constants.MTTF_ref = (double)30*24*365; // MTTF at T_ref
  
  constants.k = 8.617385e-5; // Boltzmann's constant
*/    
  ramp->MTTF_target(feature_size,clock_frequency,Vdd,MTTF);
}

double RELIABILITYLIB_RAMP::get_MTTF(double temperature, double clock_frequency, double Vdd, double activity_factor, bool is_active)
{
  if(ramp)
  {
    if(is_active)
    {
      ramp->clock_frequency = clock_frequency;
      ramp->Vdd = Vdd;
/*
cout << " EM = " << ramp->MTTF_EM(temperature,activity_factor);
cout << " SM = " << ramp->MTTF_SM(temperature);
cout << " TDDB = " << ramp->MTTF_TDDB(temperature);
cout << " TC = " << ramp->MTTF_TC(temperature);
cout << " NBTI = " << ramp->MTTF_NBTI(temperature);
*/
      return ramp->MTTF(temperature,activity_factor);
    }
    else // inactive partition
    {
      return ramp->MTTF_TC(temperature);
    }
  }
  else
    return 0.0;
}
