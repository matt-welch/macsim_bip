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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "chip.h"

/*	
	area_two_input_nand_gate_calc.m
	This function calculates the area of a two-input NAND gate.
	This function could be useful for calculating area of critical path gates.
	-------------------------------------------------------------------------------------
	Reference: R. Venkatesan, PhD thesis, Georgia Tech, 2003
	Available at www.ece.gatech.edu/research/labs/gsigroup under the publications section
	-------------------------------------------------------------------------------------
*/
double calc_area_two_input_nand (double F, double device_width) {
	double ki = 102.0;
	double gar = 17.0/6.0;
	double fi = 2.0;
	double betag = 2.0/fi;
	return ki*(1.0+4.0*sqrt(gar)*(fi-1.0)/sqrt(ki))*(1.0+(1.0+betag)*(device_width-1.0)/sqrt(ki*gar))*pow(F,2.0);
}

/*	
	area_inv_calc.m
	This function calculates the area of an inverter.
	This function could be useful for calculating repeater area.
	-------------------------------------------------------------------------------------
	Reference: R. Venkatesan, PhD thesis, Georgia Tech, 2003
	Available at www.ece.gatech.edu/research/labs/gsigroup under the publications section
	-------------------------------------------------------------------------------------
*/
double calc_area_inv (double F, double device_width) {
	double ki = 102.0;
	double gar = 17.0/6.0;
	double fi = 1.0;
	double betag = 2.0/fi;
	return ki*(1.0+4.0*sqrt(gar)*(fi-1.0)/sqrt(ki))*(1.0+(1.0+betag)*(device_width-1.0)/sqrt(ki*gar))*pow(F,2.0);
}

/*	
	av_wire_cap_calc.m
	This function computes the wire capacitance that needs to be driven by an average-size gate.
	-------------------------------------------------------------------------------------
	Reference: R. Venkatesan, PhD thesis, Georgia Tech, 2003
	Available at www.ece.gatech.edu/research/labs/gsigroup under the publications section
	-------------------------------------------------------------------------------------
*/
double calc_av_wire_cap (double cg, double cm, double k, double p, double A, double ngates, double pgates, double fo) {
	double nsockets = ngates/pgates;
	double lavg_nr = (p-0.5)/p-sqrt(nsockets)-(p-0.5)/6.0/sqrt(nsockets)/(p+0.5)+pow(nsockets,p)*(-p-1.0+pow(4.0,p-0.5))/2.0/(p+0.5)/p/(p-1.0);
	double lavg_dr = pow(nsockets,p-0.5)*(-2.0*p-1.0+pow(2.0,2.0*p-1))/2.0/p/(p-1.0)/(2.0*p-3.0)-(p-0.5)/6.0/p/sqrt(nsockets)+1.0-(p-0.5)*sqrt(nsockets)/(p-1.0);
	double lavg = lavg_nr/lavg_dr*sqrt(pgates);
	return 4.0*fo/(fo+3.0)*lavg*sqrt(A/ngates)*(2.0*cg+2.0*cm);
}

/*	
	two_input_nand_gate_sizing.m
	This function sizes NAND gates based on logic depth, etc.
	Effective fan-in of series-connected MOSFETs is 1.5 for two-input NAND gate.
	-------------------------------------------------------------------------------------
	Reference: R. Venkatesan, PhD thesis, Georgia Tech, 2003
	Available at www.ece.gatech.edu/research/labs/gsigroup under the publications section
	-------------------------------------------------------------------------------------
*/
int sizing_two_input_nand (double &pgates, double &device_width, double ngates, double A, double cg, double cm, double ro,
							double tox, double F, double ncp, double f, double margin, double drive_p_div_n, double k, double p, double fo){
	// Resistance and capacitance of NMOS in the NAND gate
	double eo = 8.854e-12;
	double Cox = 3.9*eo/tox;
	double cnmos = pow(F,2)*Cox;
	double cpmos = cnmos;

	// Resistance and capacitance of PMOS in the NAND gate
	double rpmos = ro/drive_p_div_n;

	// Total input capacitance of two-input NAND gate assuming that PMOS and NMOS are same size
	double cnand = 2*cnmos+2*cpmos;
	double av_wire_cap;
	for(double wgate = 1; wgate <= 40; wgate++) {
		// The percentage of chip area taken by logic gates for a given wgate
		pgates = ngates*calc_area_two_input_nand(F,wgate)/A;
		// Average wire capacitance for a given wgate
		av_wire_cap = calc_av_wire_cap(cg,cm,k,p,A,ngates,pgates,fo);
		// wgate selection, if the allowed frequency for a certain wgate is higher than required
		if((1/(0.5*(1/(1-margin)*ncp*(1.5*0.7*ro/wgate*(cnand*wgate*fo+av_wire_cap))
			+1/(1-margin)*ncp*(0.7*rpmos/wgate*(cnand*wgate*fo+av_wire_cap)))))>f) { 
				device_width = wgate; 
				return true; 
		}
		else device_width = 0;
	}
	return false;
}

/* 
	Initialize the arrays to all zeros
*/
void clear(double *arrays, int size){
	for(int i = 0; i < size; i++)
		arrays[i] = 0;
}

/* 
	Simple ascend sorting function
*/
void ascend(double *arrays, int size) {
	double min, temp;
	for(int i = 0; i < size; i++) {
		min = arrays[i];
		for(int j = i+1; j < size; j++) {
			if(arrays[j] < min) {
				temp = arrays[i];
				arrays[i] = arrays[j];
				arrays[j] = temp;
			}
		}
	}
}

/*	
	power_calc_two_input_nand.m
	This function computes the leakage and dynamic power of a minimum-feature-size two-input NAND gate.
	This is useful to find the total power of the gate.
*/
void calc_power_two_input_nand (double &dyn_power_two_input_nand, double &leakage_power_two_input_nand, double device_width, 
								double F, double tox, double a, double Ileak_spec, double Vdd_spec, double Vt_spec, double subvtslope_spec, 
								double Vdd, double Vt, double f) {
	double eo = 8.854e-12;
	double Cox = 3.9*eo/tox;
	double C_device = 4*pow(F,2)*Cox;

	// Dynamic power
	dyn_power_two_input_nand = 0.5*a*device_width*C_device*pow(Vdd,2)*f;

	// Leakage power (leakage width of a NAND gate = 1.25 * width of minimum-feature-size transistor)
	leakage_power_two_input_nand = 1.125*device_width*Vdd*Ileak_spec/pow(10,-Vt_spec/subvtslope_spec)*pow(10,-Vt/subvtslope_spec);
}

/* 
	power_calc_inv.m
	This function calculates the power of a minimum-feature-size inverter.
	This function can be used to calculate the power of repeater.
*/
void calc_power_inv (double &dyn_power_inv, double &leakage_power_inv, double co, double Vdd, double Vt, double f, double a, 
					 double Ileak_spec, double Vdd_spec, double Vt_spec, double subvtslope_spec, double drive_p_div_n) {
	// Dynamic power
	dyn_power_inv = 0.5*a*co*pow(Vdd,2)*f;

	// Leakage power
	leakage_power_inv = 0.5*(1+1/drive_p_div_n)*Vdd*Ileak_spec/pow(10,-Vt_spec/subvtslope_spec)*pow(10,-Vt/subvtslope_spec);
}

// alpha_size
double alpha_sizer(double factor, intsim_param_t * intsim_param) {
	return 38e-9/factor*intsim_param->R_coeff/(1-intsim_param->R_coeff);
}

// rho_size
double rho_sizer(double alpha_size_factor, double factor, intsim_param_t * intsim_param) {
	return 1.67e-8*(1.0/3/(1.0/3-alpha_size_factor/2+pow(alpha_size_factor,2)-pow(alpha_size_factor,3)*log(1.0+1.0/alpha_size_factor))+3.0/8*1.2*(1-intsim_param->p_size)*(1+intsim_param->ar)/intsim_param->ar*38e-9/factor);
}


/*
	repeater_calc_edp.m
	This function inserts repeaters into the tier if pitch after repeater is greater than or equal to 2*F,
	the number and size of repeaters are greater than 1, and area needed is smaller than available area.

*/
void calc_repeater_edp (double &lmin, double &pitch_rep, double &number_rep, double &size_rep, double &rep_flag, double &area_rep, 
						double &power_rep, double &net_wire_eff_for_this_level, double phigate, double pgates, double total_number_repeaters, 
						double nv_power_ground, double leakage_power_inv, double dyn_power_inv, double R_coeff, double p_size, 
						double max_power_repeaters, double ar, double lmax, double ro, double co, double available_area, double F, double cg, 
						double cm, double f, double margin, double rho, double router_eff, double A, double ngates, double p, 
						double alpha_wire, double k, double kai, double s, double F1,intsim_param_t * intsim_param) {
	double c = 2.0*cg+2.0*cm;
	double nsockets = ngates/pgates;
	double p1_new = 0.0;
	double p1 = 5.45*lmax/(1.0-margin)*f*sqrt(c*rho/ar*ro*co*A/nsockets);
	double p1_dummy = p1;
	double gamma_rep = pow(0.73+0.07*log(phigate),2);
	double delta_rep = pow(0.88+0.07*log(phigate),2);
	double alpha_size, rho_size;
	while (fabs(p1_dummy-p1_new) > 1e-10) {
		alpha_size = alpha_sizer(p1/2,intsim_param);
		rho_size = rho_sizer(alpha_size,p1/2,intsim_param);
		p1_new = (0.7/delta_rep+0.7*gamma_rep+0.7*delta_rep+0.4/gamma_rep)*2*lmax/(1-margin)*f*sqrt(c*rho_size/ar*ro*co*A/nsockets);
		p1_dummy = p1;
		p1 = (p1_new+p1)/2;
	}
	pitch_rep = p1;

	// Integral of (length * wire length distribution)
	double gamma = 2.0*ngates*(1-pow(ngates,p-1))/(-pow(nsockets,p)*(1+2*p-pow(2,2*p-1))/(p*(2*p-1)*(p-1)*(2*p-3))-(1/(6*p))
		+(2*sqrt(nsockets)/(2*p-1))-nsockets/(p-1));
	double kk = gamma*alpha_wire*k, y1, y2, y;

	double *intg = (double*)malloc(sizeof(double)*(int)floor(2*sqrt(nsockets)));
	double *intg_n = (double*)malloc(sizeof(double)*(int)floor(2*sqrt(nsockets)));

	for(double i = 1.0; i <= 2.0*sqrt(nsockets); i++) {
		y1 = (kk/2)*((1.0/3)*pow(i,3)-2*sqrt(nsockets)*pow(i,2)+2*nsockets*i)*pow(i,2*p-4);
		y2 = (kk/6)*pow(2.0*sqrt(nsockets)-i,3)*pow(i,2*p-4);
		y = (y1*(((sqrt(nsockets)-i)>0 ? 1:-1)+1)+y2*(((i-sqrt(nsockets))>0 ? 1:-1)+1))/2;
		if(i > 1) { 
			intg[(int)i-1] = i*y+intg[(int)i-2]; 
			intg_n[(int)i-1] = y+intg_n[(int)i-2]; 
		}
		else { 
			intg[(int)i-1] = i*y; 
			intg_n[(int)i-1] = y; 
		}
	}
	net_wire_eff_for_this_level = router_eff - sqrt((2.0*total_number_repeaters+nv_power_ground+2*(intg_n[(int)floor(2*sqrt(nsockets))-1]-intg_n[(int)floor(lmax)-1]))*pow(pitch_rep+s*F1/2,2)/A);

	double value1;
	for(double l = 1; l <= 2*sqrt(nsockets); l++) {
		value1 = net_wire_eff_for_this_level*2*A/kai/pitch_rep/sqrt(A/nsockets);
		if((intg[(int)floor(lmax)-1]-intg[(int)floor(l)-1]) < value1) { 
			lmin = l; 
			break; 
		}
	}
	alpha_size = alpha_sizer(pitch_rep/2,intsim_param);
	rho_size = rho_sizer(alpha_size,pitch_rep/2,intsim_param);
	double r = 4.0*rho_size/ar/pow(pitch_rep,2);
	gamma_rep = pow(0.73+0.07*log(phigate),2);
	delta_rep = pow(0.88+0.07*log(phigate),2);
	double total_length = (intg[(int)floor(lmax)-1]-intg[(int)floor(lmin)-1])*kai*sqrt(A/nsockets);
	size_rep = delta_rep*sqrt(ro*c/r/co);
	number_rep = gamma_rep*sqrt(r*c/ro/co)*total_length;
	double number_rep_longest = gamma_rep*sqrt(r*c/ro/co)*lmax*kai*sqrt(A/nsockets);
	double number_rep_shortest = gamma_rep*sqrt(r*c/ro/co)*lmin*kai*sqrt(A/nsockets);
	area_rep = number_rep*calc_area_inv(F,size_rep);
	double lmin_min = 1.0/1.1*sqrt(ro*co/r/c)*(0.7/delta_rep+0.7*gamma_rep+0.7*delta_rep+0.4/gamma_rep);
	double lmin_here = lmin*sqrt(A/nsockets);
	power_rep = (leakage_power_inv+dyn_power_inv)*size_rep*number_rep;
	if((area_rep<available_area)&&(pitch_rep>=2.0*F)&&(size_rep>=1)&&(lmin_here>lmin_min)) 
		rep_flag = 1.0;
	else 
		rep_flag = 0.0;
	free(intg), free(intg_n);
}

/* 
	Algorithm Step 2: Logic Gate Sizing
	Logic gates are sized based on logic gate model such that clock frequency targets are reached.
	The fraction of chip area occupied by logic gates, pgates, is found.
	This function also initializes the parameters used for iteration.
*/
int intsim_chip_t::gate_sizing(intsim_param_t * intsim_param) {
	// Average wire length and device size
	return sizing_two_input_nand(intsim_param->pgates,intsim_param->device_width,intsim_param->ngates,intsim_param->A,intsim_param->cg,intsim_param->cm,intsim_param->ro,intsim_param->tox,intsim_param->F,intsim_param->ncp,intsim_param->f,intsim_param->margin,intsim_param->drive_p_div_n,intsim_param->k,intsim_param->p,intsim_param->fo);
}

/* 
	Algorithm Step 3: Generation of Stochastic Wiring Distribution
	This step generates the stochastic wiring distribution.
*/
void intsim_chip_t::st_wiring(intsim_param_t * intsim_param) {
	// Calculation of total area for logic gates and available area
	intsim_param->nsockets = intsim_param->ngates/intsim_param->pgates;
	total_area_logic_gates = calc_area_two_input_nand(intsim_param->F,intsim_param->device_width)*intsim_param->ngates;
	total_number_repeaters = 0.0;

	// Initialization of parameters used for iteration
	intg_number = (double*)malloc(sizeof(double)*(int)floor(2*sqrt(intsim_param->nsockets)));
	intg_length = (double*)malloc(sizeof(double)*(int)floor(2*sqrt(intsim_param->nsockets)));
	pitch	= (double*)malloc(sizeof(double)*intsim_param->max_tier);
	net_wire_efficiency = (double*)malloc(sizeof(double)*intsim_param->max_tier);
	length_max = (double*)malloc(sizeof(double)*intsim_param->max_tier);
	net_wire_efficiency_repeaters = (double*)malloc(sizeof(double)*intsim_param->max_tier);
	pitch_updated_with_repeaters = (double*)malloc(sizeof(double)*intsim_param->max_tier);
	length_max_updated_with_repeaters = (double*)malloc(sizeof(double)*intsim_param->max_tier);
	number_repeaters = (double*)malloc(sizeof(double)*intsim_param->max_tier);

	clear(intg_number, (int)floor(2*sqrt(intsim_param->nsockets)));
	clear(intg_length, (int)floor(2*sqrt(intsim_param->nsockets)));
	clear(pitch, (int)intsim_param->max_tier);
	clear(net_wire_efficiency, (int)intsim_param->max_tier);
	clear(length_max, (int)intsim_param->max_tier);
	clear(net_wire_efficiency_repeaters, (int)intsim_param->max_tier);
	clear(pitch_updated_with_repeaters,(int)intsim_param->max_tier);
	clear(length_max_updated_with_repeaters,(int)intsim_param->max_tier);
	clear(number_repeaters,(int)intsim_param->max_tier);

	double nsockets = intsim_param->nsockets;
	double ngates = intsim_param->ngates;
	double p = intsim_param->p;

	// Integral of (length * wire length distribution)
	double kk = 2.0*ngates*(1-pow(ngates,p-1))/(-pow(nsockets,p)*((1+2*p-pow(2,2*p-1))/(p*(2*p-1)*(p-1)*(2*p-3)))-(1/(6*p))+(2*sqrt(nsockets)/(2*p-1))-nsockets/(p-1))*intsim_param->alpha_wire*intsim_param->k;

	double y1, y2, y;

	for(double i = 1.0; i <= 2.0*sqrt(nsockets); i++) {
		y1 = (kk/2)*((1.0/3)*pow(i,3)-2*sqrt(nsockets)*pow(i,2)+2*nsockets*i)*pow(i,2*p-4);
		y2 = (kk/6)*pow(2*sqrt(nsockets)-i,3)*pow(i,2*p-4);
		y = (y1*(((sqrt(nsockets)-i)>0 ? 1:-1)+1)+y2*(((i-sqrt(nsockets))>0 ? 1:-1)+1))/2;
		if(i > 1) { 
			intg_length[(int)i-1] = i*y+intg_length[(int)i-2]; 
			intg_number[(int)i-1] = y+intg_number[(int)i-2]; }
		else { 
			intg_length[(int)i-1] = i*y; 
			intg_number[(int)i-1] = y; }
		if(i == floor(2*sqrt(nsockets)))
			total_length = intg_length[(int)i-1]; // Total wire length
	}
}

/*
	Algorithm step 4: Set Baseline Parameters for Iterations
	The design of power interconnects and their are allocation depends on the chip power. However,
	chip power is not known until repeaters are designed in the multilevel wiring network, especially
	in sub-90nm chips where repeaters consume a significant fraction of total power. Also, design
	of the interconnect stack needs some knowledge of via blockage caused by repeaters. An
	iterative process is followed by assigning wires in a multilevel wiring network, and the initial
	chip power estimation is set with zero repeaters assigned.
*/
void intsim_chip_t::baseline_param(intsim_param_t * intsim_param, double total_estimated_power) {

	double nsockets = intsim_param->nsockets;
	double ngates = intsim_param->ngates;

	// Total power defined by parameters
	calc_power_two_input_nand(dyn_power_two_input_nand,leakage_power_two_input_nand,intsim_param->device_width,intsim_param->F,intsim_param->tox,intsim_param->a,intsim_param->Ileak_spec,intsim_param->Vdd_spec,intsim_param->Vt_spec,intsim_param->subvtslope_spec,intsim_param->Vdd,intsim_param->Vt,intsim_param->f);
	leakage_power_logic_gates = ngates*leakage_power_two_input_nand;
	dyn_power_logic_gates = ngates*dyn_power_two_input_nand;
	power_wires = 0.5*intsim_param->a*4/(intsim_param->fo+3)*total_length*sqrt(intsim_param->A/nsockets)*(2*intsim_param->cg+2*intsim_param->cm)*pow(intsim_param->Vdd,2)*intsim_param->f;
	calc_power_inv(dyn_power_inv,leakage_power_inv,intsim_param->co,intsim_param->Vdd,intsim_param->Vt,intsim_param->f,intsim_param->a,intsim_param->Ileak_spec,intsim_param->Vdd_spec,intsim_param->Vt_spec,intsim_param->subvtslope_spec,intsim_param->drive_p_div_n);

	// Clock power
	double nlatches = intsim_param->clock_factor*ngates/intsim_param->ncp;	
	double clatches = nlatches*intsim_param->device_width*2*intsim_param->co;	
	double clocal_clock = sqrt(intsim_param->A*nlatches/intsim_param->latches_per_buffer)*(2*intsim_param->cg+2*intsim_param->cm); 
	cbuffer = 0.33*(clatches+clocal_clock);	

        // Clock power breakdown
	clock_power = ((clatches+clocal_clock+cbuffer)*pow(intsim_param->Vdd,2)*intsim_param->f+cbuffer/intsim_param->co*leakage_power_inv)*(1-intsim_param->clock_gating_factor);
	max_power_repeaters = total_estimated_power-power_wires-dyn_power_logic_gates-leakage_power_logic_gates-clock_power;

	// Maximum DC current per gate
	double idc = 1/intsim_param->a*(dyn_power_logic_gates+power_wires+clock_power+max_power_repeaters*dyn_power_inv/(dyn_power_inv+leakage_power_inv))/ngates/intsim_param->Vdd; 

	// A logic block size for Reza's model
	area_two_input_nand_gate = calc_area_two_input_nand(intsim_param->F,intsim_param->device_width);

	double px_n = 1.0/intsim_param->F1*sqrt(area_two_input_nand_gate/17*6);

	// Resistivity considering size effects and W(metal 1&2) = 2*F1
	double alpha_size_power = alpha_sizer(2*intsim_param->F1,intsim_param); 
	double rho_size_power = rho_sizer(alpha_size_power,2*intsim_param->F1,intsim_param);

	// The pitch of power vias (wiring efficiency of poewr and ground set as 15%)
	double px = sqrt(pow(px_n*intsim_param->F1,2)+4*px_n*pow(intsim_param->F1,3)*intsim_param->ar*intsim_param->ir_drop_limit/2*intsim_param->Vdd/idc/rho_size_power)-px_n*intsim_param->F1; // px

	// Half of below via for power, and another half for ground
	nv_power_ground = 2.0*intsim_param->A/px/((1.0/intsim_param->F1*sqrt(17.0/6*area_two_input_nand_gate)*intsim_param->ew_power_ground/2-1)*px);
}

/*
	Algorithm Step 5: Local Interconnect Modeling
	Based on the local wire pitch set as 2*F, the longest wires routed in M1 and M2 are determined.
*/
void intsim_chip_t::local_interconnect(intsim_param_t * intsim_param) {
	pitch[0] = 2.0*intsim_param->F1;
		
	// Effective wire efficiency 
	for(double l = floor(2.0*sqrt(intsim_param->nsockets)); l > 0; l--) {
		double nv = 2.0*(intg_number[(int)floor(2.0*sqrt(intsim_param->nsockets))-1]-intg_number[(int)l-1])+2.0*total_number_repeaters; 
		double via_blockage_factor = sqrt(nv*pow(pitch[0]+intsim_param->s*intsim_param->F1/2,2)/intsim_param->A);
		net_wire_efficiency[0] = intsim_param->router_eff-intsim_param->ew_power_ground-via_blockage_factor;
		if((intg_length[(int)l-1]-intg_length[0])<(net_wire_efficiency[0]*2*intsim_param->A/intsim_param->kai/pitch[0]/sqrt(intsim_param->A/intsim_param->nsockets))) { 
				length_max[0] = l; 
				break; 
			}
	}
}

/*
	Algorithm Step 6: Arrangement of Wires without Repeaters
	Once the longest wire routed in M1/M2 is determined, the pitch of M3/M4 and maximum wire length are found.
	These are in turn set for the next pair of metal layers, and this process continues until the longest
	interconnect of the wiring distribution is assigned a pitch.
*/
void intsim_chip_t::no_repeaters(intsim_param_t * intsim_param) {
	tier= 1;
	// Set the length of the longest wire in the first tier as the max length
	l_last = length_max[0]; 
	double value1, alpha_size, rho_size, dummy_wire_eff, p1, p1_new, l_last_dummy;

	// Second and higher tier design
	while(l_last < 2.0*sqrt(intsim_param->nsockets)) {

		pitch[tier] = 2*intsim_param->F1;

		for(double l = l_last; l <= 2*sqrt(intsim_param->nsockets); l++) {
			net_wire_efficiency[tier] = intsim_param->router_eff-sqrt((nv_power_ground+2*total_number_repeaters+(2*(intg_number[(int)floor(2*sqrt(intsim_param->nsockets))-1]-intg_number[(int)floor(l)-1])))*pow(pitch[tier]+intsim_param->s*intsim_param->F1/2,2)/intsim_param->A);
			value1 = net_wire_efficiency[tier]*2*intsim_param->A/intsim_param->kai/pitch[tier]/sqrt(intsim_param->A/intsim_param->nsockets); 
			if((intg_length[(int)floor(l)-1]-intg_length[(int)floor(l_last)-1]) > value1) { 
				length_max[tier] = l; 
				break; 
			}
			else 
				length_max[tier] = 2*sqrt(intsim_param->nsockets);
		}
		
		alpha_size = alpha_sizer(pitch[tier]/2,intsim_param); 
		rho_size = rho_sizer(alpha_size,pitch[tier]/2,intsim_param); 

		if((intsim_param->f*4.4*rho_size*2*(intsim_param->cg+intsim_param->cm)/(intsim_param->T/intsim_param->W)/pow(pitch[tier],2)*intsim_param->A/intsim_param->nsockets*pow(length_max[tier],2)) < 0.25) {
			pitch[tier] = 2*intsim_param->F1;
			l_last = length_max[tier]; 
                        /* The following line is added to intsim C/C++ version to avoid accessing undefined l_last_dummy */
                        l_last_dummy = length_max[tier];
		}
		else {
			for(double l = l_last; l <= 2.0*sqrt(intsim_param->nsockets); l++) {
				p1_new = 0.0; 
				p1 = 2.0*sqrt(1.1*intsim_param->rho/(intsim_param->T/intsim_param->W)*2*(intsim_param->cg+intsim_param->cm)*intsim_param->f/(1-intsim_param->margin)*intsim_param->A/intsim_param->nsockets)*l; // p1
				while(fabs(p1-p1_new)>1e-10) {
//					if(p1_new != 0.0)
					alpha_size = alpha_sizer(p1/2,intsim_param);
					rho_size = rho_sizer(alpha_size,p1/2,intsim_param);
					p1_new = 2.0*sqrt(1.1*rho_size/(intsim_param->T/intsim_param->W)*2*(intsim_param->cg+intsim_param->cm)*intsim_param->f/(1-intsim_param->margin)*intsim_param->A/intsim_param->nsockets)*l; // p1_new
					p1 = (p1+p1_new)/2;
				}
//				p1 = (p1+p1_new)/2;
				dummy_wire_eff = intsim_param->router_eff-sqrt((2*total_number_repeaters+nv_power_ground+(2*(intg_number[(int)floor(2*sqrt(intsim_param->nsockets))-1]-intg_number[(int)floor(l)-1])))*pow(p1+intsim_param->s*intsim_param->F1/2,2)/intsim_param->A); 

				value1 = dummy_wire_eff*2*intsim_param->A/intsim_param->kai/p1/sqrt(intsim_param->A/intsim_param->nsockets); 

				if((intg_length[(int)floor(l)-1]-intg_length[(int)floor(l_last)-1])>value1) {
					length_max[tier] = l;
					pitch[tier] = p1;
					net_wire_efficiency[tier] = dummy_wire_eff;
					l_last_dummy = length_max[tier]; 
					break;
				}
                                /* else condition is added to intsim C/C++ version in order to avoid accessing undefined l_last_dummy */
                                else
                                  l_last_dummy = length_max[tier];
			}
		}
		if(pitch[tier] < 2.0*intsim_param->F1) {
			pitch[tier] = 2.0*intsim_param->F1;
			for(double l = l_last; l <= 2.0*sqrt(intsim_param->nsockets); l++) {
				net_wire_efficiency[tier] = intsim_param->router_eff-sqrt((2.0*total_number_repeaters+nv_power_ground+(2.0*(intg_number[(int)floor(2.0*sqrt(intsim_param->nsockets))-1]-intg_number[(int)floor(l)-1])))*pow(pitch[tier]+intsim_param->s*intsim_param->F1/2,2)/intsim_param->A);
				value1 = net_wire_efficiency[tier]*2.0*intsim_param->A/intsim_param->kai/pitch[tier]/sqrt(intsim_param->A/intsim_param->nsockets); 
				if((intg_number[(int)floor(l)-1]-intg_length[(int)floor(l_last)-1]) > value1) {
					length_max[tier] = l;
					l_last = length_max[tier];
					break;
				}
			}
		}
		else l_last = l_last_dummy; 

		if(tier == 40) {
                        /* ----- SoM ----- */
                        /* Scaling the number of transistors to fit into the area */
                        
                        /* ----- EoM ----- */
//			printf("Error - maximum tier reached.\n");
		}

		// Termination criterion
		p1_new = 0.0;
		p1 = 2.0*sqrt(1.1*intsim_param->rho/(intsim_param->T/intsim_param->W)*2*(intsim_param->cg+intsim_param->cm)*intsim_param->f/(1-intsim_param->margin)*intsim_param->A/intsim_param->nsockets)*2*sqrt(intsim_param->nsockets); // p1
		while(fabs(p1-p1_new)>1e-10) {
//			if(p1_new != 0.0)
			alpha_size = alpha_sizer(p1/2,intsim_param); 
			rho_size = rho_sizer(alpha_size,p1/2,intsim_param); 
			p1_new = 2.0*sqrt(1.1*rho_size/(intsim_param->T/intsim_param->W)*2*(intsim_param->cg+intsim_param->cm)*intsim_param->f/(1-intsim_param->margin)*intsim_param->A/intsim_param->nsockets)*2*sqrt(intsim_param->nsockets);
			p1 = (p1+p1_new)/2;
		}
//		p1 = (p1+p1_new)/2;
		dummy_wire_eff = 8; 
		value1 = dummy_wire_eff*2*intsim_param->A/intsim_param->kai/p1/sqrt(intsim_param->A/intsim_param->nsockets); 
		if((intg_length[(int)floor(2*sqrt(intsim_param->nsockets))-1]-intg_length[(int)floor(l_last)-1]) < value1) {
			length_max[tier+1] = 2*sqrt(intsim_param->nsockets);
			pitch[tier+1] = p1;
			net_wire_efficiency[tier+1] = dummy_wire_eff;
			tier += (int)(intsim_param->kai*p1*sqrt(intsim_param->A/intsim_param->nsockets)*(intg_length[(int)floor(2*sqrt(intsim_param->nsockets))-1]-intg_length[(int)floor(l_last)-1])/2/dummy_wire_eff/intsim_param->A);
			l_last = 2*sqrt(intsim_param->nsockets);
		}
		tier++;
	}
}
/*
	Algorithm Step 7: Global Interconnect Modeling
	A top-down process of global interconnect pitch selection and repeaters insertion begin. Based on 
	the constrained global wire pitch, the area needed for routing power wires is found. It helps 
	calculate the area available for signal wires in global wire levels. Repeaters are inserted into
	these global signal wires, and the shortest signal wire routed in global wire levels is found.
*/
void intsim_chip_t::global_interconnect(intsim_param_t * intsim_param, double total_estimated_power) {
	// Global wire pitch considering only power and signal wires
	double global_pitch_power_signal = 2*(intsim_param->kp+0.5)*intsim_param->rho*total_estimated_power/intsim_param->Vdd*pow(intsim_param->pad_to_pad_distance,2)/3.142/intsim_param->ew/intsim_param->A/intsim_param->ar/intsim_param->kp/(intsim_param->ir_drop_limit/2*intsim_param->Vdd)*log(0.65*intsim_param->pad_to_pad_distance/intsim_param->pad_length); 
		
	// Clock wire pitch requirement
	if((intsim_param->beta_clock/intsim_param->f/intsim_param->ro/intsim_param->co) < 15) 
		intsim_param->beta_clock = 0.35; // Clock frequencies are high, so inductive effects will improve the rise time.

	// Actual global wire pitch needed
	double global_pitch_clock = intsim_param->D/2*sqrt(intsim_param->c_clock*intsim_param->rho/intsim_param->ar/intsim_param->kc/intsim_param->ro/intsim_param->co)/(intsim_param->beta_clock/intsim_param->f/intsim_param->ro/intsim_param->co-11)*(sqrt(72.6+4.4*intsim_param->beta_clock/intsim_param->f/intsim_param->ro/intsim_param->co)+11); 
	global_wire_pitch = global_pitch_power_signal > global_pitch_clock ? global_pitch_power_signal:global_pitch_clock; 
	double power_wire_efficiency = 2*(intsim_param->kp+0.5)*intsim_param->rho*total_estimated_power/intsim_param->Vdd*pow(intsim_param->pad_to_pad_distance,2)/3.142/intsim_param->ar/intsim_param->kp/(intsim_param->ir_drop_limit/2*intsim_param->Vdd)/global_wire_pitch*log(0.65*intsim_param->pad_to_pad_distance/intsim_param->pad_length)/2/intsim_param->A; 
	net_wire_efficiency_repeaters[0] = intsim_param->router_eff-power_wire_efficiency;

	double value1;

	// Minimum wire length in global levels
	for(double l = 1.0; l <= 2*sqrt(intsim_param->nsockets); l++) {
		value1 = net_wire_efficiency_repeaters[0]*2*intsim_param->A/intsim_param->kai/global_wire_pitch/sqrt(intsim_param->A/intsim_param->nsockets); 
		if((intg_length[(int)floor(2*sqrt(intsim_param->nsockets))-1]-intg_length[(int)floor(l)-1]) < value1) { 
			lmin = l; 
			break; 
		}
	}
}

/*
	Algorithm Step 8: Assignment of wires with repeaters
	Based on the length of the shortest global signal wire, wires with repeaters are assigned to
	the pair of metal levels below the global wire levels. The pitch and shortest wire are found
	for this pair of wiring levels and lmin is set as lmax for the pair of wiring layers below it.
	Repeaters insertion is performed for the pair of wiring layers below it. This process continues
	until one runs out of chip area for placing more repeaters or the addition of repeaters does not
	improve wire delay.
*/
void intsim_chip_t::insert_repeaters(intsim_param_t * intsim_param) {
	// Number of repeaters, size of repeaters, and power consumed by repeaters in global wire levels
	double length_global_wire_levels = (intg_length[(int)floor(2*sqrt(intsim_param->nsockets))-1]-intg_length[(int)floor(lmin)-1])*intsim_param->kai*sqrt(intsim_param->A/intsim_param->nsockets); 
	number_rep_global = 0.5*sqrt(0.4*(2*intsim_param->cg+2*intsim_param->cm)*(4*intsim_param->rho/intsim_param->ar/pow(global_wire_pitch,2))/0.7/intsim_param->ro/intsim_param->co)*length_global_wire_levels; 
	size_rep_global = sqrt(intsim_param->ro*(2*intsim_param->cg+2*intsim_param->cm)/(4*intsim_param->rho/intsim_param->ar/pow(global_wire_pitch,2))/intsim_param->co); 

	lmax = lmin; 
	max_power_repeaters -= (number_rep_global*size_rep_global*dyn_power_inv+number_rep_global*size_rep_global*leakage_power_inv);
	pitch_updated_with_repeaters[0] = global_wire_pitch; 

	double area_rep_global = number_rep_global*calc_area_inv(intsim_param->F,size_rep_global); 
	double leakage_power_repeaters_global = number_rep_global*size_rep_global*leakage_power_inv; 
	double dyn_power_repeaters_global = number_rep_global*size_rep_global*dyn_power_inv; 

	length_max_updated_with_repeaters[0] = 2*sqrt(intsim_param->nsockets);
	length_max_updated_with_repeaters[1] = lmax;
	area_rep_total = area_rep_global;
	net_wl_rep = number_rep_global*area_rep_global; 
	leakage_power_repeaters = leakage_power_repeaters_global;
	dyn_power_repeaters = dyn_power_repeaters_global;
	available_area = intsim_param->A-intsim_param->ngates*area_two_input_nand_gate-area_rep_global;
	number_repeaters[0] = number_rep_global;
	total_number_repeaters = number_rep_global;
	phigate = dyn_power_inv/(dyn_power_inv+leakage_power_inv); 

	n = 2.0;
	for(double pitch_rep, number_rep, size_rep, area_rep, power_rep, net_wire_eff_for_this_level, 
		rep_flag = 1.0; rep_flag > 0.0; n++)
	{
		calc_repeater_edp(lmin,pitch_rep,number_rep,size_rep,rep_flag,area_rep,power_rep,net_wire_eff_for_this_level,phigate,intsim_param->pgates,total_number_repeaters,nv_power_ground,leakage_power_inv,dyn_power_inv,intsim_param->R_coeff,intsim_param->p_size,max_power_repeaters,intsim_param->ar,lmax,intsim_param->ro,intsim_param->co,available_area,intsim_param->F,intsim_param->cg,intsim_param->cm,intsim_param->f,intsim_param->margin,intsim_param->rho,intsim_param->router_eff,intsim_param->A,intsim_param->ngates,intsim_param->p,intsim_param->alpha_wire,intsim_param->k,intsim_param->kai,intsim_param->s,intsim_param->F1,intsim_param);
		if(rep_flag >  0.0) {
			lmax = lmin;
			pitch_updated_with_repeaters[(int)n-1] = pitch_rep;
			length_max_updated_with_repeaters[(int)n] = lmax;
			area_rep_total += area_rep;
			available_area -= area_rep;
			net_wl_rep += number_rep*size_rep;
			leakage_power_repeaters = net_wl_rep*leakage_power_inv;
			dyn_power_repeaters = net_wl_rep*dyn_power_inv;
			number_repeaters[(int)n-1] = number_rep;
			net_wire_efficiency_repeaters[(int)n-1] = net_wire_eff_for_this_level;
			total_number_repeaters += number_rep;
			max_power_repeaters -= power_rep;
		}
	}
	--n;
}

/* 
	Algorithm Step 9: Power Computation and Iteration
	Once repeaters are assigned, the total chip power is calculated. If the total power calculated
	is different from the estimated power used for designing power distribution wiring, IntSim sets
	new estimated power = (old estimated power + calculated power)/2 and goes back to Step 5. For 
	the next iteration, the number of repeaters is set as the value calculated in Step 8.
	Note: Algorithm Step 9 is coded in main IntSim function.	
*/

/*
	Algorithm Step 10: Data Output
	When the simulation converges, the total number of wire levels, pitches of each wire level,
	and the estimated power are output to the file.
*/
void intsim_chip_t::output(intsim_param_t * intsim_param, double total_power) {
	int index = 0, m1, m3, m4, m5, m6;
	double temp;

	// Combines data from the repeaters insertion algorithm and wire arrangement without repeaters
	for(m1 = 0; m1 < tier+1; m1++) {
		for(int n1 = 0; n1 < (int)n; n1++) {
			if(length_max[m1]>length_max_updated_with_repeaters[n1]) index = m1+1;
		}
		if(index > 0) break;
	}

	for(m3 = (int)n-1, m4 = 1; m3 > (int)ceil((n-1)/2); m3--, m4++) {
		temp = pitch_updated_with_repeaters[m3-1];
		pitch_updated_with_repeaters[m3-1] = pitch_updated_with_repeaters[m4-1];
		pitch_updated_with_repeaters[m4-1] = temp;
	}
	for(m6 = (int)n-1, m5 = 1; m6 > (int)ceil((n-1)/2); m6--, m5++) {
		temp = net_wire_efficiency_repeaters[m6-1];
		net_wire_efficiency_repeaters[m6-1] = net_wire_efficiency_repeaters[m5-1];
		net_wire_efficiency_repeaters[m5-1] = temp;
	}

	double *pitch_final = (double*)malloc(sizeof(double)*(index+(int)n-1));
	double *wire_efficiency_final = (double*)malloc(sizeof(double)*(index+(int)n-1));

	for(int i = 0; i < index+(int)n-1; i++)
	{
		if(i < index) {
			pitch_final[i] = pitch[i];
			wire_efficiency_final[i] = net_wire_efficiency[i];
		}
		else {
			pitch_final[i] = pitch_updated_with_repeaters[i-index];
			wire_efficiency_final[i] = net_wire_efficiency_repeaters[i-index];
		}
	}

	ascend(pitch_final,index+(int)n-1);

	// Output a text file with the results.
	FILE *output;
	if((output = fopen("output.txt","w")) == NULL)
		printf("Error: Output file open\n");

	fprintf(output,"-----------------------------------------------------------------------------------------------------------------\n");
	fprintf(output,"Results from INTsim\n");
	fprintf(output,"-----------------------------------------------------------------------------------------------------------------\n\n");
	fprintf(output,"Pitches of different pairs of metal levels after repeaters insertion (in nm):\n");
	for(int i = 0; i < (int)index+(int)n-1; i++)
		fprintf(output,"%lf  ",pitch_final[i]*1e9);
	fprintf(output,"\n\n");
	fprintf(output,"Number of metal levels needed after repeater insertion: %d\n\n",2*(index+(int)n-1));
	fprintf(output,"Area- \nLogic gate area = %lf mm, ",total_area_logic_gates*1e6);
	fprintf(output,"Repeater area = %lf sq mm, ",area_rep_total*1e6);
	fprintf(output,"Area available for decaps and clock circuitary = %lf sq mm\n\n",available_area*1e6);
	fprintf(output,"Power- \nLeakage power: Logic gates = %lf W,  Repeaters = %lf W\n",leakage_power_logic_gates,leakage_power_repeaters);
	fprintf(output,"Dynamic power: Logic gates = %lf W,  Repeaters = %lf W,  Interconnects = %lf W,  Clock = %lf W\n", dyn_power_logic_gates, dyn_power_repeaters, power_wires, clock_power);
	fprintf(output,"Total power = %lf W\n\n",total_power);
	fprintf(output,"Wire efficiency (from Metal 1 to Metal n)- \n");
	for(int i = 0; i < (int)index+(int)n-1; i++)
		fprintf(output,"%lf  ",wire_efficiency_final[i]);
	fprintf(output,"\n\n");
	fprintf(output,"Input parameters:\n");
	fprintf(output,"Vdd = %1.2lf V\nVt = %1.2lf V\nClock frequency = %1.2lf GHz\nNumber of gates on a critical path = %3.2lf\nActivity factor = %1.2lf \n",intsim_param->Vdd,intsim_param->Vt,intsim_param->f*1e-9,intsim_param->ncp,intsim_param->a);
	fprintf(output,"Rent constant k = %2.2lf\nRent constant p = %2.2lf\nchip area = %3.2lf sq mm\nNumber of gates = %2.2lf million\n",intsim_param->k,intsim_param->p,intsim_param->A*1e6,intsim_param->ngates*1e-6);
	fprintf(output,"Feature size = %2.2lf nm\nSaturation drain current of a minimum size nFET = %3.2f uA/um\nLeakage current of a minimum size nFET = %2.2lf uA/um\n",intsim_param->F*1e9,intsim_param->Idsat_spec/intsim_param->F,intsim_param->Ileak_spec/intsim_param->F);
	fprintf(output,"Vdd and Vt at which these currents are specified = %1.2lf V and %1.2lf V\nSubthreshold slope at 85 degrees Celcius = %2.2lf mV/dec\n",intsim_param->Vdd_spec,intsim_param->Vt_spec,intsim_param->subvtslope_spec);
	fprintf(output,"Effective oxide thickness = %2.2lf nm\nAlpha of the power law MOSFET model = %2.2lf\nRatio of drive currents of pMOS to nMOS = %2.2lf\n",intsim_param->tox*1e9,intsim_param->alpha,intsim_param->drive_p_div_n);
	fprintf(output,"chiplectric permitivity = %2.2lf\nWire resistivity without size effects = %1.2e ohm-m\nSpecularity parameter = %2.2lf\nReflectivity coefficient at grain boundaries for Cu = %2.2lf\n",intsim_param->er,intsim_param->rho,intsim_param->R_coeff,intsim_param->p_size);
	fprintf(output,"Wire aspect ratio = %2.2lf\nNumber of power pads = %4.1lf\nPad to pad distance = %3.1lf um\nPad length = %3.1lf um\nIR drop limit = %2.1lf percent\n",intsim_param->ar,intsim_param->npower_pads,intsim_param->pad_to_pad_distance*1e6,intsim_param->pad_length*1e6,intsim_param->ir_drop_limit*100);
	fprintf(output,"Router efficiency = %2.1lf percent\nAverage fan-out = %2.1lf\nPercentage of clock cycle lost to skew and process variations = %2.1lf percent\n",intsim_param->router_eff*100,intsim_param->fo,intsim_param->margin*100);
	fprintf(output,"Max span of H tree that needs to be driven = %3.1lf nm\nLatches per buffer = %3.1lf\nClock factor = %2.2lf\nPercentage of local clock power saved by clock gating = %2.1lf percent\n",intsim_param->D*1e3,intsim_param->latches_per_buffer,intsim_param->clock_factor,intsim_param->clock_gating_factor);
	fprintf(output,"-----------------------------------------------------------------------------------------------------------------\n");

	fclose(output);

	free(intg_number);
	free(intg_length);
	free(pitch);
	free(net_wire_efficiency);
	free(length_max);
	free(net_wire_efficiency_repeaters);
	free(pitch_updated_with_repeaters);
	free(length_max_updated_with_repeaters);
	free(number_repeaters); 
}

void intsim_chip_t::update_energy(intsim_param_t * intsim_param)
{
  // recalculate baseline powers
  calc_power_two_input_nand(dyn_power_two_input_nand,leakage_power_two_input_nand,intsim_param->device_width,intsim_param->F,intsim_param->tox,intsim_param->a,intsim_param->Ileak_spec,intsim_param->Vdd_spec,intsim_param->Vt_spec,intsim_param->subvtslope_spec,intsim_param->Vdd,intsim_param->Vt,intsim_param->f);
  calc_power_inv(dyn_power_inv,leakage_power_inv,intsim_param->co,intsim_param->Vdd,intsim_param->Vt,intsim_param->f,intsim_param->a,intsim_param->Ileak_spec,intsim_param->Vdd_spec,intsim_param->Vt_spec,intsim_param->subvtslope_spec,intsim_param->drive_p_div_n);

  // recalculate leakage powers
  leakage_power_logic_gates = intsim_param->ngates*leakage_power_two_input_nand;
  leakage_power_repeaters = number_rep_global*size_rep_global*leakage_power_inv; 
}
