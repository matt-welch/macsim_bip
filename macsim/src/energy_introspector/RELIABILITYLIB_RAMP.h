#ifndef RELIABILITYLIB_RAMP_H
#define RELIABILITYLIB_RAMP_H

#include "energy_introspector.h"

#include "RELIABILITYLIB_RAMP/reliability.h"

class RELIABILITYLIB_RAMP : public reliability_library_t
{
 public:
  RELIABILITYLIB_RAMP(parameters_package_t &p_package, parameters_partition_t &p_partition,\
                      parameters_technology_t &p_technology, energy_introspector_t *ei);
  ~RELIABILITYLIB_RAMP() {}
  
  virtual double get_MTTF(double temperature, double clock_frequency, double Vdd, double activity_factor, bool is_active);
  
 private:
  RAMP *ramp; 
};
#endif
