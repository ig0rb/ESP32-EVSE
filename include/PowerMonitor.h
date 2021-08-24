#ifndef _EVSE_POWERMONITOR_H_
#define _EVSE_POWERMONITOR_H_

//#include <EVSE.h>
#include "Status.h"
#include "Config.h"

namespace EVSE {
  /**
   * @brief classe per il monitoraggio dei power meter, usa un task sul core 0
   * 
   */
  class PowerMonitor {
    public:
      PowerMonitor();
      ~PowerMonitor();
      /**
       * @brief lancia il task del power monitor, che gira ogni 5 secondi sul core 0
       *  @attention lanciare solo dopo che il WiFi è connesso!!!
       * 
       */
      void begin();

    private:
      static void _begin(void *pvParameters);
      EVSE::Status *status;
      EVSE::Config *config;

  };
}

#endif
