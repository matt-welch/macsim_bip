#include "ENERGYLIB_McPAT.h"
#include "ENERGYLIB_McPAT.src"
#include <math.h>
#include <iostream>

ENERGYLIB_McPAT::ENERGYLIB_McPAT(parameters_module_t &p_module, parameters_technology_t &p_technology,\
energy_introspector_t *ei) :
  McPAT_ArrayST(NULL),
  McPAT_dep_resource_conflict_check(NULL),
  McPAT_FlashController(NULL),
  McPAT_FunctionalUnit(NULL), 
  McPAT_inst_decoder(NULL),
  McPAT_interconnect(NULL), 
  McPAT_MemoryController(NULL),
  McPAT_MCFrontEnd(NULL),
  McPAT_MCBackend(NULL),
  McPAT_MCPHY(NULL),
  McPAT_NIUController(NULL),
  McPAT_NoC(NULL),  
  McPAT_PCIeController(NULL),
  McPAT_Pipeline(NULL),
  McPAT_selection_logic(NULL),
  McPAT_UndiffCore(NULL)
{
  // energy_library_t parameters
  name = "mcpat";
  energy_introspector = ei;

  string option;
  
  set_variable<parameters_technology_t>(option,p_technology,"opt_for_clk","true");
  opt_for_clk = stricmp(option,"false");
  set_variable<parameters_module_t>(option,p_module,"opt_for_clk","true");
  opt_for_clk = opt_for_clk&(bool)stricmp(option,"false");
  
  set_variable<parameters_module_t>(option,p_module,"parse","none");
  if(stricmp(option,"none"))
  {
    energy_model = "none";
    XML_interface.parse((char*)option.c_str());
	
    Processor *McPAT_processor = new Processor(&XML_interface);
    McPAT_processor->displayEnergy(2,5);
    //parse_XML(McPAT_processor);	 	// Jieun Apr-16-2012, parse_XML() moved to Parse_XML.cc
    free(McPAT_processor);
  }
  else
  {
    if(!stricmp(p_technology.ID,"n/a"))
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): pseudo module %s has no valid technology parameters\n",p_module.ID.c_str());
      #endif
      exit(1);
    }

    set_variable<parameters_technology_t>(clock_frequency,p_technology,"clock_frequency",0.0,false);
    if(clock_frequency == 0.0)
      set_variable<parameters_module_t>(clock_frequency,p_module,"clock_frequency",0.0,true);

    // ENERGYLIB_McPAT parameters
    set_variable<parameters_module_t>(area_scaling,p_module,"area_scaling",1.0);
    set_variable<parameters_module_t>(energy_scaling,p_module,"energy_scaling",1.0);
    set_variable<parameters_module_t>(scaling,p_module,"scaling",1.0);
    set_variable<parameters_module_t>(energy_model,p_module,"energy_model","n/a",true);
    set_variable<parameters_module_t>(TDP_duty_cycle.read,p_module,"TDP_duty_cycle.read",1.0);
    set_variable<parameters_module_t>(TDP_duty_cycle.write,p_module,"TDP_duty_cycle.write",1.0);
    set_variable<parameters_module_t>(TDP_duty_cycle.search,p_module,"TDP_duty_cycle.search",1.0);
    set_variable<parameters_module_t>(TDP_duty_cycle.read_tag,p_module,"TDP_duty_cycle.read_tag",0.0);
    set_variable<parameters_module_t>(TDP_duty_cycle.write_tag,p_module,"TDP_duty_cycle.write_tag",0.0);

    // general input parsing
    set_variable<parameters_technology_t>(input_p.temp,p_technology,"temperature",0);
    if(input_p.temp == 0.0)
      set_variable<parameters_module_t>(input_p.temp,p_module,"temperature",0,true);

    set_variable<parameters_module_t>(input_p.obj_func_dyn_energy,p_module,"obj_func_dyn_energy",0);
    set_variable<parameters_module_t>(input_p.obj_func_dyn_power,p_module,"obj_func_dyn_power",0);
    set_variable<parameters_module_t>(input_p.obj_func_leak_power,p_module,"obj_func_leak_power",0);
    set_variable<parameters_module_t>(input_p.obj_func_cycle_t,p_module,"obj_func_cycle_t",1);
    set_variable<parameters_module_t>(input_p.delay_wt,p_module,"delay_wt",100);
    set_variable<parameters_module_t>(input_p.area_wt,p_module,"area_wt",0);
    set_variable<parameters_module_t>(input_p.dynamic_power_wt,p_module,"dynamic_power_wt",100);
    set_variable<parameters_module_t>(input_p.leakage_power_wt,p_module,"leakage_power_wt",0);
    set_variable<parameters_module_t>(input_p.cycle_time_wt,p_module,"cycle_time_wt",0);
    set_variable<parameters_module_t>(input_p.delay_dev,p_module,"delay_dev",10000);
    set_variable<parameters_module_t>(input_p.area_dev,p_module,"area_dev",10000);
    set_variable<parameters_module_t>(input_p.dynamic_power_dev,p_module,"dynamic_power_dev",10000);
    set_variable<parameters_module_t>(input_p.leakage_power_dev,p_module,"leakage_power_dev",10000);
    set_variable<parameters_module_t>(input_p.cycle_time_dev,p_module,"cycle_time_dev",10000);
    set_variable<parameters_module_t>(input_p.ed,p_module,"ed",2);
    set_variable<parameters_module_t>(input_p.nuca,p_module,"nuca_size",0);
    set_variable<parameters_module_t>(input_p.nuca_bank_count,p_module,"nuca_bank_count",0);
    set_variable<parameters_module_t>(input_p.delay_wt_nuca,p_module,"delay_wt_nuca",0);
    set_variable<parameters_module_t>(input_p.area_wt_nuca,p_module,"area_wt_nuca",0);
    set_variable<parameters_module_t>(input_p.dynamic_power_wt_nuca,p_module,"dynamic_power_wt_nuca",0);
    set_variable<parameters_module_t>(input_p.leakage_power_wt_nuca,p_module,"leakage_power_wt_nuca",0);
    set_variable<parameters_module_t>(input_p.cycle_time_wt_nuca,p_module,"cycle_time_wt_nuca",0);
    set_variable<parameters_module_t>(input_p.delay_dev_nuca,p_module,"delay_dev_nuca",10000);
    set_variable<parameters_module_t>(input_p.area_dev_nuca,p_module,"area_dev_nuca",10000);
    set_variable<parameters_module_t>(input_p.dynamic_power_dev_nuca,p_module,"dynamic_power_dev_nuca",10);
    set_variable<parameters_module_t>(input_p.leakage_power_dev_nuca,p_module,"leakage_power_dev_nuca",10000);
    set_variable<parameters_module_t>(input_p.cycle_time_dev_nuca,p_module,"cycle_time_dev_nuca",10000);

    set_variable<parameters_module_t>(option,p_module,"force_wire_type","false");
    input_p.force_wiretype = stricmp(option,"false");

    set_variable<parameters_module_t>(option,p_module,"repeaters_in_htree","true");
    input_p.rpters_in_htree = stricmp(option,"false");

    set_variable<parameters_module_t>(option,p_module,"with_clock_grid","false");
    input_p.with_clock_grid = stricmp(option,"false");

    set_variable<parameters_module_t>(option,p_module,"force_cache_config","false");
    input_p.force_cache_config = stricmp(option,"false");

    if(input_p.force_cache_config)
    {
      set_variable<parameters_module_t>(input_p.ndbl,p_module,"ndbl",0,true);
      set_variable<parameters_module_t>(input_p.ndwl,p_module,"ndwl",0,true);
      set_variable<parameters_module_t>(input_p.nspd,p_module,"nspd",0,true);
      set_variable<parameters_module_t>(input_p.ndsam1,p_module,"ndsam1",0,true);
      set_variable<parameters_module_t>(input_p.ndsam2,p_module,"ndsam2",0,true);
      set_variable<parameters_module_t>(input_p.ndcm,p_module,"ndcm",0,true);
	}

    // don't cares
    input_p.fast_access = false; // don't care
    input_p.is_seq_acc = false; // don't care
    input_p.fully_assoc = false; // don't care
    input_p.print_input_args = false; // don't care
    input_p.burst_len = 1; // don't care
    input_p.int_prefetch_w = 1; // dont' care
    input_p.page_sz_bits = 0; // don't care
    input_p.ram_cell_tech_type = 0; // don't care
    input_p.peri_global_tech_type = 0; // don't care
    input_p.block_sz = 0; // don't care
    input_p.tag_assoc = 1; // don't care
    input_p.data_assoc = 1; // don't care
    input_p.cores = 0; // don't care
    input_p.print_detail = 5; // don't care
    input_p.cache_level = 0; // don't care

    // set default values -- should be modified by each energy model
    input_p.line_sz = 8;
    input_p.out_w = 64;
    input_p.assoc = 1;
    input_p.nbanks = 1;
    input_p.cache_sz = 64;
    input_p.tag_w = 0;
    input_p.num_rw_ports = 1;
    input_p.num_rd_ports = 0;
    input_p.num_wr_ports = 0;
    input_p.num_se_rd_ports = 0;
    input_p.num_search_ports = 0;
    input_p.throughput = 1.0/clock_frequency;
    input_p.latency = 1.0/clock_frequency;
    input_p.is_main_mem = false;
    input_p.is_cache = false;
    input_p.pure_ram = false;
    input_p.pure_cam = false;
    input_p.add_ecc_b_ = true;

    set_variable<parameters_module_t>(option,p_module,"access_mode","normal");
    if(!stricmp(option,"normal"))
      input_p.access_mode = 0;
    else if(!stricmp(option,"sequential"))
      input_p.access_mode = 1;
    else if(!stricmp(option,"fast"))
      input_p.access_mode = 2;
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown access_mode %s\n",option.c_str());
      #endif
      exit(1);
    }

    // technology input parsing
    set_variable<parameters_technology_t>(input_p.F_sz_nm,p_technology,"feature_size",0.0,true);
    input_p.F_sz_nm *= 1e9;
    input_p.F_sz_um = input_p.F_sz_nm*0.001;

    // wire type
    set_variable<parameters_technology_t>(option,p_technology,"wire_type","global");
    if(!stricmp(option,"global"))
      input_p.wt = (Wire_type)Global;
    else if(!stricmp(option,"global_5"))
      input_p.wt = (Wire_type)Global_5;
    else if(!stricmp(option,"global_10"))
      input_p.wt = (Wire_type)Global_10;
    else if(!stricmp(option,"goobal_20"))
      input_p.wt = (Wire_type)Global_20;
    else if(!stricmp(option,"global_30"))
      input_p.wt = (Wire_type)Global_30;
    else if(!stricmp(option,"low_swing"))
      input_p.wt = (Wire_type)Low_swing;
    else if(!stricmp(option,"semi_global"))
      input_p.wt = (Wire_type)Semi_global;
    else if(!stricmp(option,"transmission"))
      input_p.wt = (Wire_type)Transmission;
    else if(!stricmp(option,"optical"))
      input_p.wt = (Wire_type)Optical;
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wire_type %s\n",option.c_str());
      #endif
      exit(1);
    }

    // general device type
    set_variable<parameters_technology_t>(option,p_technology,"device_type","hp");
    if(!stricmp(option,"hp"))
     input_p.data_arr_ram_cell_tech_type = input_p.data_arr_peri_global_tech_type
      = input_p.tag_arr_ram_cell_tech_type = input_p.tag_arr_peri_global_tech_type = 0;
    else if(!stricmp(option,"lstp"))
      input_p.data_arr_ram_cell_tech_type = input_p.data_arr_peri_global_tech_type
      = input_p.tag_arr_ram_cell_tech_type = input_p.tag_arr_peri_global_tech_type = 1;
    else if(!stricmp(option,"lop"))
     input_p.data_arr_ram_cell_tech_type = input_p.data_arr_peri_global_tech_type
      = input_p.tag_arr_ram_cell_tech_type = input_p.tag_arr_peri_global_tech_type = 2;
    else if(!stricmp(option,"lp_dram"))
      input_p.data_arr_ram_cell_tech_type = input_p.data_arr_peri_global_tech_type
      = input_p.tag_arr_ram_cell_tech_type = input_p.tag_arr_peri_global_tech_type = 3;
    else if(!stricmp(option,"comm_dram"))
      input_p.data_arr_ram_cell_tech_type = input_p.data_arr_peri_global_tech_type
      = input_p.tag_arr_ram_cell_tech_type = input_p.tag_arr_peri_global_tech_type = 4;
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown device_type %s\n",option.c_str());
      #endif
      exit(1);
    }

    // overwrite the device type if specifically assigned
    set_variable<parameters_technology_t>(option,p_technology,"device_type.data_arr_ram_cell_tech_type","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"hp"))
        input_p.data_arr_ram_cell_tech_type = 0;
      else if(!stricmp(option,"lstp"))
        input_p.data_arr_ram_cell_tech_type = 1;
      else if(!stricmp(option,"lop"))
        input_p.data_arr_ram_cell_tech_type = 2;
      else if(!stricmp(option,"lp_dram"))
        input_p.data_arr_ram_cell_tech_type = 3;
      else if(!stricmp(option,"comm_dram"))
        input_p.data_arr_ram_cell_tech_type = 4;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown device_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }
    set_variable<parameters_technology_t>(option,p_technology,"device_type.data_arr_peri_global_tech_type","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"hp"))
        input_p.data_arr_peri_global_tech_type = 0;
      else if(!stricmp(option,"lstp"))
        input_p.data_arr_peri_global_tech_type = 1;
      else if(!stricmp(option,"lop"))
        input_p.data_arr_peri_global_tech_type = 2;
      else if(!stricmp(option,"lp_dram"))
        input_p.data_arr_peri_global_tech_type = 3;
      else if(!stricmp(option,"comm_dram"))
        input_p.data_arr_peri_global_tech_type = 4;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown device_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }
    // overwrite the device type if specifically assigned
    set_variable<parameters_technology_t>(option,p_technology,"device_type.tag_arr_ram_cell_tech_type","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"hp"))
        input_p.tag_arr_ram_cell_tech_type = 0;
      else if(!stricmp(option,"lstp"))
        input_p.tag_arr_ram_cell_tech_type = 1;
      else if(!stricmp(option,"lop"))
        input_p.tag_arr_ram_cell_tech_type = 2;
      else if(!stricmp(option,"lp_dram"))
        input_p.tag_arr_ram_cell_tech_type = 3;
      else if(!stricmp(option,"comm_dram"))
        input_p.tag_arr_ram_cell_tech_type = 4;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown device_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }
    set_variable<parameters_technology_t>(option,p_technology,"device_type.tag_arr_peri_global_tech_type","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"hp"))
        input_p.tag_arr_peri_global_tech_type = 0;
      else if(!stricmp(option,"lstp"))
        input_p.tag_arr_peri_global_tech_type = 1;
      else if(!stricmp(option,"lop"))
        input_p.tag_arr_peri_global_tech_type = 2;
      else if(!stricmp(option,"lp_dram"))
        input_p.tag_arr_peri_global_tech_type = 3;
      else if(!stricmp(option,"comm_dram"))
        input_p.tag_arr_peri_global_tech_type = 4;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown device_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }

    set_variable<parameters_technology_t>(option,p_technology,"interconnect_projection","aggressive");
    if(!stricmp(option,"aggressive"))
      input_p.ic_proj_type = 0;
    else if(!stricmp(option,"conservative"))
      input_p.ic_proj_type = 1;
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");    
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown interconnect_projection %s\n",option.c_str());
      #endif
      exit(1);
    }

    // general wiring type
    input_p.ver_htree_wires_over_array = 0; // fixed
    input_p.broadcast_addr_din_over_ver_htrees = 0; // fixed

    set_variable<parameters_technology_t>(option,p_technology,"wiring_type","global");
    if(!stricmp(option,"local"))
      input_p.wire_is_mat_type = input_p.wire_os_mat_type = 0;
    else if(!stricmp(option,"semi_global"))
      input_p.wire_is_mat_type = input_p.wire_os_mat_type = 1;
    else if(!stricmp(option,"global"))
      input_p.wire_is_mat_type = input_p.wire_os_mat_type = 2;
    else if(!stricmp(option,"dram"))
      input_p.wire_is_mat_type = input_p.wire_os_mat_type = 3;
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");    
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wiring_type %s\n",option.c_str());
      #endif
      exit(1);
    }

    // overwrite the wiring type if specifically assigned
    set_variable<parameters_technology_t>(option,p_technology,"wiring_type.wire_is_mat_type","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"local"))
        input_p.wire_is_mat_type = 0;
      else if(!stricmp(option,"semi_global"))
        input_p.wire_is_mat_type = 1;
	  else if(!stricmp(option,"global"))
        input_p.wire_is_mat_type = 2;
      else if(!stricmp(option,"dram"))
        input_p.wire_is_mat_type = 3;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wiring_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }
    set_variable<parameters_technology_t>(option,p_technology,"wiring_type.wire_os_mat_type","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"local"))
        input_p.wire_os_mat_type = 0;
      else if(!stricmp(option,"semi_global"))
        input_p.wire_os_mat_type = 1;
      else if(!stricmp(option,"global"))
        input_p.wire_os_mat_type = 2;
      else if(!stricmp(option,"dram"))
        input_p.wire_os_mat_type = 3;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wiring_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }
    set_variable<parameters_technology_t>(option,p_technology,"wiring_type.ver_htree_wires_over_array","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"local"))
        input_p.ver_htree_wires_over_array = 0;
      else if(!stricmp(option,"semi_global"))
        input_p.ver_htree_wires_over_array = 1;
      else if(!stricmp(option,"global"))
        input_p.ver_htree_wires_over_array = 2;
      else if(!stricmp(option,"dram"))
        input_p.ver_htree_wires_over_array = 3;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wiring_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }
    set_variable<parameters_technology_t>(option,p_technology,"broadcast_addr_din_over_ver_htrees","n/a");
    if(stricmp(option,"n/a"))
    {
      if(!stricmp(option,"local"))
        input_p.broadcast_addr_din_over_ver_htrees = 0;
      else if(!stricmp(option,"semi_global"))
        input_p.broadcast_addr_din_over_ver_htrees = 1;
      else if(!stricmp(option,"global"))
        input_p.broadcast_addr_din_over_ver_htrees = 2;
      else if(!stricmp(option,"dram"))
        input_p.broadcast_addr_din_over_ver_htrees = 3;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wiring_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }

    // component types
    set_variable<parameters_technology_t>(option,p_technology,"component_type","n/a",true);
    if(!stricmp(option,"core"))
      device_ty = Core_device;
    else if(!stricmp(option,"llc"))
      device_ty = LLC_device;
    else if(!stricmp(option,"uncore"))
      device_ty = Uncore_device;
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");    
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): invalid component_type %s\n",option.c_str());
      #endif
      exit(1);
    }

    core_p.clockRate = clock_frequency;

    if(device_ty == Core_device)
    {
      set_variable<parameters_technology_t>(option,p_technology,"opt_local","false");
      core_p.opt_local = stricmp(option,"false");

      set_variable<parameters_technology_t>(option,p_technology,"core_type","inorder",true);
      if(!stricmp(option,"ooo"))
        core_p.core_ty = OOO;
      else if(!stricmp(option,"inorder"))
        core_p.core_ty = Inorder;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown core_type %s\n",option.c_str());
        #endif
        exit(1);
      }
    }

    set_variable<parameters_technology_t>(option,p_technology,"embedded","false");
    XML_interface.sys.Embedded = stricmp(option,"false");

    set_variable<parameters_technology_t>(option,p_technology,"longer_channel_device","false");
    XML_interface.sys.longer_channel_device = stricmp(option,"false");

    if(!stricmp(energy_model,"array"))
    {
      set_variable<parameters_module_t>(input_p.line_sz,p_module,"line_width",0,true);
      set_variable<parameters_module_t>(input_p.out_w,p_module,"output_width",0);
      set_variable<parameters_module_t>(input_p.assoc,p_module,"assoc",1);
      set_variable<parameters_module_t>(input_p.nbanks,p_module,"banks",1);
      set_variable<parameters_module_t>(input_p.nsets,p_module,"sets",0);
      set_variable<parameters_module_t>(input_p.cache_sz,p_module,"size",0);
      set_variable<parameters_module_t>(input_p.tag_w,p_module,"tag_width",0);
      set_variable<parameters_module_t>(input_p.num_rw_ports,p_module,"rw_ports",0);
      set_variable<parameters_module_t>(input_p.num_rd_ports,p_module,"rd_ports",0);
      set_variable<parameters_module_t>(input_p.num_wr_ports,p_module,"wr_ports",0);
      set_variable<parameters_module_t>(input_p.num_se_rd_ports,p_module,"se_rd_ports",0);
      set_variable<parameters_module_t>(input_p.num_search_ports,p_module,"search_ports",0);
      set_variable<parameters_module_t>(input_p.throughput,p_module,"cycle_time",1.0);
      set_variable<parameters_module_t>(input_p.latency,p_module,"access_time",1.0);
	
      set_variable<parameters_module_t>(option,p_module,"add_ecc","true");
      input_p.add_ecc_b_ = stricmp(option,"false");

      set_variable<parameters_module_t>(energy_submodel,p_module,"energy_submodel","n/a",true);
      if(!stricmp(energy_submodel,"memory"))
        input_p.is_main_mem = true;
      else if(!stricmp(energy_submodel,"cache"))
        input_p.is_cache = true;
      else if(!stricmp(energy_submodel,"ram"))
        input_p.pure_ram = true;
      else if(!stricmp(energy_submodel,"cam"))
        input_p.pure_cam = true;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown energy_submodel %s\n",energy_submodel.c_str());
        #endif
        exit(1);
      }

      if(!(input_p.is_main_mem||input_p.is_cache||input_p.pure_ram||input_p.pure_cam))
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"ERROR (McPAT): unknown array type -- memory/cache/ram/cam\n");
        #endif
        exit(1);
      }

      if(input_p.out_w == 0)
        input_p.out_w = input_p.line_sz*8;
    

      if(!(input_p.nsets||input_p.cache_sz))
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"ERROR (McPAT): unknown array size -- both sets and size are undefined\n");
        #endif
        exit(1);
      }

      if(input_p.cache_sz == 0)
        input_p.cache_sz = input_p.line_sz*input_p.nsets*(input_p.assoc>0?input_p.assoc:1);
      else
        input_p.nsets = input_p.cache_sz/(input_p.line_sz*(input_p.assoc>0?input_p.assoc:1));
      input_p.specific_tag = (input_p.tag_w > 0);
      input_p.throughput /= clock_frequency;
      input_p.latency /= clock_frequency;

      // scale up the array size to the minimum that McPAT can handle
      if((input_p.assoc>0?input_p.assoc*input_p.nsets:input_p.nsets) < 16)
      {
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI WARNING (McPAT): %s array size is smaller than the minimum\n",p_module.ID.c_str());
        #endif
        input_p.cache_sz = input_p.line_sz*16;
      }

      if(input_p.cache_sz < 64)
      {
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI WARNING (McPAT): %s array size is smaller than the minimum\n",p_module.ID.c_str());
        #endif
        input_p.cache_sz = 64;
      }

      McPAT_ArrayST = new ArrayST(&input_p,"array",device_ty,core_p.opt_local,core_p.core_ty);

      if(!McPAT_ArrayST)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");      
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an array %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"dependency_check_logic"))
    {
      int compare_bits;
      set_variable<parameters_module_t>(compare_bits,p_module,"compare_bits",0,true);
      set_variable<parameters_module_t>(core_p.decodeW,p_module,"decode_width",1,true); // to compute # of comparators

      McPAT_dep_resource_conflict_check = new dep_resource_conflict_check(&input_p,core_p,compare_bits,true);

      if(!McPAT_dep_resource_conflict_check)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create a dependency_check_logic %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"flash_controller"))
    {
      XML_interface.sys.flashc.mc_clock = clock_frequency * 1e-6; // in MHz
      
      XML_interface.sys.flashc.number_mcs = 1; // number of controller is set by scaling option
      
      set_variable<parameters_module_t>(option,p_module,"type","high_performance",true);
      if(!stricmp(option,"high_performance"))
        XML_interface.sys.flashc.type = 0;
      else if(!stricmp(option,"low_power"))
        XML_interface.sys.flashc.type = 1;
      else 
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown flash controller type %s\n",option.c_str());
        #endif
        exit(1);        
      }

      set_variable<parameters_module_t>(XML_interface.sys.flashc.total_load_perc,p_module,"load_percentage",1.0); // load to BW ratio
      set_variable<parameters_module_t>(XML_interface.sys.flashc.peak_transfer_rate,p_module,"peak_transfer_rate",200,true); // in MBps
      
      set_variable<parameters_module_t>(option,p_module,"withPHY","true",true);
      XML_interface.sys.flashc.withPHY = stricmp(option,"false");

      McPAT_FlashController = new FlashController(&XML_interface,&input_p);
      
      if(!McPAT_FlashController)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(stdout,"EI ERROR (McPAT): unable to create a flash_controller %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"functional_unit"))
    {
      int num_fu;
      set_variable<parameters_module_t>(num_fu,p_module,"num_fu",1);

      core_p.num_alus = core_p.num_fpus = core_p.num_muls = num_fu;
      
      /*
      double duty_cycle;
      set_variable<parameters_module_t>(duty_cycle,p_module,"duty_cycle",1);
      core_p.ALU_duty_cycle = core_p.ALU_duty_cycle = core_p.MUL_duty_cycle = duty_cycle;
      */
      
      set_variable<parameters_module_t>(energy_submodel,p_module,"energy_submodel","n/a",true);
      if(!stricmp(energy_submodel,"alu"))
        McPAT_FunctionalUnit = new FunctionalUnit(&XML_interface,0,&input_p,core_p,ALU);
      else if(!stricmp(energy_submodel,"mul"))
        McPAT_FunctionalUnit = new FunctionalUnit(&XML_interface,0,&input_p,core_p,MUL);
      else if(!stricmp(energy_submodel,"fpu"))
        McPAT_FunctionalUnit = new FunctionalUnit(&XML_interface,0,&input_p,core_p,FPU);
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable energy_submodel %s\n",energy_submodel.c_str());
        #endif
        exit(1);
      }

      if(!McPAT_FunctionalUnit)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create a functional_unit %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"instruction_decoder"))
    {
      set_variable<parameters_module_t>(option,p_module,"x86","false",true);
      core_p.x86 = stricmp(option,"false");
      
      /* This part works different from the McPAT. */
      /* McPAT doesn't scale the leakage of the instruction decoder even if the decodeW changes. */
      if(core_p.x86)
      {
        area_scaling = scaling;
        scaling = 1.0;
      }

      int opcode;
      set_variable<parameters_module_t>(opcode,p_module,"opcode",0,true); // opcode length
    
      McPAT_inst_decoder = new inst_decoder(true,&input_p,opcode,1,core_p.x86,device_ty,core_p.core_ty);

      if(!McPAT_inst_decoder)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an instruction_decoder %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"interconnect"))
    {
      if(XML_interface.sys.Embedded)
      {
        input_p.wire_is_mat_type = 0;
        input_p.wire_os_mat_type = 0;
        input_p.wt = Global_30;
      }
      else
      {
        input_p.wire_is_mat_type = 2;
        input_p.wire_os_mat_type = 2;
        input_p.wt = Global;
      }
      input_p.throughput = 1.0/clock_frequency;
      input_p.latency = 1.0/clock_frequency;
      
      set_variable<parameters_module_t>(option,p_module,"pipelinable","false");
      input_p.pipelinable = stricmp(option,"false");

      double routing;
      if(input_p.pipelinable)
        set_variable<parameters_module_t>(routing,p_module,"routing_over_percentage",0.5,true);

      bool opt_local;
      if(device_ty == Core_device)
        opt_local = core_p.opt_local;
      else
      {
        set_variable<parameters_technology_t>(option,p_technology,"opt_local","n/a");
        if(!stricmp(option,"n/a"))
          set_variable<parameters_module_t>(option,p_module,"opt_local","false");
        opt_local = stricmp(option,"false");
      }

      // overwrite the wire_type
      set_variable<parameters_module_t>(option,p_module,"wire_type","global");
      if(!stricmp(option,"global"))
        input_p.wt = (Wire_type)Global;
      else if(!stricmp(option,"global_5"))
        input_p.wt = (Wire_type)Global_5;
      else if(!stricmp(option,"global_10"))
        input_p.wt = (Wire_type)Global_10;
      else if(!stricmp(option,"global_20"))
        input_p.wt = (Wire_type)Global_20;
      else if(!stricmp(option,"global_30"))
        input_p.wt = (Wire_type)Global_30;
      else if(!stricmp(option,"low_swing"))
        input_p.wt = (Wire_type)Low_swing;
      else if(!stricmp(option,"semi_global"))
        input_p.wt = (Wire_type)Semi_global;
      else if(!stricmp(option,"transmission"))
        input_p.wt = (Wire_type)Transmission;
      else if(!stricmp(option,"optical"))
        input_p.wt = (Wire_type)Optical;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown wire_type %s\n",option.c_str());
        #endif
        exit(1);
      }

      // data width
      int data;
      set_variable<parameters_module_t>(data,p_module,"data",0,true);

      // wire length
      double wire_length;
      set_variable<parameters_module_t>(wire_length,p_module,"wire_length",0.0);

      if(wire_length <= 0.0)
      {
        wire_length = 0.0;
  
        string first_module = p_module.get_option("connect");
        string current_module = first_module;
        while(stricmp(current_module,"n/a"))
        {
          double increment = sqrt(energy_introspector->pull_data<dimension_t>(0.0,"module",current_module,"dimension").area);
          if(!increment)
          {
            #ifdef EI_CONFIG_FILEOUT
            fprintf(energy_introspector->fp_config,"EI WARNING (McPAT): interconnect wire %s connects to %s which has zero area\n",p_module.ID.c_str(),current_module.c_str());
            #endif
          }
          wire_length += increment;

          current_module = p_module.get_option("connect");
          if(!stricmp(first_module,current_module))
            break;
        }      
      }

      if((wire_length <= 0.0)||(wire_length >= 1.0)) // wire_length might be given as um
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): invalid interconnect wire length\n");
        #endif
        exit(1);
      }
      else
        wire_length *= 1e6; // um

      McPAT_interconnect = new interconnect("interconnect",device_ty,1,1,data,wire_length,&input_p,\
                                            3,input_p.pipelinable,routing,opt_local,core_p.core_ty,input_p.wt);

      if(!McPAT_interconnect)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an interconnect %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"memory_controller"))
    {
      // correction
      input_p.wire_is_mat_type = 2;
      input_p.wire_os_mat_type = 2;
      input_p.wt = (Wire_type)Global;

      XML_interface.sys.mc.mc_clock = clock_frequency*1e-6; // in MHz
      clock_frequency *= 2; // DDR

      XML_interface.sys.mc.number_mcs = 1; // scaled up by "scaling" option
      set_variable<parameters_module_t>(XML_interface.sys.mc.llc_line_length,p_module,"line",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.req_window_size_per_channel,p_module,"request_window",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.IO_buffer_size_per_channel,p_module,"io_buffer",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.memory_channels_per_mc,p_module,"memory_channels",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.peak_transfer_rate,p_module,"transfer_rate",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.number_ranks,p_module,"ranks",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.databus_width,p_module,"data_bus",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.mc.addressbus_width,p_module,"address_bus",0,true);
      XML_interface.sys.physical_address_width = XML_interface.sys.mc.addressbus_width;

      set_variable<parameters_module_t>(option,p_module,"lvds","true");
      XML_interface.sys.mc.LVDS = stricmp(option,"false");

      set_variable<parameters_module_t>(option,p_module,"with_phy","false",true);
      XML_interface.sys.mc.withPHY = stricmp(option,"false");

      set_variable<parameters_module_t>(option,p_module,"model","low_power",true);
      if(!stricmp(option,"high_performance"))
        XML_interface.sys.mc.type = 0;
      else if(!stricmp(option,"low_power"))
        XML_interface.sys.mc.type = 1;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown model %s\n",option.c_str());
        #endif
        exit(1);
      }

      MemoryCtrl_type memoryctrl_type;
      memoryctrl_type = MC;
  
      set_variable<parameters_module_t>(energy_submodel,p_module,"energy_submodel","memory_controller");
      if(!stricmp(energy_submodel,"memory_controller"))
      {
        McPAT_MemoryController = new MemoryController(&XML_interface,&input_p,memoryctrl_type);
        if(!McPAT_MemoryController)
        {
          fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");              
          #ifdef EI_CONFIG_FILEOUT
          fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create a MemoryController %s\n",p_module.ID.c_str());
          #endif
          exit(1);
        }
      }
      else
      {
        // the following lines are part of MemoryController::set_mc_param()
        mc_p.clockRate = XML_interface.sys.mc.mc_clock*2; // DDR
        mc_p.clockRate *= 1e6;
        mc_p.llcBlockSize = int(ceil(XML_interface.sys.mc.llc_line_length/8.0))+XML_interface.sys.mc.llc_line_length; // ECC overhead
        mc_p.dataBusWidth = int(ceil(XML_interface.sys.mc.databus_width/8.0))+XML_interface.sys.mc.databus_width;
        mc_p.addressBusWidth = int(ceil(XML_interface.sys.mc.addressbus_width));
        mc_p.opcodeW = 16; // fixed
        mc_p.num_mcs = XML_interface.sys.mc.number_mcs;
        mc_p.num_channels = XML_interface.sys.mc.memory_channels_per_mc;
        mc_p.peakDataTransferRate = XML_interface.sys.mc.peak_transfer_rate;
        mc_p.memRank = XML_interface.sys.mc.number_ranks;
        mc_p.frontend_duty_cycle = 0.5; // fixed;
        mc_p.LVDS = XML_interface.sys.mc.LVDS;
        mc_p.type = XML_interface.sys.mc.type;
        mc_p.withPHY = XML_interface.sys.mc.withPHY;

        /*
        // MC param debugging
        cout << "mcp.clockRate = " << mcp.clockRate << endl;
        cout << "mcp.llcBlockSize = " << mcp.llcBlockSize << endl;
        cout << "mcp.dataBusWidth = " << mcp.dataBusWidth << endl;
        cout << "mcp.addressBusWidth = " << mcp.addressBusWidth << endl;
        cout << "mcp.opcodeW = " << mcp.opcodeW << endl;
        cout << "mcp.num_mcs = " << mcp.num_mcs << endl;
        cout << "mcp.num_channels = " << mcp.num_channels;
        cout << "mcp.peakDataTransferRate = " << mcp.peakDataTransferRate << endl;
        cout << "mcp.memRank = " << mcp.memRank << endl;
        cout << "mcp.frontend_duty_cycle = " << mcp.frontend_duty_cycle << endl;
        cout << "mcp.LVDS = " << mcp.LVDS << endl;
        cout << "mcp.type = " << mcp.type << endl;
        cout << "mcp.withPHY = " << mcp.withPHY << endl;
        */      

        if(!stricmp(energy_submodel,"memory_controller_frontend"))
        {
          McPAT_MCFrontEnd = new MCFrontEnd(&XML_interface,&input_p,mc_p,memoryctrl_type);
          if(!McPAT_MCFrontEnd)
          {
            fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");                
            #ifdef EI_CONFIG_FILEOUT
            fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an MCFrontEnd %s\n",p_module.ID.c_str());
            #endif
            exit(1);
          }
        }
        else if(!stricmp(energy_submodel,"memory_controller_backend"))
        {
          McPAT_MCBackend = new MCBackend(&input_p,mc_p,memoryctrl_type);
          if(!McPAT_MCBackend)
          {
            fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");                
            #ifdef EI_CONFIG_FILEOUT
            fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an MCBackend %s\n",p_module.ID.c_str());
            #endif
            exit(1);
          }
        }
        else if(!stricmp(energy_submodel,"memory_controller_phy"))
        {
          McPAT_MCPHY = new MCPHY(&input_p,mc_p,memoryctrl_type);
          if(!McPAT_MCPHY)
          {
            fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");                
            #ifdef EI_CONFIG_FILEOUT
            fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an MCPHY %s\n",p_module.ID.c_str());
            #endif
            exit(1);
          }
        }
        else
        {
          fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");              
          #ifdef EI_CONFIG_FILEOUT
          fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown energy_submodel %s\n",energy_submodel.c_str());
          #endif
          exit(1);
        }
      }
    }
    else if(!stricmp(energy_model,"network"))
    {
      // correction
      if(XML_interface.sys.Embedded)
      {
        input_p.wt = (Wire_type)Global_30;
        input_p.wire_is_mat_type = 0;
        input_p.wire_os_mat_type = 1;
      }
      else
      { 
        input_p.wt = (Wire_type)Global;
        input_p.wire_is_mat_type = 2;
        input_p.wire_os_mat_type = 2;
      }

      XML_interface.sys.NoC[0].clockrate = clock_frequency*1e-6;
      // scaled up by "scaling" option
      XML_interface.sys.NoC[0].horizontal_nodes = 1;
      XML_interface.sys.NoC[0].vertical_nodes = 1;

      set_variable<parameters_module_t>(energy_submodel,p_module,"energy_submodel","bus",true);
      if(!stricmp(energy_submodel,"bus"))
        XML_interface.sys.NoC[0].type = 0;
      else if(!stricmp(energy_submodel,"router"))
        XML_interface.sys.NoC[0].type = 1;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown type %s\n",option.c_str());
        #endif
        exit(1);
      }

      set_variable<parameters_module_t>(XML_interface.sys.NoC[0].flit_bits,p_module,"flit_bits",0,true);

      if(XML_interface.sys.NoC[0].type) // router type
      {
        set_variable<parameters_module_t>(XML_interface.sys.NoC[0].input_ports,p_module,"input_ports",0,true);
        set_variable<parameters_module_t>(XML_interface.sys.NoC[0].output_ports,p_module,"output_ports",0,true);
        set_variable<parameters_module_t>(XML_interface.sys.NoC[0].virtual_channel_per_port,p_module,"virtual_channels",0,true);
      }

      set_variable<parameters_module_t>(XML_interface.sys.NoC[0].duty_cycle,p_module,"duty_cycle",1.0,true);
      set_variable<parameters_module_t>(XML_interface.sys.NoC[0].link_throughput,p_module,"link_throughput",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.NoC[0].link_latency,p_module,"link_latency",0,true);
      set_variable<parameters_module_t>(XML_interface.sys.NoC[0].chip_coverage,p_module,"chip_coverage",1.0,true);
      set_variable<parameters_module_t>(XML_interface.sys.NoC[0].route_over_perc,p_module,"route_over_percentage",0.5,true);

      double traffic_pattern;
      set_variable<parameters_module_t>(traffic_pattern,p_module,"traffic_pattern",1.0);

      double chip_area;
      set_variable<parameters_module_t>(chip_area,p_module,"chip_area",0.0);

	  if(chip_area == 0.0)
      {
        string partitionID = energy_introspector->module.find(p_module.ID)->second.partition;
        string packageID = energy_introspector->partition.find(partitionID)->second.package;
	
        chip_area = energy_introspector->pull_data<dimension_t>(0.0,"package",packageID,"dimension").area;
	  }
	
      McPAT_NoC = new NoC(&XML_interface,0,&input_p,traffic_pattern,sqrt(chip_area*XML_interface.sys.NoC[0].chip_coverage*1e12)); // chip area in um^2

      if(!McPAT_NoC)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an NoC %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"niu_controller"))
    {
      XML_interface.sys.niu.clockrate = clock_frequency * 1e-6; // in MHz
      
      XML_interface.sys.niu.number_units = 1; // number of controller is set by scaling option
      
      set_variable<parameters_module_t>(option,p_module,"type","high_performance",true);
      if(!stricmp(option,"high_performance"))
        XML_interface.sys.niu.type = 0;
      else if(!stricmp(option,"low_power"))
        XML_interface.sys.niu.type = 1;
      else 
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown NIU controller type %s\n",option.c_str());
        #endif
        exit(1);        
      }

      set_variable<parameters_module_t>(XML_interface.sys.niu.total_load_perc,p_module,"load_percentage",1.0); // load to BW ratio

      McPAT_NIUController = new NIUController(&XML_interface,&input_p);
      
      if(!McPAT_NIUController)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(stdout,"EI ERROR (McPAT): unable to create an niu_controller %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"pcie_controller"))
    {
      XML_interface.sys.pcie.clockrate = clock_frequency * 1e-6; // in MHz
      
      XML_interface.sys.pcie.number_units = 1; // number of controller is set by scaling option
      
      set_variable<parameters_module_t>(option,p_module,"type","high_performance",true);
      if(!stricmp(option,"high_performance"))
        XML_interface.sys.niu.type = 0;
      else if(!stricmp(option,"low_power"))
        XML_interface.sys.niu.type = 1;
      else 
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown NIU controller type %s\n",option.c_str());
        #endif
        exit(1);        
      }

      set_variable<parameters_module_t>(XML_interface.sys.pcie.total_load_perc,p_module,"load_percentage",1.0); // load to BW ratio
      set_variable<parameters_module_t>(XML_interface.sys.pcie.num_channels,p_module,"channels",0,true);
      set_variable<parameters_module_t>(option,p_module,"withPHY","true",true);
      XML_interface.sys.flashc.withPHY = stricmp(option,"false");

      McPAT_PCIeController = new PCIeController(&XML_interface,&input_p);
      
      if(!McPAT_PCIeController)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(stdout,"EI ERROR (McPAT): unable to create an pcie_controller %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"pipeline"))
    {
      core_p.Embedded = XML_interface.sys.Embedded;

      set_variable<parameters_module_t>(input_p.pipeline_stages,p_module,"pipeline_stages",0,true);
      set_variable<parameters_module_t>(input_p.per_stage_vector,p_module,"per_stage_vector",0);
	
      if(input_p.per_stage_vector == 0) // McPAT calculates stage vector      
      {
        core_p.pipeline_stages = input_p.pipeline_stages;	
        set_variable<parameters_module_t>(option,p_module,"x86","false",true);
        core_p.x86 = stricmp(option,"false");

        if(core_p.x86)
          set_variable<parameters_module_t>(core_p.micro_opcode_length,p_module,"microopcode",0,true);
        else
          set_variable<parameters_module_t>(core_p.opcode_length,p_module,"opcode",0,true);

        set_variable<parameters_module_t>(core_p.pc_width,p_module,"pc",0,true);
        set_variable<parameters_module_t>(core_p.fetchW,p_module,"fetch_width",0,true);
        set_variable<parameters_module_t>(core_p.decodeW,p_module,"decode_width",0,true);
        set_variable<parameters_module_t>(core_p.issueW,p_module,"issue_width",0,true);
        set_variable<parameters_module_t>(core_p.instruction_length,p_module,"instruction_length",0,true);
        set_variable<parameters_module_t>(core_p.int_data_width,p_module,"int_data_width",0,true);

        set_variable<parameters_module_t>(core_p.num_hthreads,p_module,"hthreads",1,true);
        core_p.multithreaded = (core_p.num_hthreads > 1);
        if(core_p.multithreaded)
          set_variable<parameters_module_t>(core_p.perThreadState,p_module,"thread_states",0,true);

        if(core_p.core_ty == Inorder)
        {
          set_variable<parameters_module_t>(core_p.arch_ireg_width,p_module,"arch_int_regs",0,true);
          core_p.arch_ireg_width = int(ceil(log2(core_p.arch_ireg_width)));
        }
        else
        {
          set_variable<parameters_module_t>(core_p.phy_ireg_width,p_module,"phy_int_regs",0,true);
          core_p.phy_ireg_width = int(ceil(log2(core_p.phy_ireg_width)));
          set_variable<parameters_module_t>(core_p.v_address_width,p_module,"virtual_address",0,true);
          set_variable<parameters_module_t>(core_p.commitW,p_module,"commit_width",0,true);
        }
      }

      McPAT_Pipeline = new Pipeline(&input_p,core_p,device_ty,(device_ty==Core_device)&&(input_p.per_stage_vector==0),true);

      if(!McPAT_Pipeline)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create a pipeline %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"selection_logic"))
    {
      int input, output;
      set_variable<parameters_module_t>(input,p_module,"selection_input",0,true);
      set_variable<parameters_module_t>(output,p_module,"selection_output",0,true);

      McPAT_selection_logic = new selection_logic(true,input,output,&input_p,device_ty,core_p.core_ty);

      if(!McPAT_selection_logic)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create a selection logic %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"undifferentiated_core"))
    {
      set_variable<parameters_module_t>(core_p.pipeline_stages,p_module,"pipeline_stages",0,true);
      set_variable<parameters_module_t>(core_p.num_hthreads,p_module,"hthreads",1);
      set_variable<parameters_module_t>(core_p.issueW,p_module,"issue_width",0,true);

      if(XML_interface.sys.Embedded)
      {
        set_variable<parameters_module_t>(option,p_module,"opt_clockrate","true",true);
        XML_interface.sys.opt_clockrate = stricmp(option,"false");
      }

      McPAT_UndiffCore = new UndiffCore(&XML_interface,0,&input_p,core_p);

      if(!McPAT_UndiffCore)
      {
        fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");            
        #ifdef EI_CONFIG_FILEOUT
        fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unable to create an undifferentiated core %s\n",p_module.ID.c_str());
        #endif
        exit(1);
      }
    }
    else if(stricmp(energy_model,"none"))
    {
      fprintf(stdout,"EI ERROR (McPAT): check configuration log\n");          
      #ifdef EI_CONFIG_FILEOUT
      fprintf(energy_introspector->fp_config,"EI ERROR (McPAT): unknown energy_model %s\n",energy_model.c_str());
      #endif
      exit(1);
    }

    input_p = *g_ip; // Copy back error-corrected modular parameters
    tech_p = g_tp; // Copy back initialzed technology parameters

    energy_introspector->module.find(p_module.ID)->second.queue.push<double>(MAX_TIME,MAX_TIME,"temperature",input_p.temp);

    /*
    // architectural parameters debugging
    cout << "cache_sz: " << g_ip->cache_sz << endl;
    cout << "line_sz: " << g_ip->line_sz << endl;
    cout << "assoc: " << g_ip->assoc << endl;
    cout << "nbanks: " << g_ip->nbanks << endl;
    cout << "out_w: " << g_ip->out_w << endl;
    cout << "specific_tag: " << g_ip->specific_tag << endl;
    cout << "tag_w: " << g_ip->tag_w << endl;
    cout << "access_mode: " << g_ip->access_mode << endl;
    cout << "obj_func_dyn_energy: " << g_ip->obj_func_dyn_energy << endl;
    cout << "obj_func_dyn_power: " << g_ip->obj_func_dyn_power << endl;
    cout << "obj_func_leak_power: " << g_ip->obj_func_leak_power << endl;
    cout << "obj_func_cycle_t: " << g_ip->obj_func_cycle_t << endl;
    cout << "F_sz_nm: " << g_ip->F_sz_nm << endl;
    cout << "F_sz_um: " << g_ip->F_sz_um << endl;
    cout << "num_rw_ports: " << g_ip->num_rw_ports << endl;
    cout << "num_rd_ports: " << g_ip->num_rd_ports << endl;
    cout << "num_wr_ports: " << g_ip->num_wr_ports << endl;
    cout << "num_se_rd_ports: " << g_ip->num_se_rd_ports << endl;
    cout << "num_search_ports: " << g_ip->num_search_ports << endl;
    cout << "is_main_mem: " << g_ip->is_main_mem << endl;
    cout << "is_cache: " << g_ip->is_cache << endl;
    cout << "pure_ram: " << g_ip->pure_ram << endl;
    cout << "pure_cam: " << g_ip->pure_cam << endl;
    cout << "rpters_in_htree: " << g_ip->rpters_in_htree << endl;
    cout << "ver_htree_wires_over_array: " << g_ip->ver_htree_wires_over_array << endl;
    cout << "broadcast_addr_din_over_ver_htrees: " << g_ip->broadcast_addr_din_over_ver_htrees << endl;
    cout << "temp: " << g_ip->temp << endl;
    cout << "ram_cell_tech_type: " << g_ip->ram_cell_tech_type << endl;
    cout << "peri_global_tech_type: " << g_ip->peri_global_tech_type << endl;
    cout << "data_arr_ram_cell_tech_type: " << g_ip->data_arr_ram_cell_tech_type << endl;
    cout << "data_arr_peri_global_tech_type: " << g_ip->data_arr_peri_global_tech_type << endl;
    cout << "tag_arr_ram_cell_tech_type: " << g_ip->tag_arr_ram_cell_tech_type << endl;
    cout << "tag_arr_peri_global_tech_type: " << g_ip->tag_arr_peri_global_tech_type << endl;
    cout << "burst_len: " << g_ip->burst_len << endl;
    cout << "int_prefetch_w: " << g_ip->int_prefetch_w << endl;
    cout << "page_sz_bits: " << g_ip->page_sz_bits << endl;
    cout << "ic_proj_type: " << g_ip->ic_proj_type << endl;
    cout << "wire_is_mat_type: " << g_ip->wire_is_mat_type << endl;
    cout << "wire_os_mat_type: " << g_ip->wire_os_mat_type << endl;
    cout << "Wiretype: " << g_ip->wt << endl;
    cout << "force_wiretype: " << g_ip->force_wiretype << endl;
    cout << "print_input_args: " << g_ip->print_input_args << endl;
    cout << " nuca_cache_sz: " << g_ip->nuca_cache_sz << endl;
    cout << "ndbl: " << g_ip->ndbl << endl;
    cout << "ndwl: " << g_ip->ndwl << endl;
    cout << "nspd: " << g_ip->nspd << endl;
    cout << "ndsam1: " << g_ip->ndsam1 << endl;
    cout << "ndsam2: " << g_ip->ndsam2 << endl;
    cout << "ndcm: " << g_ip->ndcm << endl;
    cout << "force_cache_config: " << g_ip->force_cache_config << endl;
    cout << "cache_level: " << g_ip->cache_level << endl;
    cout << "cores: " << g_ip->cores << endl;
    cout << "nuca_bank_count: " << g_ip->nuca_bank_count << endl;
    cout << "force_nuca_bank: " << g_ip->force_nuca_bank << endl;
    cout << "delay_wt: " << g_ip->delay_wt << endl;
    cout << "dynamic_power_wt: " << g_ip->dynamic_power_wt << endl;
    cout << "leakage_power_wt: " << g_ip->leakage_power_wt << endl;
    cout << "cycle_time_wt: " << g_ip->cycle_time_wt << endl;
    cout << "area_wt: " << g_ip->area_wt << endl;
    cout << "delay_wt_nuca: " << g_ip->delay_wt_nuca << endl;
    cout << "dynamic_power_wt_nuca: " << g_ip->dynamic_power_wt_nuca << endl;
    cout << "leakage_power_wt_nuca: " << g_ip->leakage_power_wt_nuca << endl;
    cout << "cycle_time_wt_nuca: " << g_ip->cycle_time_wt_nuca << endl;
    cout << "area_wt_nuca: " << g_ip->area_wt_nuca << endl;
    cout << "delay_dev: " << g_ip->delay_dev << endl;
    cout << "dynamic_power_dev: " << g_ip->dynamic_power_dev << endl;
    cout << "leakage_power_dev: " << g_ip->leakage_power_dev << endl;
    cout << "cycle_time_dev: " << g_ip->cycle_time_dev << endl;
    cout << "area_dev: " << g_ip->area_dev << endl;
    cout << "delay_dev_nuca: " << g_ip->delay_dev_nuca << endl;
    cout << "dynamic_power_dev_nuca: " << g_ip->dynamic_power_dev_nuca << endl;
    cout << "leakage_power_dev_nuca: " << g_ip->leakage_power_dev_nuca << endl;
    cout << "cycle_time_dev_nuca: " << g_ip->cycle_time_dev_nuca << endl;
    cout << "area_dev_nuca: " << g_ip->area_dev_nuca << endl;
    cout << "ed: " << g_ip->ed << endl;
    cout << "nuca: " << g_ip->nuca << endl;
    cout << "fast_access: " << g_ip->fast_access << endl;
    cout << "block_sz: " << g_ip->block_sz << endl;
    cout << "tag_assoc: " << g_ip->tag_assoc << endl;
    cout << "data_assoc: " << g_ip->data_assoc << endl;
    cout << "is_seq_acc: " << g_ip->is_seq_acc << endl;
    cout << "fully_assoc: " << g_ip->fully_assoc << endl;
    cout << "nsets: " << g_ip->nsets << endl;
    cout << "print_detail: " << g_ip->print_detail << endl;
    cout << "add_ecc_b_: " << g_ip->add_ecc_b_ << endl;
    cout << "throughput: " << g_ip->throughput << endl;
    cout << "latency: " << g_ip->latency << endl;
    cout << "pipelinable: " << g_ip->pipelinable << endl;
    cout << "pipeline_stages: " << g_ip->pipeline_stages << endl;
    cout << "per_stage_vector: " << g_ip->per_stage_vector << endl;
    cout << "with_clock_grid: " << g_ip->with_clock_grid << endl;
    cout << "\n" << endl;
    */
    /*
    // technology parameters debugging
    cout << "g_tp.user_set = " << g_tp.user_set << endl;
    cout << "g_tp.peri_global.Vdd = " << g_tp.peri_global.Vdd << endl;
    cout << "g_tp.peri_global.Vth = " << g_tp.peri_global.Vth << endl;
    cout << "g_tp.peri_global.l_phy = " << g_tp.peri_global.l_phy << endl;
    cout << "g_tp.peri_global.l_elec = " << g_tp.peri_global.l_elec << endl;
    cout << "g_tp.peri_global.t_ox = " << g_tp.peri_global.t_ox << endl;
    cout << "g_tp.peri_global.C_ox = " << g_tp.peri_global.C_ox << endl;
    cout << "g_tp.peri_global.C_g_ideal = " << g_tp.peri_global.C_g_ideal << endl;
    cout << "g_tp.peri_global.C_fringe = " << g_tp.peri_global.C_fringe << endl;
    cout << "g_tp.peri_global.C_overlap = " << g_tp.peri_global.C_overlap << endl;
    cout << "g_tp.peri_global.C_junc = " << g_tp.peri_global.C_junc << endl;
    cout << "g_tp.peri_global.C_junc_sidewall = " << g_tp.peri_global.C_junc_sidewall << endl;
    cout << "g_tp.peri_global.I_on_n = " << g_tp.peri_global.I_on_n << endl;
    cout << "g_tp.peri_global.I_on_p = " << g_tp.peri_global.I_on_p << endl;
    cout << "g_tp.peri_global.R_nch_on = " << g_tp.peri_global.R_nch_on << endl;
    cout << "g_tp.peri_global.R_pch_on = " << g_tp.peri_global.R_pch_on << endl;
    cout << "g_tp.peri_global.n_to_p_eff_curr_drv_ratio = " << g_tp.peri_global.n_to_p_eff_curr_drv_ratio << endl;
    cout << "g_tp.peri_global.long_channel_leakage_reduction = " << g_tp.peri_global.long_channel_leakage_reduction << endl;
    cout << "g_tp.peri_global.I_off_n = " << g_tp.peri_global.I_off_n << endl;
    cout << "g_tp.peri_global.I_off_p = " << g_tp.peri_global.I_off_p << endl;
    cout << "g_tp.peri_global.I_g_on_n = " << g_tp.peri_global.I_g_on_n << endl;
    cout << "g_tp.peri_global.I_g_on_p = " << g_tp.peri_global.I_g_on_p << endl;
    cout << "g_tp.sram_cell.Vdd = " << g_tp.sram_cell.Vdd << endl;
    cout << "g_tp.sram_cell.Vth = " << g_tp.sram_cell.Vth << endl;
    cout << "g_tp.sram_cell.l_phy = " << g_tp.sram_cell.l_phy << endl;
    cout << "g_tp.sram_cell.l_elec = " << g_tp.sram_cell.l_elec << endl;
    cout << "g_tp.sram_cell.t_ox = " << g_tp.sram_cell.t_ox << endl;
    cout << "g_tp.sram_cell.C_ox = " << g_tp.sram_cell.C_ox << endl;
    cout << "g_tp.sram_cell.C_g_ideal = " << g_tp.sram_cell.C_g_ideal << endl;
    cout << "g_tp.sram_cell.C_fringe = " << g_tp.sram_cell.C_fringe << endl;
    cout << "g_tp.sram_cell.C_overlap = " << g_tp.sram_cell.C_overlap << endl;
    cout << "g_tp.sram_cell.C_junc = " << g_tp.sram_cell.C_junc << endl;
    cout << "g_tp.sram_cell.C_junc_sidewall = " << g_tp.sram_cell.C_junc_sidewall << endl;
    cout << "g_tp.sram_cell.I_on_n = " << g_tp.sram_cell.I_on_n << endl;
    cout << "g_tp.sram_cell.I_on_p = " << g_tp.sram_cell.I_on_p << endl;
    cout << "g_tp.sram_cell.R_nch_on = " << g_tp.sram_cell.R_nch_on << endl;
    cout << "g_tp.sram_cell.R_pch_on = " << g_tp.sram_cell.R_pch_on << endl;
    cout << "g_tp.sram_cell.n_to_p_eff_curr_drv_ratio = " << g_tp.sram_cell.n_to_p_eff_curr_drv_ratio << endl;
    cout << "g_tp.sram_cell.long_channel_leakage_reduction = " << g_tp.sram_cell.long_channel_leakage_reduction << endl;
    cout << "g_tp.sram_cell.I_off_n = " << g_tp.sram_cell.I_off_n << endl;
    cout << "g_tp.sram_cell.I_off_p = " << g_tp.sram_cell.I_off_p << endl;
    cout << "g_tp.sram_cell.I_g_on_n = " << g_tp.sram_cell.I_g_on_n << endl;
    cout << "g_tp.sram_cell.I_g_on_p = " << g_tp.sram_cell.I_g_on_p << endl;
    cout << "g_tp.cam_cell.Vdd = " << g_tp.cam_cell.Vdd << endl;
    cout << "g_tp.cam_cell.Vth = " << g_tp.cam_cell.Vth << endl;
    cout << "g_tp.cam_cell.l_phy = " << g_tp.cam_cell.l_phy << endl;
    cout << "g_tp.cam_cell.l_elec = " << g_tp.cam_cell.l_elec << endl;
    cout << "g_tp.cam_cell.t_ox = " << g_tp.cam_cell.t_ox << endl;
    cout << "g_tp.cam_cell.C_ox = " << g_tp.cam_cell.C_ox << endl;
    cout << "g_tp.cam_cell.C_g_ideal = " << g_tp.cam_cell.C_g_ideal << endl;
    cout << "g_tp.cam_cell.C_fringe = " << g_tp.cam_cell.C_fringe << endl;
    cout << "g_tp.cam_cell.C_overlap = " << g_tp.cam_cell.C_overlap << endl;
    cout << "g_tp.cam_cell.C_junc = " << g_tp.cam_cell.C_junc << endl;
    cout << "g_tp.cam_cell.C_junc_sidewall = " << g_tp.cam_cell.C_junc_sidewall << endl;
    cout << "g_tp.cam_cell.I_on_n = " << g_tp.cam_cell.I_on_n << endl;
    cout << "g_tp.cam_cell.I_on_p = " << g_tp.cam_cell.I_on_p << endl;
    cout << "g_tp.cam_cell.R_nch_on = " << g_tp.cam_cell.R_nch_on << endl;
    cout << "g_tp.cam_cell.R_pch_on = " << g_tp.cam_cell.R_pch_on << endl;
    cout << "g_tp.cam_cell.n_to_p_eff_curr_drv_ratio = " << g_tp.cam_cell.n_to_p_eff_curr_drv_ratio << endl;
    cout << "g_tp.cam_cell.long_channel_leakage_reduction = " << g_tp.cam_cell.long_channel_leakage_reduction << endl;
    cout << "g_tp.cam_cell.I_off_n = " << g_tp.cam_cell.I_off_n << endl;
    cout << "g_tp.cam_cell.I_off_p = " << g_tp.cam_cell.I_off_p << endl;
    cout << "g_tp.cam_cell.I_g_on_n = " << g_tp.cam_cell.I_g_on_n << endl;
    cout << "g_tp.cam_cell.I_g_on_p = " << g_tp.cam_cell.I_g_on_p << endl;
    cout << "g_tp.wire_local.pitch = " << g_tp.wire_local.pitch << endl;
    cout << "g_tp.wire_local.R_per_um = " << g_tp.wire_local.R_per_um << endl;
    cout << "g_tp.wire_local.C_per_um = " << g_tp.wire_local.C_per_um << endl;
    cout << "g_tp.wire_local.horiz_dielectric_constant = " << g_tp.wire_local.horiz_dielectric_constant << endl;
    cout << "g_tp.wire_local.vert_dielectric_constant = " << g_tp.wire_local.vert_dielectric_constant << endl;
    cout << "g_tp.wire_local.aspect_ratio = " << g_tp.wire_local.aspect_ratio << endl;
    cout << "g_tp.wire_local.ild_thickness = " << g_tp.wire_local.ild_thickness << endl;
    cout << "g_tp.wire_local.miller_value = " << g_tp.wire_local.miller_value << endl;
    cout << "g_tp.wire_inside_mat.pitch = " << g_tp.wire_inside_mat.pitch << endl;
    cout << "g_tp.wire_inside_mat.R_per_um = " << g_tp.wire_inside_mat.R_per_um << endl;
    cout << "g_tp.wire_inside_mat.C_per_um = " << g_tp.wire_inside_mat.C_per_um << endl;
    cout << "g_tp.wire_inside_mat.horiz_dielectric_constant = " << g_tp.wire_inside_mat.horiz_dielectric_constant << endl;
    cout << "g_tp.wire_inside_mat.vert_dielectric_constant = " << g_tp.wire_inside_mat.vert_dielectric_constant << endl;
    cout << "g_tp.wire_inside_mat.aspect_ratio = " << g_tp.wire_inside_mat.aspect_ratio << endl;
    cout << "g_tp.wire_inside_mat.ild_thickness = " << g_tp.wire_inside_mat.ild_thickness << endl;
    cout << "g_tp.wire_inside_mat.miller_value = " << g_tp.wire_inside_mat.miller_value << endl;
    cout << "g_tp.wire_outside_mat.pitch = " << g_tp.wire_outside_mat.pitch << endl;
    cout << "g_tp.wire_outside_mat.R_per_um = " << g_tp.wire_outside_mat.R_per_um << endl;
    cout << "g_tp.wire_outside_mat.C_per_um = " << g_tp.wire_outside_mat.C_per_um << endl;
    cout << "g_tp.wire_outside_mat.horiz_dielectric_constant = " << g_tp.wire_outside_mat.horiz_dielectric_constant << endl;
    cout << "g_tp.wire_outside_mat.vert_dielectric_constant = " << g_tp.wire_outside_mat.vert_dielectric_constant << endl;
    cout << "g_tp.wire_outside_mat.aspect_ratio = " << g_tp.wire_outside_mat.aspect_ratio << endl;
    cout << "g_tp.wire_outside_mat.ild_thickness = " << g_tp.wire_outside_mat.ild_thickness << endl;
    cout << "g_tp.wire_outside_mat.miller_value = " << g_tp.wire_outside_mat.miller_value << endl;
    cout << "g_tp.scaling_factor.logic_scaling_co_eff = " << g_tp.scaling_factor.logic_scaling_co_eff << endl;
    cout << "g_tp.scaling_factor.core_tx_density = " << g_tp.scaling_factor.core_tx_density << endl;
    cout << "g_tp.scaling_factor.long_channel_leakage_reduction = " << g_tp.scaling_factor.long_channel_leakage_reduction << endl;
    cout << "g_tp.sram.b_w = " << g_tp.sram.b_w << endl;
    cout << "g_tp.sram.b_h = " << g_tp.sram.b_h << endl;
    cout << "g_tp.sram.cell_a_w = " << g_tp.sram.cell_a_w << endl;
    cout << "g_tp.sram.cell_pmos_w = " << g_tp.sram.cell_pmos_w << endl;
    cout << "g_tp.sram.cell_nmos_w = " << g_tp.sram.cell_nmos_w << endl;
    cout << "g_tp.sram.Vitbpre = " << g_tp.sram.Vbitpre << endl;
    cout << "g_tp.cam.b_w = " << g_tp.cam.b_w << endl;
    cout << "g_tp.cam.b_h = " << g_tp.cam.b_h << endl;
    cout << "g_tp.cam.cell_a_w = " << g_tp.cam.cell_a_w << endl;
    cout << "g_tp.cam.cell_pmos_w = " << g_tp.cam.cell_pmos_w << endl;
    cout << "g_tp.cam.cell_nmos_w = " << g_tp.cam.cell_nmos_w << endl;
    cout << "g_tp.cam.Vitbpre = " << g_tp.cam.Vbitpre << endl;
    cout << "g_tp.ram_wl_stitching_overhead_ = " << g_tp.ram_wl_stitching_overhead_ << endl;
    cout << "g_tp.min_w_nmos_ = " << g_tp.min_w_nmos_ << endl;
    cout << "g_tp.max_w_nmos_ = " << g_tp.max_w_nmos_ << endl;
    cout << "g_tp.max_w_nmos_dec = " << g_tp.max_w_nmos_dec << endl;
    cout << "g_tp.unit_len_wire_del = " << g_tp.unit_len_wire_del << endl;
    cout << "g_tp.FO4 = " << g_tp.FO4 << endl;
    cout << "g_tp.kinv = " << g_tp.kinv << endl;
    cout << "g_tp.vpp = " << g_tp.vpp << endl;
    cout << "g_tp.w_sense_en = " << g_tp.w_sense_en << endl;
    cout << "g_tp.w_sense_n = " << g_tp.w_sense_n << endl;
    cout << "g_tp.w_sense_p = " << g_tp.w_sense_p << endl;
    cout << "g_tp.sense_delay = " << g_tp.sense_delay << endl;
    cout << "g_tp.sense_dy_power = " << g_tp.sense_dy_power << endl;
    cout << "g_tp.w_iso = " << g_tp.w_iso << endl;
    cout << "g_tp.w_poly_contact = " << g_tp.w_poly_contact << endl;
    cout << "g_tp.spacing_poly_to_contact = " << g_tp.spacing_poly_to_contact << endl;
    cout << "g_tp.spacing_poly_to_poly = " << g_tp.spacing_poly_to_poly << endl;
    cout << "g_tp.w_comp_inv_p1 = " << g_tp.w_comp_inv_p1 << endl;
    cout << "g_tp.w_comp_inv_n1 = " << g_tp.w_comp_inv_n1 << endl;
    cout << "g_tp.w_comp_inv_p2 = " << g_tp.w_comp_inv_p2 << endl;
    cout << "g_tp.w_comp_inv_n2 = " << g_tp.w_comp_inv_n2 << endl;
    cout << "g_tp.w_comp_inv_p3 = " << g_tp.w_comp_inv_p3 << endl;
    cout << "g_tp.w_comp_inv_n3 = " << g_tp.w_comp_inv_n3 << endl;
    cout << "g_tp.w_eval_inv_p = " << g_tp.w_eval_inv_p << endl;
    cout << "g_tp.w_eval_inv_n = " << g_tp.w_eval_inv_n << endl;
    cout << "g_tp.w_comp_n = " << g_tp.w_comp_n << endl;
    cout << "g_tp.w_comp_p = " << g_tp.w_comp_p << endl;
    cout << "g_tp.dram_cell_I_on = " << g_tp.dram_cell_I_on << endl;
    cout << "g_tp.dram_cell_Vdd = " << g_tp.dram_cell_Vdd << endl;
    cout << "g_tp.dram_cell_I_off_worst_case_len_temp = " << g_tp.dram_cell_I_off_worst_case_len_temp << endl;
    cout << "g_tp.dram_cell_C = " << g_tp.dram_cell_C << endl;
    cout << "g_tp.gm_sense_amp_latch = " << g_tp.gm_sense_amp_latch << endl;
    cout << "g_tp.w_nmos_b_mux = " << g_tp.w_nmos_b_mux << endl;
    cout << "g_tp.w_nmos_sa_mux = " << g_tp.w_nmos_sa_mux << endl;
    cout << "g_tp.w_pmos_bl_precharge = " << g_tp.w_pmos_bl_precharge << endl;
    cout << "g_tp.w_pmos_bl_eq = " << g_tp.w_pmos_bl_eq << endl;
    cout << "g_tp.MIN_GAP_BET_P_AND_N_DIFFS = " << g_tp.MIN_GAP_BET_P_AND_N_DIFFS << endl;
    cout << "g_tp.MIN_GAP_BET_SAME_TYPE_DIFFS = " << g_tp.MIN_GAP_BET_SAME_TYPE_DIFFS << endl;
    cout << "g_tp.HPOWERRAIL = " << g_tp.HPOWERRAIL << endl;
    cout << "g_tp.cell_h_def = " << g_tp.cell_h_def << endl;
    cout << "g_tp.chip_layout_overhead = " << g_tp.chip_layout_overhead << endl;
    cout << "g_tp.macro_layout_overhead = " << g_tp.macro_layout_overhead << endl;
    cout << "g_tp.sckt_co_eff = " << g_tp.sckt_co_eff << endl;
    cout << "g_tp.fringe_cap = " << g_tp.fringe_cap << endl;
    cout << "g_tp.h_dec = " << g_tp.h_dec << endl;

    cout << "g_tp.dram_acc.Vth = " << g_tp.dram_acc.Vth << endl;
    cout << "g_tp.dram_acc.l_phy = " << g_tp.dram_acc.l_phy << endl;
    cout << "g_tp.dram_acc.l_elec = " << g_tp.dram_acc.l_elec << endl;
    cout << "g_tp.dram_acc.C_g_ideal = " << g_tp.dram_acc.C_g_ideal << endl;
    cout << "g_tp.dram_acc.C_fringe = " << g_tp.dram_acc.C_fringe << endl;
    cout << "g_tp.dram_acc.C_junc = " << g_tp.dram_acc.C_junc << endl;
    cout << "g_tp.dram_acc.C_junc_sidewall = " << g_tp.dram_acc.C_junc_sidewall << endl;
    cout << "g_tp.dram_acc.I_on_n = " << g_tp.dram_acc.I_on_n << endl;
    cout << "g_tp.dram_wl.l_phy = " << g_tp.dram_wl.l_phy << endl;
    cout << "g_tp.dram_wl.l_elec = " << g_tp.dram_wl.l_elec << endl;
    cout << "g_tp.dram_wl.C_g_ideal = " << g_tp.dram_wl.C_g_ideal << endl;
    cout << "g_tp.dram_wl.C_fringe = " << g_tp.dram_wl.C_fringe << endl;
    cout << "g_tp.dram_wl.C_junc = " << g_tp.dram_wl.C_junc << endl;
    cout << "g_tp.dram_wl.C_junc_sidewall = " << g_tp.dram_wl.C_junc_sidewall << endl;
    cout << "g_tp.dram_wl.I_on_n = " << g_tp.dram_wl.I_on_n << endl;
    cout << "g_tp.dram_wl.R_nch_on = " << g_tp.dram_wl.R_nch_on << endl;
    cout << "g_tp.dram_wl.R_pch_on = " << g_tp.dram_wl.R_pch_on << endl;
    cout << "g_tp.dram_wl.n_to_p_eff_curr_drv_ratio = " << g_tp.dram_wl.n_to_p_eff_curr_drv_ratio << endl;
    cout << "g_tp.dram_wl.long_channel_leakage_reduction = " << g_tp.dram_wl.long_channel_leakage_reduction << endl;
    cout << "g_tp.dram_wl.I_off_n = " << g_tp.dram_wl.I_off_n << endl;
    cout << "g_tp.dram_wl.I_off_p = " << g_tp.dram_wl.I_off_p << endl;
    */
  }
}


energy_t ENERGYLIB_McPAT::get_unit_energy(bool is_tdp)
{
  energy_t unit_energy;

  if(!stricmp(energy_model,"array"))
  {
    unit_energy.read = McPAT_ArrayST->local_result.power.readOp.dynamic\
                       +McPAT_ArrayST->local_result.power.readOp.short_circuit;
    unit_energy.write = McPAT_ArrayST->local_result.power.writeOp.dynamic\
                        +McPAT_ArrayST->local_result.power.writeOp.short_circuit;
    
    if(McPAT_ArrayST->l_ip.tag_w > 0)
    {
      if(McPAT_ArrayST->local_result.tag_array2) // tag array exists
      {
        unit_energy.read_tag = McPAT_ArrayST->local_result.tag_array2->power.readOp.dynamic+McPAT_ArrayST->local_result.tag_array2->power.readOp.short_circuit;
        unit_energy.write_tag = McPAT_ArrayST->local_result.tag_array2->power.writeOp.dynamic+McPAT_ArrayST->local_result.tag_array2->power.writeOp.short_circuit;
      }
      else
      {
        unit_energy.read_tag = unit_energy.read;
        unit_energy.write_tag = unit_energy.write;
      }
    }
    
    if(McPAT_ArrayST->l_ip.num_search_ports)
      unit_energy.search = McPAT_ArrayST->local_result.power.searchOp.dynamic+McPAT_ArrayST->local_result.power.searchOp.short_circuit;

    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_ArrayST->local_result.power.readOp.longer_channel_leakage+McPAT_ArrayST->local_result.power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_ArrayST->local_result.power.readOp.leakage+McPAT_ArrayST->local_result.power.readOp.gate_leakage)/clock_frequency;
    
    if(is_tdp)
    {
      unit_energy.read *= McPAT_ArrayST->l_ip.num_rd_ports?McPAT_ArrayST->l_ip.num_rd_ports:McPAT_ArrayST->l_ip.num_rw_ports;
      unit_energy.write *= McPAT_ArrayST->l_ip.num_wr_ports?McPAT_ArrayST->l_ip.num_wr_ports:McPAT_ArrayST->l_ip.num_rw_ports;
    
      if(McPAT_ArrayST->l_ip.num_search_ports)
        unit_energy.search *= McPAT_ArrayST->l_ip.num_search_ports;
      
      unit_energy.read_tag = 0.0;
      unit_energy.write_tag = 0.0;
    }
  }
  else if(!stricmp(energy_model,"dependency_check_logic"))
  {
    unit_energy.read = McPAT_dep_resource_conflict_check->power.readOp.dynamic+McPAT_dep_resource_conflict_check->power.readOp.short_circuit;
    unit_energy.write = unit_energy.read;
      
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?McPAT_dep_resource_conflict_check->coredynp.decodeW:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:McPAT_dep_resource_conflict_check->coredynp.decodeW;
      /*unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;*/
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_dep_resource_conflict_check->power.readOp.longer_channel_leakage+McPAT_dep_resource_conflict_check->power.readOp.gate_leakage)/clock_frequency;
   else
      unit_energy.leakage = (McPAT_dep_resource_conflict_check->power.readOp.leakage+McPAT_dep_resource_conflict_check->power.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"flash_controller"))
  {
    unit_energy.read = unit_energy.write = McPAT_FlashController->power_t.readOp.dynamic;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_FlashController->power_t.readOp.longer_channel_leakage+McPAT_FlashController->power_t.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_FlashController->power_t.readOp.leakage+McPAT_FlashController->power_t.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"functional_unit"))
  {
    unit_energy.baseline = McPAT_FunctionalUnit->base_energy/clock_frequency*tech_p.sckt_co_eff;
    unit_energy.read = McPAT_FunctionalUnit->per_access_energy*tech_p.sckt_co_eff;
    unit_energy.write = unit_energy.read;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
      unit_energy.baseline *= TDP_duty_cycle.read>0.0?TDP_duty_cycle.read:TDP_duty_cycle.write;
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_FunctionalUnit->leakage*longer_channel_device_reduction(device_ty,McPAT_FunctionalUnit->coredynp.core_ty)+McPAT_FunctionalUnit->gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_FunctionalUnit->leakage+McPAT_FunctionalUnit->gate_leakage)/clock_frequency;
	  
    if(!stricmp(energy_submodel,"alu")||!stricmp(energy_submodel,"mul")) // This doesn't seem to be right
      unit_energy.leakage *= scaling;

  }
  else if(!stricmp(energy_model,"instruction_decoder"))
  {
    unit_energy.read = McPAT_inst_decoder->power.readOp.dynamic+McPAT_inst_decoder->power.readOp.short_circuit;
    unit_energy.write = unit_energy.read;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
      
      if(core_p.x86)
      {
        unit_energy.read *= area_scaling;
        unit_energy.write *= area_scaling;
      }
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_inst_decoder->power.readOp.longer_channel_leakage+McPAT_inst_decoder->power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_inst_decoder->power.readOp.leakage+McPAT_inst_decoder->power.readOp.gate_leakage)/clock_frequency;
      
    /* This part works different from the McPAT. */
    /* x86 instruction decoder leakage does not scale correctly. */
    if(core_p.x86)
      unit_energy.leakage *= (area_scaling/4.0);
  }
  else if(!stricmp(energy_model,"interconnect"))
  {
    unit_energy.read = McPAT_interconnect->power.readOp.dynamic+McPAT_interconnect->power.readOp.short_circuit;
    unit_energy.write = unit_energy.read;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_interconnect->power.readOp.longer_channel_leakage+McPAT_interconnect->power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_interconnect->power.readOp.leakage+McPAT_interconnect->power.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"memory_controller"))
  {
    if(!stricmp(energy_submodel,"memory_controller"))
    {
      McPAT_MemoryController->computeEnergy(true); // leakage is calculated here
      
      if(is_tdp)
        unit_energy.read = McPAT_MemoryController->power.readOp.dynamic;
      else
      {
        McPAT_MemoryController->mcp.executionTime = (double)1.0/McPAT_MemoryController->mcp.clockRate; // single clock time

        // separate calculation for read and write energies
        McPAT_MemoryController->XML->sys.mc.memory_reads = McPAT_MemoryController->mcp.reads = 1;
        McPAT_MemoryController->XML->sys.mc.memory_writes = McPAT_MemoryController->mcp.writes = 0;

        McPAT_MemoryController->frontend->XML = McPAT_MemoryController->XML;
        McPAT_MemoryController->frontend->mcp = McPAT_MemoryController->mcp;
        McPAT_MemoryController->transecEngine->mcp = McPAT_MemoryController->mcp;
        McPAT_MemoryController->PHY->mcp = McPAT_MemoryController->mcp;

        McPAT_MemoryController->computeEnergy(false);
        unit_energy.read = McPAT_MemoryController->rt_power.readOp.dynamic+McPAT_MemoryController->rt_power.readOp.short_circuit;
      
        McPAT_MemoryController->XML->sys.mc.memory_reads = McPAT_MemoryController->mcp.reads = 0;
        McPAT_MemoryController->XML->sys.mc.memory_writes = McPAT_MemoryController->mcp.writes = 1;

        McPAT_MemoryController->frontend->XML = McPAT_MemoryController->XML;
        McPAT_MemoryController->frontend->mcp = McPAT_MemoryController->mcp;
        McPAT_MemoryController->transecEngine->mcp = McPAT_MemoryController->mcp;
        McPAT_MemoryController->PHY->mcp = McPAT_MemoryController->mcp;

        McPAT_MemoryController->computeEnergy(false);
        unit_energy.write = McPAT_MemoryController->rt_power.readOp.dynamic+McPAT_MemoryController->rt_power.readOp.short_circuit; // McPAT MC has no power.writeOp
      }
      /*  
      if(is_tdp)
      {
        unit_energy.read *= (0.5*McPAT_MemoryController->mcp.num_channels);
        unit_energy.write *= (0.5*McPAT_MemoryController->mcp.num_channels);
      }
      */
      if(XML_interface.sys.longer_channel_device)
        unit_energy.leakage = (McPAT_MemoryController->rt_power.readOp.longer_channel_leakage+McPAT_MemoryController->rt_power.readOp.gate_leakage)/clock_frequency;
      else
        unit_energy.leakage = (McPAT_MemoryController->rt_power.readOp.leakage+McPAT_MemoryController->rt_power.readOp.gate_leakage)/clock_frequency;
    }
    else if(!stricmp(energy_submodel,"memory_controller_frontend"))
    {
      McPAT_MCFrontEnd->computeEnergy(true); // leakage is calculated here
      
      if(is_tdp)
        unit_energy.read = McPAT_MCFrontEnd->power.readOp.dynamic;
      else
      {
        McPAT_MCFrontEnd->mcp.executionTime = (double)1.0/McPAT_MCFrontEnd->mcp.clockRate; // single clock time

        // separate calculation for read and write energies
        McPAT_MCFrontEnd->XML->sys.mc.memory_reads = McPAT_MCFrontEnd->mcp.reads = 1;
        McPAT_MCFrontEnd->XML->sys.mc.memory_writes = McPAT_MCFrontEnd->mcp.writes = 0;
        McPAT_MCFrontEnd->computeEnergy(false);
        unit_energy.read = McPAT_MCFrontEnd->rt_power.readOp.dynamic+McPAT_MCFrontEnd->rt_power.readOp.short_circuit;

        McPAT_MCFrontEnd->XML->sys.mc.memory_reads = McPAT_MCFrontEnd->mcp.reads = 0;
        McPAT_MCFrontEnd->XML->sys.mc.memory_writes = McPAT_MCFrontEnd->mcp.writes = 1;
        McPAT_MCFrontEnd->computeEnergy(false);
        unit_energy.write = McPAT_MCFrontEnd->rt_power.readOp.dynamic+McPAT_MCFrontEnd->rt_power.readOp.short_circuit; // McPAT MC has no power.writeOp
      }
      /*  
      if(is_tdp)
      {
        unit_energy.read *= (0.5*McPAT_MCFrontEnd->mcp.num_channels);
        unit_energy.write *= (0.5*McPAT_MCFrontEnd->mcp.num_channels);
      }
      */
      if(XML_interface.sys.longer_channel_device)
        unit_energy.leakage = (McPAT_MCFrontEnd->rt_power.readOp.longer_channel_leakage+McPAT_MCFrontEnd->rt_power.readOp.gate_leakage)/clock_frequency;
      else
        unit_energy.leakage = (McPAT_MCFrontEnd->rt_power.readOp.leakage+McPAT_MCFrontEnd->rt_power.readOp.gate_leakage)/clock_frequency;
    }
    else if(!stricmp(energy_submodel,"memory_controller_backend"))
    { 
      McPAT_MCBackend->computeEnergy(true); // leakage is calculated here
      
      if(is_tdp)
        unit_energy.read = McPAT_MCBackend->power.readOp.dynamic;
      else
      {
        McPAT_MCBackend->mcp.executionTime = (double)1.0/McPAT_MCBackend->mcp.clockRate; // single clock time

        // separate calculation for read and write energies
        McPAT_MCBackend->mcp.reads = 1;
        McPAT_MCBackend->mcp.writes = 0;
        McPAT_MCBackend->computeEnergy(false);
        unit_energy.read = McPAT_MCBackend->rt_power.readOp.dynamic+McPAT_MCBackend->rt_power.readOp.short_circuit;

        McPAT_MCBackend->mcp.reads = 0;
        McPAT_MCBackend->mcp.writes = 1;
        McPAT_MCBackend->computeEnergy(false);
        unit_energy.write = McPAT_MCBackend->rt_power.readOp.dynamic+McPAT_MCBackend->rt_power.readOp.short_circuit; // McPAT MC has no power.writeOp
      }
      /*  
      if(is_tdp)
      {
        unit_energy.read *= (0.5*McPAT_MCBackend->mcp.num_channels);
        unit_energy.write *= (0.5*McPAT_MCBackend->mcp.num_channels);
      }
      */
      if(XML_interface.sys.longer_channel_device)
        unit_energy.leakage = (McPAT_MCBackend->rt_power.readOp.longer_channel_leakage+McPAT_MCBackend->rt_power.readOp.gate_leakage)/clock_frequency;
      else
        unit_energy.leakage = (McPAT_MCBackend->rt_power.readOp.leakage+McPAT_MCBackend->rt_power.readOp.gate_leakage)/clock_frequency;
    }
    else if(!stricmp(energy_submodel,"memory_controller_phy"))
    {
      McPAT_MCPHY->computeEnergy(true); // leakage is calculated here
      
      if(is_tdp)
        unit_energy.read = McPAT_MCPHY->power.readOp.dynamic;
      else
      {
        McPAT_MCPHY->mcp.executionTime = (double)1.0/McPAT_MCPHY->mcp.clockRate; // single clock time

        // separate calculation for read and write energies
        McPAT_MCPHY->mcp.reads = 1;
        McPAT_MCPHY->mcp.writes = 0;
        McPAT_MCPHY->computeEnergy(false);
        unit_energy.read = McPAT_MCPHY->rt_power.readOp.dynamic+McPAT_MCPHY->rt_power.readOp.short_circuit;

        McPAT_MCPHY->mcp.reads = 0;
        McPAT_MCPHY->mcp.writes = 1;
        McPAT_MCPHY->computeEnergy(false);
        unit_energy.write = McPAT_MCPHY->rt_power.readOp.dynamic+McPAT_MCPHY->rt_power.readOp.short_circuit; // McPAT MC has no power.writeOp
      }
      /*
      if(is_tdp)
      {
        unit_energy.read *= (0.5*McPAT_MCPHY->mcp.num_channels);
        unit_energy.write *= (0.5*McPAT_MCPHY->mcp.num_channels);
      }
      */
      if(XML_interface.sys.longer_channel_device)
        unit_energy.leakage = (McPAT_MCPHY->rt_power.readOp.longer_channel_leakage+McPAT_MCPHY->rt_power.readOp.gate_leakage)/clock_frequency;
      else
        unit_energy.leakage = (McPAT_MCPHY->rt_power.readOp.leakage+McPAT_MCPHY->rt_power.readOp.gate_leakage)/clock_frequency;
    }
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): unknown energy_submodel %s of memory_controller in get_unit_energy()\n",energy_submodel.c_str());
      exit(1);
    }
  }
  else if(!stricmp(energy_model,"network"))
  {
    McPAT_NoC->computeEnergy(true); // leakage is calculated here
    
    if(is_tdp)
      unit_energy.read = McPAT_NoC->power.readOp.dynamic;
    else
    {
      McPAT_NoC->XML->sys.NoC[0].total_accesses = 1;
      McPAT_NoC->computeEnergy(false);
      unit_energy.read = McPAT_NoC->rt_power.readOp.dynamic+McPAT_NoC->rt_power.readOp.short_circuit;
      unit_energy.write = unit_energy.read;
    }

    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_NoC->rt_power.readOp.longer_channel_leakage+McPAT_NoC->rt_power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_NoC->rt_power.readOp.leakage+McPAT_NoC->rt_power.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"niu_controller"))
  {
    unit_energy.read = unit_energy.write = McPAT_NIUController->power_t.readOp.dynamic;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_NIUController->power_t.readOp.longer_channel_leakage+McPAT_NIUController->power_t.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_NIUController->power_t.readOp.leakage+McPAT_NIUController->power_t.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"pcie_controller"))
  {
    unit_energy.read = unit_energy.write = McPAT_PCIeController->power_t.readOp.dynamic;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_PCIeController->power_t.readOp.longer_channel_leakage+McPAT_PCIeController->power_t.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_PCIeController->power_t.readOp.leakage+McPAT_PCIeController->power_t.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"pipeline"))
  {
    unit_energy.read = McPAT_Pipeline->power.readOp.dynamic+McPAT_Pipeline->power.readOp.short_circuit;
    unit_energy.write = unit_energy.read;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
    }

    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_Pipeline->power.readOp.longer_channel_leakage+McPAT_Pipeline->power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_Pipeline->power.readOp.leakage+McPAT_Pipeline->power.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"selection_logic"))
  {
    unit_energy.read = McPAT_selection_logic->power.readOp.dynamic+McPAT_selection_logic->power.readOp.short_circuit;
    unit_energy.write = unit_energy.read;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?McPAT_selection_logic->issue_width:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:McPAT_selection_logic->issue_width;
    }
    
    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_selection_logic->power.readOp.longer_channel_leakage+McPAT_selection_logic->power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_selection_logic->power.readOp.leakage+McPAT_selection_logic->power.readOp.gate_leakage)/clock_frequency;
  }
  else if(!stricmp(energy_model,"undifferentiated_core"))
  {
    unit_energy.read = McPAT_UndiffCore->power.readOp.dynamic+McPAT_UndiffCore->power.readOp.short_circuit;
    unit_energy.write = unit_energy.read;
    
    if(is_tdp)
    {
      unit_energy.read *= TDP_duty_cycle.read>0.0?1.0:0.0;
      unit_energy.write *= TDP_duty_cycle.read>0.0?0.0:1.0;
    }

    if(XML_interface.sys.longer_channel_device)
      unit_energy.leakage = (McPAT_UndiffCore->power.readOp.longer_channel_leakage+McPAT_UndiffCore->power.readOp.gate_leakage)/clock_frequency;
    else
      unit_energy.leakage = (McPAT_UndiffCore->power.readOp.leakage+McPAT_UndiffCore->power.readOp.gate_leakage)/clock_frequency;
  }
  else if(stricmp(energy_model,"none"))
  {
    fprintf(stdout,"EI ERROR (McPAT): unknown energy_model %s in get_unit_energy()\n",energy_model.c_str());
    exit(1);
  }
  
  if(is_tdp)
  {
    unit_energy.read *= TDP_duty_cycle.read*scaling;
    unit_energy.write *= TDP_duty_cycle.write*scaling;
    unit_energy.read_tag *= TDP_duty_cycle.read_tag*scaling;
    unit_energy.write_tag *= TDP_duty_cycle.write_tag*scaling;
    unit_energy.search *= TDP_duty_cycle.search*scaling;
  }

  unit_energy.baseline *= energy_scaling;
  unit_energy.read *= energy_scaling;
  unit_energy.read_tag *= energy_scaling;
  unit_energy.write *= energy_scaling;
  unit_energy.write_tag *= energy_scaling;
  unit_energy.leakage *= (energy_scaling*scaling);

  return unit_energy;
}


double ENERGYLIB_McPAT::get_area(void)
{
  double area = 0.0;

  if(!stricmp(energy_model,"array"))
    area = McPAT_ArrayST->local_result.area;
  else if(!stricmp(energy_model,"dependency_check_logic"))
    area = McPAT_dep_resource_conflict_check->area.get_area();
  else if(!stricmp(energy_model,"flash_controller"))
    area = McPAT_FlashController->area.get_area();
  else if(!stricmp(energy_model,"functional_unit"))
  {
    area = McPAT_FunctionalUnit->area.get_area();
    if(!stricmp(energy_submodel,"alu")||!stricmp(energy_submodel,"mul")) // This doesn't seem to be right
      area *= scaling;
  }
  else if(!stricmp(energy_model,"instruction_decoder"))
    area = McPAT_inst_decoder->area.get_area();
  else if(!stricmp(energy_model,"interconnect"))
    area = McPAT_interconnect->area.get_area();
  else if(!stricmp(energy_model,"memory_controller"))
  {
    if(!stricmp(energy_submodel,"memory_controller"))
      area = McPAT_MemoryController->area.get_area();
    else if(!stricmp(energy_submodel,"memory_controller_frontend"))
      area = McPAT_MCFrontEnd->area.get_area();
    else if(!stricmp(energy_submodel,"memory_controller_backend"))
      area = McPAT_MCBackend->area.get_area();
    else if(!stricmp(energy_submodel,"memory_controller_phy"))
      area = McPAT_MCPHY->area.get_area();
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): unknown energy_submodel %s of memory_controller in get_area()\n",energy_submodel.c_str());
      exit(1);
    }
  }
  else if(!stricmp(energy_model,"network"))
    area = McPAT_NoC->area.get_area();
  else if(!stricmp(energy_model,"niu_controller"))
    area = McPAT_NIUController->area.get_area();
  else if(!stricmp(energy_model,"pcie_controller"))
    area = McPAT_PCIeController->area.get_area();
  else if(!stricmp(energy_model,"pipeline"))
    area = McPAT_Pipeline->area.get_area();
  else if(!stricmp(energy_model,"selection_logic"))
    area = McPAT_selection_logic->area.get_area();
  else if(!stricmp(energy_model,"undifferentiated_core"))
    area = McPAT_UndiffCore->area.get_area();
  else if(stricmp(energy_model,"none"))
  {
    fprintf(stdout,"EI ERROR (McPAT): unknown energy_model %s in get_area()\n",energy_model.c_str());
    exit(1);
  }

  return area*area_scaling*scaling*1e-12; // um^2 to m^2
}

void ENERGYLIB_McPAT::update_energy(string var, void *value)
{
  if(!stricmp(var,"temperature"))
  {
    double temperature = *(double*)value;
    if((temperature < 300.0)||(temperature > 400.0))
    {
      fprintf(stdout,"EI ERROR (McPAT): invalid temperature %lfK in update_energy()\n",temperature);
    }
    input_p.temp = (unsigned int)temperature;

    init_interface(&input_p); // reset g_tp

    // update leakage current
    tech_p.peri_global.I_off_n = g_tp.peri_global.I_off_n;
    tech_p.peri_global.I_off_p = g_tp.peri_global.I_off_p;
    tech_p.peri_global.I_g_on_n = g_tp.peri_global.I_g_on_n;
    tech_p.peri_global.I_g_on_p = g_tp.peri_global.I_g_on_p;
    tech_p.sram_cell.I_off_n = g_tp.sram_cell.I_off_n;
    tech_p.sram_cell.I_off_p = g_tp.sram_cell.I_off_p;
    tech_p.sram_cell.I_g_on_n = g_tp.sram_cell.I_g_on_n;
    tech_p.sram_cell.I_g_on_p = g_tp.sram_cell.I_g_on_p;
    tech_p.dram_wl.I_off_n = g_tp.dram_wl.I_off_n;
    tech_p.dram_wl.I_off_p = g_tp.dram_wl.I_off_p;
    tech_p.cam_cell.I_off_n = g_tp.cam_cell.I_off_n;
    tech_p.cam_cell.I_off_p = g_tp.cam_cell.I_off_p;
    tech_p.cam_cell.I_g_on_n = g_tp.cam_cell.I_g_on_n;
    tech_p.cam_cell.I_g_on_p = g_tp.cam_cell.I_g_on_p;
  }
  else if(!stricmp(var,"frequency"))
  {
    clock_frequency = *(double*)value;

    if(!stricmp(energy_model,"array"))
      ; // do nothing
    else if(!stricmp(energy_model,"dependency_check_logic"))
      McPAT_dep_resource_conflict_check->coredynp.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"flash_controller"))
      McPAT_FlashController->fcp.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"functional_unit"))
      McPAT_FunctionalUnit->clockRate = McPAT_FunctionalUnit->coredynp.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"instruction_decoder"))
      ; // do nothing
    else if(!stricmp(energy_model,"interconnect"))
      ; // do nothing
    else if(!stricmp(energy_model,"memory_controller"))
    {
      clock_frequency *= 2; // DDR
      if(!stricmp(energy_submodel,"memory_controller"))
        McPAT_MemoryController->mcp.clockRate = clock_frequency;
      else if(!stricmp(energy_submodel,"memory_controller_frontend"))
        McPAT_MCFrontEnd->mcp.clockRate = clock_frequency;
      else if(!stricmp(energy_submodel,"memory_controller_backend"))
        McPAT_MCBackend->mcp.clockRate = clock_frequency;
      else if(!stricmp(energy_submodel,"memory_controller_phy"))
        McPAT_MCPHY->mcp.clockRate = clock_frequency;
      else
      {
        fprintf(stdout,"EI ERROR (McPAT): unknown energy_submodel %s of memory_controller in update_energy()\n",energy_submodel.c_str());
        exit(1);
      }
    }
    else if(!stricmp(energy_model,"network"))
      McPAT_NoC->nocdynp.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"niu_controller"))
      McPAT_NIUController->niup.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"pcie_controller"))
      McPAT_PCIeController->pciep.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"pipeline"))
      McPAT_Pipeline->coredynp.clockRate = clock_frequency;
    else if(!stricmp(energy_model,"selection_logic"))
      ; // do nothing
    else if(!stricmp(energy_model,"undifferentiated_core"))
      McPAT_UndiffCore->coredynp.clockRate = clock_frequency;
    else if(stricmp(energy_model,"none"))
    {
      fprintf(stdout,"EI ERROR (McPAT): unknown energy_model %s in update_energy()\n",energy_model.c_str());
      exit(1);
    }
  }
  else if(!stricmp(var,"voltage")) // TBD -- this should do better than updating all tech flavors' Vdd
  {
    double voltage = *(double*)value;
    // McPAT library should be using one of the following tech types
    tech_p.cam_cell.R_nch_on *= (voltage/tech_p.cam_cell.Vdd);
    tech_p.cam_cell.R_pch_on *= (voltage/tech_p.cam_cell.Vdd);
    tech_p.cam_cell.Vdd = voltage;
    tech_p.dram_wl.R_nch_on *= (voltage/tech_p.dram_wl.Vdd);
    tech_p.dram_wl.R_pch_on *= (voltage/tech_p.dram_wl.Vdd);
    tech_p.dram_wl.Vdd = voltage;
    tech_p.sram_cell.R_nch_on *= (voltage/tech_p.sram_cell.Vdd);
    tech_p.sram_cell.R_pch_on *= (voltage/tech_p.sram_cell.Vdd);
    tech_p.sram_cell.Vdd = voltage;
    tech_p.peri_global.R_nch_on *= (voltage/tech_p.peri_global.Vdd);
    tech_p.peri_global.R_pch_on *= (voltage/tech_p.peri_global.Vdd);
    tech_p.peri_global.Vdd = voltage;
  }
  else
  {
    fprintf(stdout,"EI WARNING (McPAT): updating undefined variable %s in update_energy()\n",name.c_str());
  }

  if(!stricmp(energy_model,"array"))
    McPAT_ArrayST->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"dependency_check_logic"))
    McPAT_dep_resource_conflict_check->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"flash_controller"))
    McPAT_FlashController->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"functional_unit"))
    McPAT_FunctionalUnit->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"instruction_decoder"))
    McPAT_inst_decoder->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"interconnect"))
    McPAT_interconnect->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"memory_controller"))
  {
    if(!stricmp(energy_submodel,"memory_controller"))
      McPAT_MemoryController->update_energy(input_p,tech_p);
    else if(!stricmp(energy_submodel,"memory_controller_frontend"))
      McPAT_MCFrontEnd->update_energy(input_p,tech_p);
    else if(!stricmp(energy_submodel,"memory_controller_backend"))
      McPAT_MCBackend->update_energy(input_p,tech_p);
    else if(!stricmp(energy_submodel,"memory_controller_phy"))
      McPAT_MCPHY->update_energy(input_p,tech_p);
    else
    {
      fprintf(stdout,"EI ERROR (McPAT): unknown energy_submodel %s of memory_controller in update_energy()\n",energy_submodel.c_str());
      exit(1);
    }
  }
  else if(!stricmp(energy_model,"network"))
    McPAT_NoC->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"niu_controller"))
    McPAT_NIUController->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"pcie_controller"))
    McPAT_PCIeController->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"pipeline"))
  {
    McPAT_Pipeline->update_energy(input_p,tech_p);
  }
  else if(!stricmp(energy_model,"selection_logic"))
    McPAT_selection_logic->update_energy(input_p,tech_p);
  else if(!stricmp(energy_model,"undifferentiated_core"))
    McPAT_UndiffCore->update_energy(input_p,tech_p);
  else if(stricmp(energy_model,"none"))
  {
    fprintf(stdout,"EI ERROR (McPAT): unknown energy_model %s in update_energy()\n",energy_model.c_str());
    exit(1);
  }

  tech_p = g_tp;
  input_p = *g_ip;
}

void ENERGYLIB_McPAT::EI_module_config(FILE *McPAT_config, ENERGYLIB_McPAT *McPAT_obj, int ref_idx, int idx,\
double TDP_read_cycle, double TDP_write_cycle, double TDP_search_cycle, double TDP_read_tag_cycle, double TDP_write_tag_cycle)
{
  if(!McPAT_config) return;
  
  if(McPAT_obj->input_p.obj_func_dyn_energy != 0) // 0 by default
    fprintf(McPAT_config,"-module.obj_func_dyn_energy\t\t%u\n",McPAT_obj->input_p.obj_func_dyn_energy);
  if(McPAT_obj->input_p.obj_func_dyn_power != 0)
    fprintf(McPAT_config,"-module.obj_func_dyn_power\t\t%u\n",McPAT_obj->input_p.obj_func_dyn_power);
  if(McPAT_obj->input_p.obj_func_leak_power != 0)
    fprintf(McPAT_config,"-module.obj_func_leak_power\t\t%u\n",McPAT_obj->input_p.obj_func_leak_power);
  if(McPAT_obj->input_p.obj_func_cycle_t != 1)
    fprintf(McPAT_config,"-module.obj_func_cycle_t\t\t%u\n",McPAT_obj->input_p.obj_func_cycle_t);
  if(McPAT_obj->input_p.delay_wt != 100)
    fprintf(McPAT_config,"-module.delay_wt\t\t%d\n",McPAT_obj->input_p.delay_wt);
  if(McPAT_obj->input_p.area_wt != 0)
    fprintf(McPAT_config,"-module.area_wt\t\t%d\n",McPAT_obj->input_p.area_wt);
  if(McPAT_obj->input_p.dynamic_power_wt != 100)
    fprintf(McPAT_config,"-module.dynamic_power_wt\t\t%d\n",McPAT_obj->input_p.dynamic_power_wt);
  if(McPAT_obj->input_p.leakage_power_wt != 0)
    fprintf(McPAT_config,"-module.leakage_power_wt\t\t%d\n",McPAT_obj->input_p.leakage_power_wt);
  if(McPAT_obj->input_p.cycle_time_wt != 0)
    fprintf(McPAT_config,"-module.cycle_time_wt\t\t%d\n",McPAT_obj->input_p.cycle_time_wt);
  if(McPAT_obj->input_p.delay_dev != 10000)
    fprintf(McPAT_config,"-module.delay_dev\t\t%d\n",McPAT_obj->input_p.delay_dev);
  if(McPAT_obj->input_p.area_dev != 10000)
    fprintf(McPAT_config,"-module.area_dev\t\t%d\n",McPAT_obj->input_p.area_dev);
  if(McPAT_obj->input_p.dynamic_power_dev != 10000)
    fprintf(McPAT_config,"-module.dynamic_power_dev\t\t%d\n",McPAT_obj->input_p.dynamic_power_dev);
  if(McPAT_obj->input_p.leakage_power_dev != 10000)
    fprintf(McPAT_config,"-module.leakage_power_dev\t\t%d\n",McPAT_obj->input_p.leakage_power_dev);
  if(McPAT_obj->input_p.cycle_time_dev != 10000)
    fprintf(McPAT_config,"-module.cycle_time_dev\t\t%d\n",McPAT_obj->input_p.cycle_time_dev);
  if(McPAT_obj->input_p.ed != 2)
    fprintf(McPAT_config,"-module.ed\t\t%d\n",McPAT_obj->input_p.ed);
  if(McPAT_obj->input_p.nuca != 0)
  {
    fprintf(McPAT_config,"-module.nuca\t\t%d\n",McPAT_obj->input_p.nuca);
    if(McPAT_obj->input_p.nuca_bank_count != 0)
      fprintf(McPAT_config,"-module.nuca_bank_count\t\t%d\n",McPAT_obj->input_p.nuca_bank_count);
    if(McPAT_obj->input_p.delay_wt_nuca != 0)
      fprintf(McPAT_config,"-module.delay_wt_nuca\t\t%d\n",McPAT_obj->input_p.delay_wt_nuca);
    if(McPAT_obj->input_p.area_wt_nuca != 0)
      fprintf(McPAT_config,"-module.area_wt_nuca\t\t%d\n",McPAT_obj->input_p.area_wt_nuca);
    if(McPAT_obj->input_p.dynamic_power_wt_nuca != 0)
      fprintf(McPAT_config,"-module.dynamic_power_wt_nuca\t\t%d\n",McPAT_obj->input_p.dynamic_power_wt_nuca);
    if(McPAT_obj->input_p.leakage_power_wt_nuca != 0)
      fprintf(McPAT_config,"-module.leakage_power_wt_nuca\t\t%d\n",McPAT_obj->input_p.leakage_power_wt_nuca);
    if(McPAT_obj->input_p.cycle_time_wt_nuca != 0)
      fprintf(McPAT_config,"-module.cycle_time_wt_nuca\t\t%d\n",McPAT_obj->input_p.cycle_time_wt_nuca);
    if(McPAT_obj->input_p.delay_dev_nuca != 10000)
      fprintf(McPAT_config,"-module.delay_dev_nuca\t\t%d\n",McPAT_obj->input_p.delay_dev_nuca);
    if(McPAT_obj->input_p.area_dev_nuca != 10000)
      fprintf(McPAT_config,"-module.area_dev_nuca\t\t%d\n",McPAT_obj->input_p.area_dev_nuca);
    if(McPAT_obj->input_p.dynamic_power_dev_nuca != 10)
      fprintf(McPAT_config,"-module.dynamic_power_dev_nuca\t\t%d\n",McPAT_obj->input_p.dynamic_power_dev_nuca);
    if(McPAT_obj->input_p.leakage_power_dev_nuca != 10000)
      fprintf(McPAT_config,"-module.leakage_power_dev_nuca\t\t%d\n",McPAT_obj->input_p.leakage_power_dev_nuca);
    if(McPAT_obj->input_p.cycle_time_dev_nuca != 10000)
      fprintf(McPAT_config,"-module.cycle_time_dev_nuca\t\t%d\n",McPAT_obj->input_p.cycle_time_dev_nuca);
  }
  if(McPAT_obj->input_p.force_wiretype) // false by default
    fprintf(McPAT_config,"-module.force_wire_type\t\ttrue\n");
  if(!McPAT_obj->input_p.rpters_in_htree) // true by default
    fprintf(McPAT_config,"-module.repeaters_in_htree\t\tfalse\n");
  if(!McPAT_obj->input_p.with_clock_grid) // true by default
    fprintf(McPAT_config,"-module.with_clock_grid\t\tfalse\n");
  if(McPAT_obj->input_p.force_cache_config) // false by default
  {
    fprintf(McPAT_config,"-module.force_cache_config\t\ttrue\n");
  
    fprintf(McPAT_config,"-module.ndbl\t\t%d\n",McPAT_obj->input_p.ndbl);
    fprintf(McPAT_config,"-module.ndwl\t\t%d\n",McPAT_obj->input_p.ndwl);
    fprintf(McPAT_config,"-module.nspd\t\t%d\n",McPAT_obj->input_p.nspd);
    fprintf(McPAT_config,"-module.ndsam1\t\t%d\n",McPAT_obj->input_p.ndsam1);
    fprintf(McPAT_config,"-module.ndsam2\t\t%d\n",McPAT_obj->input_p.ndsam2);
    fprintf(McPAT_config,"-module.ndcm\t\t%d\n",McPAT_obj->input_p.ndcm);
  }
  
  // skip printing don't-care parameters
  
  double TDP_duty_cycle = 1.0/McPAT_obj->scaling;
  
  if(!stricmp(McPAT_obj->energy_model,"array"))
  {
    assert(McPAT_obj->McPAT_ArrayST);
    fprintf(McPAT_config,"-module.line_width\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.line_sz);
    fprintf(McPAT_config,"-module.output_width\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.out_w);
    if(McPAT_obj->input_p.assoc != 1)
      fprintf(McPAT_config,"-module.assoc\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.assoc);
    if(McPAT_obj->input_p.nbanks != 1)
      fprintf(McPAT_config,"-module.banks\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.nbanks);
    fprintf(McPAT_config,"-module.size\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.cache_sz);
    if(McPAT_obj->input_p.tag_w > 0)
      fprintf(McPAT_config,"-module.tag_width\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.tag_w);
    if(McPAT_obj->input_p.num_rw_ports > 0)
      fprintf(McPAT_config,"-module.rw_ports\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.num_rw_ports);
    if(McPAT_obj->input_p.num_rd_ports > 0)
      fprintf(McPAT_config,"-module.rd_ports\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.num_rd_ports);
    if(McPAT_obj->input_p.num_wr_ports > 0)
      fprintf(McPAT_config,"-module.wr_ports\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.num_wr_ports);
    if(McPAT_obj->input_p.num_se_rd_ports > 0)
      fprintf(McPAT_config,"-module.se_rd_ports\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.num_se_rd_ports);
    if(McPAT_obj->input_p.num_search_ports > 0)
      fprintf(McPAT_config,"-module.search_ports\t\t%u\n",McPAT_obj->McPAT_ArrayST->l_ip.num_search_ports);
    if(!McPAT_obj->input_p.add_ecc_b_)
      fprintf(McPAT_config,"-module.add_ecc\t\tfalse\n");
      
    int cycle_time = (int)ceil(McPAT_obj->McPAT_ArrayST->l_ip.throughput*McPAT_obj->clock_frequency);
    int access_time = (int)ceil(McPAT_obj->McPAT_ArrayST->l_ip.latency*McPAT_obj->clock_frequency);
    if((cycle_time != 1)||(access_time != 1))
    {
      fprintf(McPAT_config,"-module.cycle_time\t\t%d\n",cycle_time);
      fprintf(McPAT_config,"-module.access_time\t\t%d\n",access_time);
    }
    
    switch(McPAT_obj->McPAT_ArrayST->l_ip.access_mode)
    {
      case 0: fprintf(McPAT_config,"-module.access_mode\t\tnormal\n"); break;
      case 1: fprintf(McPAT_config,"-module.access_mode\t\tsequential\n"); break;
      case 2: fprintf(McPAT_config,"-module.access_mode\t\tfast\n"); break;
      default: fprintf(McPAT_config,"-module.access_mode\t\tnormal\n"); break;
    }
    
    // TDP read duty cycle
    TDP_duty_cycle = 1.0/McPAT_obj->scaling;
    TDP_duty_cycle *= TDP_read_cycle/(McPAT_obj->McPAT_ArrayST->l_ip.num_rd_ports>0?McPAT_obj->McPAT_ArrayST->l_ip.num_rd_ports:McPAT_obj->McPAT_ArrayST->l_ip.num_rw_ports);
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
    // TDP write duty cycle
    TDP_duty_cycle = 1.0/McPAT_obj->scaling;
    TDP_duty_cycle *= TDP_write_cycle/(McPAT_obj->McPAT_ArrayST->l_ip.num_wr_ports>0?McPAT_obj->McPAT_ArrayST->l_ip.num_wr_ports:McPAT_obj->McPAT_ArrayST->l_ip.num_rw_ports);
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.write\t\t%lf\n",TDP_duty_cycle);
    // TDP search duty cycle
    if(McPAT_obj->McPAT_ArrayST->l_ip.num_search_ports > 0)
    {
      TDP_duty_cycle = 1.0/McPAT_obj->scaling;
      TDP_duty_cycle *= TDP_search_cycle/McPAT_obj->McPAT_ArrayST->l_ip.num_search_ports;
      if(TDP_duty_cycle != 1.0)
        fprintf(McPAT_config,"-module.TDP_duty_cycle.search\t\t%lf\n",TDP_duty_cycle);
    }
    // TDP read_tag duty cycle
    TDP_duty_cycle = 1.0/McPAT_obj->scaling;
    TDP_duty_cycle *= TDP_read_tag_cycle/(McPAT_obj->McPAT_ArrayST->l_ip.num_rd_ports>0?McPAT_obj->McPAT_ArrayST->l_ip.num_rd_ports:McPAT_obj->McPAT_ArrayST->l_ip.num_rw_ports);
    if(TDP_duty_cycle > 0.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read_tag\t\t%lf\n",TDP_duty_cycle);
    // TDP write_tag duty cycle
    TDP_duty_cycle = 1.0/McPAT_obj->scaling;
    TDP_duty_cycle *= TDP_write_tag_cycle/(McPAT_obj->McPAT_ArrayST->l_ip.num_wr_ports>0?McPAT_obj->McPAT_ArrayST->l_ip.num_wr_ports:McPAT_obj->McPAT_ArrayST->l_ip.num_rw_ports);
    if(TDP_duty_cycle > 0.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.write_tag\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"dependency_check_logic"))
  {
    assert(McPAT_obj->McPAT_dep_resource_conflict_check);
    fprintf(McPAT_config,"-module.compare_bits\t\t%d # TODO: check logic.cc:225-228\n",McPAT_obj->McPAT_dep_resource_conflict_check->compare_bits-16-8-8);
    fprintf(McPAT_config,"-module.decode_width\t\t%d\n",McPAT_obj->McPAT_dep_resource_conflict_check->coredynp.decodeW);
    
    TDP_duty_cycle *= TDP_read_cycle/McPAT_obj->McPAT_dep_resource_conflict_check->coredynp.decodeW;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"flash_controller"))
  {
    assert(McPAT_obj->McPAT_FlashController);
    fprintf(McPAT_config,"-module.type\t\t%s\n",McPAT_obj->McPAT_FlashController->fcp.type==0?"high_performance":"low_power");
    fprintf(McPAT_config,"-module.load_percentage\t\t%lf # load to bandwidth ratio\n",McPAT_obj->McPAT_FlashController->fcp.perc_load);
    fprintf(McPAT_config,"-module.peak_transfer_rate\t\t%lf # in MBps\n",McPAT_obj->McPAT_FlashController->fcp.peakDataTransferRate);
    if(!McPAT_obj->McPAT_FlashController->fcp.withPHY)
      fprintf(McPAT_config,"-module.withPHY\t\tfalse\n");

    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"functional_unit"))
  {
    assert(McPAT_obj->McPAT_FunctionalUnit);
    
    fprintf(McPAT_config,"-module.scaling\t\t%lf\n",McPAT_obj->McPAT_FunctionalUnit->num_fu);
    
    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"instruction_decoder"))
  {
    assert(McPAT_obj->McPAT_inst_decoder);
    fprintf(McPAT_config,"-module.x86\t\t%s\n",McPAT_obj->McPAT_inst_decoder->x86?"true":"false");
    fprintf(McPAT_config,"-module.opcode\t\t%d\n",McPAT_obj->McPAT_inst_decoder->opcode_length);
    
    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"interconnect"))
  {
    assert(McPAT_obj->McPAT_interconnect);
    if((bool)McPAT_obj->McPAT_interconnect->pipelinable)
    {
      fprintf(McPAT_config,"-module.pipelinable\t\ttrue\n");
      fprintf(McPAT_config,"-module.routing_over_percentage\t\t%lf\n",McPAT_obj->McPAT_interconnect->route_over_perc);
    }
    if(McPAT_obj->McPAT_interconnect->opt_local)
      fprintf(McPAT_config,"-module.opt_local\t\ttrue\n");
    if(McPAT_obj->McPAT_interconnect->l_ip.wt != (Wire_type)Global)
    {
      switch(McPAT_obj->McPAT_interconnect->l_ip.wt)
      {
        case (Wire_type)Global_5: fprintf(McPAT_config,"-module.wire_type\t\tglobal_5\n"); break;
        case (Wire_type)Global_10: fprintf(McPAT_config,"-module.wire_type\t\tglobal_10\n"); break;
        case (Wire_type)Global_20: fprintf(McPAT_config,"-module.wire_type\t\tglobal_20\n"); break;
        case (Wire_type)Global_30: fprintf(McPAT_config,"-module.wire_type\t\tglobal_30\n"); break;
        case (Wire_type)Low_swing: fprintf(McPAT_config,"-module.wire_type\t\tlow_swing\n"); break;
        case (Wire_type)Semi_global: fprintf(McPAT_config,"-module.wire_type\t\tsemi_global\n"); break;
        case (Wire_type)Transmission: fprintf(McPAT_config,"-module.wire_type\t\ttransmission\n"); break;
        case (Wire_type)Optical: fprintf(McPAT_config,"-module.wire_type\t\toptical\n"); break;
        default: fprintf(McPAT_config,"-module.wire_type\t\tglobal\n"); break;
      }
    }
    fprintf(McPAT_config,"-module.data\t\t%d\n",McPAT_obj->McPAT_interconnect->data_width);
    fprintf(McPAT_config,"-module.wire_length\t\t%lf # if wire_length is unknown list the connected modules (e.g., -module.connect icache)\n",McPAT_obj->McPAT_interconnect->length*1e-6);
    
   TDP_duty_cycle *= TDP_read_cycle;
   if(TDP_duty_cycle != 1.0)
     fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"memory_controller"))
  {
    assert(McPAT_obj->McPAT_MemoryController);
    fprintf(McPAT_config,"-module.line\t\t%d # LLC line width\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.llc_line_length);
    fprintf(McPAT_config,"-module.request_window\t\t%d # request window size per channel\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.req_window_size_per_channel);
    fprintf(McPAT_config,"-module.io_buffer\t\t%d # IO buffer size per channel\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.IO_buffer_size_per_channel);
    fprintf(McPAT_config,"-module.memory_channels\t\t%d # memory channels per MC\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.memory_channels_per_mc);
    fprintf(McPAT_config,"-module.transfer_rate\t\t%lf # peak transfer rate\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.peak_transfer_rate);
    fprintf(McPAT_config,"-module.ranks\t\t%d # number of ranks\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.number_ranks);
    fprintf(McPAT_config,"-module.data_bus\t\t%d # data bus width\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.databus_width);
    fprintf(McPAT_config,"-module.address_bus\t\t%d # address bus width\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.addressbus_width);
    if(!McPAT_obj->McPAT_MemoryController->XML->sys.mc.LVDS)
      fprintf(McPAT_config,"-module.lvds\t\tfalse\n");
    if(McPAT_obj->McPAT_MemoryController->XML->sys.mc.withPHY)
      fprintf(McPAT_config,"-module.with_phy\t\ttrue\n");
    fprintf(McPAT_config,"-module.model\t\t%s\n",McPAT_obj->McPAT_MemoryController->XML->sys.mc.type==0?"high_performance":"low_power");
    
    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"network"))
  {
    assert(McPAT_obj->McPAT_NoC);
    fprintf(McPAT_config,"-module.flit_bits\t\t%d\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].flit_bits);    
    if(McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].type) // router
    {
      fprintf(McPAT_config,"-module.input_ports\t\t%d\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].input_ports);
      fprintf(McPAT_config,"-module.output_ports\t\t%d\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].output_ports);
      fprintf(McPAT_config,"-module.virtual_channels\t\t%d\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].virtual_channel_per_port);
    }
    fprintf(McPAT_config,"-module.duty_cycle\t\t%lf\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].duty_cycle);
    fprintf(McPAT_config,"-module.link_throughput\t\t%d\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].link_throughput);
    fprintf(McPAT_config,"-module.link_latency\t\t%d\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].link_latency);
    fprintf(McPAT_config,"-module.chip_coverage\t\t%lf\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].chip_coverage);
    fprintf(McPAT_config,"-module.route_over_percentage\t\t%lf\n",McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].route_over_perc);
    fprintf(McPAT_config,"-module.traffic_pattern\t\t%lf\n",McPAT_obj->McPAT_NoC->M_traffic_pattern);
    fprintf(McPAT_config,"-module.chip_area\t\t%lf\n",pow(McPAT_obj->McPAT_NoC->link_len,2)/McPAT_obj->McPAT_NoC->XML->sys.NoC[McPAT_obj->McPAT_NoC->ithNoC].chip_coverage*1e-12);
    
    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else if(!stricmp(McPAT_obj->energy_model,"niu_controller"))
  {
    assert(McPAT_obj->McPAT_NIUController);
    fprintf(McPAT_config,"-module.type\t\t%s\n",McPAT_obj->McPAT_NIUController->niup.type==0?"high_performance":"low_power");
    fprintf(McPAT_config,"-module.load_percentage\t\t%lf # load to bandwidth ratio\n",McPAT_obj->McPAT_NIUController->niup.perc_load);
    
    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);    
  }
  else if(!stricmp(McPAT_obj->energy_model,"pcie_controller"))
  {
    assert(McPAT_obj->McPAT_PCIeController);
    fprintf(McPAT_config,"-module.type\t\t%s\n",McPAT_obj->McPAT_PCIeController->pciep.type==0?"high_performance":"low_power");
    fprintf(McPAT_config,"-module.load_percentage\t\t%lf # load to bandwidth ratio\n",McPAT_obj->McPAT_PCIeController->pciep.perc_load);
    fprintf(McPAT_config,"-module.channels\t\t%d\n",McPAT_obj->McPAT_PCIeController->pciep.num_channels);
    if(!McPAT_obj->McPAT_PCIeController->pciep.withPHY)
      fprintf(McPAT_config,"-module.withPHY\t\tfalse\n");

    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);      
  }
  else if(!stricmp(McPAT_obj->energy_model,"pipeline"))
  {
    assert(McPAT_obj->McPAT_Pipeline);
    fprintf(McPAT_config,"-module.pipeline_stages\t\t%d\n",McPAT_obj->McPAT_Pipeline->l_ip.pipeline_stages);
    //if(McPAT_obj->McPAT_Pipeline->l_ip.per_stage_vector > 0)
      //fprintf(McPAT_config,"-module.per_stage_vector\t\t%d\n",McPAT_obj->McPAT_Pipeline->l_ip.per_stage_vector);
    if(McPAT_obj->McPAT_Pipeline->coredynp.x86)
    {
      fprintf(McPAT_config,"-module.x86\t\ttrue\n");
      fprintf(McPAT_config,"-module.microopcode\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.micro_opcode_length);
    }
    else
      fprintf(McPAT_config,"-module.opcode\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.opcode_length);
    fprintf(McPAT_config,"-module.pc\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.pc_width);
    fprintf(McPAT_config,"-module.fetch_width\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.fetchW);
    fprintf(McPAT_config,"-module.decode_width\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.decodeW);
    fprintf(McPAT_config,"-module.issue_width\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.issueW);
    fprintf(McPAT_config,"-module.instruction_length\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.instruction_length);
    fprintf(McPAT_config,"-module.int_data_width\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.int_data_width);
    fprintf(McPAT_config,"-module.hthreads\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.num_hthreads);    
    if(McPAT_obj->McPAT_Pipeline->coredynp.num_hthreads > 1)
    {
      fprintf(McPAT_config,"-module.thread_states\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.perThreadState);
    }
    if(McPAT_obj->McPAT_Pipeline->coredynp.core_ty == Inorder)
    {
      fprintf(McPAT_config,"-module.arch_int_regs\t\t%d\n",(int)pow(2.0,McPAT_obj->McPAT_Pipeline->coredynp.arch_ireg_width));
    }
    else
    {
      fprintf(McPAT_config,"-module.phy_int_regs\t\t%d\n",(int)pow(2.0,McPAT_obj->McPAT_Pipeline->coredynp.phy_ireg_width));
      fprintf(McPAT_config,"-module.virtual_address\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.v_address_width);
      fprintf(McPAT_config,"-module.commit_width\t\t%d\n",McPAT_obj->McPAT_Pipeline->coredynp.commitW);
    }
    
    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);    
  }
  else if(!stricmp(McPAT_obj->energy_model,"selection_logic"))
  {
    assert(McPAT_obj->McPAT_selection_logic);
    fprintf(McPAT_config,"-module.selection_input\t\t%d\n",XML_interface.sys.core[ref_idx].instruction_window_size);
    fprintf(McPAT_config,"-module.selection_output\t\t%d\n",McPAT_obj->McPAT_selection_logic->issue_width);
    
    TDP_duty_cycle *= (TDP_read_cycle/McPAT_obj->McPAT_selection_logic->issue_width);
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);    
  }
  else if(!stricmp(McPAT_obj->energy_model,"undifferentiated_core"))
  {
    assert(McPAT_obj->McPAT_UndiffCore);
    fprintf(McPAT_config,"-module.pipeline_stages\t\t%d\n",McPAT_obj->McPAT_UndiffCore->coredynp.pipeline_stages);
    fprintf(McPAT_config,"-module.hthreads\t\t%d\n",McPAT_obj->McPAT_UndiffCore->coredynp.num_hthreads);
    fprintf(McPAT_config,"-module.issue_width\t\t%d\n",McPAT_obj->McPAT_UndiffCore->coredynp.issueW);
    
    if(McPAT_obj->McPAT_UndiffCore->XML->sys.Embedded&&!McPAT_obj->McPAT_UndiffCore->XML->sys.opt_clockrate)
      fprintf(McPAT_config,"-module.opt_clockrate\t\tfalse\n");

    TDP_duty_cycle *= TDP_read_cycle;
    if(TDP_duty_cycle != 1.0)
      fprintf(McPAT_config,"-module.TDP_duty_cycle.read\t\t%lf\n",TDP_duty_cycle);
  }
  else
  {
    fprintf(stdout,"EI ERROR (McPAT): unknown energy_model %s in EI_module_config()\n",McPAT_obj->energy_model.c_str());
    exit(1);
  }
}

void ENERGYLIB_McPAT::EI_technology_config(FILE *McPAT_config, string technologyID, InputParameter McPAT_input,\
                                           double target_clock_frequency, string component_type, string core_type)
{
  if(McPAT_config)
  {
    fprintf(McPAT_config,"-technology.ID\t\t%s\n",technologyID.c_str());
    fprintf(McPAT_config,"-technology.opt_for_clk\t\t%s # optimize the result on clock\n",opt_for_clk?"true":"false");
    fprintf(McPAT_config,"-technology.embedded\t\t%s # embedded processor?\n",XML_interface.sys.Embedded?"true":"false");
    fprintf(McPAT_config,"-technology.feature_size\t\t%de-9 # technology node in meters\n",(int)XML_interface.sys.core_tech_node);
    fprintf(McPAT_config,"-technology.temperature\t\t%u # temperature in Kelvin\n",(unsigned int)XML_interface.sys.temperature);
    fprintf(McPAT_config,"-technology.component_type\t\t%s # core, uncore, llc(non-private caches)\n",component_type.c_str()); // core device
    fprintf(McPAT_config,"-technology.clock_frequency\t\t%e # clock frequency in Hz\n",target_clock_frequency); // clock frequency in Hz
    fprintf(McPAT_config,"-technology.longer_channel_device\t\t%s\n",XML_interface.sys.longer_channel_device?"true":"false");
    if(!stricmp(component_type,"core"))
      fprintf(McPAT_config,"-technology.core_type\t\t%s\n",core_type.c_str());

    switch(McPAT_input.wt) // TODO: designate a particular input parameters of core components
    {
      case (Wire_type)Global: fprintf(McPAT_config,"-technology.wire_type\t\tglobal\n"); break;
      case (Wire_type)Global_5: fprintf(McPAT_config,"-technology.wire_type\t\tglobal_5\n"); break;
      case (Wire_type)Global_10: fprintf(McPAT_config,"-technology.wire_type\t\tglobal_10\n"); break;
      case (Wire_type)Global_20: fprintf(McPAT_config,"-technology.wire_type\t\tglobal_20\n"); break;
      case (Wire_type)Global_30: fprintf(McPAT_config,"-technology.wire_type\t\tglobal_30\n"); break;
      case (Wire_type)Low_swing: fprintf(McPAT_config,"-technology.wire_type\t\tlow_swing\n"); break;
      case (Wire_type)Semi_global: fprintf(McPAT_config,"-technology.wire_type\t\tsemi_global\n"); break;
      case (Wire_type)Transmission: fprintf(McPAT_config,"-technology.wire_type\t\ttransmission\n"); break;
      case (Wire_type)Optical: fprintf(McPAT_config,"-technology.wire_type\t\toptical\n"); break;
      default: fprintf(McPAT_config,"-technology.wire_type\t\tglobal\n"); break;
    }
  
    if((McPAT_input.data_arr_ram_cell_tech_type-McPAT_input.data_arr_peri_global_tech_type\
       -McPAT_input.tag_arr_ram_cell_tech_type-McPAT_input.tag_arr_peri_global_tech_type)==0)
    {
      switch(McPAT_input.data_arr_ram_cell_tech_type)
      {
        case 0: fprintf(McPAT_config,"-technology.device_type\t\thp\n"); break;
        case 1: fprintf(McPAT_config,"-technology.device_type\t\tlstp\n"); break;
        case 2: fprintf(McPAT_config,"-technology.device_type\t\tlop\n"); break;
        case 3: fprintf(McPAT_config,"-technology.device_type\t\tlp_dram\n"); break;
        case 4: fprintf(McPAT_config,"-technology.device_type\t\tcomm_dram\n"); break;
        default: fprintf(McPAT_config,"-technology.device_type\t\thp\n"); break;
      }
    }
    else 
    {
      switch(McPAT_input.data_arr_ram_cell_tech_type)
      {
        case 0: fprintf(McPAT_config,"-technology.device_type.data_arr_ram_cell_tech_type\t\thp\n"); break;
        case 1: fprintf(McPAT_config,"-technology.device_type.data_arr_ram_cell_tech_type\t\tlstp\n"); break;
        case 2: fprintf(McPAT_config,"-technology.device_type.data_arr_ram_cell_tech_type\t\tlop\n"); break;
        case 3: fprintf(McPAT_config,"-technology.device_type.data_arr_ram_cell_tech_type\t\tlp_dram\n"); break;
        case 4: fprintf(McPAT_config,"-technology.device_type.data_arr_ram_cell_tech_type\t\tcomm_dram\n"); break;
        default: fprintf(McPAT_config,"-technology.device_type.data_arr_ram_cell_tech_type\t\thp\n"); break;
      }
      switch(McPAT_input.data_arr_peri_global_tech_type)
      {
        case 0: fprintf(McPAT_config,"-technology.device_type.data_arr_peri_global_tech_type\t\thp\n"); break;
        case 1: fprintf(McPAT_config,"-technology.device_type.data_arr_peri_global_tech_type\t\tlstp\n"); break;
        case 2: fprintf(McPAT_config,"-technology.device_type.data_arr_peri_global_tech_type\t\tlop\n"); break;
        case 3: fprintf(McPAT_config,"-technology.device_type.data_arr_peri_global_tech_type\t\tlp_dram\n"); break;
        case 4: fprintf(McPAT_config,"-technology.device_type.data_arr_peri_global_tech_type\t\tcomm_dram\n"); break;
        default: fprintf(McPAT_config,"-technology.device_type.data_arr_peri_global_tech_type\t\thp\n"); break;
      }
      switch(McPAT_input.tag_arr_ram_cell_tech_type)
      {
        case 0: fprintf(McPAT_config,"-technology.device_type.tag_arr_ram_cell_tech_type\t\thp\n"); break;
        case 1: fprintf(McPAT_config,"-technology.device_type.tag_arr_ram_cell_tech_type\t\tlstp\n"); break;
        case 2: fprintf(McPAT_config,"-technology.device_type.tag_arr_ram_cell_tech_type\t\tlop\n"); break;
        case 3: fprintf(McPAT_config,"-technology.device_type.tag_arr_ram_cell_tech_type\t\tlp_dram\n"); break;
        case 4: fprintf(McPAT_config,"-technology.device_type.tag_arr_ram_cell_tech_type\t\tcomm_dram\n"); break;
        default: fprintf(McPAT_config,"-technology.device_type.tag_arr_ram_cell_tech_type\t\thp\n"); break;
      }
      switch(McPAT_input.tag_arr_peri_global_tech_type)
      {
        case 0: fprintf(McPAT_config,"-technology.device_type.tag_arr_peri_global_tech_type\t\thp\n"); break;
        case 1: fprintf(McPAT_config,"-technology.device_type.tag_arr_peri_global_tech_type\t\tlstp\n"); break;
        case 2: fprintf(McPAT_config,"-technology.device_type.tag_arr_peri_global_tech_type\t\tlop\n"); break;
        case 3: fprintf(McPAT_config,"-technology.device_type.tag_arr_peri_global_tech_type\t\tlp_dram\n"); break;
        case 4: fprintf(McPAT_config,"-technology.device_type.tag_arr_peri_global_tech_type\t\tcomm_dram\n"); break;
        default: fprintf(McPAT_config,"-technology.device_type.tag_arr_peri_global_tech_type\t\thp\n"); break;
      }
    }

    fprintf(McPAT_config,"-technology.interconnect_projection\t\t%s\n",McPAT_input.ic_proj_type==0?"aggressive":"conservative");
    
    if((McPAT_input.wire_is_mat_type-McPAT_input.wire_os_mat_type) == 0)
    {
      switch(McPAT_input.wire_is_mat_type)
      {
        case 0: fprintf(McPAT_config,"-technology.wiring_type\t\tlocal\n"); break;
        case 1: fprintf(McPAT_config,"-technology.wiring_type\t\tsemi_global\n"); break;
        case 2: fprintf(McPAT_config,"-technology.wiring_type\t\tglobal\n"); break;
        case 3: fprintf(McPAT_config,"-technology.wiring_type\t\tdram\n"); break;
        default: fprintf(McPAT_config,"-technology.wiring_type\t\tglobal\n"); break;
      }
    }
    else
    {
      switch(McPAT_input.wire_is_mat_type)
      {
        case 0: fprintf(McPAT_config,"-technology.wiring_type.wire_is_mat_type\t\tlocal\n"); break;
        case 1: fprintf(McPAT_config,"-technology.wiring_type.wire_is_mat_type\t\tsemi_global\n"); break;
        case 2: fprintf(McPAT_config,"-technology.wiring_type.wire_is_mat_type\t\tglobal\n"); break;
        case 3: fprintf(McPAT_config,"-technology.wiring_type.wire_is_mat_type\t\tdram\n"); break;
        default: fprintf(McPAT_config,"-technology.wiring_type.wire_is_mat_type\t\tglobal\n"); break;
      }
      switch(McPAT_input.wire_os_mat_type)
      {
        case 0: fprintf(McPAT_config,"-technology.wiring_type.wire_os_mat_type\t\tlocal\n"); break;
        case 1: fprintf(McPAT_config,"-technology.wiring_type.wire_os_mat_type\t\tsemi_global\n"); break;
        case 2: fprintf(McPAT_config,"-technology.wiring_type.wire_os_mat_type\t\tglobal\n"); break;
        case 3: fprintf(McPAT_config,"-technology.wiring_type.wire_os_mat_type\t\tdram\n"); break;
        default: fprintf(McPAT_config,"-technology.wiring_type.wire_os_mat_type\t\tglobal\n"); break;
      }
    }
/*  
    switch(McPAT_input.ver_htree_wires_over_array)
    {
      case 0: fprintf(McPAT_config,"-technology.ver_htree_wires_over_array\t\tlocal\n"); break;
      case 1: fprintf(McPAT_config,"-technology.ver_htree_wires_over_array\t\tsemi_global\n"); break;
      case 2: fprintf(McPAT_config,"-technology.ver_htree_wires_over_array\t\tglobal\n"); break;
      case 3: fprintf(McPAT_config,"-technology.ver_htree_wires_over_array\t\tdram\n"); break;
      default: fprintf(McPAT_config,"-technology.ver_htree_wires_over_array\t\tlocal\n"); break;
    }
    switch(McPAT_input.broadcast_addr_din_over_ver_htrees)
    {
      case 0: fprintf(McPAT_config,"-technology.broadcast_addr_din_over_ver_htrees\t\tlocal\n"); break;
      case 1: fprintf(McPAT_config,"-technology.broadcast_addr_din_over_ver_htrees\t\tsemi_global\n"); break;
      case 2: fprintf(McPAT_config,"-technology.broadcast_addr_din_over_ver_htrees\t\tglobal\n"); break;
      case 3: fprintf(McPAT_config,"-technology.braodcast_addr_din_over_ver_htrees\t\tdram\n"); break;
      default: fprintf(McPAT_config,"-technology.broadcast_addr_din_over_ver_htrees\t\tlocal\n"); break;
    }
*/  
    fprintf(McPAT_config,"-technology.end\n\n");
  }
}

