//#include <EVSE.h>

#include "PowerManager.h"
#include "MyLog.h"


namespace EVSE {
  PowerManager::PowerManager() {

  }

  PowerManager::~PowerManager() {
    

  }

  void PowerManager::handle() {
    if ((millis() - handle_last_ts) < 1000) {
      return;
    }
    handle_last_ts = millis();

    if (status->state == Status::states_t::BOOTING || status->state == Status::states_t::RELOAD) {
      status->state = Status::states_t::WAITING_POWER;
      set_evse_power(0);
      ts_off = millis();
      return;
    }


    //int evse_max = status->v2_evse_modes->mode->evse_max_power;
    int grid_max = *status->evse_modes->mode->grid_max_power;
    //int hysteresis_on  = status->v2_evse_modes->mode->hysteresis_on;
    //int hysteresis_off =  status->v2_evse_modes->mode->hysteresis_off;
    int evse_min = config->power_evse_min;

    // Gestione dello stato OFF in entrata
    if (status->state != Status::states_t::OFF && status->evse_modes->mode->mode_i == EvseModes::OFF) {
      status->state = Status::states_t::OFF;
      evse_power = 0;
      set_evse_power(0);
      return;      
    } 

    // Gestione uscita dello stato off
    if (status->state == Status::states_t::OFF && status->evse_modes->mode->mode_i != EvseModes::OFF) {
      status->state = Status::states_t::WAITING_POWER;
      evse_power = 0;
      set_evse_power(0);
      return;      
    } 

    if (status->state == Status::states_t::OFF) {
      return;
    }

    if (status->state == Status::states_t::PAUSE) {
      if (millis() < status->pause_untill) {
        return;
      }
      l_notice("Exiting from PAUSE");
      overpower = false;
      status->state = Status::states_t::WAITING_POWER;
    }

    //l_debug("controllo del sovraccarico qua...");


    if (status->state == Status::states_t::WAITING_POWER) {
      boolean valid;
      unsigned long ts = 0;
      int grid_now = status->get_power_avg(Status::POWER_METER_GRID, 60, &valid, &ts);
      l_info("grid %i evse_min %i grid_max %i valid %i last %i", grid_now, evse_min, grid_max, valid, (millis()-ts)/1000);
      if (millis()-ts > 20000) {
        l_error("power meter error");
        status->power_meter_fail = true;
        return;
      } 
      status->power_meter_fail = false;
      if (grid_now + evse_min < grid_max) {
        status->state = Status::states_t::STANDBY;
        evse_power = evse_min;
        ts_on = millis();
        set_evse_power(evse_min);
        return;
      }
    }

    if (status->state == Status::states_t::STANDBY) {
      if (j1772->state == J1772::states::READY || j1772->state == J1772::states::VENTILATION) {
        status->state = Status::states_t::READY;
        ts_on = millis();
      }
      return;
    }

    if (status->state == Status::READY) {
      if (j1772->state == J1772::states::READY || j1772->state == J1772::states::VENTILATION) {
        // ok sto caricando...
        // Inserire le logiche per la potenza massima....
        power_tracker();
      } else {
        status->state = Status::states_t::STANDBY;
        set_evse_power(evse_min);
      }
      return;
    }









    
    // check_over_power();
  }

  void PowerManager::power_tracker() {
    if (status->state != Status::READY) {
      return;     // lavoro solo nella fare ready...
    }

    if (power_tracker_next_run_ts > millis()) {
      return;
    }

    power_tracker_next_run_ts = millis() + 5000;
    
    int evse_max = *status->evse_modes->mode->evse_max_power;
    int grid_max = *status->evse_modes->mode->grid_max_power;
    int hysteresis_on  = *status->evse_modes->mode->hysteresis_on;
    int hysteresis_off =  *status->evse_modes->mode->hysteresis_off;
    int evse_min = config->power_evse_min;

    boolean valid = false;
    unsigned long ts = 0;
    int grid_power = status->get_power_avg(Status::power_meters_t::POWER_METER_GRID, 30, &valid, &ts);

    // Controllo timeout del misuratore
    if (millis() - ts > config->power_meter_timeout_fatal) {
      l_warning("Last valid power meter read %i seconds ago, reducing power", (millis()-ts)/1000);
      set_evse_power(0);
      status->state = Status::states_t::WAITING_POWER;
      //power_tracker_next_run = millis() + 5000;
      status->power_meter_fail = true;
      return;
    }

    if (millis() - ts > config->power_meter_timeout_error) {
      l_warning("Last valid power meter read %i seconds ago, reducing power", (millis()-ts)/1000);
      set_evse_power(evse_min);
      //power_tracker_next_run = millis() + 10000;
      status->power_meter_fail = true;
      return;
    }
    if (millis() - ts > config->power_meter_timeout_warning) {
      l_warning("Last valid power meter read %i seconds ago", (millis()-ts)/1000);
      //power_tracker_next_run = millis() + 15000;
      status->power_meter_fail = true;
      return;
    }

    status->power_meter_fail = false;


    // Controllo overpower...
    /*
    if (overpower == true) {
      if (millis() - overpower_ts > 30000) {
        l_notice("Pause for overpower");
        set_evse_power(0);
        status->state = Status::states_t::PAUSE;
        status->pause_untill = millis() + 60000;
        return;
      }
    }
    */

    grid_power = status->get_power_avg(Status::power_meters_t::POWER_METER_GRID, 15);
    int overpower_percentage = ((grid_power*100)/config->power_capacity_grid)-100;
    //l_debug("Over power %% %i", overpower_percentage);
    if (overpower_percentage < 10) {
      overpower = false;
      status->grid_over_power = false;
    } else if(overpower_percentage > 30) {
      overpower = true;
      overpower_ts = millis();
      l_notice("Pause for overpower %i%%", overpower_percentage);
      set_evse_power(0);
      status->state = Status::states_t::PAUSE;
      status->pause_untill = millis() + 60000;
      return;
    } else {
      set_evse_power(evse_min);
      if (overpower == false) {
        l_notice("Overpower %i%%", overpower_percentage);
        overpower = true;
        overpower_ts = millis();
      }
      if (millis() - overpower_ts > 6*60*1000) {
        l_notice("Pause for overpower %i%%", overpower_percentage);
        set_evse_power(0);
        status->state = Status::states_t::PAUSE;
        status->pause_untill = millis() + 60000;
        return;
      }
    }

    overpower = false;

    grid_power = status->get_power_avg(Status::power_meters_t::POWER_METER_GRID, 1);

    /*
    int evse_max = status->v2_evse_modes->mode->evse_max_power;
    int grid_max = status->v2_evse_modes->mode->grid_max_power;
    int hysteresis_on  = status->v2_evse_modes->mode->hysteresis_on;
    int hysteresis_off =  status->v2_evse_modes->mode->hysteresis_off;
    int evse_min = config->power_evse_min;
    */
    l_debug("evse_max %i grid_max %i hysteresis_on %i hysteresis_off %i evse_min %i",
      evse_max, grid_max, hysteresis_on, hysteresis_off, evse_min
    );
    int grid_offset = grid_power - grid_max;
    int val = 100;
    l_debug("grid_offset %i", grid_offset);
    if (grid_offset >= 100) {
      if (grid_offset > 500) {
        val = 200;
      } else if(grid_offset > 250) {
        val = 150;
      }
      if(set_evse_down(val)) {
        l_info("evse power down %i %i", val, evse_power);
      }
      if (grid_over_target == false) {
        grid_over_target = true;
        grid_over_target_ts = millis();
      } else {
        if ((millis() - grid_over_target_ts) > hysteresis_off*1000) {
          l_notice("Pause for grid over power");
          set_evse_power(0);
          ts_off = millis();
          status->state = Status::states_t::PAUSE;
          status->pause_untill = millis() + hysteresis_on;
        }
      }
      return;
    }

    grid_over_target = false;

    if (evse_power >= evse_max) {
      return;
    } 
    
    if (grid_offset < 0) {
       if (grid_offset < 500) {
        val = 200;
      } else if(grid_offset < 250) {
        val = 150;
      }
      if(set_evse_up(val, evse_max)) {
        l_info("evse power up %i %i", val, evse_power);
      }
    }
    

    return;









  }


  void PowerManager::set_evse_power(int power) {
    status->power_evse_allocated = power;
    set_evse_up_ts = millis();
    set_evse_down_ts = millis();
    j1772->set_power(power);
  }


  int PowerManager::set_evse_up(int value, int max) {
    if ((millis() - set_evse_up_ts) < 30000) {
      return 0;
    }
    if (evse_power + value > max) {
      evse_power = max;
    } else {
      evse_power += value;
    }
    evse_power+=100;
    set_evse_power(evse_power);
    set_evse_up_ts = millis();
    return 1;
  }


  int PowerManager::set_evse_down(int value) {
    if ((millis() - set_evse_down_ts) < 30000) {
      return 0;
    }
    if (evse_power <= config->power_evse_min) {
      Serial.printf("Sono al minimo %i %i\n", evse_power, config->power_evse_min);
    } else if (evse_power - value < config->power_evse_min){
      evse_power = config->power_evse_min;
    } else {
      evse_power-=value;
    }
    set_evse_power(evse_power);
    set_evse_down_ts = millis();
    return 1;
  }






  void PowerManager::begin() {
    status = EVSE::Status::getInstance();
    config = EVSE::Config::getInstance();
    j1772 = EVSE::J1772::getInstance();

    status->state = Status::WAITING_POWER;
  }

}
