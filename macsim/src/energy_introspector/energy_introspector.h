#ifndef EI_H
#define EI_H

#include <string>
#include <map>

#include "definitions.h"
#include "parameters.h"
#include "parser.h"
#include "library.h"
#include "queue.h"

class pseudo_module_t;
class pseudo_partition_t;
class pseudo_package_t;

class pseudo_module_t
{
 public:
  pseudo_module_t() : energy_library(NULL) {}
  ~pseudo_module_t() { queue.reset(); }
  
  energy_library_t *energy_library; // pointer to energy library
  string partition; // partition ID

  queue_t queue;
};


class pseudo_partition_t
{
 public:
  pseudo_partition_t() {}
  ~pseudo_partition_t() { module.clear(); queue.reset(); }

  reliability_library_t *reliability_library; // pointer to reliability library
  vector<string> module; // modules mapped to this partition
  string package; // package ID

  queue_t queue;
};


class pseudo_package_t
{
 public:
  pseudo_package_t() : thermal_library(NULL) {}
  ~pseudo_package_t() { partition.clear(); queue.reset(); }

  thermal_library_t *thermal_library; // pointer to thermal library
  vector<string> partition; // partitions in the package

  queue_t queue;
};


class pseudo_sensor_t
{
 public:
  pseudo_sensor_t() : sensor_library(NULL) {}
  ~pseudo_sensor_t() { }

  sensor_library_t *sensor_library; // pointer to sensor library
  int index; // sensor index if multiple sensors are bound to the same data
  string component_type, component_name, component_data; // sensed component and data information

  queue_t queue;
};

#ifdef MANIFOLD
class energy_introspector_t : public manifold::kernel::Component
#else
class energy_introspector_t
#endif
{
 private:
  // operators
  friend counters_t operator+(const counters_t & x, const counters_t & y);
  friend counters_t operator-(const counters_t & x, const counters_t & y);
  friend energy_t operator+(const energy_t & x, const energy_t & y);
  friend energy_t operator-(const energy_t & x, const energy_t & y);
  friend power_t operator+(const power_t & x, const power_t & y);
  friend power_t operator-(const power_t & x, const power_t & y);
  friend power_t operator*(const energy_t & x, const double y);
  friend energy_t operator/(const power_t & x, const double y);

  // internal configuration functions
  void pre_config(parameters_t *parameters);
  void post_config(parameters_t *parameters);

 public:
  energy_introspector_t(char *config);
  ~energy_introspector_t();

  FILE *fp_config; // config result

  // initialization function
  void configure(char *config);

  // computational functions -- data-dependent
  void compute_power(double time_tick, double period, string name, counters_t counters, bool is_tdp = false);
  void compute_temperature(double time_tick, double period, string name);
  void compute_reliability(double time_tick, double period, string name, double clock_frequency, double Vdd, double activity_factor, bool is_active);

  // update_energy() re-computes the per-access energy of the pseudo module
  template <typename T>
  void update_energy(string name, string updater, T value)
  {
    map<string,pseudo_module_t>::iterator module_it = module.find(name);
    if(module_it != module.end())
    {
      if(module_it->second.energy_library)
        module_it->second.energy_library->update_energy(updater,&value);
      #ifdef EI_WARNING
      else
        fprintf(stdout,"WARNING: pseudo module %s has no link to the energy library\n",name.c_str());
      #endif
    }
    #ifdef EI_WARNING
    else
      fprintf(stdout,"WARNING: cannot find pseudo module %s\n",name.c_str());
    #endif
  }

  // push_data() stores the data in the runtime queue of a psuedo component
  template <typename T>
  void push_data(double time_tick, double period, string type, string name, string data, T value)
  {
    if(type == "package")
    {
      map<string,pseudo_package_t>::iterator package_it = package.find(name);
      if(package_it != package.end())
      {
        package_it->second.queue.push<T>(time_tick,period,data,value);

        int sensor_index = 0;
        pseudo_sensor_t *pseudo_sensor = package_it->second.queue.get_sensor(data,sensor_index);
        while(pseudo_sensor)
        {
          // EI cannot access the sensor queue without knowing the proper data type.
          // sensor_library is responsible for storing the data in the sensor queue.
          if(pseudo_sensor->sensor_library)
            pseudo_sensor->sensor_library->read_data(time_tick,&value);

          pseudo_sensor = package_it->second.queue.get_sensor(data,++sensor_index);
        }
      }
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo package %s\n",name.c_str());
        #endif
      }
    }
    else if(type == "partition")
    {
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(name);
      if(partition_it != partition.end())
      {
        partition_it->second.queue.push<T>(time_tick,period,data,value);

        int sensor_index = 0;
        pseudo_sensor_t *pseudo_sensor = partition_it->second.queue.get_sensor(data,sensor_index);
        while(pseudo_sensor)
        {
          // EI cannot access the sensor queue without knowing the proper data type.
          // sensor_library is responsible for storing the data in the sensor queue.
          if(pseudo_sensor->sensor_library)
            pseudo_sensor->sensor_library->read_data(time_tick,&value);

          pseudo_sensor = partition_it->second.queue.get_sensor(data,++sensor_index);
        }
      }
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo partition %s\n",name.c_str());
        #endif
      }
    }
    else if(type == "module")
    {
      map<string,pseudo_module_t>::iterator module_it = module.find(name);
      if(module_it != module.end())
      {
        module_it->second.queue.push<T>(time_tick,period,data,value);

        int sensor_index = 0;
        pseudo_sensor_t *pseudo_sensor = module_it->second.queue.get_sensor(data,sensor_index);
        while(pseudo_sensor)
        {
          // EI cannot access the sensor queue without knowing the proper data type.
          // sensor_library is responsible for storing the data in the sensor queue.
          if(pseudo_sensor->sensor_library)
            pseudo_sensor->sensor_library->read_data(time_tick,&value);

          pseudo_sensor = module_it->second.queue.get_sensor(data,++sensor_index);
        }
      }
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo module %s\n",name.c_str());
        #endif
      }
    }
    else if(type == "sensor")
    {
      map<string,pseudo_sensor_t>::iterator sensor_it = sensor.find(name);
      if(sensor_it != sensor.end())
        sensor_it->second.queue.push<T>(time_tick,period,data,value);
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo sensor %s\n",name.c_str());
        #endif
      }
    }
    else
    {
      #ifdef EI_DEBUG
      fprintf(stdout,"WARNING (EI): unknown pseudo component type %s\n",type.c_str());
      #endif
    }
  }


  // push_data() loads the data from the runtime queue of a psuedo component
  template <typename T>
  T pull_data(double time_tick, string type, string name, string data)
  {
    T value;
    if(type == "package")
    {
      map<string,pseudo_package_t>::iterator package_it = package.find(name);
      if(package_it != package.end())
        value = package_it->second.queue.pull<T>(time_tick,data);
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo package %s\n",name.c_str());
        #endif
      }
    }
    else if(type == "partition")
    {
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(name);
      if(partition_it != partition.end())
        value = partition_it->second.queue.pull<T>(time_tick,data);
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo partition %s\n",name.c_str());
        #endif
      }
    }
    else if(type == "module")
    {
      map<string,pseudo_module_t>::iterator module_it = module.find(name);
      if(module_it != module.end())
        value = module_it->second.queue.pull<T>(time_tick,data);
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo module %s\n",name.c_str());
        #endif
      }
    }
    else if(type == "sensor")
    {
      map<string,pseudo_sensor_t>::iterator sensor_it = sensor.find(name);
      if(sensor_it != sensor.end())
        value = sensor_it->second.queue.pull<T>(time_tick,data);
      else
      {
        #ifdef EI_DEBUG
        fprintf(stdout,"WARNING (EI): cannot find pseudo sensor %s\n",name.c_str());
        #endif
      }
    }
    else
    {
      #ifdef EI_DEBUG
      fprintf(stdout,"WARNING (EI): unknown pseudo component type %s\n",type.c_str());
      #endif
    }

    return value;
  }

  // map of the pseudo entities
  map<string,pseudo_package_t> package;
  map<string,pseudo_partition_t> partition;
  map<string,pseudo_module_t> module;
  map<string,pseudo_sensor_t> sensor;
};

#endif
