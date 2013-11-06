#include "ENERGYLIB_IntSim.h"
#include "ENERGYLIB_IntSim.src"
#include <iostream>
#include <math.h>

ENERGYLIB_IntSim::ENERGYLIB_IntSim(parameters_module_t &p_module, parameters_technology_t &p_technology,\
energy_introspector_t *ei)
{
  // energy_library_t parameters
  name = "intsim";
  energy_introspector = ei;
  set_variable<parameters_technology_t>(clock_frequency,p_technology,"clock_frequency",0.0);
  if(clock_frequency == 0.0)
    set_variable<parameters_module_t>(clock_frequency,p_module,"clock_frequency",0.0,true);

  chip = new intsim_chip_t();
  param = new intsim_param_t();

  param->f = clock_frequency;

  set_variable<parameters_module_t>(param->A,p_module,"area",0.0,true);
  set_variable<parameters_module_t>(param->temp,p_module,"temperature",0.0,true);
  set_variable<parameters_module_t>(param->ngates,p_module,"transistors",0.0,true);
  set_variable<parameters_technology_t>(param->F,p_technology,"feature_size",0.0);
  if(param->F == 0.0)
    set_variable<parameters_module_t>(param->F,p_module,"feature_size",0.0,true);

  param->setup(); // set default values based on feature_size

  double value;
  set_variable<parameters_technology_t>(value,p_technology,"critical_path_depth",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"critical_path_depth",0.0);
  if(value != 0.0)
    param->ncp = value;

  set_variable<parameters_technology_t>(value,p_technology,"activity_factor",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"activity_factor",0.0);
  if(value != 0.0)
    param->a = value;

  set_variable<parameters_technology_t>(value,p_technology,"power_pad_distance",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"power_pad_distance",0.0);
  if(value != 0.0)
    param->pad_to_pad_distance = value;

  set_variable<parameters_technology_t>(value,p_technology,"power_pad_length",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"power_pad_length",0.0);
  if(value != 0.0)
    param->pad_length = value;

  set_variable<parameters_technology_t>(value,p_technology,"power_pads",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"power_pads",0.0);
  if(value != 0.0)
    param->npower_pads = value;

  set_variable<parameters_technology_t>(value,p_technology,"router_efficiency",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"router_efficiency",0.0);
  if(value != 0.0)
    param->router_eff = value;

  set_variable<parameters_technology_t>(value,p_technology,"repeater_efficiency",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"repeater_efficiency",0.0);
  if(value != 0.0)
    param->rep_eff = value;

  set_variable<parameters_technology_t>(value,p_technology,"average_fanouts",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"average_fanouts",0.0);
  if(value != 0.0)
    param->fo = value;

  set_variable<parameters_technology_t>(value,p_technology,"max_tier",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"max_tier",0.0);
  if(value != 0.0)
    param->max_tier = value;

  set_variable<parameters_technology_t>(value,p_technology,"average_latches_per_buffer",0.0);
  if(value == 0.0)
    set_variable<parameters_module_t>(value,p_module,"average_latches_per_buffer",0.0);
  if(value != 0.0)
    param->latches_per_buffer = value;

/*
  // debugging
  cout << p_module.ID << endl;
  cout << "param->Vdd = " << param->Vdd << endl;
  cout << "param->Vt = " << param->Vt << endl;
  cout << "param->tox = " << param->tox << endl;
  cout << "param->drive_p_div_n = " << param->drive_p_div_n << endl;
  cout << "param->f = " << param->f << endl;
  cout << "param->F = " << param->F << endl;
  cout << "param->A = " << param->A << endl;
  cout << "param->ngates = " << param->ngates << endl;
  cout << "param->F1 = " << param->F1 << endl;
  cout << "param->Vdd_spec = " << param->Vdd_spec << endl;
  cout << "param->Vt_spec = " << param->Vt_spec << endl;
  cout << "param->rho = " << param->rho << endl;
  cout << "param->Ileak_spec = " << param->Ileak_spec << endl;
  cout << "param->Idsat_spec = " << param->Idsat_spec << endl;
  cout << "param->ncp = " << param->ncp << endl;
  cout << "param->er = " << param->er << endl;
  cout << "param->k = " << param->k << endl;
  cout << "param->p = " << param->p << endl;
  cout << "param->a = " << param->a << endl;
  cout << "param->alpha = " << param->alpha << endl;
  cout << "param->subvtslope_spec = " << param->subvtslope_spec << endl;
  cout << "param->s = " << param->s << endl;
  cout << "param->ar = " << param->ar << endl;
  cout << "param->p_size = " << param->p_size << endl;
  cout << "param->pad_to_pad_distance = " << param->pad_to_pad_distance << endl;
  cout << "param->pad_length = " << param->pad_length << endl;
  cout << "param->ir_drop_limit = " << param->ir_drop_limit << endl;
  cout << "param->router_eff = " << param->router_eff << endl;
  cout << "param->rep_eff = " << param->rep_eff << endl;
  cout << "param->fo = " << param->fo << endl;
  cout << "param->margin = " << param->margin << endl;
  cout << "param->D = " << param->D << endl;
  cout << "param->latches_per_buffer = " << param->latches_per_buffer << endl;
  cout << "param->clock_factor = " << param->clock_factor << endl;
  cout << "param->clock_gating_factor = " << param->clock_gating_factor << endl;
  cout << "param->kp = " << param->kp << endl;
  cout << "param->kc = " << param->kc << endl;
  cout << "param->beta_clock = " << param->beta_clock << endl;
  cout << "param->ew_ground_power = " << param->ew_power_ground << endl;
  cout << "param->kai = " << param->kai << endl;
  cout << "param->alpha_wire = " << param->alpha_wire << endl;
  cout << "param->ro = " << param->ro << endl;
  cout << "param->co = " << param->co << endl;
  cout << "param->H = " << param->H << endl;
  cout << "param->W = " << param->W << endl;
  cout << "param->T = " << param->T << endl;
  cout << "param->S = " << param->S << endl;
  cout << "param->cg = " << param->cg << endl;
  cout << "param->cm = " << param->cm << endl;
  cout << "param->ew = " << param->ew << endl;
  cout << "param->H_global = " << param->H_global << endl;
  cout << "param->W_global = " << param->W_global << endl;
  cout << "param->T_global = " << param->T_global << endl;
  cout << "param->S_global = " << param->S_global << endl;
  cout << "param->c_clock = " << param->c_clock << endl;
  cout << "param->max_tier = " << param->max_tier << endl;
*/
  intsim(chip,param);
}



energy_t ENERGYLIB_IntSim::get_unit_energy(bool is_tdp)
{
  energy_t unit_energy;

  unit_energy.read = (chip->dyn_power_logic_gates+chip->dyn_power_repeaters+chip->power_wires)/param->f; 
  unit_energy.write = unit_energy.read;
  unit_energy.leakage = (chip->leakage_power_logic_gates+chip->leakage_power_repeaters)/param->f;

  return unit_energy;
}

double ENERGYLIB_IntSim::get_area(void)
{
  return param->A;
}

void ENERGYLIB_IntSim::update_energy(string name, void *value)
{
  if(!stricmp(name,"frequency"))
  {
    param->f = clock_frequency = *(double*)value;
  }
  else if(!stricmp(name,"temperature"))
  {
    param->temp = *(double*)value;
    param->Ileak_spec = param->I_off_n[(int)param->temp-300]*param->L;
  }
  else if(!stricmp(name,"voltage"))
  {
    param->Vdd = *(double*)value;
    param->Vdd_spec = param->Vdd;
  }
  else
  {
    fprintf(stdout,"EI WARNING (IntSim): updating undefined variable %s in update_energy()\n",name.c_str());
  }

  chip->update_energy(param);
}
