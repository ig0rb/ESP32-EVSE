#ifndef _EVSE_OTA_H_
#define _EVSE_OTA_H_

//#include <EVSE.h>
#include <ArduinoOTA.h>

namespace EVSE {
  class OTA {
    public:
      OTA();
      //~OTA();

      void begin();
      void handle();

    private:
      //EVSE::Config *config;
      //EVSE::Status *status;
      ArduinoOTAClass arduinoOTA;


  };
}


#endif