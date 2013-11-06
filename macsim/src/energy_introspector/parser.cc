#include "parser.h"

#define LINESIZE 1024

using namespace std;

void parser_t::parse(char *filename, parameters_t *parameters)
{
  FILE *file = fopen(filename,"r");
  if(!file)
  {
    fprintf(stdout,"EI ERROR (parser): cannot open config file %s\n",filename);
    exit(1);
  }

  char parseline[1024];
  char *option, *component, *variable, *value, *valid_line;
  unsigned int line_index = 0;

  while(1)
  {
    line_index++;
    fflush(file);
    valid_line = fgets(parseline,LINESIZE,file);
    if(/*feof(file)||*/!parseline||!valid_line)
      break;
    //cout << "parseline = " << parseline;
    if(parseline[0] != '-')
      continue;

    option = strtok(parseline," \t\r\n#");
    //cout << "option = " << option << endl;
    value = strtok(NULL," \t\r\n#");

    if(strcspn(option,".")>=strlen(option))
    {
      if(feof(file))
        break;
      else
      {
        fprintf(stdout,"EI ERROR (parser): incomponent config option (line %u): %s\n",line_index,option);
        exit(1);
      }
    }
    option = lowerstring(option);
    component = strtok(option,".");
    //cout << "component = " << component << endl;
    variable = strtok(NULL," \t\r\n#");
    //cout << "variable = " << variable << endl;
    if(!variable)
    {
      if(feof(file))
        break;
      else
      {
        fprintf(stdout,"EI ERROR (parser): incomplete config option (line %u): %s\n",line_index,parseline);
        exit(1);
      }
    }
    if(value)
    {
      //cout << "value = " << value << endl;

      if(!stricmp(component,"-package"))
        package.add_option(variable,value);
      else if(!stricmp(component,"-partition"))
        partition.add_option(variable,value);
      else if(!stricmp(component,"-module"))
        module.add_option(variable,value);
      else if(!stricmp(component,"-technology"))
        technology.add_option(variable,value);
      else if(!stricmp(component,"-sensor"))
        sensor.add_option(variable,value);
      else
      {
        fprintf(stdout,"EI ERROR (parser): unknown -component (line %u): %s\n",line_index,component);
        exit(1);
      }
    }
    else
    {
      if(stricmp(variable,"end"))
      {
        fprintf(stdout,"EI ERROR (parser): missing config value (line %u): %s.%s\n",line_index,component,variable);
		exit(1);
      }
      else
      {
        map<string,pair<string,bool> >::iterator opt_it;

        if(!stricmp(component,"-package"))
        {
          opt_it = package.options.find("id");
          if(opt_it == package.options.end())
          {
            fprintf(stdout,"EI ERROR (parser): pseudo package has no ID %u %s %s\n", line_index, component, variable);
            exit(1);
          }
          package.ID = opt_it->second.first;
          package.options.erase(opt_it);
          parameters->package.push_back(package);
          package.reset();
        }
        else if(!stricmp(component,"-partition"))
        {
          opt_it = partition.options.find("id");
          if(opt_it == partition.options.end())
          {
            fprintf(stdout,"EI ERROR (parser): pseudo partition has no ID %u %s %s\n", line_index, component, variable);
            exit(1);
          }
          partition.ID = opt_it->second.first;
          partition.options.erase(opt_it);
          parameters->partition.push_back(partition);
          partition.reset();
        }
        else if(!stricmp(component,"-module"))
        {
          opt_it = module.options.find("id");
          if(opt_it == module.options.end())
          {
            fprintf(stdout,"EI ERROR (parser): pseudo module has no ID\n");
            exit(1);
          }
          module.ID = opt_it->second.first;
          module.options.erase(opt_it);
/*
          opt_it = module.options.find("technology");
          if(opt_it == module.options.end())
            module.add_option("technology","default");
*/
          parameters->module.push_back(module);
          module.reset();
        }
        else if(!stricmp(component,"-technology"))
        {
          opt_it = technology.options.find("id");
          if(opt_it == technology.options.end())
          {
            fprintf(stdout,"EI ERROR (parser): technology has no ID\n");
            exit(1);
          }
          technology.ID = opt_it->second.first;
          technology.options.erase(opt_it);
          parameters->technology.push_back(technology);
          technology.reset();
        }
        else if(!stricmp(component,"-sensor"))
        {
          opt_it = sensor.options.find("id");
          if(opt_it == sensor.options.end())
          {
            fprintf(stdout,"EI ERROR (parser): technology has no ID\n");
            exit(1);
          }
          sensor.ID = opt_it->second.first;
          sensor.options.erase(opt_it);
          parameters->sensor.push_back(sensor);
          sensor.reset();
        }
        else
        {
          fprintf(stdout,"EI ERROR (parser): unknown -component (line %u): %s\n",line_index,component);
          exit(1);
        }
      }
    }
    if(feof(file))
      break;
  }
  
  fclose(file);
}
