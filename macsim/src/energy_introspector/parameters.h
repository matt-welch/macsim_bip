/* 
 * parameters.h - input parameters
 * Copyright 2011 by William Song and Sudhakar Yalamanchili
 * Georgia Tech Research Corporation, Atlanta, GA 30332
 * All Rights Reserved.
 */
 
#ifndef EI_PARAM_H
#define EI_PARAM_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "definitions.h"

class parameters_package_t
{
 public:
  parameters_package_t() : ID("n/a") { reset(); }
  ~parameters_package_t() { reset(); }

  string ID;
  multimap<string,pair<string,bool> > options;

  void check_option(FILE *fp)
  {
    for(multimap<string,pair<string,bool> >::iterator options_it = options.begin();
        options_it != options.end(); options_it++)
    {
      if(!options_it->second.second) // unused option
        fprintf(fp,"EI WARNING (parameters): package option <%s,%s> is unused in %s\n",
                options_it->first.c_str(),options_it->second.first.c_str(),ID.c_str());
    }
  }
  
  void reset(void)
  {
    options.clear();
  }
  
  void add_option(string opt, string val) // add a string pair option
  {
    options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,false)));
  }
  
  void remove_option(string opt, string val = "n/a") // remove matching options
  {
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    // remove matching options
    for(multimap<string,pair<string,bool> >::iterator options_it = options_rng.first;\
        options_it != options_rng.second; options_it++)
      if((options_it->second.first == "n/a")||(options_it->second.first == val))
        options.erase(options_it);
  }
  
  string get_option(string opt)
  {
    string val;
    
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    if(options_rng.first != options_rng.second)
    {
      val = options_rng.first->second.first; // return the first found entry
      options_rng.first->second.second = true; // mark as a used option
      
      // re-order the multimap
      multimap<string,pair<string,bool> >::iterator it_last = options_rng.second;
      it_last--; // last matching option in the multimap
      
      if(it_last != options_rng.first) // multiple matching options
      {
        // remove the first entry and re-insert to the map to place it at the end
        remove_option(opt,val);
        options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,true)));
      }
    }
    else 
      val = "n/a";
    
    return val;
  }
};

class parameters_partition_t
{ 
 public:
  parameters_partition_t() : ID("n/a") { reset(); }
  ~parameters_partition_t() { reset(); }

  string ID;
  multimap<string,pair<string,bool> > options;

  void check_option(FILE *fp)
  {
    for(multimap<string,pair<string,bool> >::iterator options_it = options.begin();
        options_it != options.end(); options_it++)
    {
      if(!options_it->second.second) // unused option
        fprintf(fp,"EI WARNING (parameters): partition option <%s,%s> is unused in %s\n",
                options_it->first.c_str(),options_it->second.first.c_str(),ID.c_str());
    }
  }
  
  void reset(void)
  {
    options.clear();
  }
  
  void add_option(string opt, string val) // add a string pair option
  {
    options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,false)));
  }
  
  void remove_option(string opt, string val = "n/a") // remove matching options
  {
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    // remove matching options
    for(multimap<string,pair<string,bool> >::iterator options_it = options_rng.first;\
        options_it != options_rng.second; options_it++)
      if((options_it->second.first == "n/a")||(options_it->second.first == val))
        options.erase(options_it);
  }
  
  string get_option(string opt)
  {
    string val;
    
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    if(options_rng.first != options_rng.second)
    {
      val = options_rng.first->second.first; // return the first found entry
      options_rng.first->second.second = true; // mark as a used option
      
      // re-order the multimap
      multimap<string,pair<string,bool> >::iterator it_last = options_rng.second;
      it_last--; // last matching option in the multimap
      
      if(it_last != options_rng.first) // multiple matching options
      {
        // remove the first entry and re-insert to the map to place it at the end
        remove_option(opt,val);
        options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,true)));
      }
    }
    else 
      val = "n/a";
    
    return val;
  }
};

class parameters_module_t
{
 public:
  parameters_module_t() : ID("n/a") { reset(); } 
  ~parameters_module_t() { reset(); }

  string ID;
  multimap<string,pair<string,bool> > options;

  void check_option(FILE *fp)
  {
    for(multimap<string,pair<string,bool> >::iterator options_it = options.begin();
        options_it != options.end(); options_it++)
    {
      if(!options_it->second.second) // unused option
        fprintf(fp,"EI WARNING (parameters): module option <%s,%s> is unused in %s\n",
                options_it->first.c_str(),options_it->second.first.c_str(),ID.c_str());
    }
  }
  
  void reset(void)
  {
    options.clear();
  }
  
  void add_option(string opt, string val) // add a string pair option
  {
    options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,false)));
  }
  
  void remove_option(string opt, string val = "n/a") // remove matching options
  {
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    // remove matching options
    for(multimap<string,pair<string,bool> >::iterator options_it = options_rng.first;\
        options_it != options_rng.second; options_it++)
      if((options_it->second.first == "n/a")||(options_it->second.first == val))
        options.erase(options_it);
  }
  
  string get_option(string opt)
  {
    string val;
    
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    if(options_rng.first != options_rng.second)
    {
      val = options_rng.first->second.first; // return the first found entry
      options_rng.first->second.second = true; // mark as a used option
      
      // re-order the multimap
      multimap<string,pair<string,bool> >::iterator it_last = options_rng.second;
      it_last--; // last matching option in the multimap
      
      if(it_last != options_rng.first) // multiple matching options
      {
        // remove the first entry and re-insert to the map to place it at the end
        remove_option(opt,val);
        options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,true)));
      }
    }
    else 
      val = "n/a";
    
    return val;
  }
};

class parameters_technology_t
{
 public:
  parameters_technology_t() : ID("n/a") { reset(); }
  ~parameters_technology_t() { reset(); }

  string ID;
  multimap<string,pair<string,bool> > options;

  void check_option(FILE *fp)
  {
    for(multimap<string,pair<string,bool> >::iterator options_it = options.begin();
        options_it != options.end(); options_it++)
    {
      if(!options_it->second.second) // unused option
        fprintf(fp,"EI WARNING (parameters): technology option <%s,%s> is unused in %s\n",
                options_it->first.c_str(),options_it->second.first.c_str(),ID.c_str());
    }
  }
  
  void reset(void)
  {
    options.clear();
  }
  
  void add_option(string opt, string val) // add a string pair option
  {
    options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,false)));
  }
  
  void remove_option(string opt, string val = "n/a") // remove matching options
  {
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    // remove matching options
    for(multimap<string,pair<string,bool> >::iterator options_it = options_rng.first;\
        options_it != options_rng.second; options_it++)
      if((options_it->second.first == "n/a")||(options_it->second.first == val))
        options.erase(options_it);
  }
  
  string get_option(string opt)
  {
    string val;
    
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    if(options_rng.first != options_rng.second)
    {
      val = options_rng.first->second.first; // return the first found entry
      options_rng.first->second.second = true; // mark as a used option
      
      // re-order the multimap
      multimap<string,pair<string,bool> >::iterator it_last = options_rng.second;
      it_last--; // last matching option in the multimap
      
      if(it_last != options_rng.first) // multiple matching options
      {
        // remove the first entry and re-insert to the map to place it at the end
        remove_option(opt,val);
        options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,true)));
      }
    }
    else 
      val = "n/a";
    
    return val;
  }
};

class parameters_sensor_t
{
 public:
  parameters_sensor_t() : ID("n/a") { reset(); }
  ~parameters_sensor_t() { reset(); }

  string ID;
  multimap<string,pair<string,bool> > options;

  void check_option(FILE *fp)
  {
    for(multimap<string,pair<string,bool> >::iterator options_it = options.begin();
        options_it != options.end(); options_it++)
    {
      if(!options_it->second.second) // unused option
        fprintf(fp,"EI WARNING (parameters): sensor option <%s,%s> is unused in %s\n",
                options_it->first.c_str(),options_it->second.first.c_str(),ID.c_str());
    }
  }
  
  void reset(void)
  {
    options.clear();
  }
  
  void add_option(string opt, string val) // add a string pair option
  {
    options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,false)));
  }
  
  void remove_option(string opt, string val = "n/a") // remove matching options
  {
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    // remove matching options
    for(multimap<string,pair<string,bool> >::iterator options_it = options_rng.first;\
        options_it != options_rng.second; options_it++)
      if((options_it->second.first == "n/a")||(options_it->second.first == val))
        options.erase(options_it);
  }
  
  string get_option(string opt)
  {
    string val;
    
    // find matching option range
    pair<multimap<string,pair<string,bool> >::iterator,multimap<string,pair<string,bool> >::iterator> options_rng = options.equal_range(opt);
    
    if(options_rng.first != options_rng.second)
    {
      val = options_rng.first->second.first; // return the first found entry
      options_rng.first->second.second = true; // mark as a used option
      
      // re-order the multimap
      multimap<string,pair<string,bool> >::iterator it_last = options_rng.second;
      it_last--; // last matching option in the multimap
      
      if(it_last != options_rng.first) // multiple matching options
      {
        // remove the first entry and re-insert to the map to place it at the end
        remove_option(opt,val);
        options.insert(pair<string,pair<string,bool> >(opt,pair<string,bool>(val,true)));
      }
    }
    else 
      val = "n/a";
    
    return val;
  }
};

class parameters_t {
 public:
  parameters_t() { reset(); }
  ~parameters_t() { reset(); }

  vector<parameters_package_t> package;
  vector<parameters_partition_t> partition;
  vector<parameters_module_t> module;
  vector<parameters_technology_t> technology;
  vector<parameters_sensor_t> sensor;

  // parameters initialization
  void parse(char *config);

  void reset(void)
  {
    package.clear();
    partition.clear();
    module.clear();
    technology.clear();
    sensor.clear();
  }
  
  void check_option(FILE *fp)
  {
    for(vector<parameters_package_t>::iterator it = package.begin(); it < package.end(); it++)
      it->check_option(fp);
    for(vector<parameters_partition_t>::iterator it = partition.begin(); it < partition.end(); it++)
      it->check_option(fp);
    for(vector<parameters_module_t>::iterator it = module.begin(); it < module.end(); it++)
      it->check_option(fp);
    for(vector<parameters_technology_t>::iterator it = technology.begin(); it < technology.end(); it++)
      it->check_option(fp);
    for(vector<parameters_sensor_t>::iterator it = sensor.begin(); it < sensor.end(); it++)
      it->check_option(fp);
  }
};

char lowercase(char ch);

char uppercase(char ch);

char* lowerstring(char *str);

string lowerstring(string str);

char* upperstring(char *str);

string upperstring(string str);

int stricmp(const char *str1, const char *str2);

int stricmp(string str1, string str2);

/*
static char lowercase(char ch)
{
  int alphabets = 'a'-'A';
  if((ch >= 'A') && (ch <= 'Z'))
    return ch+alphabets;
  else
    return ch;
}

static char uppercase(char ch)
{
  int alphabets = 'a'-'A';
  if((ch >= 'a') && (ch <= 'z'))
    return ch-alphabets;
  else
    return ch;
}

static char* lowerstring(char *str)
{
  for(int i = 0; i < strlen(str); i++)
    if((str[i] >= 'A')||(str[i] <= 'Z'))
      str[i] = lowercase(str[i]);
  return str;
}

static string lowerstring(string str)
{
  return (string)lowerstring((char*)str.c_str());
}

static char* upperstring(char *str)
{
  for(int i = 0; i < strlen(str); i++)
    if((str[i] >= 'a')||(str[i] <= 'z'))
      str[i] = uppercase(str[i]);
  return str;
}

static string upperstring(string str)
{
  return (string)upperstring((char*)str.c_str());
}

static int stricmp(const char *str1, const char *str2)
{
  unsigned char ch1, ch2;

  for (;;)
  {
    ch1 = (unsigned char)*str1++; ch1 = lowercase(ch1);
    ch2 = (unsigned char)*str2++; ch2 = lowercase(ch2);

    if (ch1 != ch2)
      return ch1 - ch2;
    if (ch1 == '\0')
      return 0;
  }
}

static int stricmp(string str1, string str2)
{
  return stricmp(str1.c_str(),str2.c_str());
}
*/

template <typename P>
static void set_variable(double &var, P &param, string opt, double def, bool err = false)
{
  string option = lowerstring(opt);
  string val = param.get_option(option);

  if(val == "n/a")
  {
    if(err)
    {
      fprintf(stdout,"EI ERROR (parameters): %s is not defined in %s\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    else
      var = def;
  }
  else
    var = atof(val.c_str());
}

template <typename P>
static void set_variable(int &var, P &param, string opt, int def, bool err = false)
{
  string option = lowerstring(opt);
  string val = param.get_option(option);

  if(val == "n/a")
  {
    if(err)
    {
      fprintf(stdout,"EI ERROR (parameters): %s is not defined in %s\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    else
      var = def;
  }
  else
    var = atoi(val.c_str());
}

template <typename P>
static void set_variable(unsigned int &var, P &param, string opt, unsigned int def, bool err = false)
{
  string option = lowerstring(opt);
  string val = param.get_option(option);

  if(val == "n/a")
  {
    if(err)
    {
      fprintf(stdout,"EI ERROR (parameters): %s is not defined in %s\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    else
      var = def;
  }
  else
    var = (unsigned int)atoi(val.c_str());
}

template <typename P>
static void set_variable(bool &var, P &param, string opt, bool def, bool err = false)
{
  string option = lowerstring(opt);
  string val = param.get_option(option);

  if(val == "n/a")
  {
    if(err)
    {
      fprintf(stdout,"EI ERROR (parameters): %s is not defined in %s\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    else
      var = def;
  }
  else
  {
    if(stricmp(val,"true")&&stricmp(val,"false")&&stricmp(val,"1")&&stricmp(val,"0"))
    {
      fprintf(stdout,"EI ERROR (parameters): option %s in %s should be true or false\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    var = !(stricmp(val,"true")&&stricmp(val,"1"));
  }
}

template <typename P>
static void set_variable(char* var, P &param, string opt, char* def, bool err = false)
{
  string option = lowerstring(opt);
  string val = param.get_option(option);

  if(val == "n/a")
  {
    if(err)
    {
      fprintf(stdout,"EI ERROR (parameters): %s is not defined in %s\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    else
      var = def;
  }
  else
    var = val.c_str();
}

template <typename P>
static void set_variable(string &var, P &param, string opt, string def, bool err = false)
{
  string option = lowerstring(opt);
  string val = param.get_option(option);

  if(val == "n/a")
  {
    if(err)
    {
      fprintf(stdout,"EI ERROR (parameters): %s is not defined in %s\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    else
      var = def;
  }
  else
  {
    if(!(stricmp(def,"true")&&stricmp(def,"false"))&&\
       stricmp(val,"true")&&stricmp(val,"false")&&stricmp(val,"1")&&stricmp(val,"0"))
    {
      fprintf(stdout,"EI ERROR (parameters): option %s in %s should be true or false\n",opt.c_str(),param.ID.c_str());
      exit(1);
    }
    var = val;
  }
}
#endif
