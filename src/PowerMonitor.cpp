//#include <EVSE.h>

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>


#include "PowerMonitor.h"
#include "Config.h"
#include "Status.h"
#include "MyLog.h"



namespace EVSE {
  PowerMonitor::PowerMonitor() {

  }

  PowerMonitor::~PowerMonitor() {

  }

  void PowerMonitor::begin() {
    xTaskCreatePinnedToCore(PowerMonitor::_begin, "PowerMonitor", 8192, NULL, 0, NULL, 0);
  }

  void PowerMonitor::_begin(void *pvParameters) {
    EVSE::Status *status = EVSE::Status::getInstance();
    EVSE::Config *config = EVSE::Config::getInstance();
    l_info("Start TASK POWER MONITOR");
    
    int i = 0;
    int power_house = 0;
    int power_solar = 0;
    int power_grid = 0;

    for(;;) {
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("PowerMonitor OFFLINE");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        continue;
      }
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      HTTPClient http;
      http.begin(config->power_meter_url);
      int httpResponseCode = http.GET();
      if (httpResponseCode != 200) {
        l_error("Error code: %i\n", httpResponseCode);
        http.end();
        continue;
      }

      //Serial.printf("HTTP Response code: %i\n", httpResponseCode);
      String input = http.getString();

      StaticJsonDocument<0> filter;
      filter.set(true);

      StaticJsonDocument<4192> doc;

      DeserializationError error = deserializeJson(doc, input, DeserializationOption::Filter(filter));

      if (error) {
        l_error("deserializeJson() failed: %s", error.c_str());
        http.end();
        continue;
      }

      i = 0;
      power_house = 0;
      power_solar = 0;
      power_grid = 0;
      // 0 house, 1 solar, 2 grid = house+solar
      for (JsonObject elem : doc["emeters"].as<JsonArray>()) {
        float power = elem["power"]; // 2290.4, 0
        //float reactive = elem["reactive"]; // -799.35, 0
        //float voltage = elem["voltage"]; // 231.1, 231.1
        //bool is_valid = elem["is_valid"]; // true, true
        //float total = elem["total"]; // 393529.7, 1086.8
        //float total_returned = elem["total_returned"]; // 716.1, 442396
        //Serial.printf("Power[%i] %f \n", i, power);
        if (i == 0) {
          power_house = int(power);
        } else if (i == 1) {
          power_solar = int(power);
        }
        i++;
      }
      
      power_grid = power_house + power_solar;

      status->set_power_buffer(power_house, Status::POWER_METER_HOUSE);
      status->set_power_buffer(power_solar, Status::POWER_METER_SOLAR);
      status->set_power_buffer(power_grid, Status::POWER_METER_GRID);

    }
    vTaskDelete( NULL );
  }
}
