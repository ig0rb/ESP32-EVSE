//#include <EVSE.h>

#include <Syslog.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <time.h>

#include "Networking.h"



namespace EVSE {
    char Networking::hostname[] = "EVSE-xxxxxx";

    Networking::Networking() {

    }

    Networking::~Networking() {

    }

    void Networking::ntp() {
      configTime(config->ntp_gmt_offset, config->ntp_daylight_offset, config->ntp_pool);
      if (WiFi.status() == WL_CONNECTED) {
        if (!getLocalTime(&status->localtime)) {
          Serial.printf("Unable to connect to ntp server %s\n", config->ntp_pool);
        }
        Serial.println(&status->localtime, "%A, %B %d %Y %H:%M:%S");
      }
    }


    void Networking::mdns() {
      MDNS.begin(hostname);
      MDNS.addService("http", "tcp", 80);
    }


    void Networking::begin() {
      status = EVSE::Status::getInstance();
      config = EVSE::Config::getInstance();
      uint32_t chipid = 0;
      for(int i=0; i<17; i=i+8) {
	     chipid |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	    }
      snprintf(hostname, sizeof(hostname), "EVSE-%s", String(chipid, HEX).c_str());

      xTaskCreatePinnedToCore(Networking::_WifiConnect, "WiFi Connect", 8192, NULL, 1, NULL, 0);
      while(WiFi.status() != WL_CONNECTED) {
        delay(1000);
      }


      //syslog(udp, SYSLOG_PROTO_IETF);

    }


    void Networking::_WifiConnect(void *pvParameters) {
      EVSE::Config *config = EVSE::Config::getInstance();
      WiFi.setHostname(Networking::hostname);
      Serial.printf("_WifiConnect START!!! %s\n", hostname);

      WiFi.begin(config->wifi_ssid, config->wifi_pass);
      int i = 0;
      while (WiFi.status() != WL_CONNECTED) {
        Serial.printf("WiFi connecting to %s [pass %s] attempt %i\n", config->wifi_ssid, config->wifi_pass, i);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        i++;
      }
      Serial.printf("Connected [%s]\n", WiFi.localIP().toString().c_str());
      Serial.printf("WiFi CONNECT Executing on core %i\n", xPortGetCoreID());
      for(;;) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        if (WiFi.status() == WL_CONNECTED) {
          continue;
        }
        Serial.print("WiFi connection lost\n");
        WiFi.reconnect();
      }
      vTaskDelete( NULL );
    }









}
