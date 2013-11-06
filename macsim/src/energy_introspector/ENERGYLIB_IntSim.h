#ifndef ENERGYLIB_INTSIM_H
#define ENERGYLIB_INTSIM_H

#include "energy_introspector.h"

#include "ENERGYLIB_IntSim/intsim.h"
#include "ENERGYLIB_IntSim/chip.h"
#include "ENERGYLIB_IntSim/parameters.h"

class ENERGYLIB_IntSim : public energy_library_t
{
 public:
  ENERGYLIB_IntSim(parameters_module_t &p_module, parameters_technology_t &p_technology,\
  energy_introspector_t *ei);
  ~ENERGYLIB_IntSim() {}

  virtual energy_t get_unit_energy(bool is_tdp);
  virtual double get_area(void);
  virtual void update_energy(string name, void *value);

 private:
  double energy_scaling;
  intsim_chip_t *chip;
  intsim_param_t *param;
  
  // IntSim main algorithm
  void IntSim(intsim_chip_t *chip, intsim_param_t *param);
};
#endif
