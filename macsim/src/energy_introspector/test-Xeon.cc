#include <iostream>

#include "energy_introspector.h"

int main(void)
{
  energy_introspector_t *energy_introspector = new energy_introspector_t("config/Xeon.config");

  /* Runtime Test */
  uint64_t cycles = 100000;
    
  double current_time, period;
  current_time = period = (double)cycles/3.4e9;

  double area, area_subblock, area_block, area_stage;
  power_t power, power_subblock, power_block, power_stage;
    
  counters_t counters;

  // Stats are printed in bottom-up fashion
  area_block = area_stage = 0;
  power_block.reset(); power_stage.reset();

  counters.reset();
  energy_introspector->compute_power(current_time,period,"Pipeline",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","Pipeline","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","Pipeline","power");
  cout << "        Pipeline:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
    
  counters.reset();
  counters.read = 200000;
  energy_introspector->compute_power(current_time,period,"icache",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","icache","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","icache","power");
  cout << "        icache:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  energy_introspector->compute_power(current_time,period,"icacheMissBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","icacheMissBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","icacheMissBuffer","power");
  cout << "        icacheMissBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"icacheFillBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","icacheFillBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","icacheFillBuffer","power");
  cout << "        icacheFillBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"icachePrefetchBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","icachePrefetchBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","icachePrefetchBuffer","power");
  cout << "        icachePrefetchBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    instCache:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;
	
  counters.reset();
  counters.read = 400000;
  energy_introspector->compute_power(current_time,period,"branchTargetBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","branchTargetBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","branchTargetBuffer","power");
  cout << "    branchTargetBuffer:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  cout << endl;
  area_stage += area;
  power_stage = power_stage + power;

  counters.reset();
  counters.read = cycles;
  counters.write = 0.1*cycles;

  energy_introspector->compute_power(current_time,period,"globalPredictor",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","globalPredictor","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","globalPredictor","power");
  cout << "        globalPredictor:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L1_localPredictor",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L1_localPredictor","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L1_localPredictor","power");
  cout << "        L1_localPredictor:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L2_localPredictor",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L2_localPredictor","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L2_localPredictor","power");
  cout << "        L2_localPredictor:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"chooser",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","chooser","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","chooser","power");
  cout << "        chooser:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 5;
  counters.write = 5;
  energy_introspector->compute_power(current_time,period,"RAS",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","RAS","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","RAS","power");
  cout << "        RAS:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    branchPredictor:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  counters.reset();
  counters.read = 400000;
  counters.write = 400000;
  energy_introspector->compute_power(current_time,period,"instBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","instBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","instBuffer","power");
  cout << "    instBuffer:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;

  counters.reset();
  counters.read = 400000;
  energy_introspector->compute_power(current_time,period,"ID_inst",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","ID_inst","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","ID_inst","power");
  cout << "        ID_inst:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"ID_operand",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","ID_operand","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","ID_operand","power");
  cout << "        ID_operand:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"ID_misc",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","ID_misc","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","ID_misc","power");
  cout << "        ID_misc:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    instDecoder:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  area = energy_introspector->pull_data<dimension_t>(current_time,"module","Pipeline","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","Pipeline","power");
  cout << "instFetchUnit:" << endl;
  cout << "  area = " << (area_stage + area/5.0)*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = -- L2_localPredictor power is missed in McPAT" << (power_stage.total - power_stage.leakage)+(power.total-power.leakage)/5.0 << "W" << endl;
  cout << "  power.leakage = " << power_stage.leakage+power.leakage/5.0 << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; cout << endl;


  counters.reset();
  counters.read = 800000;
  counters.write = 400000;
  energy_introspector->compute_power(current_time,period,"intFRAT",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","intFRAT","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","intFRAT","power");
  cout << "        intFRAT:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;
  
  counters.reset();
  counters.read = 800000;
  energy_introspector->compute_power(current_time,period,"intDepCheck",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","intDepCheck","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","intDepCheck","power");
  power.total -= power.leakage;
  power.leakage = 0.0;
  cout << "        intDepCheck:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W -- suppressed leakage?" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    intFrontendRAT:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  counters.reset();
  counters.read = 200000;
  counters.write = 100000;
  energy_introspector->compute_power(current_time,period,"fpFRAT",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpFRAT","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpFRAT","power");
  cout << "        fpFRAT:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;
  
  counters.reset();
  counters.read = 200000;
  energy_introspector->compute_power(current_time,period,"fpDepCheck",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpDepCheck","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpDepCheck","power");
  power.total -= power.leakage;
  power.leakage = 0.0;
  cout << "        fpDepCheck:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W -- suppressed leakage?" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    fpFrontendRAT:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;  
  
  counters.reset();
  counters.read = 800000;
  counters.write = 800000;
  energy_introspector->compute_power(current_time,period,"intFreelist",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","intFreelist","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","intFreelist","power");
  cout << "    intFreelist:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;
    
  counters.reset();
  counters.read = 400000;
  counters.write = 400000;
  energy_introspector->compute_power(current_time,period,"intRRAT",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","intRRAT","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","intRRAT","power");
  cout << "    intRRAT:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;
    
  counters.reset();
  counters.read = 100000;
  counters.write = 100000;
  energy_introspector->compute_power(current_time,period,"fpRRAT",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpRRAT","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpRRAT","power");
  cout << "    fpRRAT:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;
    
  counters.reset();
  counters.read = 200000;
  counters.write = 200000;
  energy_introspector->compute_power(current_time,period,"fpFreelist",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpFreelist","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpFreelist","power");
  cout << "    fpFreelist:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;  

  area = energy_introspector->pull_data<dimension_t>(current_time,"module","Pipeline","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","Pipeline","power");
  cout << "renamingUnit:" << endl;
  cout << "  area = " << (area_stage + area/5.0)*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power_stage.total - power_stage.leakage)+(power.total-power.leakage)/5.0 << "W" << endl;
  cout << "  power.leakage = " << power_stage.leakage+power.leakage/5.0 << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; cout << endl;


  counters.reset();
  counters.read = 800000;
  counters.write = 27276;
  counters.read_tag = 183; // write miss
  energy_introspector->compute_power(current_time,period,"dcache",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","dcache","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","dcache","power");
  cout << "        dcache:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.search = 183;
  counters.write = 183;
  energy_introspector->compute_power(current_time,period,"dcacheMissBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","dcacheMissBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","dcacheMissBuffer","power");
  cout << "        dcacheMissBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"dcacheFillBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","dcacheFillBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","dcacheFillBuffer","power");
  cout << "        dcacheFillBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"dcachePrefetchBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","dcachePrefetchBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","dcachePrefetchBuffer","power");
  cout << "        dcachePrefetchBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"dcacheWritebackBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","dcacheWritebackBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","dcacheWritebackBuffer","power");
  cout << "        dcacheWritebackBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    dataCache:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;
  
  counters.reset();
  counters.read = 50000;
  counters.write = 50000;
  counters.search = 50000;
  energy_introspector->compute_power(current_time,period,"loadQueue",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","loadQueue","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","loadQueue","power");
  cout << "    loadQueue:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_block = area;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;  

  counters.reset();
  counters.read = 100000;
  counters.write = 100000;
  counters.search = 100000;
  energy_introspector->compute_power(current_time,period,"storeQueue",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","storeQueue","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","storeQueue","power");
  cout << "    storeQueue:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;  
  
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","Pipeline","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","Pipeline","power");
  cout << "loadStoreUnit:" << endl;
  cout << "  area = " << (((area_stage-area_block)*1.1+area_block)*1.1 + area/5.0)*1e6 << "mm^2 -- McPAT is wrong" << endl;
  cout << "  power.dynamic = " << (power_stage.total - power_stage.leakage)+(power.total-power.leakage)/5.0 << "W" << endl;
  cout << "  power.leakage = " << power_stage.leakage+power.leakage/5.0 << "W" << endl;
  area_block = 0;
  area_stage = 0; power_stage.reset();
  cout << endl; cout << endl;


  counters.reset();
  counters.write = 4;
  counters.search = 200000;
  energy_introspector->compute_power(current_time,period,"ITLB",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","ITLB","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","ITLB","power");
  cout << "    ITLB:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;  

  counters.reset();
  counters.write = 4;
  counters.search = 400000;
  energy_introspector->compute_power(current_time,period,"DTLB",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","DTLB","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","DTLB","power");
  cout << "    DTLB:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;
  cout << endl;  

  area = energy_introspector->pull_data<dimension_t>(current_time,"module","Pipeline","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","Pipeline","power");
  cout << "memoryManagementUnit:" << endl;
  cout << "  area = " << (area_stage + area/5.0)*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power_stage.total - power_stage.leakage)+(power.total-power.leakage)/5.0 << "W" << endl;
  cout << "  power.leakage = " << power_stage.leakage+power.leakage/5.0 << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; cout << endl;


  counters.reset();
  counters.read = 600000;
  counters.write = 300000;
  energy_introspector->compute_power(current_time,period,"IRF",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","IRF","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","IRF","power");
  cout << "        IRF:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 100000;
  counters.write = 50000;
  energy_introspector->compute_power(current_time,period,"FRF",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","FRF","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","FRF","power");
  cout << "        FRF:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    registerFiles:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  counters.reset();
  counters.read = 400000;
  counters.write = 400000;
  counters.search = 800000;
  energy_introspector->compute_power(current_time,period,"instIssueQueue",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","instIssueQueue","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","instIssueQueue","power");
  cout << "            instIssueQueue:" << endl;
  cout << "              area = " << area*1e6 << "mm^2" << endl;
  cout << "              power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "              power.leakage = " << power.leakage << "W" << endl;
  area_subblock += area; area_block += area; area_stage += area;
  power_subblock = power_subblock + power; power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 400000;
  energy_introspector->compute_power(current_time,period,"instSelection",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","instSelection","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","instSelection","power");
  cout << "            instSelection:" << endl;
  cout << "              area = " << area*1e6 << "mm^2" << endl;
  cout << "              power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "              power.leakage = " << power.leakage << "W" << endl;
  area_subblock += area; area_block += area; area_stage += area;
  power_subblock = power_subblock + power; power_block = power_block + power; power_stage = power_stage + power;
  
  cout << "        instructionWindow:" << endl;
  cout << "          area = " << area_block*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "          power.leakage = " << power_block.leakage << "W" << endl;
  area_subblock = 0; power_subblock.reset();
  cout << endl;

  counters.reset();
  counters.read = 200000;
  counters.write = 200000;
  counters.search = 400000;
  energy_introspector->compute_power(current_time,period,"fpIssueQueue",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpIssueQueue","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpIssueQueue","power");
  cout << "            fpIssueQueue:" << endl;
  cout << "              area = " << area*1e6 << "mm^2" << endl;
  cout << "              power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "              power.leakage = " << power.leakage << "W" << endl;
  area_subblock += area; area_block += area; area_stage += area;
  power_subblock = power_subblock + power; power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 200000;
  energy_introspector->compute_power(current_time,period,"instSelection",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","instSelection","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","instSelection","power");
  cout << "            instSelection:" << endl;
  cout << "              area = " << area*1e6 << "mm^2" << endl;
  cout << "              power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "              power.leakage = " << power.leakage << "W" << endl;
  area_subblock += area; area_block += area; area_stage += area;
  power_subblock = power_subblock + power; power_block = power_block + power; power_stage = power_stage + power;
  
  cout << "        fpInstructionWindow:" << endl;
  cout << "          area = " << area_block*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "          power.leakage = " << power_block.leakage << "W" << endl;
  area_subblock = 0; power_subblock.reset();
  cout << endl;

  counters.reset();
  counters.read = 400000;
  counters.write = 400000;
  energy_introspector->compute_power(current_time,period,"ROB",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","ROB","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","ROB","power");
  cout << "        ROB:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    instScheduler:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  counters.reset();
  counters.read = 300000;
  energy_introspector->compute_power(current_time,period,"ALU",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","ALU","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","ALU","power");
  cout << "    ALU:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;

  counters.reset();
  counters.read = 100000;
  energy_introspector->compute_power(current_time,period,"FPU",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","FPU","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","FPU","power");
  cout << "    FPU:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;

  counters.reset();
  counters.read = 200000;
  energy_introspector->compute_power(current_time,period,"MUL",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","MUL","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","MUL","power");
  cout << "    MUL:" << endl;
  cout << "      area = " << area*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "      power.leakage = " << power.leakage << "W" << endl;
  area_stage += area;
  power_stage = power_stage + power;

  counters.reset();
  counters.read = 300000;
  energy_introspector->compute_power(current_time,period,"aluBypass",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","aluBypass","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","aluBypass","power");
  cout << "        aluBypass:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 300000;
  energy_introspector->compute_power(current_time,period,"aluTagBypass",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","aluTagBypass","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","aluTagBypass","power");
  cout << "        aluTagBypass:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 100000;
  energy_introspector->compute_power(current_time,period,"fpuBypass",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpuBypass","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpuBypass","power");
  cout << "        fpuBypass:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 100000;
  energy_introspector->compute_power(current_time,period,"fpuTagBypass",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","fpuTagBypass","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","fpuTagBypass","power");
  cout << "        fpuTagBypass:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 200000;
  energy_introspector->compute_power(current_time,period,"mulBypass",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","mulBypass","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","mulBypass","power");
  cout << "        mulBypass:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.read = 200000;
  energy_introspector->compute_power(current_time,period,"mulTagBypass",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","mulTagBypass","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","mulTagBypass","power");
  cout << "        mulTagBypass:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    bypassBus:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  area = energy_introspector->pull_data<dimension_t>(current_time,"module","Pipeline","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","Pipeline","power");
  cout << "executionUnit:" << endl;
  cout << "  area = " << (area_stage + area/5.0)*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << (power_stage.total - power_stage.leakage)+(power.total-power.leakage)/5.0 << "W" << endl;
  cout << "  power.leakage = " << power_stage.leakage+power.leakage/5.0 << "W" << endl;
  area_stage = 0; power_stage.reset();
  cout << endl; cout << endl;

  counters.reset();
  counters.read = 200000-1632;
  counters.write = 27276;
  counters.read_tag = 1632;
  counters.write_tag = 183; // write miss
  energy_introspector->compute_power(current_time,period,"L2cache",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L2cache","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L2cache","power");
  cout << "        L2cache:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.search = 183;
  counters.write = 183;
  energy_introspector->compute_power(current_time,period,"L2cacheMissBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L2cacheMissBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L2cacheMissBuffer","power");
  cout << "        L2cacheMissBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L2cacheFillBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L2cacheFillBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L2cacheFillBuffer","power");
  cout << "        L2cacheFillBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L2cachePrefetchBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L2cachePrefetchBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L2cachePrefetchBuffer","power");
  cout << "        L2cachePrefetchBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L2cacheWritebackBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L2cacheWritebackBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L2cacheWritebackBuffer","power");
  cout << "        L2cacheWritebackBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    L2Cache:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  counters.reset();
  counters.read = 11824-1632;
  counters.write = 11276;
  counters.read_tag = 1632;
  counters.write_tag = 183; // write miss
  energy_introspector->compute_power(current_time,period,"L3cache",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L3cache","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L3cache","power");
  cout << "        L3cache:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  counters.reset();
  counters.search = 183;
  counters.write = 183;
  energy_introspector->compute_power(current_time,period,"L3cacheMissBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L3cacheMissBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L3cacheMissBuffer","power");
  cout << "        L3cacheMissBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L3cacheFillBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L3cacheFillBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L3cacheFillBuffer","power");
  cout << "        L3cacheFillBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L3cachePrefetchBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L3cachePrefetchBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L3cachePrefetchBuffer","power");
  cout << "        L3cachePrefetchBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  energy_introspector->compute_power(current_time,period,"L3cacheWritebackBuffer",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","L3cacheWritebackBuffer","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","L3cacheWritebackBuffer","power");
  cout << "        L3cacheWritebackBuffer:" << endl;
  cout << "          area = " << area*1e6 << "mm^2" << endl;
  cout << "          power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "          power.leakage = " << power.leakage << "W" << endl;
  area_block += area; area_stage += area;
  power_block = power_block + power; power_stage = power_stage + power;

  cout << "    L3Cache:" << endl;
  cout << "      area = " << area_block*1e6 << "mm^2" << endl;
  cout << "      power.dynamic = " << power_block.total - power_block.leakage << "W" << endl;
  cout << "      power.leakage = " << power_block.leakage << "W" << endl;
  area_block = 0; power_block.reset();
  cout << endl;

  counters.reset();
  counters.read = 100000;
  energy_introspector->compute_power(current_time,period,"network",counters);
  area = energy_introspector->pull_data<dimension_t>(current_time,"module","network","dimension").area;
  power = energy_introspector->pull_data<power_t>(current_time,"module","network","power");
  cout << "network:" << endl;
  cout << "  area = " << area*1e6 << "mm^2" << endl;
  cout << "  power.dynamic = " << power.total-power.leakage << "W" << endl;
  cout << "  power.leakage = " << power.leakage << "W" << endl;
  
  return 0;
}
