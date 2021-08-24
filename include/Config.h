#ifndef _EVSE_CONFIG_H
#define _EVSE_CONFIG_H

#include <Arduino.h>

#ifndef BUILD_TIMESTAMP
#error "MISSING BUILD_TIMESTAMP"
#endif


namespace EVSE {
  class Config {
  public:
    static Config* getInstance();


    void read();
    void save();
    void reset();
    void init();












    // -E- Nuove proprietà


    //parametri verificati 20210817
    
    char power_meter_url[255] = "";       // URL del powermeter 
    char power_meter_type[32] = "";         // tipo del power meter (per future modifiche)

    char ntp_pool[64];                    // servers net pool
    long ntp_gmt_offset;                  // timezone offset
    long ntp_daylight_offset;             // ora legale offset

    char wifi_ssid[32];                   // wifi ssid
    char wifi_pass[32];                   // wifi password
    char syslog[32];                      // syslog server

    int power_capacity_grid = 3000;
    int power_capacity_evse = 3500;

    // AAA gestire i parametri per la configurazione e la parte WEB
    int mode_eco_evse_power_limit = 1500;
    int mode_eco_grid_power_limit = -500;
    int mode_eco_hysteresis_on = 180;
    int mode_eco_hysteresis_off = 600;

    int mode_solar_evse_power_limit = 3500;
    int mode_solar_grid_power_limit = 0;
    int mode_solar_hysteresis_on = 180;
    int mode_solar_hysteresis_off = 1800;

    int mode_slow_evse_power_limit = 1500;
    int mode_slow_grid_power_limit = 2500;
    int mode_slow_hysteresis_on = 180;
    int mode_slow_hysteresis_off = 3600;

    int mode_fast_evse_power_limit = 3500;
    int mode_fast_grid_power_limit = 2900;
    int mode_fast_hysteresis_on = 180;
    int mode_fast_hysteresis_off = 3600;

    // AAA vedere come gestire la riaccensione , ultimo stato od off
    int mode_evse_default = 0;
    int mode_evse_last = 0;


    // NO WEB
    unsigned long power_meter_timeout_warning = 30000;    // powermeter unresponsive warning (da un avviso)
    unsigned long power_meter_timeout_error = 60000;      // powermeter unresponsive error (riduce la potenza)
    unsigned long power_meter_timeout_fatal = 120000;     // powermeter unresponsive fatal (mette in pausa la ricarica)
        
    int power_evse_min = 1500;            // potenza minima allocabile per l'EVSE

    unsigned long build_timestamp = BUILD_TIMESTAMP;

  private:
    Config();
    static Config *instance;
  };

};

#endif
