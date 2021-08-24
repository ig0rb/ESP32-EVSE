#ifndef _EVSE_MY_CONFIG_
#define _EVSE_MY_CONFIG_

//mettere qua i valori di default
// oppure creare un file chiamato "my_config.h"

#define MY_NTP_POOL "pool.ntp.org"
#define MY_NTP_GMT_OFFSET 3600
#define MY_NTP_DAYLIGHT_OFFSET 3600

//#define MY_POWERMETER_URL   "http://192.168.6.108/status"
#define MY_POWERMETER_URL   "http://192.168.6.145:8081/status"
#define MY_POWERMETER_TYPE  "SHELLY2EM"

#define MY_WIFI_SSID    "ESP"
#define MY_WIFI_PASS    "ESP"

#define MY_SYSLOG       "192.168.6.158"

#define MY_GRID_CAPACITY 3000
#define MY_EVSE_CAPACITY 3500

#define MODE_ECO_EVSE_POWER_LIMIT  1500
#define MODE_ECO_GRID_POWER_LIMIT  -500
#define MODE_ECO_HYSTERESIS_ON  180
#define MODE_ECO_HYSTERESIS_OFF  600

#define MODE_SOLAR_EVSE_POWER_LIMIT  3500
#define MODE_SOLAR_GRID_POWER_LIMIT  0
#define MODE_SOLAR_HYSTERESIS_ON  180
#define MODE_SOLAR_HYSTERESIS_OFF  1800

#define MODE_SLOW_EVSE_POWER_LIMIT  1500
#define MODE_SLOW_GRID_POWER_LIMIT  2500
#define MODE_SLOW_HYSTERESIS_ON  180
#define MODE_SLOW_HYSTERESIS_OFF  3600

#define MODE_FAST_EVSE_POWER_LIMIT  3500
#define MODE_FAST_GRID_POWER_LIMIT  2900
#define MODE_FAST_HYSTERESIS_ON  180
#define MODE_FAST_HYSTERESIS_OFF  3600


#define POWER_METER_TIMEOUT_WARNING 30    // timeout del powermeter, dopo il quale diventa un problema
#define POWER_METER_TIMEOUT_ERROR   60    // timeout del powermeter, dopo il quale viene ridotta la potenza
#define POWER_METER_TIMEOUT_FATAL   120   // timeout del powermeter, dopo il quale viene fermata la ricarica


#endif
