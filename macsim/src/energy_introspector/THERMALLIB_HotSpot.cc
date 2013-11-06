#include "THERMALLIB_HotSpot.h"
#include "THERMALLIB_HotSpot.src"

THERMALLIB_HotSpot::THERMALLIB_HotSpot(parameters_package_t &p_package, parameters_technology_t &p_technology,\
energy_introspector_t *ei) 
{
  // energy_library_t parameters
  name = "hotspot";
  energy_introspector = ei;
  set_variable<parameters_technology_t>(clock_frequency,p_technology,"clock_frequency",0.0);
  if(clock_frequency == 0.0)
    set_variable<parameters_package_t>(clock_frequency,p_package,"clock_frequency",0.0,true);

  // THERMALLIB_HotSpot parameters
  thermal_config = default_thermal_config();
  package_config = default_package_config();

  thermal_config.base_proc_freq = clock_frequency;
  set_variable<parameters_package_t>(thermal_config.thermal_threshold,p_package,"thermal_threshold",273.15+81.8);
  set_variable<parameters_package_t>(thermal_config.t_chip,p_package,"chip_thickness",0.15e-3);
  set_variable<parameters_package_t>(thermal_config.k_chip,p_package,"chip_thermal_conduct",100.0);
  set_variable<parameters_package_t>(thermal_config.p_chip,p_package,"chip_heat",1.75e6);
  set_variable<parameters_package_t>(thermal_config.c_convec,p_package,"heatsink_convection_capacitance,",140.4);
  set_variable<parameters_package_t>(thermal_config.r_convec,p_package,"heatsink_convection_resistance",0.1);
  set_variable<parameters_package_t>(thermal_config.s_sink,p_package,"heatsink_side",60e-3);
  set_variable<parameters_package_t>(thermal_config.t_sink,p_package,"heatsink_thickness",6.9e-3);
  set_variable<parameters_package_t>(thermal_config.k_sink,p_package,"heatsink_thermal_conduct",400.0);
  set_variable<parameters_package_t>(thermal_config.p_sink,p_package,"heatsink_heat",3.55e6);
  set_variable<parameters_package_t>(thermal_config.s_spreader,p_package,"spreader_side",30e-3);
  set_variable<parameters_package_t>(thermal_config.t_spreader,p_package,"spreader_thickness",1e-3);
  set_variable<parameters_package_t>(thermal_config.k_spreader,p_package,"spreader_thermal_conduct",400.0);
  set_variable<parameters_package_t>(thermal_config.p_spreader,p_package,"spreader_heat",3.55e6);
  set_variable<parameters_package_t>(thermal_config.t_interface,p_package,"interface_thickness",20e-6);
  set_variable<parameters_package_t>(thermal_config.k_interface,p_package,"interface_thermal_conduct",400.0);
  set_variable<parameters_package_t>(thermal_config.p_interface,p_package,"interface_heat",4.0e6);
  set_variable<parameters_package_t>(thermal_config.n_metal,p_package,"metal_layers",9);
  set_variable<parameters_package_t>(thermal_config.t_metal,p_package,"metal_thickness",10e-6);
  set_variable<parameters_package_t>(thermal_config.t_c4,p_package,"c4_thickness",0.0001);
  set_variable<parameters_package_t>(thermal_config.s_c4,p_package,"c4_side",20e-6);
  set_variable<parameters_package_t>(thermal_config.n_c4,p_package,"c4_pads",400);
  set_variable<parameters_package_t>(thermal_config.s_sub,p_package,"substrate_side",0.021);
  set_variable<parameters_package_t>(thermal_config.t_sub,p_package,"substrate_thickness",0.001);
  set_variable<parameters_package_t>(thermal_config.s_solder,p_package,"solder_side",0.021);
  set_variable<parameters_package_t>(thermal_config.t_solder,p_package,"solder_thickness",0.00094);
  set_variable<parameters_package_t>(thermal_config.s_pcb,p_package,"pcb_side",0.1);
  set_variable<parameters_package_t>(thermal_config.t_pcb,p_package,"pcb_thickness",0.002);
  set_variable<parameters_package_t>(thermal_config.r_convec_sec,p_package,"secondary_convection_resistance",1.0);
  set_variable<parameters_package_t>(thermal_config.c_convec_sec,p_package,"secondary_convection_capacitance",140.0);
  set_variable<parameters_package_t>(thermal_config.ambient,p_package,"ambient_temperature",273.15+45.0);
  set_variable<parameters_package_t>(thermal_config.grid_rows,p_package,"grid_rows",64);
  set_variable<parameters_package_t>(thermal_config.grid_cols,p_package,"grid_columns",64);
  set_variable<parameters_package_t>(thermal_config.leakage_mode,p_package,"leakage_mode",0); // how are the modes indexed?

  string option;
  set_variable<parameters_package_t>(option,p_package,"secondary_model","false");
  thermal_config.model_secondary = stricmp(option,"false");

  set_variable<parameters_package_t>(option,p_package,"dtm_used","false");
  thermal_config.dtm_used = stricmp(option,"false");  

  set_variable<parameters_package_t>(option,p_package,"block_omit_lateral","false");
  thermal_config.block_omit_lateral = stricmp(option,"false");

  set_variable<parameters_package_t>(option,p_package,"leakage_used","false");
  thermal_config.leakage_used = stricmp(option,"false");

  set_variable<parameters_package_t>(option,p_package,"package_model_used","false");
  thermal_config.package_model_used = stricmp(option,"false");

  if((thermal_config.t_chip <= 0)||(thermal_config.s_sink <= 0)||(thermal_config.t_sink <= 0)
     ||(thermal_config.s_spreader <= 0)||(thermal_config.t_spreader <= 0)||(thermal_config.t_interface <= 0))
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): chip and package dimensions should be greater than zero\n");
    #endif
    exit(1);
  }
  if((thermal_config.t_metal <= 0)||(thermal_config.n_metal <= 0)||(thermal_config.t_c4 <= 0)
     ||(thermal_config.s_c4 <= 0)||(thermal_config.n_c4 <= 0)||(thermal_config.s_sub <= 0)||(thermal_config.t_sub <= 0)
     ||(thermal_config.s_solder <= 0)||(thermal_config.t_solder <= 0)||(thermal_config.s_pcb <= 0)
     ||(thermal_config.t_solder <= 0)||(thermal_config.r_convec_sec <= 0)||(thermal_config.c_convec_sec <= 0))
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): secondary heat tranfer layer dimensions should be greater than zero\n");
    #endif
    exit(1);
  }
  if(thermal_config.leakage_used == 1)
  {
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI WARNING (HotSpot): transient leakage iteration is not supported in this release -- all transient results are without thermal-leakage loop.\n");
    #endif
  }
  if((thermal_config.model_secondary == 1)&&(!strcasecmp(thermal_config.model_type, BLOCK_MODEL_STR)))
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): secondary heat tranfer path is supported only in the grid mode\n");
    #endif
    exit(1);
  }
  if((thermal_config.thermal_threshold < 0)||(thermal_config.c_convec < 0)||(thermal_config.r_convec < 0)
     || (thermal_config.ambient < 0)||(thermal_config.base_proc_freq <= 0)||(thermal_config.sampling_intvl <= 0))
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): invalid thermal simulation parameters\n");
    #endif
    exit(1);
  }
  if(thermal_config.grid_rows <= 0||thermal_config.grid_cols <= 0||
    (thermal_config.grid_rows & (thermal_config.grid_rows-1))||
    (thermal_config.grid_cols & (thermal_config.grid_cols-1)))
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): grid rows and columns should both be powers of two\n");
    #endif
    exit(1);
  }

  if(thermal_config.package_model_used)
  {
    set_variable<parameters_package_t>(option,p_package,"natural_convection","false");
    package_config.natural_convec = stricmp(option,"false");

    set_variable<parameters_package_t>(option,p_package,"flow_type","side");
    if(!stricmp(option,"side"))
      package_config.flow_type = 0;
    else if(!stricmp(option,"top"))
      package_config.flow_type = 1;
    else
    {
      fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");    
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): unknown flow_type %s\n",option.c_str());
      #endif
      exit(1);
    }

    set_variable<parameters_package_t>(option,p_package,"sink_type","fin_channel");
    if(!stricmp(option,"fin_channel"))
      package_config.sink_type = 0;
    else if(!stricmp(option,"pin_fin"))
      package_config.sink_type = 1;
    else
    {
      fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");    
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): unknown sink_type %s\n",option.c_str());
      #endif
      exit(1);
    }

    set_variable<parameters_package_t>(package_config.fin_height,p_package,"fin_height",0.03);
    set_variable<parameters_package_t>(package_config.fin_width,p_package,"fin_width",0.001);
    set_variable<parameters_package_t>(package_config.channel_width,p_package,"channel_width",0.002);
    set_variable<parameters_package_t>(package_config.pin_height,p_package,"pin_height",0.02);
    set_variable<parameters_package_t>(package_config.pin_diam,p_package,"pin_diameter",0.002);
    set_variable<parameters_package_t>(package_config.pin_dist,p_package,"pin_distance",0.005);
    set_variable<parameters_package_t>(package_config.fan_radius,p_package,"fan_radius",0.03);
    set_variable<parameters_package_t>(package_config.motor_radius,p_package,"motor_radius",0.01);
    set_variable<parameters_package_t>(package_config.rpm,p_package,"rpm",1000);

    if(!package_config.natural_convec)
      calculate_flow(&convection,&package_config,&thermal_config);
    else
      calc_natural_convec(&convection,&package_config,&thermal_config,thermal_config.ambient+SMALL_FOR_CONVEC);

    thermal_config.r_convec = convection.r_th;

    if(thermal_config.r_convec<R_CONVEC_LOW||thermal_config.r_convec>R_CONVEC_HIGH)
    {
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI WARNING (HotSpot): heatsink convection resistance is not realistic, double-check your package settings\n");
      #endif
    }
  }

  /* Sampling interval is dynamically adjusted in THERMALLIB_HotSpot::compute() function.
     Clock frequency really does not affect the result but complicates parameters setup.
    thermal_config.sampling_intvl = 0.0;*/

  set_variable<parameters_package_t>(option,p_package,"model","grid");
  if(!stricmp(option,"block"))
    strcpy(thermal_config.model_type,"block");
  else if(!stricmp(option,"grid"))
    strcpy(thermal_config.model_type,"grid");
  else
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): unknown model %s\n",option.c_str());
    #endif
    exit(1);
  }

  set_variable<parameters_package_t>(option,p_package,"grid_map_mode","center");
  if(!stricmp(option,"center"))
    strcpy(thermal_config.grid_map_mode,"center");
  else if(!stricmp(option,"avg")||!stricmp(option,"average"))
    strcpy(thermal_config.grid_map_mode,"avg");
  else if(!stricmp(option,"min")||!stricmp(option,"minimum"))
    strcpy(thermal_config.grid_map_mode,"min");
  else if(!stricmp(option,"max")||!stricmp(option,"maximum"))
    strcpy(thermal_config.grid_map_mode,"max");
  else
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): unknown grid_map_mode %s\n",option.c_str());
    #endif
    exit(1);
  }

  double interface_temperature, spreader_temperature, heatsink_temperature;
  set_variable<parameters_package_t>(interface_temperature,p_package,"interface_temperature",0.0,true);
  set_variable<parameters_package_t>(spreader_temperature,p_package,"spreader_temperature",0.0,true);
  set_variable<parameters_package_t>(heatsink_temperature,p_package,"heatsink_temperature",0.0,true);

  // following lines are part of hotspot main, interfacing with energy introspector
  map<string,pseudo_package_t>::iterator package_it = energy_introspector->package.find(p_package.ID);

  // create floorplan
  if(package_it->second.partition.size() == 0)
  {
    fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");  
    #ifdef EI_CONFIG_FILEOUT
    fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): pseudo package %s is not partitioned\n",package_it->first.c_str());
    #endif
    exit(1);
  }
  flp = flp_alloc_init_mem(package_it->second.partition.size());

  // update floorplan
  vector<string>::iterator partition_str_it = package_it->second.partition.begin();
  for(int i = 0; (i < flp->n_units)||(partition_str_it < package_it->second.partition.end()); i++)
  {
    dimension_t flp_dimension = energy_introspector->pull_data<dimension_t>(0.0,"partition",*partition_str_it,"dimension");
  
    flp->units[i].width = flp_dimension.width;
    flp->units[i].height = flp_dimension.length;
    flp->units[i].leftx = flp_dimension.x_left;
    flp->units[i].bottomy = flp_dimension.y_bottom;
    strcpy(flp->units[i].name,partition_str_it->c_str());
    partition_str_it++;
  }

  // update thermal correlation between floorplans
  for(int i = 0; i < flp->n_units; i++)
  {
    for(int j = 0; j < flp->n_units; j++)
      flp->wire_density[i][j] = 1.0;
  }

  flp_translate(flp,0,0);

  model = alloc_RC_model(&thermal_config,flp);

  // transient simulation by default
  populate_R_model(model,flp);
  populate_C_model(model,flp);
  
  temperature = hotspot_vector(model);
  power = hotspot_vector(model);

  // set initial temperature
  for(int n = 0; n < NL; n++)
  {
    for(int i = 0; i < flp->n_units; i++)
    {
      switch(n)
      {
        case 0:
          temperature[i+n*flp->n_units] = energy_introspector->pull_data<double>(0.0,"partition",string(flp->units[i].name),"temperature");
          break;
        case 1:
          temperature[i+n*flp->n_units] = interface_temperature;
          break;
        case 2:
          temperature[i+n*flp->n_units] = spreader_temperature;
          break;
        case 3:
          temperature[i+n*flp->n_units] = heatsink_temperature;
          break;
        default:
          fprintf(stdout,"EI ERROR (HotSpot): check configuration log\n");        
          #ifdef EI_CONFIG_FILEOUT
          fprintf(energy_introspector->fp_config,"EI ERROR (HotSpot): out of package stack\n");
          #endif
          exit(1);
          break;
      }
    }
  }
  for(int i = 0; i < EXTRA; i++) // 4 spreader (side) nodes + 4 heatsink (inner side) nodes + 4 heatsink (outer side) nodes
  {
    if(i < 4)
      temperature[i+NL*flp->n_units] = spreader_temperature;
    else
      temperature[i+NL*flp->n_units] = heatsink_temperature;
  }
}



void THERMALLIB_HotSpot::put_partition_power(string partitionID, power_t partition_power)
{
  // find the HotSpot floorplan to save the partition power
  int i;
  for(i = 0; i < flp->n_units; i++)
  {
    if(flp->units[i].name == partitionID)
    {
      power[i] = partition_power.total;
      break;
    }
  }

  if(i >= flp->n_units)
  {
    fprintf(stdout,"EI WARNING (HotSpot): unknown partition ID %s for put_partition_power()\n",partitionID.c_str());
  }
}


grid_t<double> THERMALLIB_HotSpot::get_thermal_map(void)
{
  grid_t<double> thermal_map;

  thermal_map.cell_width = get_total_width(flp)/thermal_config.grid_cols;
  thermal_map.cell_length = get_total_height(flp)/thermal_config.grid_rows;

  if(!strcmp(thermal_config.model_type,"grid")) // grid model
  {
    for(int i = 0; i < thermal_config.grid_rows; i++) // HotSpot grid has wrong orientation
      for(int j = 0; j < thermal_config.grid_cols; j++)
        thermal_map.push(/*x*/thermal_config.grid_rows-i-1,/*y*/j,/*layer*/0,
                         /*grid temp*/model->grid->last_trans->cuboid[0][i][j]); // HotSpot is 2D
  }
  else // block model
  {
    for(int i = 0; i < thermal_config.grid_rows; i++)
      for(int j = 0; j < thermal_config.grid_cols; j++)
      {
        double cell_center_x = (double)thermal_map.cell_width*(0.5+j);
        double cell_center_y = (double)thermal_map.cell_length*(0.5+i);

        for(int k = 0; k < flp->n_units; k++)
        {
          if((cell_center_x >= flp->units[k].leftx)&&
             (cell_center_x < (flp->units[k].leftx+flp->units[k].width))&&
             (cell_center_y >= flp->units[k].bottomy)&&
             (cell_center_y < (flp->units[k].bottomy+flp->units[k].height)))
          {
            thermal_map.push(/*x*/i,/*y*/j,/*layer*/0,
                             /*grid temp*/temperature[k]); // HotSpot is 2D
          }
        }
      }
  }

  return thermal_map;
}


void THERMALLIB_HotSpot::compute_temperature(double time_tick, double period)
{
  model->config->sampling_intvl = period;

  // HotSpot - compute temperature
  compute_temp(model,power,temperature,model->config->sampling_intvl);
/*
  if(!strcmp(thermal_config.model_type,"block"))
    compute_temp(model,power,temperature,model->config->sampling_intvl);
  else
    compute_temp(model,power,NULL,model->config->sampling_intvl);
*/
}

