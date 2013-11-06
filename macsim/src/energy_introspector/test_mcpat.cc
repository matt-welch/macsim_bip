#include <iostream>
#include <assert.h>

#include "energy_introspector.h"

int main(void)
{
  energy_introspector_t *energy_introspector = new energy_introspector_t("config/Xeon2.config"); // McPAT-based
  
  /* test values -- does not represent any realistic numbers */
  double current_time = 0.0000001;
  double period = 0.0000001;
  counters_t counts;
  counts.read = counts.write = counts.read_tag = counts.write_tag = 100;
  
  /* refer to config.out to see what pseudo modules are created after parsing Xeon2.config */
  /* personally don't like the lack of consistency for naming components in the McPAT */
  /* spaces in string name might be a problem in the EI's config file */
  energy_introspector->compute_power(current_time,period,"core[0].Branch Target Buffer",counts);
  energy_introspector->compute_power(current_time,period,"core[0].DTLB",counts);  
  energy_introspector->compute_power(current_time,period,"core[0].FP Bypass Data",counts);  
  energy_introspector->compute_power(current_time,period,"core[0].FP Bypass tag",counts);    
  
  return 0;
}
