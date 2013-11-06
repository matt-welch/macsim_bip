/* 
 * Copyright 2011 by William Song and Sudhakar Yalamanchili
 * Georgia Tech Research Corporation, Atlanta, GA 30332
 * All Rights Reserved.
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <math.h>

#include "THERMALLIB_HotSpot.h"
#include "ENERGYLIB_McPAT.h"
#include "ENERGYLIB_IntSim.h"
#include "RELIABILITYLIB_RAMP.h"
#include "SENSORLIB_RNG.h"
#include "energy_introspector.h"

energy_introspector_t::energy_introspector_t(char *config) :
fp_config(NULL)
{
  configure(config);
}


energy_introspector_t::~energy_introspector_t()
{
  package.clear();
  partition.clear();
  module.clear();
  sensor.clear();
}


void energy_introspector_t::configure(char *config)
{
  // create input parameters
  parameters_t *parameters = new parameters_t();

  if(!parameters)
  {
    fprintf(stdout,"EI ERROR: cannot configure energy_introspector parameters\n");
    exit(1);
  }

  // parse the configuration file
  parameters->parse(config);
  
  #ifdef EI_CONFIG_FILEOUT
  fp_config = fopen("config.out","w");
  
  if(!fp_config)
  {
    fprintf(stdout,"EI ERROR: cannot open configuration log file config.out\n");
  }
  else 
  {
    fprintf(stdout,"\n### energy_introspector configuration is being logged in config.out ###\n");    
  }
  #else
  fprintf(stdout,"\n### configuring energy_introspector ###\n");
  #endif

  // configure the EI
  pre_config(parameters);
  post_config(parameters);
  
  #ifdef EI_CONFIG_FILEOUT
  parameters->check_option(fp_config);
  #endif

  #ifdef EI_CONFIG_FILEOUT
  fprintf(fp_config,"### CONFIGURATION SUMMARY ###\n");
  for(map<string,pseudo_package_t>::iterator package_it = package.begin(); 
      package_it != package.end(); package_it++)
  {
    fprintf(fp_config,"package %s\n",package_it->first.c_str());
    fprintf(fp_config,"  area = %6.6lfmm^2\n",pull_data<dimension_t>(0.0,"package",package_it->first,"dimension").area*1e6);
    fprintf(fp_config,"  thermal_library: ");
    if(package_it->second.thermal_library)
      fprintf(fp_config,"%s\n",package_it->second.thermal_library->name.c_str());
    else
      fprintf(fp_config,"n/a\n");
    fprintf(fp_config,"  partitions: ");
    if(package_it->second.partition.size() > 0)
    {
      fprintf(fp_config,"\n");
      for(vector<string>::iterator partition_str_it = package_it->second.partition.begin();\
          partition_str_it < package_it->second.partition.end(); partition_str_it++)
        fprintf(fp_config,"    %s\n",partition_str_it->c_str());
      power_t TDP_power = package_it->second.queue.pull<power_t>(0.0,"TDP");
      if(TDP_power.baseline > 0.0)
        fprintf(fp_config,"  TDP.baseline = %3.3lfW\n",TDP_power.baseline);
      if(TDP_power.read > 0.0)
        fprintf(fp_config,"  TDP.read = %3.3lfW\n",TDP_power.read);
      if(TDP_power.write > 0.0)
        fprintf(fp_config,"  TDP.write = %3.3lfW\n",TDP_power.write);
      if(TDP_power.search > 0.0)
        fprintf(fp_config,"  TDP.search = %3.3lfW\n",TDP_power.search);
      if(TDP_power.read_tag > 0.0)
        fprintf(fp_config,"  TDP.read_tag = %3.3lfW\n",TDP_power.read_tag);
      if(TDP_power.write_tag > 0.0)
        fprintf(fp_config,"  TDP.write_tag = %3.3lfW\n",TDP_power.write_tag);
      if(TDP_power.total > 0.0)
        fprintf(fp_config,"  TDP.total = %3.3lfW (dynamic = %3.3lfW, leakage = %3.3lfW)\n",TDP_power.total,TDP_power.total-TDP_power.leakage,TDP_power.leakage);
      
    }
    else
      fprintf(fp_config,"n/a\n");
  }
  for(map<string,pseudo_partition_t>::iterator partition_it = partition.begin();
      partition_it != partition.end(); partition_it++)
  {
    fprintf(fp_config,"partition %s\n",partition_it->first.c_str());
    dimension_t dimension = pull_data<dimension_t>(0.0,"partition",partition_it->first,"dimension");
    fprintf(fp_config,"  area = %6.6lfmm^2(%3.3lfmm x %3.3lfmm)\n",dimension.area*1e6,dimension.width*1e3,dimension.length*1e3);
    fprintf(fp_config,"  coordinate: x_left = %3.3lfmm, y_bottom = %3.3lfmm, layer = %d\n",dimension.x_left*1e3,dimension.y_bottom*1e3,dimension.layer);
    fprintf(fp_config,"  reliability_library: ");
    if(partition_it->second.reliability_library)
      fprintf(fp_config,"%s\n",partition_it->second.reliability_library->name.c_str());
    else
      fprintf(fp_config,"n/a\n");
    fprintf(fp_config,"  package: %s\n",partition_it->second.package.c_str());
    fprintf(fp_config,"  modules: ");
    if(partition_it->second.module.size() > 0)
    {
      fprintf(fp_config,"\n");
      for(vector<string>::iterator module_str_it = partition_it->second.module.begin();
          module_str_it < partition_it->second.module.end(); module_str_it++)
        fprintf(fp_config,"    %s\n",module_str_it->c_str());

      power_t TDP_power = partition_it->second.queue.pull<power_t>(0.0,"TDP");
      if(TDP_power.baseline > 0.0)
        fprintf(fp_config,"  TDP.baseline = %3.3lfW\n",TDP_power.baseline);
      if(TDP_power.read > 0.0)
        fprintf(fp_config,"  TDP.read = %3.3lfW\n",TDP_power.read);
      if(TDP_power.write > 0.0)
        fprintf(fp_config,"  TDP.write = %3.3lfW\n",TDP_power.write);
      if(TDP_power.search > 0.0)
        fprintf(fp_config,"  TDP.search = %3.3lfW\n",TDP_power.search);
      if(TDP_power.read_tag > 0.0)
        fprintf(fp_config,"  TDP.read_tag = %3.3lfW\n",TDP_power.read_tag);
      if(TDP_power.write_tag > 0.0)
        fprintf(fp_config,"  TDP.write_tag = %3.3lfW\n",TDP_power.write_tag);
      if(TDP_power.total > 0.0)
        fprintf(fp_config,"  TDP.total = %3.3lfW (dynamic = %3.3lfW, leakage = %3.3lfW)\n",TDP_power.total,TDP_power.total-TDP_power.leakage,TDP_power.leakage);
    }
    else
      fprintf(fp_config,"n/a\n");
  }
  for(map<string,pseudo_module_t>::iterator module_it = module.begin();
      module_it != module.end(); module_it++)
  {
    fprintf(fp_config,"module %s\n",module_it->first.c_str());
    fprintf(fp_config,"  area = %6.6lfmm^2\n",pull_data<dimension_t>(0.0,"module",module_it->first,"dimension").area*1e6);
    fprintf(fp_config,"  energy_library: ");
    if(module_it->second.energy_library)
    {
      fprintf(fp_config,"%s\n",module_it->second.energy_library->name.c_str());
      energy_t unit_energy = module_it->second.energy_library->get_unit_energy();
      power_t TDP_power = module_it->second.queue.pull<power_t>(0.0,"TDP");
      
      if(unit_energy.baseline > 0.0)
      {
        fprintf(fp_config,"  unit_energy.baseline = %6.3lfpJ",unit_energy.baseline*1e12);
        if(TDP_power.baseline > 0.0)
          fprintf(fp_config," (TDP = %6.3lfmW)",TDP_power.baseline*1e3);
        fprintf(fp_config,"\n");
      }
      
      fprintf(fp_config,"  unit_energy.read = %6.3lfpJ",unit_energy.read*1e12);
      if(TDP_power.read > 0.0)
        fprintf(fp_config," (TDP = %6.3lfmW)",TDP_power.read*1e3);
      fprintf(fp_config,"\n");
      fprintf(fp_config,"  unit_energy.write = %6.3lfpJ",unit_energy.write*1e12);
      if(TDP_power.write > 0.0)
        fprintf(fp_config," (TDP = %6.3lfmW)",TDP_power.write*1e3);
      fprintf(fp_config,"\n");
      if(unit_energy.search > 0.0)
      {
        fprintf(fp_config,"  unit_energy.search = %6.3lfpJ",unit_energy.search*1e12);
        if(TDP_power.search > 0.0)
          fprintf(fp_config," (TDP = %6.3lfmW)",TDP_power.search*1e3);
        fprintf(fp_config,"\n");
      }
      if((unit_energy.read_tag > 0.0)||(unit_energy.write_tag > 0.0))
      {
        fprintf(fp_config,"  unit_energy.read_tag = %6.3lfpJ",unit_energy.read_tag*1e12);
        if(TDP_power.read_tag > 0.0)
          fprintf(fp_config," (TDP = %6.3lfmW)",TDP_power.read_tag*1e3);
        fprintf(fp_config,"\n");
        fprintf(fp_config,"  unit_energy.write_tag = %6.3lfpJ",unit_energy.write_tag*1e12);
        if(TDP_power.write_tag > 0.0)
          fprintf(fp_config," (TDP = %6.3lfmW)",TDP_power.write_tag*1e3);
        fprintf(fp_config,"\n");
      }
      fprintf(fp_config,"  unit_energy.leakage = %6.3lfpJ (%3.6lfmW) at %3.1lfK\n",unit_energy.leakage*1e12,unit_energy.leakage*module_it->second.energy_library->clock_frequency*1e3,module_it->second.queue.pull<double>(0.0,"temperature"));
      if(TDP_power.total > 0.0)
        fprintf(fp_config,"  TDP total = %6.3lfmW (dynamic = %6.3lfmW, leakage = %6.3lfmW)\n",TDP_power.total*1e3,(TDP_power.total-TDP_power.leakage)*1e3,TDP_power.leakage*1e3);
    }
    else
      fprintf(fp_config,"n/a\n");
    fprintf(fp_config,"  partition: %s\n",module_it->second.partition.c_str());
  }
  for(map<string,pseudo_sensor_t>::iterator sensor_it = sensor.begin();
      sensor_it != sensor.end(); sensor_it++)
  {
    fprintf(fp_config,"sensor %s\n",sensor_it->first.c_str());
    fprintf(fp_config,"  sensor_library: ");
    if(sensor_it->second.sensor_library)
    {
      fprintf(fp_config,"%s\n",sensor_it->second.sensor_library->name.c_str());
      fprintf(fp_config,"  sensing: %s(%s).%s\n",sensor_it->second.component_type.c_str(),sensor_it->second.component_name.c_str(),sensor_it->second.component_data.c_str());
    }
    else
      fprintf(fp_config,"n/a\n");
  }
  #endif

  #ifdef EI_CONFIG_FILEOUT
  fclose(fp_config);
  #endif
}

// pre_config() creates empty pseudo components
void energy_introspector_t::pre_config(parameters_t *parameters)
{
  #ifdef EI_CONFIG_FILEOUT
  fprintf(fp_config,"\n### pre_config processing ###\n");
  #endif

  // create empty pseudo packages
  for(vector<parameters_package_t>::iterator p_package_it = parameters->package.begin(); 
      p_package_it < parameters->package.end(); p_package_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"creating pseudo package %s\n",p_package_it->ID.c_str());
    #endif

    pseudo_package_t pseudo_package;

    // create runtime queues -- default queue size is 1
    int queue_size;
    set_variable<parameters_package_t>(queue_size,*p_package_it,"queue_size",1);

    // default queues
    pseudo_package.queue.create<dimension_t>("dimension",1);
    pseudo_package.queue.create<power_t>("TDP",1);
    pseudo_package.queue.create<grid_t<double> >("thermal_map",queue_size);
    pseudo_package.queue.create<grid_t<double> >("reliability_map",queue_size);
    pseudo_package.queue.create<power_t>("power",queue_size);

    // check extra options
    string first_partition = p_package_it->get_option("partition");
    string current_partition = first_partition;
    while(stricmp(current_partition,"n/a"))
    {
      pseudo_package.partition.push_back(current_partition);
      current_partition = p_package_it->get_option("partition");
      if(!stricmp(first_partition,current_partition))
        break;
    }

    // store the pseudo package in the EI
    if(package.find(p_package_it->ID) == package.end())
      package.insert(pair<string,pseudo_package_t>(p_package_it->ID,pseudo_package));
    else
    {
      fprintf(stdout,"EI ERROR: check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(fp_config,"EI ERROR: duplicated pseudo package ID %s\n",p_package_it->ID.c_str());      
      #endif
      exit(1);
    }
  }

  // create emtpy pseudo partitions and link to appropriate pseudo packages
  for(vector<parameters_partition_t>::iterator p_partition_it = parameters->partition.begin();
      p_partition_it < parameters->partition.end(); p_partition_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"creating pseudo partition %s\n",p_partition_it->ID.c_str());
    #endif

    pseudo_partition_t pseudo_partition;

    // link to a pseudo package
    string packageID;
    set_variable<parameters_partition_t>(packageID,*p_partition_it,"package","n/a");
    if(packageID != "n/a")
    {
      map<string,pseudo_package_t>::iterator package_it = package.find(packageID);
      if(package_it == package.end())
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: pseudo partition %s cannot be linked to non-existing pseudo package %s\n",p_partition_it->ID.c_str(),packageID.c_str());
        #endif
        exit(1);
      }

      // check if the partition ID already exists in the pseudo package
      vector<string>::iterator partition_str_it;
      for(partition_str_it = package_it->second.partition.begin();
          partition_str_it < package_it->second.partition.end(); partition_str_it++)
        if(*partition_str_it == p_partition_it->ID)
          break;
      if(partition_str_it == package_it->second.partition.end())
        package_it->second.partition.push_back(p_partition_it->ID);
    }

    pseudo_partition.package = packageID;

    // create runtime queues
    int queue_size;
    set_variable<parameters_partition_t>(queue_size,*p_partition_it,"queue_size",1);

    // default queues
    pseudo_partition.queue.create<dimension_t>("dimension",1);
    pseudo_partition.queue.create<power_t>("TDP",1);
    pseudo_partition.queue.create<double>("temperature",queue_size);
    pseudo_partition.queue.create<power_t>("power",queue_size);
    pseudo_partition.queue.create<grid_t<double> >("reliability_map",queue_size);
    pseudo_partition.queue.create<double>("activity_factor",queue_size);

    // check extra options
    string first_module = p_partition_it->get_option("module");
    string current_module = first_module;
    while(stricmp(current_module,"n/a"))
    {
      pseudo_partition.module.push_back(current_module);
      current_module = p_partition_it->get_option("module");
      if(!stricmp(first_module,current_module))
        break;
    }

    // store the pseudo partition in the EI
    if(partition.find(p_partition_it->ID) == partition.end())
      partition.insert(pair<string,pseudo_partition_t>(p_partition_it->ID,pseudo_partition));
    else
    {
      fprintf(stdout,"EI ERROR: check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(fp_config,"EI ERROR: duplicated pseudo partition ID %s\n",p_partition_it->ID.c_str());      
      #endif
      exit(1);
    }
  }

  // package-to-partition links
  for(map<string,pseudo_package_t>::iterator package_it = package.begin();
      package_it != package.end(); package_it++)
  {
    for(vector<string>::iterator partition_str_it = package_it->second.partition.begin();
        partition_str_it < package_it->second.partition.end(); partition_str_it++)
    {
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(*partition_str_it);
      if((partition_it != partition.end())&&(partition_it->second.package != package_it->first))
      {
        if(partition_it->second.package != "n/a") // pseudo package and partition are not pointing to each other
        {
          fprintf(stdout,"EI ERROR: check configuration log\n");
          #ifdef EI_CONFIG_FILEOUT
          fprintf(fp_config,"EI ERROR: pseudo package %s and pseudo partition %s have conflicting link\n",package_it->first.c_str(),partition_it->first.c_str());
          #endif
          exit(1);
        }
        partition_it->second.package = package_it->first;
      }
    }
  }

  // create emtpy pseudo modules and link to appropriate pseudo partitions
  for(vector<parameters_module_t>::iterator p_module_it = parameters->module.begin();
      p_module_it < parameters->module.end(); p_module_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"creating pseudo module %s\n",p_module_it->ID.c_str());
    #endif

    pseudo_module_t pseudo_module;

    // link to a pseudo partition
    string partitionID;
    set_variable<parameters_module_t>(partitionID,*p_module_it,"partition","n/a");
    if(partitionID != "n/a")
    {
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(partitionID);
      if(partition_it == partition.end())
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: pseudo module %s cannot be linked to non-existing pseudo partition %s\n",p_module_it->ID.c_str(),partitionID.c_str());
        #endif
        exit(1);
      }

      // check if pseudo partition already includes this pseudo module ID
      vector<string>::iterator module_str_it;
      for(module_str_it = partition_it->second.module.begin();
          module_str_it < partition_it->second.module.end(); module_str_it++)
        if(*module_str_it == p_module_it->ID)
          break;
      if(module_str_it == partition_it->second.module.end())
        partition_it->second.module.push_back(p_module_it->ID);
    }

    pseudo_module.partition = partitionID;

    // create runtime queues
    int queue_size;
    set_variable<parameters_module_t>(queue_size,*p_module_it,"queue_size",1);

    // default queues
    pseudo_module.queue.create<dimension_t>("dimension",1);
    pseudo_module.queue.create<power_t>("TDP",1);
    pseudo_module.queue.create<double>("temperature",queue_size);
    pseudo_module.queue.create<power_t>("power",queue_size);

    // store the pseudo module in the EI
    if(module.find(p_module_it->ID) == module.end())
      module.insert(pair<string,pseudo_module_t>(p_module_it->ID,pseudo_module));
    else
    {
      fprintf(stdout,"EI ERROR: check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(fp_config,"EI ERROR: duplicated pseudo module ID %s\n",p_module_it->ID.c_str());      
      #endif
      exit(1);
    }
  }

  // partition-to-module links
  for(map<string,pseudo_partition_t>::iterator partition_it = partition.begin();
      partition_it != partition.end(); partition_it++)
  {
    for(vector<string>::iterator module_str_it = partition_it->second.module.begin();
        module_str_it < partition_it->second.module.end(); module_str_it++)
    {
      map<string,pseudo_module_t>::iterator module_it = module.find(*module_str_it);
      if((module_it != module.end())&&(module_it->second.partition != partition_it->first))
      {
        if(module_it->second.partition != "n/a") // pseudo partition and module are not pointing to each other
        {
          fprintf(stdout,"EI ERROR: check configuration log\n");
          #ifdef EI_CONFIG_FILEOUT
          fprintf(fp_config,"EI ERROR: pseudo partition %s and pseudo module %s have conflicting link\n",partition_it->first.c_str(),module_it->first.c_str());          
          #endif
          exit(1);
        }
        module_it->second.partition = partition_it->first;
      }
    }
  }

  // create empty pseudo sensors
  for(vector<parameters_sensor_t>::iterator p_sensor_it = parameters->sensor.begin();
      p_sensor_it < parameters->sensor.end(); p_sensor_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"creating pseudo sensor %s\n",p_sensor_it->ID.c_str());
    #endif

    pseudo_sensor_t pseudo_sensor;

    if(sensor.find(p_sensor_it->ID) == sensor.end())
      sensor.insert(pair<string,pseudo_sensor_t>(p_sensor_it->ID,pseudo_sensor));
    else
    {
      fprintf(stdout,"EI ERROR: check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(fp_config,"EI ERROR: duplicated pseudo sensor ID %s\n",p_sensor_it->ID.c_str());      
      #endif
      exit(1);
    }
  }
}


// post_config() initializes the pseudo components and links them to modeling libraries
void energy_introspector_t::post_config(parameters_t *parameters)
{
  #ifdef EI_CONFIG_FILEOUT
  fprintf(fp_config,"\n### post_config processing ###\n");
  #endif

  // set initial temperature of pseudo partitions
  for(vector<parameters_partition_t>::iterator p_partition_it = parameters->partition.begin();
      p_partition_it < parameters->partition.end(); p_partition_it++)
  {
    map<string,pseudo_partition_t>::iterator partition_it = partition.find(p_partition_it->ID);

    double temperature;
    set_variable<parameters_partition_t>(temperature,*p_partition_it,"temperature",0.0,true);

    // initialize the temperature queue if valid
    if((temperature >= 300.0)&&(temperature <= 400.0))
      partition_it->second.queue.push<double>(0.0,0.0,"temperature",temperature);
    else
    {
      fprintf(stdout,"EI ERROR: check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(fp_config,"EI ERROR: pseudo partition %s has invalid initial temperature %lfK\n",p_partition_it->ID.c_str(),temperature);
      #endif
      exit(1);
    }
  }

  // initialize pseudo modules
  for(vector<parameters_module_t>::iterator p_module_it = parameters->module.begin();
      p_module_it < parameters->module.end(); p_module_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"initializing pseudo module %s\n",p_module_it->ID.c_str());
    #endif

    map<string,pseudo_module_t>::iterator module_it = module.find(p_module_it->ID);

    // if pseudo module is linked to a partition, overwrite the module temperature by the partition's value
    if(module_it->second.partition != "n/a")
    {
      double temperature = pull_data<double>(0.0,"partition",module_it->second.partition,"temperature");

      if((temperature >= 300.0)&&(temperature <= 400.0))
      {
        char temp[8];
        sprintf(temp,"%3.2lf",temperature);
        p_module_it->remove_option("temperature");
        p_module_it->add_option("temperature",string(temp));

        // initialize temperature queue
        module_it->second.queue.push<double>(0.0,0.0,"temperature",temperature);
      }
    }

    // link energy library
    string energy_library;
    set_variable<parameters_module_t>(energy_library,*p_module_it,"energy_library","n/a");

    // use appropriate technology parameters
    parameters_technology_t *p_technology = NULL;

    string technology;
    set_variable<parameters_module_t>(technology,*p_module_it,"technology","n/a");

    if(technology != "n/a")
    {
      for(vector<parameters_technology_t>::iterator p_technology_it = parameters->technology.begin();
          p_technology_it < parameters->technology.end(); p_technology_it++)
        if(p_technology_it->ID == technology)
        {
          p_technology = &(*p_technology_it);
          break;
        }
    }
    
    if(!p_technology)
      p_technology = new parameters_technology_t(); // dummy technology parameters

    if(energy_library != "n/a")
    {
      if(!stricmp(energy_library,"mcpat"))
        module_it->second.energy_library = new ENERGYLIB_McPAT(*p_module_it,*p_technology,this);
      else if(!stricmp(energy_library,"intsim"))
        module_it->second.energy_library = new ENERGYLIB_IntSim(*p_module_it,*p_technology,this);
      else if(stricmp(energy_library,"none"))
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: unknown energy library %s\n",energy_library.c_str());
        #endif
        exit(1);
      }
      
      counters_t counters; // dummy counters
      compute_power(MAX_TIME,MAX_TIME,module_it->first,counters,true); // compute TDP

      // update area
      dimension_t dimension;
      if(module_it->second.energy_library) // area is zero by default if no energy_library is linked
        dimension.area = module_it->second.energy_library->get_area();
      module_it->second.queue.push<dimension_t>(MAX_TIME,MAX_TIME,"dimension",dimension);
    }
  }

  // initialize pseudo partitions
  for(vector<parameters_partition_t>::iterator p_partition_it = parameters->partition.begin();
      p_partition_it < parameters->partition.end(); p_partition_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"initializing pseudo partition %s\n",p_partition_it->ID.c_str());
    #endif
    map<string,pseudo_partition_t>::iterator partition_it = partition.find(p_partition_it->ID);

    // update area
    dimension_t dimension;
    for(vector<string>::iterator module_str_it = partition_it->second.module.begin();
        module_str_it < partition_it->second.module.end(); module_str_it++)
      dimension.area += pull_data<dimension_t>(0.0,"module",*module_str_it,"dimension").area;

    // update coordinate
    if(partition_it->second.package != "n/a")
    {
      set_variable<parameters_partition_t>(dimension.x_left,*p_partition_it,"x_left",0.0,false);	// FIXME: Jieun Apr-4-2012 : 
      set_variable<parameters_partition_t>(dimension.y_bottom,*p_partition_it,"y_bottom",0.0,false);
      set_variable<parameters_partition_t>(dimension.width,*p_partition_it,"width",0.0,false);
      set_variable<parameters_partition_t>(dimension.length,*p_partition_it,"length",0.0,false);
      set_variable<parameters_partition_t>(dimension.layer,*p_partition_it,"layer",0,false);
		/*
      set_variable<parameters_partition_t>(dimension.x_left,*p_partition_it,"x_left",0.0,true);
      set_variable<parameters_partition_t>(dimension.y_bottom,*p_partition_it,"y_bottom",0.0,true);
      set_variable<parameters_partition_t>(dimension.width,*p_partition_it,"width",0.0,true);
      set_variable<parameters_partition_t>(dimension.length,*p_partition_it,"length",0.0,true);
      set_variable<parameters_partition_t>(dimension.layer,*p_partition_it,"layer",0,true);
		*/
      if(dimension.area == 0.0)
        dimension.area = dimension.width*dimension.length;
    }

    partition_it->second.queue.push<dimension_t>(MAX_TIME,MAX_TIME,"dimension",dimension);

    dimension = partition_it->second.queue.pull<dimension_t>(0.0,"dimension");

    // use appropriate technology parameters
    parameters_technology_t *p_technology = NULL;

    string technology;
    set_variable<parameters_partition_t>(technology,*p_partition_it,"technology","n/a");

    if(technology != "n/a")
    {
      for(vector<parameters_technology_t>::iterator p_technology_it = parameters->technology.begin();
          p_technology_it < parameters->technology.end(); p_technology_it++)
        if(p_technology_it->ID == technology)
        {
          p_technology = &(*p_technology_it);
          break;
        }
    }
    
    if(!p_technology)
      p_technology = new parameters_technology_t(); // dummy technology parameters
    
    // use appropriate package parameters
    parameters_package_t p_package;
    
    for(vector<parameters_package_t>::iterator p_package_it = parameters->package.begin();
        p_package_it < parameters->package.end(); p_package_it++)
      if(p_package_it->ID == partition_it->second.package)
        p_package = *p_package_it;
    

    string reliability_library;
    set_variable<parameters_partition_t>(reliability_library,*p_partition_it,"reliability_library","n/a");

    if(reliability_library != "n/a")
    {
      if(!stricmp(reliability_library,"ramp"))
        partition_it->second.reliability_library = new RELIABILITYLIB_RAMP(p_package,*p_partition_it,*p_technology,this);
      else if(stricmp(reliability_library,"none"))
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: unknown reliability library %s\n",reliability_library.c_str());
        #endif
        exit(1);
      }
    }

    // cross-check partition-to-module links
    for(vector<string>::iterator module_str_it = partition_it->second.module.begin();
        module_str_it < partition_it->second.module.end(); module_str_it++)
    {
      map<string,pseudo_module_t>::iterator module_it = module.find(*module_str_it);
      if(module_it == module.end()) // pseudo partition includes a pseudo module that is not present in the EI
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: pseudo partition %s includes non-existing pseudo module %s\n",partition_it->first.c_str(),module_str_it->c_str());
        #endif
        exit(1);
      }
      if(module_it->second.partition != partition_it->first)
      {
        if(module_it->second.partition != "n/a") // pseudo partition and module are not pointing to each other
        {
          fprintf(stdout,"EI ERROR: check configuration log\n");
          #ifdef EI_CONFIG_FILEOUT
          fprintf(fp_config,"EI ERROR: pseudo partition %s and pseudo module %s have conflicting link\n",partition_it->first.c_str(),module_it->first.c_str());
          #endif
          exit(1);
        }
        module_it->second.partition = partition_it->first;
      }
    }
  }

  // initialize pseudo packages
  for(vector<parameters_package_t>::iterator p_package_it = parameters->package.begin();
      p_package_it < parameters->package.end(); p_package_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"initializing pseudo package %s\n",p_package_it->ID.c_str());
    #endif

    map<string,pseudo_package_t>::iterator package_it = package.find(p_package_it->ID);

    // update area
    dimension_t dimension;
    for(vector<string>::iterator partition_str_it = package_it->second.partition.begin();
        partition_str_it < package_it->second.partition.end(); partition_str_it++)
      dimension.area += pull_data<dimension_t>(0.0,"partition",*partition_str_it,"dimension").area;

    package_it->second.queue.push<dimension_t>(MAX_TIME,MAX_TIME,"dimension",dimension);

    // link thermal and reliability library
    string thermal_library;
    set_variable<parameters_package_t>(thermal_library,*p_package_it,"thermal_library","n/a");

    // use appropriate technology parameters
    parameters_technology_t *p_technology = NULL;

    string technology;
    set_variable<parameters_package_t>(technology,*p_package_it,"technology","n/a");

    if(technology != "n/a")
    {
      for(vector<parameters_technology_t>::iterator p_technology_it = parameters->technology.begin();
          p_technology_it < parameters->technology.end(); p_technology_it++)
        if(p_technology_it->ID == technology)
        {
          p_technology = &(*p_technology_it);
          break;
        }
    }
    
    if(!p_technology)
      p_technology = new parameters_technology_t(); // dummy technology parameters

    if(thermal_library != "n/a")
    {
      if(!stricmp(thermal_library,"hotspot"))
        package_it->second.thermal_library = new THERMALLIB_HotSpot(*p_package_it,*p_technology,this);
      else if(stricmp(thermal_library,"none"))
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: unknown thermal library %s\n",thermal_library.c_str());
        #endif
        exit(1);
      }
    }

    // cross-check package-to-partition links
    for(vector<string>::iterator partition_str_it = package_it->second.partition.begin();
        partition_str_it < package_it->second.partition.end(); partition_str_it++)
    {
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(*partition_str_it);
      if(partition_it == partition.end()) // pseudo package includes a pseudo partition that is not stored in the EI
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: pseudo package %s includes non-existing pseudo partition %s\n",package_it->first.c_str(),partition_str_it->c_str());
        #endif
        exit(1);
      }
      if(partition_it->second.package != package_it->first)
      {
        if(partition_it->second.package != "n/a") // pseudo package and partition are not pointing to each other
        {
          fprintf(stdout,"EI ERROR: check configuration log\n");
          #ifdef EI_CONFIG_FILEOUT
          fprintf(fp_config,"EI ERROR: pseudo package %s and pseudo partition %s have conflicting link\n",package_it->first.c_str(),partition_it->first.c_str());
          #endif
          exit(1);
        }
        partition_it->second.package = package_it->first;
      }
    }
  }

  // initialize pseudo sensors
  for(vector<parameters_sensor_t>::iterator p_sensor_it = parameters->sensor.begin();
      p_sensor_it < parameters->sensor.end(); p_sensor_it++)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(fp_config,"initializing pseudo sensor %s\n",p_sensor_it->ID.c_str());
    #endif

    map<string,pseudo_sensor_t>::iterator sensor_it = sensor.find(p_sensor_it->ID);

    // link sensor library
    string sensor_library;
    set_variable<parameters_sensor_t>(sensor_library,*p_sensor_it,"sensor_library","n/a");

    // use appropriate technology parameters
    parameters_technology_t *p_technology = NULL;

    string technology;
    set_variable<parameters_sensor_t>(technology,*p_sensor_it,"technology","n/a");

    if(technology != "n/a")
    {
      for(vector<parameters_technology_t>::iterator p_technology_it = parameters->technology.begin();
          p_technology_it < parameters->technology.end(); p_technology_it++)
        if(p_technology_it->ID == technology)
        {
          p_technology = &(*p_technology_it);
          break;
        }
    }
    
    if(!p_technology)
      p_technology = new parameters_technology_t(); // dummy technology parameters

    if(sensor_library != "n/a")
    {
      if(!stricmp(sensor_library,"rng"))
        sensor_it->second.sensor_library = new SENSORLIB_RNG(*p_sensor_it,*p_technology,this);
      else if(stricmp(sensor_library,"none"))
      {
        fprintf(stdout,"EI ERROR: check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(fp_config,"EI ERROR: unknown sensor library %s\n",sensor_library.c_str());        
        #endif
        exit(1);
      }
    }

    //check if sensor monitors valid data
    string component_type, component_name, component_data;
    set_variable(sensor_it->second.component_type,*p_sensor_it,"component_type","n/a",true);
    set_variable(sensor_it->second.component_name,*p_sensor_it,"component_name","n/a",true);
    set_variable(sensor_it->second.component_data,*p_sensor_it,"component_data","n/a",true);

    bool valid = true;
    if(sensor_it->second.component_type == "package")
    {
      map<string,pseudo_package_t>::iterator package_it = package.find(sensor_it->second.component_name);
      if((package_it == package.end())||(!package_it->second.queue.is_queue(sensor_it->second.component_data)))
        valid = false;
      else
        sensor_it->second.index = package_it->second.queue.add_sensor(sensor_it->second.component_data,sensor_it->second);
    }        
    else if(sensor_it->second.component_type == "partition")
    {
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(sensor_it->second.component_name);
      if((partition_it == partition.end())||(!partition_it->second.queue.is_queue(sensor_it->second.component_data)))
        valid = false;
      else
        sensor_it->second.index = partition_it->second.queue.add_sensor(sensor_it->second.component_data,sensor_it->second);
    }
    else if(sensor_it->second.component_type == "module")
    {
      map<string,pseudo_module_t>::iterator module_it = module.find(sensor_it->second.component_name);
      if((module_it == module.end())||(!module_it->second.queue.is_queue(sensor_it->second.component_data)))
        valid = false;
      else
        sensor_it->second.index = module_it->second.queue.add_sensor(sensor_it->second.component_data,sensor_it->second);
    }
    else
      valid = false;

    if(!valid)
    {
      fprintf(stdout,"EI ERROR: check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(fp_config,"EI ERROR: pseudo sensor %s monitors invalid data\n",p_sensor_it->ID.c_str());
      #endif
      exit(1);
    }
  }
}


// energy computation
void energy_introspector_t::compute_power \
(double time_tick, double period, string moduleID, counters_t counters, bool is_tdp)
{
  #ifdef EI_DEBUG
  fprintf(stdout,"EI_DEBUG: computing the power of pseudo module %s\n",moduleID.c_str());
  #endif
  
  // find pseudo module
  map<string,pseudo_module_t>::iterator module_it = module.find(moduleID);

  if((module_it == module.end())||((period == 0.0)&&!is_tdp))
  {
    fprintf(stdout,"EI WARNING: cannot compute power for pseudo module %s\n",moduleID.c_str());
    return; // exit without energy computation
  }

  energy_t energy, unit_energy;
  power_t power;

  // compute energy
  if(module_it->second.energy_library != NULL)
  {
    unit_energy = module_it->second.energy_library->get_unit_energy();  
    
    if(is_tdp)
    {
      energy = module_it->second.energy_library->get_unit_energy(true); // TDP
      energy.total = energy.baseline+energy.search+energy.read+energy.read_tag+energy.write+energy.write_tag+energy.leakage;
      power = energy*module_it->second.energy_library->clock_frequency;
      
      // store compute TDP in the stats queue
      module_it->second.queue.push<power_t>(MAX_TIME,MAX_TIME,"TDP",power);
      
      // update partition TDP
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(module_it->second.partition);
      
      if(partition_it != partition.end())
      {
        power_t partition_power = partition_it->second.queue.pull<power_t>(0.0,"TDP");
        partition_power = partition_power + power;
        partition_it->second.queue.update<power_t>(MAX_TIME,MAX_TIME,"TDP",partition_power);
       
        map<string,pseudo_package_t>::iterator package_it = package.find(partition_it->second.package);
        
        if(package_it != package.end())
        {
          power_t package_power = package_it->second.queue.pull<power_t>(0.0,"TDP");
          package_power = package_power + power;
          package_it->second.queue.update<power_t>(MAX_TIME,MAX_TIME,"TDP",package_power);
        }
      }
    }
    else
    {
      //energy.baseline = unit_energy.baseline*period*module_it->second.energy_library->clock_frequency;
      energy.baseline = unit_energy.baseline*(double)(counters.read+counters.write);
      energy.search = unit_energy.search*(double)counters.search;
      energy.read = unit_energy.read*(double)counters.read;
      energy.write = unit_energy.write*(double)counters.write;
      energy.read_tag = unit_energy.read_tag*(double)counters.read_tag;
      energy.write_tag = unit_energy.write_tag*(double)counters.write_tag;
      energy.leakage = unit_energy.leakage*period*module_it->second.energy_library->clock_frequency;
      energy.total = energy.baseline+energy.search+energy.read+energy.read_tag+energy.write+energy.write_tag+energy.leakage;
      power = energy*(1.0/period); // conversion from energy to power

      #ifdef EI_DEBUG
      fprintf(stdout,"EI_DEBUG:   time tick = %lfsec\n",time_tick);
      if(energy.baseline > 0.0)
        fprintf(stdout,"EI_DEBUG:   baseline power = %lfW\n",power.baseline);
      if(energy.search > 0.0)
       fprintf(stdout,"EI_DEBUG:   search power = %lfW\n",power.search);
      if(energy.read > 0.0)
       fprintf(stdout,"EI_DEBUG:   read power = %lfW\n",power.read);
      if(energy.read_tag > 0.0)
        fprintf(stdout,"EI_DEBUG:   read_tag power = %lfW\n",power.read_tag);
      if(energy.write > 0.0)
        fprintf(stdout,"EI_DEBUG:   write power = %lfW\n",power.write);
      if(energy.write_tag > 0.0)
        fprintf(stdout,"EI_DEBUG:   write_tag power = %lfW\n",power.write_tag);
      if(energy.leakage > 0.0) 
        fprintf(stdout,"EI_DEBUG:   leakage power = %lfW\n",power.leakage);
      fprintf(stdout,"EI_DEBUG:   total power = %lfW\n",power.total);
      #endif

      // store computed power in the stats queue
      module_it->second.queue.push<power_t>(time_tick,period,"power",power);

      // update partition power
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(module_it->second.partition);

      if(partition_it != partition.end())
      {
        power_t partition_power = partition_it->second.queue.pull<power_t>(time_tick,"power");
        partition_power = partition_power + power;
    
        // update partition power
        if(partition_it->second.queue.is_synchronous<power_t>(time_tick,period,"power"))
          partition_it->second.queue.update<power_t>(time_tick,period,"power",partition_power);
        else
        {
          fprintf(stdout,"EI WARNING: partition %s power is asynchronous\n",partition_it->first.c_str());
        }

        // update package power
        map<string,pseudo_package_t>::iterator package_it = package.find(partition_it->second.package);

        if(package_it != package.end())
        {
          power_t package_power = package_it->second.queue.pull<power_t>(time_tick,"power");
          package_power = package_power + power;

          if(package_it->second.queue.is_synchronous<power_t>(time_tick,period,"power"))
            package_it->second.queue.update<power_t>(time_tick,period,"power",package_power);
          else
          {
            fprintf(stdout,"EI WARNING: package %s power is asynchronous\n",package_it->first.c_str());
          }
        }
      }
    }
  }
}


// temperature computation
void energy_introspector_t::compute_temperature(double time_tick, double period, string packageID)
{
  // find pseudo package
  map<string,pseudo_package_t>::iterator package_it = package.find(packageID);

  if((period == 0.0)||(package_it == package.end()))
  {
    fprintf(stdout,"EI WARNING: cannot compute temperature for pseudo package %s\n",packageID.c_str());
    return; // exit without temperature computation
  }

  // collect package and partition power, and check synchronization
  if(package_it->second.thermal_library != NULL)
  {
    power_t package_power;

    for(vector<string>::iterator partition_str_it = package_it->second.partition.begin();
        partition_str_it < package_it->second.partition.end(); partition_str_it++)
    {
      #ifdef EI_DEBUG
      fprintf(stdout,"EI_DEBUG: collecting the power of pseudo partition %s\n",partition_str_it->c_str());
      #endif

      bool synchronized = true;

      // find pseudo partition
      map<string,pseudo_partition_t>::iterator partition_it = partition.find(*partition_str_it);

      power_t partition_power = partition_it->second.queue.pull<power_t>(time_tick,"power");

      for(vector<string>::iterator module_str_it = partition_it->second.module.begin();
          module_str_it < partition_it->second.module.end(); module_str_it++)
      {
        map<string,pseudo_module_t>::iterator module_it = module.find(*module_str_it);

        if(!module_it->second.queue.is_synchronous<power_t>(time_tick,period,"power"))
        {
          synchronized = false;
          break;
        }

        // no power stats exists in the module
        if(time_tick > module_it->second.queue.end<power_t>("power")+max_trunc) // time precision = 1e-15sec
        {
          fprintf(stdout,"EI WARNING: no power was reported to pseudo module %s -- assuming static power (zero counter)\n",module_it->first.c_str());
          counters_t counters; // zero counters
          compute_power(time_tick,period,module_it->first,counters); // compute leakage power

          partition_power = partition_power + pull_data<power_t>(time_tick,"module",module_it->first,"power");
          partition_it->second.queue.update<power_t>(time_tick,period,"power",partition_power);
        }
      }

      if(!synchronized)
      {
        fprintf(stdout,"EI ERROR: terminating thermal simulation for pseudo package %s due to asynchronous power stats\n",partition_str_it->c_str());

        // empty and erase pseudo partitions
        for(partition_str_it = package_it->second.partition.begin();
            partition_str_it < package_it->second.partition.end(); partition_str_it++)
        {
          partition_it = partition.find(*partition_str_it);
          partition_it->second.queue.reset();
          partition_it->second.module.clear();
          delete(partition_it->second.reliability_library);
          partition_it->second.reliability_library = NULL;
          partition.erase(partition_it);
        }

        // delete thermal library
        delete(package_it->second.thermal_library);
        package_it->second.thermal_library = NULL;

        // empty and erase the pseudo package
        package_it->second.queue.reset();
        package_it->second.partition.clear();
        package.erase(package_it);

        return;
      }

      package_it->second.thermal_library->put_partition_power(partition_it->first,partition_power);
    }

    // call the thermal library to compute temperature
    #ifdef EI_DEBUG
    fprintf(stdout,"EI_DEBUG: computing temperature for pseudo package %s\n",packageID.c_str());
    #endif
    package_it->second.thermal_library->compute_temperature(time_tick,period);

    grid_t<double> thermal_map = package_it->second.thermal_library->get_thermal_map();

    package_it->second.queue.push<grid_t<double> >(time_tick,period,"thermal_map",thermal_map);

    // check for leakage feedback
    for(vector<string>::iterator partition_str_it = package_it->second.partition.begin();
        partition_str_it < package_it->second.partition.end(); partition_str_it++)
    {
      dimension_t dimension = pull_data<dimension_t>(0.0,"partition",*partition_str_it,"dimension");
      double new_temperature = thermal_map.pull((int)(dimension.get_y_center()/thermal_map.cell_length),(int)(dimension.get_x_center()/thermal_map.cell_width),dimension.layer);
      double old_temperature = pull_data<double>(time_tick-period,"partition",*partition_str_it,"temperature");

      push_data<double>(time_tick,period,"partition",*partition_str_it,"temperature",new_temperature);

      #ifdef EI_DEBUG
      // power density is computed based on user-given partition area instead of computed area
      fprintf(stdout,"EI_DEBUG:   partition %s\n",partition_str_it->c_str());
      fprintf(stdout,"EI_DEBUG:     time_tick = %lfsec\n",time_tick);
      fprintf(stdout,"EI_DEBUG:     temperature = %lfK\n",new_temperature);
      fprintf(stdout,"EI_DEBUG:     power density = %lfW/mm^2\n",pull_data<power_t>(time_tick,"partition",*partition_str_it,"power").total/(dimension.width*dimension.length)/1e6);
      #endif

      map<string,pseudo_partition_t>::iterator partition_it = partition.find(*partition_str_it);
      for(vector<string>::iterator module_str_it = partition_it->second.module.begin();
          module_str_it < partition_it->second.module.end(); module_str_it++)
      {
        if((int)new_temperature != (int)old_temperature)
        {
          #ifdef EI_DEBUG
          fprintf(stdout,"EI_DEBUG: leakage feedback -- updating energy of pseudo module %s\n",module_str_it->c_str());
          #endif 
          module.find(*module_str_it)->second.energy_library->update_energy("temperature",&new_temperature);
        }

        push_data<double>(time_tick,period,"module",*module_str_it,"temperature",new_temperature);
      }
    }
  }
}

void energy_introspector_t::compute_reliability(double time_tick, double period, string partitionID,\
                                                double clock_frequency, double Vdd, double activity_factor, bool is_active)
{
  #ifdef EI_DEBUG
  fprintf(stdout,"EI_DEBUG: computing the reliability of pseudo partition %s\n",partitionID.c_str());
  #endif
    
  // find pseudo partition
  map<string,pseudo_partition_t>::iterator partition_it = partition.find(partitionID);
  map<string,pseudo_package_t>::iterator package_it = package.find(partition_it->second.package);

  if((period == 0.0)||(partition_it == partition.end())||(package_it == package.end()))
  {
    fprintf(stdout,"EI WARNING: cannot compute reliability for pseudo partition %s\n",partitionID.c_str());
    return; // exit without temperature computation
  }

  // create sptiotemporal MTTF grid
  if((partition_it->second.reliability_library != NULL)&&(package_it->second.thermal_library != NULL))
  {
    double temperature, MTTF;
    dimension_t dimension;
    grid_t<double> partition_reliability_map = partition_it->second.queue.pull<grid_t<double> >(time_tick-period,"reliability_map");
    grid_t<double> package_reliability_map = package_it->second.queue.pull<grid_t<double> >(time_tick,"reliability_map");
    if(package_reliability_map.grid.size() == 0)
      package_reliability_map = package_it->second.queue.pull<grid_t<double> >(time_tick-period,"reliability_map");
    grid_t<double> thermal_map = package_it->second.queue.pull<grid_t<double> >(time_tick,"thermal_map");

    if(thermal_map.grid.size() == 0) // no temperature input
    {
      fprintf(stdout,"EI WARNING: cannot compute reliability for pseudo package %s because temperature is unavailable\n",partitionID.c_str());
      return;
    }
    
    partition_reliability_map.cell_width = package_reliability_map.cell_width = thermal_map.cell_width;
    partition_reliability_map.cell_length = package_reliability_map.cell_length = thermal_map.cell_length;

    // find grid cells that map to the partition
    dimension = pull_data<dimension_t>(0.0,"partition",partition_it->first,"dimension");
    
    int x_min = (int)(dimension.x_left/thermal_map.cell_width);
    x_min += (int)((thermal_map.cell_width*((double)x_min+0.5) >= dimension.x_left)?0:1);
    int x_max = (int)((dimension.x_left+dimension.width)/thermal_map.cell_width);
    x_max -= (int)((thermal_map.cell_width*((double)x_max+0.5) >= (dimension.x_left+dimension.width))?1:0);
    int y_min = (int)(dimension.y_bottom/thermal_map.cell_length);
    y_min += (int)((thermal_map.cell_length*((double)y_min+0.5) >= dimension.y_bottom)?0:1);
    int y_max = (int)((dimension.y_bottom+dimension.length)/thermal_map.cell_length);
    y_max -= (int)((thermal_map.cell_length*((double)y_max+0.5) >= (dimension.y_bottom+dimension.length))?1:0);
    
    bool memoryless = (partition_reliability_map.grid.size()==0);

    for(int x = x_min; x <= x_max; x++)
    {
      for(int y = y_min; y <= y_max; y++)
      {
        MTTF = partition_it->second.reliability_library->get_MTTF(thermal_map.pull(x,y,dimension.layer),clock_frequency,Vdd,activity_factor,is_active);
        partition_reliability_map.update(x,y,dimension.layer,(memoryless?(double)1.0:\
                                       partition_reliability_map.pull(x,y,dimension.layer))*exp(-period/MTTF));
        package_reliability_map.update(x,y,dimension.layer,(memoryless?(double)1.0:\
                                     package_reliability_map.pull(x,y,dimension.layer))*exp(-period/MTTF));
      }
    }

    if(partition_reliability_map.grid.size() > 0)
    {
      partition_it->second.queue.push<grid_t<double> >(time_tick,period,"reliability_map",partition_reliability_map);
      package_it->second.queue.update<grid_t<double> >(time_tick,period,"reliability_map",package_reliability_map);
    }
  }
  else
  {
    fprintf(stdout,"EI WARNING: cannot compute reliability for pseudo partition %s\n",partitionID.c_str());
  }
}
