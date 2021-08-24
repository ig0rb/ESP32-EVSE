#include <Preferences.h>
#include "Config.h"

#if __has_include("my_config.h")
#include "my_config.h"
#else
#include "def_config.h"
#endif

namespace EVSE {

  Config* Config::instance = 0;

  Config* Config::getInstance() {
    if (instance == 0) {
      instance = new Config();
    }
    return instance;
  }



  void Config::read() {
    Preferences p;
    p.begin("evse", false);
    if (p.getULong64("build") != build_timestamp) {
      Serial.printf("Preferences not initialized... %li\n", build_timestamp);
      p.end();
      reset();
      return;
    }
    
    ntp_gmt_offset = p.getULong64("ntp_g_o");
    ntp_daylight_offset = p.getULong64("ntp_d_o");
    p.getString("ntp", ntp_pool, sizeof(ntp_pool));
    
    p.getString("pm_url", power_meter_url, sizeof(power_meter_url));
    p.getString("pm_t", power_meter_type, sizeof(power_meter_type));

    p.getString("wifi_ssid", wifi_ssid, sizeof(wifi_ssid));
    p.getString("wifi_pass", wifi_pass, sizeof(wifi_pass));
    
    p.getString("syslog", syslog, sizeof(syslog));
    
    power_capacity_grid = p.getInt("p_cap_grid");
    power_capacity_evse = p.getInt("p_cap_evse");
    

    mode_eco_evse_power_limit = p.getInt("m_eco_evse_pl");
     mode_eco_grid_power_limit= p.getInt("m_eco_grid_pl");
     mode_eco_hysteresis_on = p.getInt("m_eco_hy_on");
     mode_eco_hysteresis_off = p.getInt("m_eco_hy_off");
    
     mode_solar_evse_power_limit = p.getInt("m_sol_evse_pl");
     mode_solar_grid_power_limit = p.getInt("m_sol_grid_pl");
     mode_solar_hysteresis_on  = p.getInt("m_sol_hy_on");
     mode_solar_hysteresis_off = p.getInt("m_sol_hy_off");
    
     mode_slow_evse_power_limit = p.getInt("m_slow_evse_pl");
     mode_slow_grid_power_limit = p.getInt("m_slow_grid_pl");
     mode_slow_hysteresis_on  = p.getInt("m_slow_hy_on");
     mode_slow_hysteresis_off = p.getInt("m_slow_hy_off");
    
     mode_fast_evse_power_limit = p.getInt("m_fast_evse_pl");
     mode_fast_grid_power_limit = p.getInt("m_fasr_grid_pl");
     mode_fast_hysteresis_on  = p.getInt("m_fast_hy_on");
     mode_fast_hysteresis_off = p.getInt("m_fast_hy_off");


    p.end();
  }

  void Config::save() {
    Preferences p;
    p.begin("evse", false);
    p.putULong64("build", build_timestamp);
    
    p.putULong64("ntp_g_o", ntp_gmt_offset);
    p.putULong64("ntp_d_o", ntp_daylight_offset);
    p.putString("ntp", ntp_pool);
    
    p.putString("pm_url", power_meter_url);
    p.putString("pm_t", power_meter_type);

    p.putString("wifi_ssid", wifi_ssid);
    p.putString("wifi_pass", wifi_pass);
    
    p.putString("syslog", syslog);
    
    p.putInt("p_cap_grid", power_capacity_grid);
    p.putInt("p_cap_evse", power_capacity_evse);
    

    p.putInt("m_eco_evse_pl", mode_eco_evse_power_limit);
    p.putInt("m_eco_grid_pl", mode_eco_grid_power_limit);
    p.putInt("m_eco_hy_on",   mode_eco_hysteresis_on);
    p.putInt("m_eco_hy_off",  mode_eco_hysteresis_off);
    
    p.putInt("m_sol_evse_pl", mode_solar_evse_power_limit);
    p.putInt("m_sol_grid_pl", mode_solar_grid_power_limit);
    p.putInt("m_sol_hy_on",   mode_solar_hysteresis_on);
    p.putInt("m_sol_hy_off",  mode_solar_hysteresis_off);
    
    p.putInt("m_slow_evse_pl", mode_slow_evse_power_limit);
    p.putInt("m_slow_grid_pl", mode_slow_grid_power_limit);
    p.putInt("m_slow_hy_on",   mode_slow_hysteresis_on);
    p.putInt("m_slow_hy_off",  mode_slow_hysteresis_off);
    
    p.putInt("m_fast_evse_pl", mode_fast_evse_power_limit);
    p.putInt("m_fasr_grid_pl", mode_fast_grid_power_limit);
    p.putInt("m_fast_hy_on",   mode_fast_hysteresis_on);
    p.putInt("m_fast_hy_off",  mode_fast_hysteresis_off);
    

    p.end();

  }

  void Config::reset() {
    Preferences p;
    p.begin("evse", false);
    p.clear();
    p.end();
    init();
    save();
  }


  Config::Config() {
    init();
    
    read();
  }

  void Config::init() {
    ntp_gmt_offset = MY_NTP_GMT_OFFSET;
    ntp_daylight_offset = MY_NTP_DAYLIGHT_OFFSET;
    strncpy(ntp_pool, MY_NTP_POOL, sizeof(ntp_pool));

    strcpy(power_meter_url, MY_POWERMETER_URL);
    strcpy(power_meter_type, MY_POWERMETER_TYPE);
    

    strcpy(wifi_ssid, MY_WIFI_SSID);
    strcpy(wifi_pass, MY_WIFI_PASS);

    strcpy(syslog, MY_SYSLOG);

    power_capacity_grid = MY_GRID_CAPACITY;
    power_capacity_evse = MY_EVSE_CAPACITY;

    mode_eco_evse_power_limit = MODE_ECO_EVSE_POWER_LIMIT;
    mode_eco_grid_power_limit = MODE_ECO_GRID_POWER_LIMIT;
    mode_eco_hysteresis_on = MODE_ECO_HYSTERESIS_ON;
    mode_eco_hysteresis_off = MODE_ECO_HYSTERESIS_OFF;

    mode_solar_evse_power_limit = MODE_SOLAR_EVSE_POWER_LIMIT;
    mode_solar_grid_power_limit = MODE_SOLAR_GRID_POWER_LIMIT;
    mode_solar_hysteresis_on = MODE_SOLAR_HYSTERESIS_ON;
    mode_solar_hysteresis_off = MODE_SOLAR_HYSTERESIS_OFF;

    mode_slow_evse_power_limit = MODE_SLOW_EVSE_POWER_LIMIT;
    mode_slow_grid_power_limit = MODE_SLOW_GRID_POWER_LIMIT;
    mode_slow_hysteresis_on = MODE_SLOW_HYSTERESIS_ON;
    mode_slow_hysteresis_off = MODE_SLOW_HYSTERESIS_OFF;

    mode_fast_evse_power_limit = MODE_FAST_EVSE_POWER_LIMIT;
    mode_fast_grid_power_limit = MODE_FAST_GRID_POWER_LIMIT;
    mode_fast_hysteresis_on = MODE_FAST_HYSTERESIS_ON;
    mode_fast_hysteresis_off = MODE_FAST_HYSTERESIS_OFF;
    
    // NO WEB per ORA
    power_meter_timeout_warning = POWER_METER_TIMEOUT_WARNING*1000;    // powermeter unresponsive warning (da un avviso)
    power_meter_timeout_error = POWER_METER_TIMEOUT_ERROR*1000;      // powermeter unresponsive error (riduce la potenza)
    power_meter_timeout_fatal = POWER_METER_TIMEOUT_FATAL*1000;     // powermeter unresponsive fatal (mette in pausa la ricarica)
  }

/*
  void Config::read() {
    Preferences preferences;
    preferences.begin("evse", false);
    //power_max_grid = preferences.getInt("p_max_grid", 0);
    //power_max_evse = preferences.getInt("p_max_evse", 0);

    power_capacity_grid = preferences.getInt("p_capacity_grid", 3000);
    //power_capacity_evse = preferences.getInt("p_capacity_evse", 3000);

    mode_eco_evse_power_limit = preferences.getInt("m_eco_p_limit", 3000);
    mode_solar_evse_power_limit = preferences.getInt("m_sol_p_limit", 3000);
    mode_slow_evse_power_limit = preferences.getInt("m_slow_p_limit", 1600);
    mode_fast_evse_power_limit = preferences.getInt("m_fast_p_limit", 3000);
    //mode_manual_evse_power_limit = preferences.getInt("m_man_p_limit", 3000);

    mode_evse_default = preferences.getInt("m_evse_defaul", 0);
    mode_evse_last = preferences.getInt("m_evse_last", 0);
    //dev_mode = preferences.getInt("dev_mode", 0);

    //power_meter_ignore = preferences.getBool("p_meter_ignore", true);
    preferences.end();

    //power_capacity_grid_10 = power_capacity_grid*1.1;
    //power_capacity_grid_30 = power_capacity_grid*1.3;
  }

  void Config::save() {
    Preferences preferences;
    preferences.begin("evse", false);
    //preferences.putInt("p_max_grid", power_max_grid);
    //preferences.putInt("p_max_evse", power_max_evse);

    preferences.putInt("p_capacity_grid", power_capacity_grid);
    //preferences.putInt("p_capacity_evse", power_capacity_evse);

    preferences.putInt("m_eco_p_limit", mode_eco_evse_power_limit);
    preferences.putInt("m_sol_p_limit", mode_solar_evse_power_limit);
    preferences.putInt("m_slow_p_limit", mode_slow_evse_power_limit);
    preferences.putInt("m_fast_p_limit", mode_fast_evse_power_limit);
    //preferences.putInt("m_man_p_limit", mode_manual_evse_power_limit);

    preferences.putInt("m_evse_defaul", mode_evse_default);

    //preferences.putInt("m_evse_last", mode_evse_last);
    //preferences.putInt("dev_mode", dev_mode);

    //preferences.putBool("p_meter_ignore", power_meter_ignore);
    
    preferences.end();

    read();
  }

*/
};
