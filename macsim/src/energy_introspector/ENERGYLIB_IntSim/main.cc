#include "intsim.h"

int main(void)
{
  intsim_chip_t *chip = new intsim_chip_t();
  intsim_param_t *param = new intsim_param_t();

  param->setup();

  intsim(chip,param);

  return 0;
}
