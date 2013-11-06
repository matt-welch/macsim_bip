/*
	This simulator is C/C++ version of IntSim.

	Original copy was developed by D. Sekar, R. Venkatesan, R. Sarvari, K. Shakeri,
	J. Davis, and Prof. J. Meindl from Microelectronics Research Center,
	Georgia Tech, Atlanta, GA.
	
	Reference: D. Sekar, "Optimal Signal, Power, Clock, and Thermal Interconnect
	Networks for High-Performance 2D and 3D Integrated Circuits," Ph.D. Dissertation,
	School of ECE, Georgia Tech, Atlanta, GA, 2008.

	Copyright 2009
	William Song, Prof. Sudhakar Yalamanchili, and Prof. Saibal Mukhopadhyay
	Georgia Tech Research Corporation, Atlanta, GA 30332
*/
#ifndef CHIP_T
#define CHIP_T
#include "parameters.h"

class intsim_chip_t {
public:
  intsim_chip_t() {}
  ~intsim_chip_t() {}

  double total_power;
//  struct intsim_param_t * param;

  int gate_sizing(intsim_param_t * param); // Algorithm Step 2: Logic Gate Sizing
  void st_wiring(intsim_param_t * param); // Algorithm Step 3: Generation of Stochastic Wiring Distribution
  void baseline_param(intsim_param_t * param, double total_estimated_power); // Algorithm Step 4: Set Baseline Parameters for Iterations
  void local_interconnect(intsim_param_t * param); // Algorithm Step 5: Local Interconnect Modeling
  void no_repeaters(intsim_param_t * param); // Algorithm Step 6: Arrangement of Wires without Repeaters
  void global_interconnect(intsim_param_t * param, double total_estimated_power); // Algorithm Step 7: Global Interconnect Modeling
  void insert_repeaters(intsim_param_t * param); // Algorithm Step 8: Assignment of Wires with Repeaters
  // Algorithm Step 9: Power Computation and Iteration
  void output(intsim_param_t * param, double total_power); // Algorithm Step 10: Data Output

  // The leakage feedback algorithm was added for run-time leakage calculation with thermal simulation
  void update_energy(intsim_param_t * intsim_param);

  double leakage_power_logic_gates, dyn_power_logic_gates;
  double leakage_power_repeaters, dyn_power_repeaters;
  double power_wires, clock_power;
  double clock_power_dynamic, clock_power_leakage;
  int tier;

private:
  // Power variables
  double total_area_logic_gates;
  
  // Data arrays for integral equations
  double *intg_number;
  double *intg_length;
  double *pitch;
  double *net_wire_efficiency;
  double *length_max;
  double *net_wire_efficiency_repeaters;
  double *pitch_updated_with_repeaters;
  double *length_max_updated_with_repeaters;
  double *number_repeaters;

  // Local parameters
  double total_length;
  double leakage_power_two_input_nand, dyn_power_two_input_nand;
  double leakage_power_inv, dyn_power_inv;
  double area_two_input_nand_gate, area_rep_total, available_area;
  double total_number_repeaters;
  double max_power_repeaters, nv_power_ground;
  double global_wire_pitch;
  double l_last, lmin, lmax, n;
  double phigate, net_wl_rep;
  double cbuffer, number_rep_global, size_rep_global;
};

#endif
