#ifndef ENERGYLIB_ORION_H
#define ENERGYLIB_ORION_H

#include "energy_introspector.h"

class ENERGYLIB_Orion : energy_library_t
{
 public:
  ENERGYLIB_Orion(parameters_module_t &p_module, parameters_technology_t &p_technology, energy_introspector_t *ei);

};
#endif
