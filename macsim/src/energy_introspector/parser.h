#ifndef EI_PARSE_H
#define EI_PARSE_H

#include "parameters.h"

class parser_t
{
 public:
  void parse(char *filename, class parameters_t *parameters);

 private:
  parameters_package_t package;
  parameters_partition_t partition;
  parameters_module_t module;
  parameters_technology_t technology;
  parameters_sensor_t sensor;
};

#endif
