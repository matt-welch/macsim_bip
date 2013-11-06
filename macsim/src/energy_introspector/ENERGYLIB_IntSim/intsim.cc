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
#include "parameters.h"

void intsim(intsim_chip_t *chip, intsim_param_t *param) {
        //intsim_chip_t * chip = new intsim_chip_t();
	// Step 1: Initializing The Parameters -- Model of D. Sekar's Ph.D Dissertation
	//param_t* param = new param_t();	
	//param->setup(max_tier);

	// Step 2: Logic Gate Sizing
	if(!chip->gate_sizing(param)) {
		// Failure of gate sizing terminates the simulation.
		printf("The specified clock frequency cannot be reached with this logic depth, supply voltage, and threshold voltage.\n");
		//return 0;
	}

	// Step 3: Generation of Stochastic Wiring Distribution
	chip->st_wiring(param);

	double intsim_total_estimated_power = 120.0; // Initial power estimation
	double intsim_old_total_power = intsim_total_estimated_power;
	double intsim_total_power = 0.0;

	// Iteration until the difference between estimation and total power is less than 0.5
	while(fabs(intsim_old_total_power-intsim_total_power)>0.5) {
		// Step 4: Set Baseline Parameters for Iteration
		chip->baseline_param(param, intsim_total_estimated_power);

		// Step 5: Local Interconnect Modeling
		chip->local_interconnect(param);

		// Step 6: Arrangement of Wires without Repeaters
		chip->no_repeaters(param);

		// If the limit of tier is reached, simulation fails and terminates.
		if(chip->tier == param->max_tier) {
			printf("Max tier is reached, power with given parameters cannot be found.\n");
			break;
		}

		// Step 7: Global Interconnect Modeling
		chip->global_interconnect(param, intsim_total_estimated_power);

		// Step 8: Assignment of Wires with Repeaters
		chip->insert_repeaters(param);
		
		// Step 9: Total Power Calculation
		intsim_total_power = chip->leakage_power_logic_gates+chip->leakage_power_repeaters+chip->dyn_power_logic_gates+chip->dyn_power_repeaters+chip->power_wires+chip->clock_power;
		intsim_old_total_power = intsim_total_estimated_power;
		intsim_total_estimated_power = (intsim_total_estimated_power+intsim_total_power)/2; // Estimated power is averaged with calculated power.
	}

	// Step 10: Data Output
	chip->output(param,intsim_total_power); 

	//return 0;
}
