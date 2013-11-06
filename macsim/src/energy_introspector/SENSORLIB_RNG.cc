#include "SENSORLIB_RNG.h"
#include "SENSORLIB_RNG.src"

SENSORLIB_RNG::SENSORLIB_RNG(parameters_sensor_t &p_sensor, parameters_technology_t &p_technology,\
                             energy_introspector_t *ei)
{
  // sensor_library_t parameters
  name = "rng";
  energy_introspector = ei;

  set_variable<parameters_sensor_t>(x,p_sensor,"x",0.0,true);
  set_variable<parameters_sensor_t>(y,p_sensor,"y",0.0,true);
  set_variable<parameters_sensor_t>(layer,p_sensor,"layer",0,true);

  string option;
  set_variable<parameters_sensor_t>(option,p_sensor,"noise_model","n/a",true);
  if(!stricmp(option,"chisquare"))
  {
    noise_model = CHISQUARE;
    int n;
    set_variable<parameters_sensor_t>(n,p_sensor,"n",0,true);
    noise = new Chisquare_t(n);
  }
  else if(!stricmp(option,"lognormal"))
  {
    noise_model = LOGNORMAL;
    double mean, std;
    set_variable<parameters_sensor_t>(mean,p_sensor,"mean",0,true);
    set_variable<parameters_sensor_t>(std,p_sensor,"std",0,true);
    noise = new Lognormal_t(mean,std);
  }
  else if(!stricmp(option,"normal"))
  {
    noise_model = NORMAL;
    double mean, std;
    set_variable<parameters_sensor_t>(mean,p_sensor,"mean",0,true);
    set_variable<parameters_sensor_t>(std,p_sensor,"std",0,true);
    noise = new Normal_t(mean,std);
  }
  else if(!stricmp(option,"uniform"))
  {
    noise_model = UNIFORM;
    int min, max;
    set_variable<parameters_sensor_t>(min,p_sensor,"min",0,true);
    set_variable<parameters_sensor_t>(max,p_sensor,"max",0,true);
    noise = new Uniform_t(min,max);
  }
  else
  {
    fprintf(energy_introspector->fp_config,"ERROR (RNG): unknown noise model %s\n",option.c_str());
    exit(1);
  }

  set_variable<parameters_sensor_t>(option,p_sensor,"delay_model","n/a",true);
  if(!stricmp(option,"exponential"))
  {
    delay_model = EXPONENTIAL;
    double lambda;
    set_variable<parameters_sensor_t>(lambda,p_sensor,"lambda",0,true);
    delay = new Exponential_t(lambda);
  }
  else if(!stricmp(option,"uniform"))
  {
    delay_model = UNIFORM;
    int min, max;
    set_variable<parameters_sensor_t>(min,p_sensor,"min",0,true);
    set_variable<parameters_sensor_t>(max,p_sensor,"max",0,true);
    delay = new Uniform_t(min,max);
  }
  else
  {
    fprintf(energy_introspector->fp_config,"ERROR (RNG): unknown delay model %s\n",option.c_str());
    exit(1);
  }
  
  int queue_size;
  set_variable<parameters_sensor_t>(queue_size,p_sensor,"queue_size",2);

  // create pseudo sensor queue
  energy_introspector->sensor.find(p_sensor.ID)->second.queue.create<double>("temperature",queue_size);
}

void SENSORLIB_RNG::read_data(double time_tick, void *data)
{
  grid_t<double> *thermal_map = (grid_t<double>*)data;

  if(!thermal_map)
  {
    #ifdef EI_DEBUG
    fprintf(stdout,"WARNING (RNG): thermal_map is null\n");
    #endif
    return;
  }

  if(!(noise||delay))
  {
    #ifdef EI_DEBUG
    fprintf(stdout,"WARNING (RNG): no valid noise/delay model\n");
    #endif
    return;
  }

  // temperature error
  double temperature = thermal_map->pull((int)(x/thermal_map->cell_width),(int)(y/thermal_map->cell_length),layer)\
                       + noise->get_random();
  // delay error
  double read_time = time_tick + delay->get_random();
  // find the time_tick of the last entry
  double prev_time = energy_introspector->sensor.find(sensorID)->second.queue.end<double>("temperature");

  // store sensor-read data
  energy_introspector->push_data<double>(read_time,read_time-prev_time,"sensor",sensorID,"temperature",temperature);
}
