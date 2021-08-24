
#ifndef _EVSE_NETWORKING_H_
#define _EVSE_NETWORKING_H_

//#include <EVSE.h>

#include "Config.h"
#include "Status.h"


namespace EVSE {
  class Networking {
    public:
      Networking();
      ~Networking();

      void begin();

      void WiFiConnect();

      void ntp();
      void mdns();

      static void _WifiConnect(void *pvParameters);
      const char* get_hostname();

      //WiFiUDP udp;
      //Syslog *syslog;

    private:
      EVSE::Status *status;
      EVSE::Config *config;
      static char hostname[64];
      //AsyncWebServer *server;


    };
}



#endif
