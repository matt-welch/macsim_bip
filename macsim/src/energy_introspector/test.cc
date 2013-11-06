#include <iostream>

#include "energy_introspector.h"

int main(void)
{
  energy_introspector_t *energy_introspector = new energy_introspector_t("config/ei2.config");

  /* Runtime Test */
  uint64_t cycles = 1000;
    
  double current_time, period;
  current_time = period = (double)cycles/3.4e9;

  double area, area_subblock, area_block, area_stage;
  power_t power, power_subblock, power_block, power_stage;
    
  counters_t counters;

  // Stats are printed in bottom-up fashion
  area_block = area_stage = 0;
  power_block.reset(); power_stage.reset();
  
  
/*  
  counters.reset();
	counters.read=100;
  energy_introspector->compute_power(current_time,period,"SP_alu1",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","SP_alu1","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","SP_alu1","power");
  cout << "SP_alu1 :" << endl;
  cout << "  area = " << area*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power.total-power.leakage) << "W" << endl;
  cout << "  power.leakage = " << power.leakage << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; 


  counters.reset();
	counters.read=100;
  energy_introspector->compute_power(current_time,period,"SP_alu2",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","SP_alu2","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","SP_alu2","power");
  cout << "SP_alu2 :" << endl;
  cout << "  area = " << area*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power.total-power.leakage) << "W" << endl;
  cout << "  power.leakage = " << power.leakage << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; 


  counters.reset();
	counters.read=100;
  energy_introspector->compute_power(current_time,period,"SP_alu3",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","SP_alu3","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","SP_alu3","power");
  cout << "SP_alu3 :" << endl;
  cout << "  area = " << area*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power.total-power.leakage) << "W" << endl;
  cout << "  power.leakage = " << power.leakage << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; 
*/
  counters.reset();
	counters.read=100;
  energy_introspector->compute_power(current_time,period,"core0:fetch",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","core0:fetch","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","core0:fetch","power");
  cout << "core0:fetch:" << endl;
  cout << "  area = " << area*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power.total-power.leakage) << "W" << endl;
  cout << "  power.leakage = " << power.leakage << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; 

  counters.reset();
	counters.read=100;
  energy_introspector->compute_power(current_time,period,"core0:byteQ",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","core0:byteQ","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","core0:byteQ","power");
  cout << "core0:byteQ:" << endl;
  cout << "  area = " << area*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power.total-power.leakage) << "W" << endl;
  cout << "  power.leakage = " << power.leakage << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; 
  return 0;
}
