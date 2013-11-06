#ifndef EI_LIB_H
#define EI_LIB_H

#include "energy_introspector.h"

class energy_library_t 
{
 public:
  virtual ~energy_library_t() {}

  string name; // modeling tool name

  double clock_frequency; // clock frequency for power-energy conversion
  virtual energy_t get_unit_energy(bool is_tdp = false) = 0; // returns per-access energy
  virtual double get_area(void) = 0; // returns area (if available)
  virtual void update_energy(string name, void *value) = 0; // update per-access energy w.r.t the updater

 protected:
  class energy_introspector_t *energy_introspector; // back pointer to energy introspector
};


// parent class of thermal library (e.g., HotSpot, ICE)
class thermal_library_t
{
 public:
  virtual ~thermal_library_t() {}

  string name; // modeling tool name

  double clock_frequency;

  //virtual void set_partition(parameters_partition_t p_partition) = 0; // partition the processor-layer of the chip
  virtual void compute_temperature(double current_time, double period) = 0; // compute temperature
  virtual grid_t<double> get_thermal_map(void) = 0; // returns 3D thermal grid map
  virtual void put_partition_power(string partitionID, power_t partition_power) = 0; // update partition power

 protected:
  class energy_introspector_t *energy_introspector; // back pointer to energy introspector
};


// TBD
class sensor_library_t
{
 public:
  virtual ~sensor_library_t() {}

  string name; // modeling tool name

  double clock_frequency;

  virtual void read_data(double time_tick, void *data) = 0; // read and store data in the sensor queue

 protected:
  class energy_introspector_t *energy_introspector; // back pointer to energy introspector
};


// TBD
class reliability_library_t
{
 public:
  virtual ~reliability_library_t() {}

  string name; // modeling tool name

  double clock_frequency;
  
  virtual double get_MTTF(double temperature, double clock_frequency, double Vdd, double activity_factor, bool is_active) = 0;

 protected:
  class energy_introspector_t *energy_introspector; // back pointer to energy introspector
};

#endif
