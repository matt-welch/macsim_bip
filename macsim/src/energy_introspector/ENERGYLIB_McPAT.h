/* This file is a link to mcpat energy library */
#ifndef ENERGYLIB_MCPAT_H
#define ENERGYLIB_MCPAT_H
#include "energy_introspector.h"

#include "ENERGYLIB_McPAT/arch_const.h"
#include "ENERGYLIB_McPAT/array.h"
#include "ENERGYLIB_McPAT/basic_components.h"
#include "ENERGYLIB_McPAT/core.h"	
#include "ENERGYLIB_McPAT/interconnect.h"
#include "ENERGYLIB_McPAT/iocontrollers.h"
#include "ENERGYLIB_McPAT/logic.h"
#include "ENERGYLIB_McPAT/memoryctrl.h"
#include "ENERGYLIB_McPAT/noc.h"
#include "ENERGYLIB_McPAT/processor.h"
#include "ENERGYLIB_McPAT/sharedcache.h"
#include "ENERGYLIB_McPAT/version.h"
#include "ENERGYLIB_McPAT/XML_Parse.h"
#include "ENERGYLIB_McPAT/xmlParser.h"

#include "ENERGYLIB_McPAT/arbiter.h"
#include "ENERGYLIB_McPAT/area.h"
#include "ENERGYLIB_McPAT/bank.h"
#include "ENERGYLIB_McPAT/basic_circuit.h"
#include "ENERGYLIB_McPAT/cacti_interface.h"
#include "ENERGYLIB_McPAT/component.h"
#include "ENERGYLIB_McPAT/const.h"
#include "ENERGYLIB_McPAT/crossbar.h"
#include "ENERGYLIB_McPAT/decoder.h"
//#include "ENERGYLIB_McPAT/highradix.h"
#include "ENERGYLIB_McPAT/htree2.h"
#include "ENERGYLIB_McPAT/io.h"
#include "ENERGYLIB_McPAT/mat.h"
#include "ENERGYLIB_McPAT/nuca.h"
#include "ENERGYLIB_McPAT/parameter.h"
#include "ENERGYLIB_McPAT/router.h"
#include "ENERGYLIB_McPAT/subarray.h"
#include "ENERGYLIB_McPAT/Ucache.h"
#include "ENERGYLIB_McPAT/uca.h"
#include "ENERGYLIB_McPAT/wire.h"

class ENERGYLIB_McPAT : public energy_library_t
{
 public:
  ENERGYLIB_McPAT() : energy_scaling(1.0), area_scaling(1.0), scaling(1.0) {} 
  ENERGYLIB_McPAT(parameters_module_t &p_module, parameters_technology_t &p_technology,\
  energy_introspector_t *ei);  

  virtual energy_t get_unit_energy(bool is_tdp);
  virtual double get_area(void);
  virtual void update_energy(string name, void *value);

 private:
  string energy_model;
  string energy_submodel;

  double energy_scaling;
  double area_scaling;
  double scaling;
  
  class {
   public:
    double read, write, search, read_tag, write_tag;
  }TDP_duty_cycle;

  // McPAT input classes
  ParseXML XML_interface;
  InputParameter input_p;
  Device_ty device_ty;
  TechnologyParameter tech_p;
  MCParam mc_p;
  NoCParam noc_p;
  NIUParam niu_p;
  PCIeParam pci_p;
  ProcParam proc_p;
  CacheDynParam cache_p;
  CoreDynParam core_p;

  // McPAT library models
  ArrayST *McPAT_ArrayST;
  dep_resource_conflict_check *McPAT_dep_resource_conflict_check;
  FlashController *McPAT_FlashController;
  FunctionalUnit *McPAT_FunctionalUnit;
  inst_decoder *McPAT_inst_decoder;
  interconnect *McPAT_interconnect;
  MemoryController *McPAT_MemoryController;
  MCFrontEnd *McPAT_MCFrontEnd;
  MCBackend *McPAT_MCBackend;
  MCPHY *McPAT_MCPHY;
  NIUController *McPAT_NIUController;
  NoC *McPAT_NoC;
  PCIeController *McPAT_PCIeController;
  Pipeline *McPAT_Pipeline;
  selection_logic *McPAT_selection_logic;
  UndiffCore *McPAT_UndiffCore;

  void parse_XML(Processor *McPAT_processor);
  void EI_technology_config(FILE *McPAT_config, string technologyID, InputParameter McPAT_input,\
                            double target_clock_frequency, string component_type, string core_type);
  void EI_module_config(FILE *McPAT_config, ENERGYLIB_McPAT *McPAT_obj, int ref_idx, int idx,\
                        double TDP_read_cycle, double TDP_write_cycle, double TDP_search_cycle,\
                        double TDP_read_tag_cycle, double TDP_write_tag_cycle);

  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, ArrayST *obj) { McPAT_obj->McPAT_ArrayST = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, dep_resource_conflict_check *obj) { McPAT_obj->McPAT_dep_resource_conflict_check = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, FlashController *obj) { McPAT_obj->McPAT_FlashController = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, FunctionalUnit *obj) { McPAT_obj->McPAT_FunctionalUnit = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, inst_decoder *obj) { McPAT_obj->McPAT_inst_decoder = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, interconnect *obj) { McPAT_obj->McPAT_interconnect = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, MemoryController *obj) { McPAT_obj->McPAT_MemoryController = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, MCFrontEnd *obj) { McPAT_obj->McPAT_MCFrontEnd = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, MCBackend *obj) { McPAT_obj->McPAT_MCBackend = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, MCPHY *obj) { McPAT_obj->McPAT_MCPHY = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, NIUController *obj) { McPAT_obj->McPAT_NIUController = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, NoC *obj) { McPAT_obj->McPAT_NoC = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, PCIeController *obj) { McPAT_obj->McPAT_PCIeController = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, Pipeline *obj) { McPAT_obj->McPAT_Pipeline = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, selection_logic *obj) { McPAT_obj->McPAT_selection_logic = obj; }
  template <typename T>
  void assign(ENERGYLIB_McPAT *McPAT_obj, UndiffCore *obj) { McPAT_obj->McPAT_UndiffCore = obj; }

  template <typename T>
  void populate_EI(FILE *McPAT_config, int ref_idx, int idx,\
                   double target_clock_frequency, T *model_obj, InputParameter McPAT_input,\
                   double scale, double area_scale, double energy_scale,\
                   double TDP_read_cycle, double TDP_write_cycle, double TDP_search_cycle,\
                   double TDP_read_tag_cycle, double TDP_write_tag_cycle,\
                   string technologyID, string moduleID, string energy_model_name, string energy_submodel_name = "none")
  {
    ENERGYLIB_McPAT *McPAT_obj = new ENERGYLIB_McPAT();

    McPAT_obj->name = "mcpat";
    McPAT_obj->XML_interface = XML_interface;
    McPAT_obj->clock_frequency = target_clock_frequency;
    McPAT_obj->energy_model = energy_model_name;
    McPAT_obj->energy_submodel = energy_submodel_name;
    McPAT_obj->scaling = scale;
    McPAT_obj->area_scaling = area_scale;
    McPAT_obj->energy_scaling = energy_scale;

    assign<T>(McPAT_obj,model_obj);
    
    init_interface(&McPAT_input);
    McPAT_obj->input_p = *g_ip;
    McPAT_obj->tech_p = g_tp;
    
    if(McPAT_config)
    {
      fprintf(McPAT_config,"-module.ID\t\t%s\n",moduleID.c_str());
      fprintf(McPAT_config,"-module.technology\t\t%s\n",technologyID.c_str());
      fprintf(McPAT_config,"-module.energy_library\t\tMcPAT\n");
      fprintf(McPAT_config,"-module.energy_model\t\t%s\n",energy_model_name.c_str());
      if(stricmp(energy_submodel_name,"none"))
        fprintf(McPAT_config,"-module.energy_submodel\t\t%s\n",energy_submodel_name.c_str());
      EI_module_config(McPAT_config,McPAT_obj,ref_idx,idx,\
                       TDP_read_cycle,TDP_write_cycle,TDP_search_cycle,TDP_read_tag_cycle,TDP_write_tag_cycle);
      if(area_scale != 1.0)
        fprintf(McPAT_config,"-module.area_scaling\t\t%lf\n",area_scale);
      if(energy_scale != 1.0)
        fprintf(McPAT_config,"-module.energy_scaling\t\t%lf\n",energy_scale);
      if(scale != 1.0)
        fprintf(McPAT_config,"-module.scaling\t\t%lf\n",scale);      
      /*
      //if(TDP_read_cycle != 1.0)
        fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_read_cycle);
      //if(TDP_write_cycle != 1.0)
        fprintf(McPAT_config,"-module.TDP_duty_cycle.write\t\t%lf\n",TDP_write_cycle);
      //if(TDP_search_cycle != 0.0)
        fprintf(McPAT_config,"-module.TDP_duty_cycle.search\t\t%lf\n",TDP_search_cycle);
      //if(TDP_read_tag_cycle != 0.0)
        fprintf(McPAT_config,"-module.TDP_duty_cycle.read_tag\t\t%lf\n",TDP_read_tag_cycle);
      //if(TDP_write_tag_cycle != 0.0)
        fprintf(McPAT_config,"-module.TDP_duty_cycle.write_tag\t\t%lf\n",TDP_write_tag_cycle);
      */
      fprintf(McPAT_config,"-module.end\n\n");
    }

    pseudo_module_t pseudo_module;
    pseudo_module.energy_library = (energy_library_t*)McPAT_obj;
    pseudo_module.energy_library->get_unit_energy();
    pseudo_module.partition = "n/a";
	
    // default pseudo module queues
    pseudo_module.queue.create<dimension_t>("dimension",1);
    pseudo_module.queue.create<double>("temperature",1);
    pseudo_module.queue.create<power_t>("power",1);
    pseudo_module.queue.create<power_t>("TDP",1);
    
    // update area
    dimension_t dimension;
    dimension.area = McPAT_obj->get_area();
    pseudo_module.queue.push<dimension_t>(MAX_TIME,MAX_TIME,"dimension",dimension);

    // store temperature in the queue
    pseudo_module.queue.push<double>(MAX_TIME,MAX_TIME,"temperature",McPAT_input.temp);

    // initialize temperature queue
    //pseudo_module.queue.push<double>(0.0,0.0,"temperature",(double)XML_interface.sys.temperature);

    energy_introspector->module.insert(pair<string,pseudo_module_t>(moduleID,pseudo_module));
/*
    counters_t counters; // dummy counters
    energy_introspector->compute_power(0.0,0.0,moduleID,counters,true); // compute TDP power
*/
  }
};
#endif
