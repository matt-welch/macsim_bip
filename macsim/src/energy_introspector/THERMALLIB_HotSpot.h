#ifndef THERMALLIB_HOTSPOT_H
#define THERMALLIB_HOTSPOT_H
#include "energy_introspector.h"

#include "THERMALLIB_HotSpot/flp.h"
#include "THERMALLIB_HotSpot/npe.h"
#include "THERMALLIB_HotSpot/package.h"
#include "THERMALLIB_HotSpot/shape.h"
#include "THERMALLIB_HotSpot/temperature.h"
#include "THERMALLIB_HotSpot/temperature_block.h"
#include "THERMALLIB_HotSpot/temperature_grid.h"
#include "THERMALLIB_HotSpot/util.h"

class THERMALLIB_HotSpot : public thermal_library_t
{
 public:
  THERMALLIB_HotSpot(parameters_package_t &p_package, parameters_technology_t &p_technology,\
  energy_introspector_t *ei);
  ~THERMALLIB_HotSpot() {}

 private:
  thermal_config_t thermal_config;	// thermal configuration
  package_config_t package_config;	// package configuration (if used)
  convection_t convection;		// airflow parameters
  RC_model_t *model;			// RC model
  flp_t *flp;				// floorplan

  double *temperature, *power;

  virtual void compute_temperature(double time_tick, double period);
  virtual grid_t<double> get_thermal_map(void);
  virtual void put_partition_power(string partitionID, power_t partition_power);
};
#endif

