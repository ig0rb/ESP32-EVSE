#include "EvseModes.h"
#include "Config.h"



namespace EVSE {

  EvseMode::EvseMode(const char* mode_c, int mode_i, int *evse_max_power, int *grid_max_power, int *hysteresis_on, int *hysteresis_off) {
    strcpy(this->mode_c, mode_c);
    this->mode_i = mode_i;
    this->evse_max_power = evse_max_power;
    this->grid_max_power = grid_max_power;
    this->hysteresis_on = hysteresis_on;
    this->hysteresis_off = hysteresis_off;
  }

  EvseMode::~EvseMode() {}


  EvseModes::EvseModes() {
    init();
  }

  void EvseModes::init() {
    Config* config = Config::getInstance();
    modes[OFF] = new EvseMode("OFF", OFF, &zero, &zero, &cinque, &cinque);
    
    modes[ECO] = new EvseMode("ECO", ECO,
      &config->mode_eco_evse_power_limit,
      &config->mode_eco_grid_power_limit,
      &config->mode_eco_hysteresis_on,
      &config->mode_eco_hysteresis_off
    );

    modes[SOLAR] = new EvseMode("SOLAR", SOLAR, 
      &config->mode_solar_evse_power_limit,
      &config->mode_solar_grid_power_limit,
      &config->mode_solar_hysteresis_on,
      &config->mode_solar_hysteresis_off
    );

    modes[SLOW] = new EvseMode("SLOW", SLOW, 
      &config->mode_slow_evse_power_limit,
      &config->mode_slow_grid_power_limit,
      &config->mode_slow_hysteresis_on,
      &config->mode_slow_hysteresis_off
    );

    modes[FAST] = new EvseMode("FAST", FAST, 
      &config->mode_fast_evse_power_limit,
      &config->mode_fast_grid_power_limit,
      &config->mode_fast_hysteresis_on,
      &config->mode_fast_hysteresis_off
    );






  }

}