/* parameters.cc - input parameters
 * Copyright 2011 by William Song and Sudhakar Yalamanchili
 * Georgia Tech Research Corporation, Atlanta, GA 30332
 * All Rights Reserved.
 */

#include "parameters.h"
#include "parser.h"

void parameters_t::parse(char *config)
{
  #ifdef EI_DEBUG
  fprintf(stdout,"EI_DEBUG: parsing input config file %s\n",config);
  #endif
  parser_t *parser = new parser_t();
  parser->parse(config,this);
}
