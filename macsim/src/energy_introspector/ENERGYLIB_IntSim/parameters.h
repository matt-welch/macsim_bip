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

#ifndef PARAM_T
#define PARAM_T

#include "../definitions.h"

class intsim_param_t {
public:
  intsim_param_t() {}
  ~intsim_param_t() {}

  double calc_cg(double W, double T, double H, double S, double er);
  double calc_cm(double W, double T, double H, double S, double er);
  double calc_co(double tox, double F);
  double calc_ro(double Vdd, double Vt, double Vdd_spec, double Vt_spec, double Idsat_spec, double alpha);
  void setup(void);

  // User defined parameters	
  bool user_set;	

  // Default system parameters
  double Vdd; // Supply voltage
  double Vt; // Threshold voltage
  double f; // Clock frequency
  double ncp; // Number of gates on a critical path
  double k; // Rent's constant k
  double p; // Rent's constant p
  double a; // Activity factor
  double A; // Die area
  double temp; // Average chip temperature;
  double ngates; // Number of gates
  double pgates; // Percentage of chip area taken by logic gates for a given wgate
  double nsockets; // ngates/pgates

  // Default device parameters
  double F; // Minimum feature size
  double L; // Electrical length
  double Idsat_spec; // Saturation drain current for a minimum-size NFET
  double Ileak_spec; // Leakage current of a minimum-size NFET
  double Vdd_spec; // Vdd at which Idsat and Ileak are specified
  double Vt_spec; // Vt at which Idsat and Ileak are specified
  double tox; // Effective oxide thickness
  double alpha; // Alpha value of the power-law MOSFET model
  double drive_p_div_n; // Ratio of drive current of pMOS and nMOS
  double subvtslope_spec; // Subthreshold slope at 85 degrees Celsius
  double F1; // Minimum wire pitch
  double s; // Design rule for vias
  double device_width; // wgate

  // Default interconnect package parameters
  double er; // Dielectric permitivity
  double rho; // Resistivity of copper
  double ar; // Aspect ratio of wiring levels
  double R_coeff; // Reflectivity coefficient at grain boundaries for copper
  double p_size; // Specularity parameter
  double npower_pads; // Number of power pads
  double pad_to_pad_distance; // Average distance from one power pad to the next
  double pad_length; // Length of a pad
  double ir_drop_limit; // IR drop limit in percentage, half for each global and local

  // Default design parameters
  double router_eff; // Router efficiency
  double rep_eff; // Repeaters efficiency
  double fo; // Average fan-out of logic gates
  double margin; // Percentage of clock cycle lost due to the process variation and clock skew
  double D;	// Max span of H tree that needs to be driven
  double latches_per_buffer; // Latches per buffer
  double clock_factor; // Clock factor (number of latches = clock factor x total number of gates / number of gates on a critical path)
  double clock_gating_factor; // Percentage of local clock power saved by clock gating
  double kai; // Point-to-point to net length correction factor
  double alpha_wire; // Alpha wire
  double ro; // Output resistance of a minimum-size inverter
  double co; // Input capacitance of a minimum-size inverter

  // Other parameters assumed
  double ew_power_ground;
  double H; // ILD thickness
  double W;	// Wire width
  double T;	// Wire thickness
  double S;	// Spacing between wires
  double cg; // Ground capacitance per unit length of a wire
  double cm; // Ground capacitance per unit length of a wire
  double kp; // Ratio of widths of power and signal wires
  double kc; // Ratio of widths of clock and signal wires
  double ew; // Wiring efficiency considering only via blockage and router efficiency
  double beta_clock; // Max slew allowable on clock wire
  double c_clock; // Capacitance per unit length of clock wire
  int max_tier;	// Limit of tiers for iteration

  // Dimensions of wires
  double W_global;
  double T_global;
  double H_global;
  double S_global;
  
  double I_off_n[TEMP_DEGREE_STEPS];
};

#endif
