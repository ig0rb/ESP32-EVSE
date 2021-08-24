#ifndef _EVSE_WEBSERVER_H_
#define _EVSE_WEBSERVER_H_

//#include <EVSE.h>
#include <ESPAsyncWebServer.h>


namespace EVSE {
  class WebServer {

    public:
      WebServer();
      ~WebServer();

      void begin();


    private:
      static void error_404(AsyncWebServerRequest *request);
      static void index(AsyncWebServerRequest *request);
      static void index_post(AsyncWebServerRequest *request);

      static void status(AsyncWebServerRequest *request);
      static void system(AsyncWebServerRequest *request);
      /*
      static void config_get(AsyncWebServerRequest *request);
      static void config_put(AsyncWebServerRequest *request);
      */

      AsyncWebServer server = AsyncWebServer(80);

  };
};

#endif