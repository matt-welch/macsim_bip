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

#include <math.h>
#include <iostream>
#include <stdio.h>
#include "parameters.h"

/*
	cg_calc.m
	This function computes the ground capacitance per unit length of a wire.
	-------------------------------------------------------------------------------------
	Reference: R. Venkatesan, PhD thesis, Georgia Tech, 2003
	Available at www.ece.gatech.edu/research/labs/gsigroup under the publications section
	-------------------------------------------------------------------------------------
*/
double intsim_param_t::calc_cg (double W, double T, double H, double S, double er) {
	double eo = 8.854e-12;
	return er*eo*(W/H+1.086*(1+0.685*pow(2.718,-T/1.343/S)-0.9964*pow(2.718,-S/1.421/H))*pow(S/(S+2*H),0.0476)*pow(T/H,0.337));
}

/*
	cm_calc.m
	This function calculates the ground capacitance per unit length of a wire.
	-------------------------------------------------------------------------------------
	Reference: R. Venkatesan, PhD thesis, Georgia Tech, 2003
	Available at www.ece.gatech.edu/research/labs/gsigroup under the publications section
	-------------------------------------------------------------------------------------
*/
double intsim_param_t::calc_cm(double W, double T, double H, double S, double er) {
	double eo = 8.854e-12;
	return er*eo*(T/S*(1-1.897*pow(2.718,-H/0.31/S-T/2.474/S)+1.302*pow(2.718,-H/0.082/S)-0.1292*pow(2.718,-T/1.326/S))
		+1.722*(1-0.6548*pow(2.718,-W/0.3477/H))*pow(2.718,-S/0.651/H));
}

/*
	co_calc.m
	This function computes the input capacitance of a minimum-size inverter.
*/
double intsim_param_t::calc_co(double tox, double F) {
	double beta_ratio_inverter = 2.0;
	double eo = 8.854e-12;
	double Cox = 3.9*eo/tox;
	return pow(F,2)*Cox*(1+beta_ratio_inverter);
}

/*
	ro_calc.m
	This function calculates the output resistance of a minimum-size inverter.
	ro is the output resistance of a minimum-size repeater.
*/
double intsim_param_t::calc_ro(double Vdd, double Vt, double Vdd_spec, double Vt_spec, double Idsat_spec, double alpha) {
	return 0.8*Vdd/(Idsat_spec/pow(Vdd_spec-Vt_spec,alpha)*pow(Vdd-Vt,alpha));
}

void intsim_param_t::setup(void) 
{
/*
// Default value provided in the original copy of the IntSim at 22nm node
	// Default system parameters
	Vdd = 0.5;				// Supply voltage
	Vt = 0.2;				// Threshold voltage
	f = 8*1e9;				// Clock frequency
	ncp = 10;				// Number of gates on a critical path
	k = 4.0;				// Rent's constant k
	p = 0.6;				// Rent's constant p
	a = 0.1;				// Activity factor
	A = 35*1e-6;			// Die area
	ngates = 58*1e6;			// Number of gates

	// Default device parameters
	F = 22*1e-9;			// Minimum feature size
	Idsat_spec = 760*F;		// Saturation drain current for a minimum-size NFET
	Ileak_spec = 0.2*F;		// Leakage current of a minimum-size NFET
	Vdd_spec = 0.5;			// Vdd at which Idsat and Ileak are specified
	Vt_spec = 0.24;			// Vt at which Idsat and Ileak are specified
	tox = 1.1*1e-9;			// Effective oxide thickness
	alpha = 1.3;			// Alpha value of the power-law MOSFET model
	drive_p_div_n = 0.5;		// Ratio of drive current of pMOS and nMOS
	subvtslope_spec = 100*1e-3;	// Subthreshold slope at 85 degrees Celsius
	F1 = F;				// Minimum wire pitch
	s = 3;				// Design rule for vias

	// Default interconnect package parameters
	er = 2.0;				// Dielectric permitivity
	rho = 1.95e-8;			// Resistivity of copper
	ar = 2.0;				// Aspect ratio of wiring levels
	R_coeff = 0.5;				// Reflectivity coefficient at grain boundaries for copper
	p_size = 0.5;			// Specularity parameter
	npower_pads = 600.0;		// Number of power pads
	pad_to_pad_distance = 300e-6;	// Average distance from one power pad to the next
	pad_length = 50e-6;		// Length of a pad
	ir_drop_limit = 0.02;		// IR drop limit in percentage, half for each global and local

	// Default design parameters
	router_eff = 0.5;			// Router efficiency
	rep_eff = 0.5;			// Repeaters efficiency
	fo = 3.0;				// Average fan-out of logic gates
	margin = 0.2;			// Percentage of clock cycle lost due to the process variation and clock skew
	D = 3*1e-3;				// Max span of H tree that needs to be driven
	latches_per_buffer = 20.0;	// Latches per buffer
	clock_factor = 1.0;		// Clock factor (number of latches = clock factor x total number of gates / number of gates on a critical path)
	clock_gating_factor = 0.4;	// Percentage of local clock power saved by clock gating
	kai = 4/(fo+3);			// Point-to-point to net length correction factor
	alpha_wire = fo/(fo+1);		// Alpha wire

	// Resistance and capacitance of a minimum-size inverter
	ro = calc_ro(Vdd,Vt,Vdd_spec,Vt_spec,Idsat_spec,alpha);
	co = calc_co(tox,F);

	// Other assumed parameters
	ew_power_ground = 0.15;
	H = F;					// ILD thickness
	W = F;					// Wire width
	T = ar*F;					// Wire thickness
	S = F;					// Spacing between wires

	// Wire capacitance per unit length
	cg = calc_cg(W,T,H,S,er);
	cm = calc_cm(W,T,H,S,er);


	kp = 2;				// Ratio of widths of power and signal wires
	kc = 4;				// Ratio of widths of clock and signal wires
	ew = router_eff;			// Wiring efficiency considering only via blockage and router efficiency
	beta_clock = 0.25;		// Max slew allowable on clock wire
	
	// Dimensions of wires
	W_global = kc*1e-6;
	T_global = ar*1e-6;
	H_global = 1e-6*ar;
	S_global = 1e-6;

	// Capacitance per unit length of clock wire
	c_clock = 2*calc_cg(W_global,T_global,H_global,S_global,er)+2*calc_cm(W_global,T_global,H_global,S_global,er);

	// limit of tiers for iteration
	max_tier = max;
*/

  rho = 2.2e-8;
  R_coeff = 0.5;
  ncp = 10.0;
  k = 4.0;
  p = 0.6;
  a = 0.1;    
  subvtslope_spec = 100e-3;
  s = 3.0;
  p_size = 0.5;
  pad_to_pad_distance = 300e-6;
  pad_length = 50e-6;
  npower_pads = 600.0;
  ir_drop_limit = 0.2;
  router_eff = 0.5;
  rep_eff = 0.5;
  fo = 3.0;
  margin = 0.2;
  D = 3e-3;
  latches_per_buffer = 20;
  clock_factor = 1.0;
  clock_gating_factor = 0.4;
  kp = 2.0;
  kc = 4.0;
  beta_clock = 0.25;
  max_tier = 20;
  ew_power_ground = 0.15;

  switch((int)(F*1e9))
  {
    case 16:
      Vdd = 0.7; // vdd[0] from McPAT
      Vt = 0.1489; // v_th[0] from McPAT
      tox = 0.5e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.0; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 1.998; // horiz_dielectric_constant[1][0]
      L = 0.00315; // Lelec[0] from McPAT
      Idsat_spec = 2768.4e-6; // I_on_n[0] from McPAT
      alpha = 1.38; // gmp_to_gmn_multiplier[0] from McPAT

      I_off_n[0] = 1.30e-7; 
      I_off_n[1] = 1.30e-7; I_off_n[2] = 1.31e-7; I_off_n[3] = 1.31e-7; I_off_n[4] = 1.31e-7; I_off_n[5] = 1.31e-7; 
      I_off_n[6] = 1.32e-7; I_off_n[7] = 1.32e-7; I_off_n[8] = 1.32e-7; I_off_n[9] = 1.32e-7; I_off_n[10] = 1.33e-7;
      I_off_n[11] = 1.33e-7; I_off_n[12] = 1.33e-7; I_off_n[13] = 1.34e-7; I_off_n[14] = 1.34e-7; I_off_n[15] = 1.34e-7; 
      I_off_n[16] = 1.34e-7; I_off_n[17] = 1.35e-7; I_off_n[18] = 1.35e-7; I_off_n[19] = 1.36e-7; I_off_n[20] = 1.36e-7;
      I_off_n[21] = 1.37e-7; I_off_n[22] = 1.37e-7; I_off_n[23] = 1.38e-7; I_off_n[24] = 1.38e-7; I_off_n[25] = 1.39e-7; 
      I_off_n[26] = 1.40e-7; I_off_n[27] = 1.41e-7; I_off_n[28] = 1.42e-7; I_off_n[29] = 1.43e-7; I_off_n[30] = 1.44e-7;
      I_off_n[31] = 1.45e-7; I_off_n[32] = 1.46e-7; I_off_n[33] = 1.48e-7; I_off_n[34] = 1.49e-7; I_off_n[35] = 1.51e-7; 
      I_off_n[36] = 1.53e-7; I_off_n[37] = 1.55e-7; I_off_n[38] = 1.57e-7; I_off_n[39] = 1.60e-7; I_off_n[40] = 1.63e-7;
      I_off_n[41] = 1.66e-7; I_off_n[42] = 1.70e-7; I_off_n[43] = 1.75e-7; I_off_n[44] = 1.81e-7; I_off_n[45] = 1.87e-7; 
      I_off_n[46] = 1.94e-7; I_off_n[47] = 2.02e-7; I_off_n[48] = 2.11e-7; I_off_n[49] = 2.20e-7; I_off_n[50] = 2.30e-7;
      I_off_n[51] = 2.42e-7; I_off_n[52] = 2.58e-7; I_off_n[53] = 2.76e-7; I_off_n[54] = 2.96e-7; I_off_n[55] = 3.19e-7; 
      I_off_n[56] = 3.44e-7; I_off_n[57] = 3.70e-7; I_off_n[58] = 3.98e-7; I_off_n[59] = 4.26e-7; I_off_n[60] = 4.55e-7;
      I_off_n[61] = 4.87e-7; I_off_n[62] = 5.22e-7; I_off_n[63] = 5.60e-7; I_off_n[64] = 6.00e-7; I_off_n[65] = 6.43e-7; 
      I_off_n[66] = 6.88e-7; I_off_n[67] = 7.34e-7; I_off_n[68] = 7.80e-7; I_off_n[69] = 8.27e-7; I_off_n[70] = 8.73e-7;
      I_off_n[71] = 9.19e-7; I_off_n[72] = 9.64e-7; I_off_n[73] = 1.01e-6; I_off_n[74] = 1.06e-6; I_off_n[75] = 1.10e-6; 
      I_off_n[76] = 1.15e-6; I_off_n[77] = 1.21e-6; I_off_n[78] = 1.26e-6; I_off_n[79] = 1.32e-6; I_off_n[80] = 1.39e-6;
      I_off_n[81] = 1.45e-6; I_off_n[82] = 1.53e-6; I_off_n[83] = 1.60e-6; I_off_n[84] = 1.68e-6; I_off_n[85] = 1.77e-6; 
      I_off_n[86] = 1.86e-6; I_off_n[87] = 1.96e-6; I_off_n[88] = 2.08e-6; I_off_n[89] = 2.20e-6; I_off_n[90] = 2.34e-6;
      I_off_n[91] = 2.50e-6; I_off_n[92] = 2.70e-6; I_off_n[93] = 2.93e-6; I_off_n[94] = 3.19e-6; I_off_n[95] = 3.48e-6; 
      I_off_n[96] = 3.79e-6; I_off_n[97] = 4.12e-6; I_off_n[98] = 4.47e-6; I_off_n[99] = 4.84e-6; I_off_n[100] = 5.22e-6;
      break;

    case 22:
      Vdd = 0.8; // vdd[0] from McPAT
      Vt = 0.1395; // v_th[0] from McPAT
      tox = 0.55e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.0; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 2.104; // horiz_dielectric_constant[1][0]
      L = 0.00468; // Lelec[0] from McPAT
      Idsat_spec = 2626.4e-6; // I_on_n[0] from McPAT
      alpha = 1.38; // gmp_to_gmn_multiplier[0] from McPAT
      	  	  
      I_off_n[0] = 1.22e-7; 
      I_off_n[1] = 1.22e-7; I_off_n[2] = 1.22e-7; I_off_n[3] = 1.22e-7; I_off_n[4] = 1.22e-7; I_off_n[5] = 1.23e-7; 
      I_off_n[6] = 1.23e-7; I_off_n[7] = 1.23e-7; I_off_n[8] = 1.23e-7; I_off_n[9] = 1.24e-7; I_off_n[10] = 1.24e-7;
      I_off_n[11] = 1.24e-7; I_off_n[12] = 1.25e-7; I_off_n[13] = 1.25e-7; I_off_n[14] = 1.25e-7; I_off_n[15] = 1.25e-7; 
      I_off_n[16] = 1.26e-7; I_off_n[17] = 1.26e-7; I_off_n[18] = 1.26e-7; I_off_n[19] = 1.27e-7; I_off_n[20] = 1.27e-7;
      I_off_n[21] = 1.28e-7; I_off_n[22] = 1.28e-7; I_off_n[23] = 1.29e-7; I_off_n[24] = 1.29e-7; I_off_n[25] = 1.30e-7; 
      I_off_n[26] = 1.31e-7; I_off_n[27] = 1.32e-7; I_off_n[28] = 1.32e-7; I_off_n[29] = 1.33e-7; I_off_n[30] = 1.34e-7;
      I_off_n[31] = 1.35e-7; I_off_n[32] = 1.37e-7; I_off_n[33] = 1.38e-7; I_off_n[34] = 1.39e-7; I_off_n[35] = 1.41e-7; 
      I_off_n[36] = 1.43e-7; I_off_n[37] = 1.45e-7; I_off_n[38] = 1.47e-7; I_off_n[39] = 1.49e-7; I_off_n[40] = 1.52e-7;
      I_off_n[41] = 1.55e-7; I_off_n[42] = 1.59e-7; I_off_n[43] = 1.64e-7; I_off_n[44] = 1.69e-7; I_off_n[45] = 1.75e-7; 
      I_off_n[46] = 1.82e-7; I_off_n[47] = 1.89e-7; I_off_n[48] = 1.97e-7; I_off_n[49] = 2.06e-7; I_off_n[50] = 2.15e-7;
      I_off_n[51] = 2.27e-7; I_off_n[52] = 2.41e-7; I_off_n[53] = 2.58e-7; I_off_n[54] = 2.77e-7; I_off_n[55] = 2.98e-7; 
      I_off_n[56] = 3.21e-7; I_off_n[57] = 3.46e-7; I_off_n[58] = 3.72e-7; I_off_n[59] = 3.98e-7; I_off_n[60] = 4.26e-7;
      I_off_n[61] = 4.55e-7; I_off_n[62] = 4.87e-7; I_off_n[63] = 5.23e-7; I_off_n[64] = 5.61e-7; I_off_n[65] = 6.01e-7; 
      I_off_n[66] = 6.43e-7; I_off_n[67] = 6.86e-7; I_off_n[68] = 7.29e-7; I_off_n[69] = 7.73e-7; I_off_n[70] = 8.16e-7;
      I_off_n[71] = 8.59e-7; I_off_n[72] = 9.01e-7; I_off_n[73] = 9.44e-7; I_off_n[74] = 9.87e-7; I_off_n[75] = 1.03e-6; 
      I_off_n[76] = 1.08e-6; I_off_n[77] = 1.13e-6; I_off_n[78] = 1.18e-6; I_off_n[79] = 1.24e-6; I_off_n[80] = 1.30e-6;
      I_off_n[81] = 1.36e-6; I_off_n[82] = 1.43e-6; I_off_n[83] = 1.50e-6; I_off_n[84] = 1.57e-6; I_off_n[85] = 1.65e-6; 
      I_off_n[86] = 1.74e-6; I_off_n[87] = 1.84e-6; I_off_n[88] = 1.94e-6; I_off_n[89] = 2.06e-6; I_off_n[90] = 2.18e-6;
      I_off_n[91] = 2.34e-6; I_off_n[92] = 2.52e-6; I_off_n[93] = 2.74e-6; I_off_n[94] = 2.98e-6; I_off_n[95] = 3.25e-6; 
      I_off_n[96] = 3.54e-6; I_off_n[97] = 3.85e-6; I_off_n[98] = 4.18e-6; I_off_n[99] = 4.52e-6; I_off_n[100] = 4.88e-6;
      break;

    case 32:
      Vdd = 0.9; // vdd[0] from McPAT
      Vt = 0.21835; // v_th[0] from McPAT
      tox = 0.5e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.41; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 2.214; // horiz_dielectric_constant[1][0]
      L = 0.01013; // Lelec[0] from McPAT
      Idsat_spec = 2211.7e-6; // I_on_n[0] from McPAT
      alpha = 1.38; // gmp_to_gmn_multiplier[0] from McPAT
	  
      I_off_n[0] = 1.52e-7; 
      I_off_n[1] = 1.52e-7; I_off_n[2] = 1.53e-7; I_off_n[3] = 1.53e-7; I_off_n[4] = 1.53e-7; I_off_n[5] = 1.53e-7; 
      I_off_n[6] = 1.54e-7; I_off_n[7] = 1.54e-7; I_off_n[8] = 1.54e-7; I_off_n[9] = 1.55e-7; I_off_n[10] = 1.55e-7;
      I_off_n[11] = 1.55e-7; I_off_n[12] = 1.56e-7; I_off_n[13] = 1.56e-7; I_off_n[14] = 1.56e-7; I_off_n[15] = 1.57e-7; 
      I_off_n[16] = 1.57e-7; I_off_n[17] = 1.58e-7; I_off_n[18] = 1.58e-7; I_off_n[19] = 1.58e-7; I_off_n[20] = 1.59e-7;
      I_off_n[21] = 1.60e-7; I_off_n[22] = 1.60e-7; I_off_n[23] = 1.61e-7; I_off_n[24] = 1.62e-7; I_off_n[25] = 1.63e-7; 
      I_off_n[26] = 1.64e-7; I_off_n[27] = 1.65e-7; I_off_n[28] = 1.66e-7; I_off_n[29] = 1.67e-7; I_off_n[30] = 1.68e-7;
      I_off_n[31] = 1.69e-7; I_off_n[32] = 1.71e-7; I_off_n[33] = 1.72e-7; I_off_n[34] = 1.74e-7; I_off_n[35] = 1.76e-7; 
      I_off_n[36] = 1.79e-7; I_off_n[37] = 1.81e-7; I_off_n[38] = 1.84e-7; I_off_n[39] = 1.87e-7; I_off_n[40] = 1.90e-7;
      I_off_n[41] = 1.94e-7; I_off_n[42] = 1.99e-7; I_off_n[43] = 2.04e-7; I_off_n[44] = 2.11e-7; I_off_n[45] = 2.19e-7; 
      I_off_n[46] = 2.27e-7; I_off_n[47] = 2.36e-7; I_off_n[48] = 2.46e-7; I_off_n[49] = 2.57e-7; I_off_n[50] = 2.69e-7;
      I_off_n[51] = 2.83e-7; I_off_n[52] = 3.01e-7; I_off_n[53] = 3.22e-7; I_off_n[54] = 3.46e-7; I_off_n[55] = 3.73e-7; 
      I_off_n[56] = 4.02e-7; I_off_n[57] = 4.33e-7; I_off_n[58] = 4.65e-7; I_off_n[59] = 4.98e-7; I_off_n[60] = 5.32e-7;
      I_off_n[61] = 5.69e-7; I_off_n[62] = 6.09e-7; I_off_n[63] = 6.54e-7; I_off_n[64] = 7.01e-7; I_off_n[65] = 7.51e-7; 
      I_off_n[66] = 8.04e-7; I_off_n[67] = 8.57e-7; I_off_n[68] = 9.11e-7; I_off_n[69] = 9.66e-7; I_off_n[70] = 1.02e-6;
      I_off_n[71] = 1.07e-6; I_off_n[72] = 1.13e-6; I_off_n[73] = 1.18e-6; I_off_n[74] = 1.23e-6; I_off_n[75] = 1.29e-6; 
      I_off_n[76] = 1.35e-6; I_off_n[77] = 1.41e-6; I_off_n[78] = 1.48e-6; I_off_n[79] = 1.54e-6; I_off_n[80] = 1.62e-6;
      I_off_n[81] = 1.70e-6; I_off_n[82] = 1.78e-6; I_off_n[83] = 1.87e-6; I_off_n[84] = 1.96e-6; I_off_n[85] = 2.06e-6; 
      I_off_n[86] = 2.17e-6; I_off_n[87] = 2.29e-6; I_off_n[88] = 2.43e-6; I_off_n[89] = 2.57e-6; I_off_n[90] = 2.73e-6;
      I_off_n[91] = 2.92e-6; I_off_n[92] = 3.15e-6; I_off_n[93] = 3.42e-6; I_off_n[94] = 3.72e-6; I_off_n[95] = 4.06e-6; 
      I_off_n[96] = 4.43e-6; I_off_n[97] = 4.82e-6; I_off_n[98] = 5.23e-6; I_off_n[99] = 5.66e-6; I_off_n[100] = 6.10e-6;
      break;
	  
    case 45:
      Vdd = 1.0; // vdd[0] from McPAT
      Vt = 0.18035; // v_th[0] from McPAT
      tox = 0.65e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.41; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 2.46; // horiz_dielectric_constant[1][0]
      L = 0.01345; // Lelec[0] from McPAT
      Idsat_spec = 2046.6e-6; // I_on_n[0] from McPAT
      alpha = 1.38; // gmp_to_gmn_multiplier[0] from McPAT

      I_off_n[0] = 2.80e-7; 
      I_off_n[1] = 2.85e-7; I_off_n[2] = 2.89e-7; I_off_n[3] = 2.94e-7; I_off_n[4] = 2.99e-7; I_off_n[5] = 3.03e-7; 
      I_off_n[6] = 3.08e-7; I_off_n[7] = 3.13e-7; I_off_n[8] = 3.18e-7; I_off_n[9] = 3.23e-7; I_off_n[10] = 3.28e-7;
      I_off_n[11] = 3.33e-7; I_off_n[12] = 3.38e-7; I_off_n[13] = 3.43e-7; I_off_n[14] = 3.49e-7; I_off_n[15] = 3.54e-7; 
      I_off_n[16] = 3.59e-7; I_off_n[17] = 3.65e-7; I_off_n[18] = 3.70e-7; I_off_n[19] = 3.75e-7; I_off_n[20] = 3.81e-7;
      I_off_n[21] = 3.87e-7; I_off_n[22] = 3.92e-7; I_off_n[23] = 3.98e-7; I_off_n[24] = 4.04e-7; I_off_n[25] = 4.09e-7; 
      I_off_n[26] = 4.15e-7; I_off_n[27] = 4.21e-7; I_off_n[28] = 4.27e-7; I_off_n[29] = 4.33e-7; I_off_n[30] = 4.39e-7;
      I_off_n[31] = 4.45e-7; I_off_n[32] = 4.51e-7; I_off_n[33] = 4.57e-7; I_off_n[34] = 4.64e-7; I_off_n[35] = 4.70e-7; 
      I_off_n[36] = 4.76e-7; I_off_n[37] = 4.83e-7; I_off_n[38] = 4.89e-7; I_off_n[39] = 4.96e-7; I_off_n[40] = 5.02e-7;
      I_off_n[41] = 5.09e-7; I_off_n[42] = 5.15e-7; I_off_n[43] = 5.22e-7; I_off_n[44] = 5.28e-7; I_off_n[45] = 5.35e-7; 
      I_off_n[46] = 5.42e-7; I_off_n[47] = 5.48e-7; I_off_n[48] = 5.55e-7; I_off_n[49] = 5.62e-7; I_off_n[50] = 5.69e-7;
      I_off_n[51] = 5.76e-7; I_off_n[52] = 5.83e-7; I_off_n[53] = 5.90e-7; I_off_n[54] = 5.98e-7; I_off_n[55] = 6.05e-7; 
      I_off_n[56] = 6.12e-7; I_off_n[57] = 6.20e-7; I_off_n[58] = 6.27e-7; I_off_n[59] = 6.34e-7; I_off_n[60] = 6.42e-7;
      I_off_n[61] = 6.50e-7; I_off_n[62] = 6.57e-7; I_off_n[63] = 6.65e-7; I_off_n[64] = 6.73e-7; I_off_n[65] = 6.80e-7; 
      I_off_n[66] = 6.88e-7; I_off_n[67] = 6.96e-7; I_off_n[68] = 7.04e-7; I_off_n[69] = 7.12e-7; I_off_n[70] = 7.20e-7;
      I_off_n[71] = 7.28e-7; I_off_n[72] = 7.36e-7; I_off_n[73] = 7.44e-7; I_off_n[74] = 7.53e-7; I_off_n[75] = 7.61e-7; 
      I_off_n[76] = 7.69e-7; I_off_n[77] = 7.78e-7; I_off_n[78] = 7.86e-7; I_off_n[79] = 7.94e-7; I_off_n[80] = 8.03e-7;
      I_off_n[81] = 8.12e-7; I_off_n[82] = 8.20e-7; I_off_n[83] = 8.29e-7; I_off_n[84] = 8.38e-7; I_off_n[85] = 8.46e-7; 
      I_off_n[86] = 8.55e-7; I_off_n[87] = 8.64e-7; I_off_n[88] = 8.73e-7; I_off_n[89] = 8.82e-7; I_off_n[90] = 8.91e-7;
      I_off_n[91] = 9.00e-7; I_off_n[92] = 9.09e-7; I_off_n[93] = 9.18e-7; I_off_n[94] = 9.28e-7; I_off_n[95] = 9.37e-7; 
      I_off_n[96] = 9.46e-7; I_off_n[97] = 9.56e-7; I_off_n[98] = 9.65e-7; I_off_n[99] = 9.74e-7; I_off_n[100] = 9.84e-7;
      break;
	  
    case 65:
      Vdd = 1.1; // vdd[0] from McPAT
      Vt = 0.19491; // v_th[0] from McPAT
      tox = 1.1e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.41; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 2.734; // horiz_dielectric_constant[1][0]
      L = 0.019; // Lelec[0] from McPAT
      Idsat_spec = 1197.2e-6; // I_on_n[0] from McPAT
      alpha = 1.38; // gmp_to_gmn_multiplier[0] from McPAT
	  
      I_off_n[0] = 1.96e-7; 
      I_off_n[1] = 1.99e-7; I_off_n[2] = 2.02e-7; I_off_n[3] = 2.05e-7; I_off_n[4] = 2.09e-7; I_off_n[5] = 2.12e-7; 
      I_off_n[6] = 2.15e-7; I_off_n[7] = 2.19e-7; I_off_n[8] = 2.22e-7; I_off_n[9] = 2.26e-7; I_off_n[10] = 2.29e-7;
      I_off_n[11] = 2.33e-7; I_off_n[12] = 2.36e-7; I_off_n[13] = 2.40e-7; I_off_n[14] = 2.43e-7; I_off_n[15] = 2.47e-7; 
      I_off_n[16] = 2.51e-7; I_off_n[17] = 2.55e-7; I_off_n[18] = 2.58e-7; I_off_n[19] = 2.62e-7; I_off_n[20] = 2.66e-7;
      I_off_n[21] = 2.70e-7; I_off_n[22] = 2.74e-7; I_off_n[23] = 2.77e-7; I_off_n[24] = 2.81e-7; I_off_n[25] = 2.85e-7; 
      I_off_n[26] = 2.89e-7; I_off_n[27] = 2.93e-7; I_off_n[28] = 2.97e-7; I_off_n[29] = 3.01e-7; I_off_n[30] = 3.05e-7;
      I_off_n[31] = 3.09e-7; I_off_n[32] = 3.13e-7; I_off_n[33] = 3.18e-7; I_off_n[34] = 3.22e-7; I_off_n[35] = 3.27e-7; 
      I_off_n[36] = 3.31e-7; I_off_n[37] = 3.35e-7; I_off_n[38] = 3.40e-7; I_off_n[39] = 3.44e-7; I_off_n[40] = 3.49e-7;
      I_off_n[41] = 3.53e-7; I_off_n[42] = 3.58e-7; I_off_n[43] = 3.63e-7; I_off_n[44] = 3.67e-7; I_off_n[45] = 3.72e-7; 
      I_off_n[46] = 3.76e-7; I_off_n[47] = 3.81e-7; I_off_n[48] = 3.86e-7; I_off_n[49] = 3.90e-7; I_off_n[50] = 3.95e-7;
      I_off_n[51] = 4.00e-7; I_off_n[52] = 4.05e-7; I_off_n[53] = 4.10e-7; I_off_n[54] = 4.15e-7; I_off_n[55] = 4.20e-7; 
      I_off_n[56] = 4.25e-7; I_off_n[57] = 4.30e-7; I_off_n[58] = 4.35e-7; I_off_n[59] = 4.40e-7; I_off_n[60] = 4.45e-7;
      I_off_n[61] = 4.50e-7; I_off_n[62] = 4.55e-7; I_off_n[63] = 4.60e-7; I_off_n[64] = 4.66e-7; I_off_n[65] = 4.71e-7; 
      I_off_n[66] = 4.76e-7; I_off_n[67] = 4.81e-7; I_off_n[68] = 4.87e-7; I_off_n[69] = 4.92e-7; I_off_n[70] = 4.97e-7;
      I_off_n[71] = 5.02e-7; I_off_n[72] = 5.07e-7; I_off_n[73] = 5.13e-7; I_off_n[74] = 5.18e-7; I_off_n[75] = 5.23e-7; 
      I_off_n[76] = 5.28e-7; I_off_n[77] = 5.33e-7; I_off_n[78] = 5.38e-7; I_off_n[79] = 5.43e-7; I_off_n[80] = 5.48e-7;
      I_off_n[81] = 5.53e-7; I_off_n[82] = 5.58e-7; I_off_n[83] = 5.63e-7; I_off_n[84] = 5.67e-7; I_off_n[85] = 5.72e-7; 
      I_off_n[86] = 5.77e-7; I_off_n[87] = 5.81e-7; I_off_n[88] = 5.86e-7; I_off_n[89] = 5.90e-7; I_off_n[90] = 5.94e-7;
      I_off_n[91] = 5.98e-7; I_off_n[92] = 6.02e-7; I_off_n[93] = 6.06e-7; I_off_n[94] = 6.10e-7; I_off_n[95] = 6.13e-7; 
      I_off_n[96] = 6.17e-7; I_off_n[97] = 6.20e-7; I_off_n[98] = 6.24e-7; I_off_n[99] = 6.27e-7; I_off_n[100] = 6.30e-7;
      break;

    case 90:
      Vdd = 1.2; // vdd[0] from McPAT
      Vt = 0.23707; // v_th[0] from McPAT
      tox = 1.2e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.45; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 3.038; // horiz_dielectric_constant[1][0]
      L = 0.0266; // Lelec[0] from McPAT
      Idsat_spec = 1076.9e-6; // I_on_n[0] from McPAT
      alpha = 1.22; // gmp_to_gmn_multiplier[0] from McPAT
	  
      I_off_n[0] = 3.24e-8; 
      I_off_n[1] = 3.31e-8; I_off_n[2] = 3.38e-8; I_off_n[3] = 3.46e-8; I_off_n[4] = 3.53e-8; I_off_n[5] = 3.61e-8; 
      I_off_n[6] = 3.69e-8; I_off_n[7] = 3.77e-8; I_off_n[8] = 3.85e-8; I_off_n[9] = 3.93e-8; I_off_n[10] = 4.01e-8;
      I_off_n[11] = 4.09e-8; I_off_n[12] = 4.18e-8; I_off_n[13] = 4.26e-8; I_off_n[14] = 4.35e-8; I_off_n[15] = 4.44e-8; 
      I_off_n[16] = 4.53e-8; I_off_n[17] = 4.62e-8; I_off_n[18] = 4.71e-8; I_off_n[19] = 4.81e-8; I_off_n[20] = 4.90e-8;
      I_off_n[21] = 5.00e-8; I_off_n[22] = 5.09e-8; I_off_n[23] = 5.19e-8; I_off_n[24] = 5.29e-8; I_off_n[25] = 5.39e-8; 
      I_off_n[26] = 5.50e-8; I_off_n[27] = 5.60e-8; I_off_n[28] = 5.71e-8; I_off_n[29] = 5.81e-8; I_off_n[30] = 5.92e-8;
      I_off_n[31] = 6.03e-8; I_off_n[32] = 6.14e-8; I_off_n[33] = 6.25e-8; I_off_n[34] = 6.37e-8; I_off_n[35] = 6.48e-8; 
      I_off_n[36] = 6.60e-8; I_off_n[37] = 6.72e-8; I_off_n[38] = 6.84e-8; I_off_n[39] = 6.96e-8; I_off_n[40] = 7.08e-8;
      I_off_n[41] = 7.20e-8; I_off_n[42] = 7.33e-8; I_off_n[43] = 7.45e-8; I_off_n[44] = 7.58e-8; I_off_n[45] = 7.71e-8; 
      I_off_n[46] = 7.84e-8; I_off_n[47] = 7.98e-8; I_off_n[48] = 8.11e-8; I_off_n[49] = 8.24e-8; I_off_n[50] = 8.38e-8;
      I_off_n[51] = 8.52e-8; I_off_n[52] = 8.66e-8; I_off_n[53] = 8.80e-8; I_off_n[54] = 8.94e-8; I_off_n[55] = 9.08e-8; 
      I_off_n[56] = 9.23e-8; I_off_n[57] = 9.37e-8; I_off_n[58] = 9.52e-8; I_off_n[59] = 9.67e-8; I_off_n[60] = 9.82e-8;
      I_off_n[61] = 9.97e-8; I_off_n[62] = 1.01e-7; I_off_n[63] = 1.03e-7; I_off_n[64] = 1.04e-7; I_off_n[65] = 1.06e-7; 
      I_off_n[66] = 1.08e-7; I_off_n[67] = 1.09e-7; I_off_n[68] = 1.11e-7; I_off_n[69] = 1.12e-7; I_off_n[70] = 1.14e-7;
      I_off_n[71] = 1.16e-7; I_off_n[72] = 1.17e-7; I_off_n[73] = 1.19e-7; I_off_n[74] = 1.20e-7; I_off_n[75] = 1.22e-7; 
      I_off_n[76] = 1.23e-7; I_off_n[77] = 1.25e-7; I_off_n[78] = 1.26e-7; I_off_n[79] = 1.28e-7; I_off_n[80] = 1.29e-7;
      I_off_n[81] = 1.30e-7; I_off_n[82] = 1.32e-7; I_off_n[83] = 1.33e-7; I_off_n[84] = 1.35e-7; I_off_n[85] = 1.36e-7; 
      I_off_n[86] = 1.38e-7; I_off_n[87] = 1.39e-7; I_off_n[88] = 1.40e-7; I_off_n[89] = 1.42e-7; I_off_n[90] = 1.43e-7;
      I_off_n[91] = 1.44e-7; I_off_n[92] = 1.45e-7; I_off_n[93] = 1.47e-7; I_off_n[94] = 1.48e-7; I_off_n[95] = 1.49e-7; 
      I_off_n[96] = 1.50e-7; I_off_n[97] = 1.51e-7; I_off_n[98] = 1.52e-7; I_off_n[99] = 1.53e-7; I_off_n[100] = 1.54e-7;
      break;

    case 180:
      Vdd = 1.5; // vdd[0] from McPAT
      Vt = 0.4407; // v_th[0] from McPAT
      tox = 2.4e-9; // t_ox[0] from McPAT
      drive_p_div_n = 1.0/2.45; // n_to_p_eff_curr_drv_ratio[0] from McPAT
      ar = 2.0; // aspect_ratio[1][0] from McPAT
      er = 3.038; // horiz_dielectric_constant[1][0]
      L = 0.10; // Lelec[0] from McPAT
      Idsat_spec = 750e-6; // I_on_n[0] from McPAT
      alpha = 1.22; // gmp_to_gmn_multiplier[0] from McPAT

      I_off_n[0] = 7.00e-10; 
      I_off_n[1] = 7.12e-10; I_off_n[2] = 7.23e-10; I_off_n[3] = 7.36e-10; I_off_n[4] = 7.48e-10; I_off_n[5] = 7.60e-10; 
      I_off_n[6] = 7.73e-10; I_off_n[7] = 7.86e-10; I_off_n[8] = 7.99e-10; I_off_n[9] = 8.12e-10; I_off_n[10] = 8.26e-10;
      I_off_n[11] = 8.40e-10; I_off_n[12] = 8.54e-10; I_off_n[13] = 8.68e-10; I_off_n[14] = 8.82e-10; I_off_n[15] = 8.97e-10; 
      I_off_n[16] = 9.12e-10; I_off_n[17] = 9.27e-10; I_off_n[18] = 9.42e-10; I_off_n[19] = 9.58e-10; I_off_n[20] = 9.74e-10;
      I_off_n[21] = 9.90e-10; I_off_n[22] = 1.01e-9; I_off_n[23] = 1.02e-9; I_off_n[24] = 1.04e-9; I_off_n[25] = 1.06e-9; 
      I_off_n[26] = 1.08e-9; I_off_n[27] = 1.09e-9; I_off_n[28] = 1.11e-9; I_off_n[29] = 1.13e-9; I_off_n[30] = 1.15e-9;
      I_off_n[31] = 1.17e-9; I_off_n[32] = 1.19e-9; I_off_n[33] = 1.21e-9; I_off_n[34] = 1.23e-9; I_off_n[35] = 1.25e-9; 
      I_off_n[36] = 1.27e-9; I_off_n[37] = 1.29e-9; I_off_n[38] = 1.31e-9; I_off_n[39] = 1.33e-9; I_off_n[40] = 1.35e-9;
      I_off_n[41] = 1.37e-9; I_off_n[42] = 1.40e-9; I_off_n[43] = 1.42e-9; I_off_n[44] = 1.44e-9; I_off_n[45] = 1.47e-9; 
      I_off_n[46] = 1.50e-9; I_off_n[47] = 1.52e-9; I_off_n[48] = 1.55e-9; I_off_n[49] = 1.57e-9; I_off_n[50] = 1.60e-9;
      I_off_n[51] = 1.63e-9; I_off_n[52] = 1.65e-9; I_off_n[53] = 1.68e-9; I_off_n[54] = 1.70e-9; I_off_n[55] = 1.73e-9; 
      I_off_n[56] = 1.76e-9; I_off_n[57] = 1.79e-9; I_off_n[58] = 1.82e-9; I_off_n[59] = 1.85e-9; I_off_n[60] = 1.88e-9;
      I_off_n[61] = 1.91e-9; I_off_n[62] = 1.95e-9; I_off_n[63] = 1.99e-9; I_off_n[64] = 2.03e-9; I_off_n[65] = 2.08e-9; 
      I_off_n[66] = 2.12e-9; I_off_n[67] = 2.16e-9; I_off_n[68] = 2.21e-9; I_off_n[69] = 2.25e-9; I_off_n[70] = 2.29e-9;
      I_off_n[71] = 2.33e-9; I_off_n[72] = 2.37e-9; I_off_n[73] = 2.41e-9; I_off_n[74] = 2.45e-9; I_off_n[75] = 2.49e-9; 
      I_off_n[76] = 2.53e-9; I_off_n[77] = 2.57e-9; I_off_n[78] = 2.61e-9; I_off_n[79] = 2.66e-9; I_off_n[80] = 2.70e-9;
      I_off_n[81] = 2.75e-9; I_off_n[82] = 2.79e-9; I_off_n[83] = 2.84e-9; I_off_n[84] = 2.89e-9; I_off_n[85] = 2.93e-9; 
      I_off_n[86] = 2.98e-9; I_off_n[87] = 3.03e-9; I_off_n[88] = 3.09e-9; I_off_n[89] = 3.14e-9; I_off_n[90] = 3.19e-9;
      I_off_n[91] = 3.24e-9; I_off_n[92] = 3.30e-9; I_off_n[93] = 3.35e-9; I_off_n[94] = 3.41e-9; I_off_n[95] = 3.46e-9; 
      I_off_n[96] = 3.52e-9; I_off_n[97] = 3.58e-9; I_off_n[98] = 3.64e-9; I_off_n[99] = 3.70e-9; I_off_n[100] = 3.76e-9;
      break;
	  
    default:
      fprintf(stdout,"ERROR (IntSim): Invalid technology node %dnm\n",int(F*1e9));
      exit(1);
  }
  
  Ileak_spec = I_off_n[(int)temp-300]*L;
  Idsat_spec *= L;

  F1 = F;
  Vdd_spec = Vdd;
  Vt_spec = Vt;
  kai = 4.0/(fo+3.0);
  alpha_wire = fo/(fo+1.0);
  ro = calc_ro(Vdd,Vt,Vdd_spec,Vt_spec,Idsat_spec,alpha);
  co = calc_co(tox,F);
  H = F;
  W = F;
  T = ar*F;
  S = F;
  cg = calc_cg(W,T,H,S,er);
  cm = calc_cm(W,T,H,S,er);
  ew = router_eff;
  W_global = kc*1e-6;
  T_global = ar*1e-6;
  H_global = ar*1e-6;
  S_global = 1e-6;
  c_clock = 2.0*calc_cg(W_global,T_global,H_global,S_global,er)+2.0*calc_cm(W_global,T_global,H_global,S_global,er);
}
