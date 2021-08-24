//#include <EVSE.h>

#include <Arduino.h>
#include <SPIFFS.h>
#include <SPIFFSEditor.h>

#include <AsyncJson.h>

#include "WebServer.h"
#include "Config.h"
#include "Status.h"
#include "J1772.h"



namespace EVSE {

  WebServer::WebServer() {

  }

  WebServer::~WebServer() {

  }

  void WebServer::begin() {
    server.on("/", HTTP_GET, index);
    server.on("/", HTTP_POST, index_post);
    server.on("/status", HTTP_GET, status);
    server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request){
      Status *status = Status::getInstance();
      request->send(200, "text/plain", String("reboot ok"));   
      status->reboot = true;
    });
    server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
      Status *status = Status::getInstance();
      Config *config = Config::getInstance();
      config->reset();
      request->send(200, "text/plain", String("reset ok"));   
      status->reboot = true;
    });

    

    server.on("/system", HTTP_GET, system);

    server.addHandler(new SPIFFSEditor(SPIFFS));

    server.onNotFound(error_404);
    server.begin();
  }


  void WebServer::status(AsyncWebServerRequest *request) {
    EVSE::Status *status = EVSE::Status::getInstance();
    EVSE::J1772 *j1772 = EVSE::J1772::getInstance();
    
    AsyncJsonResponse *response = new AsyncJsonResponse();
    
    JsonObject root = response->getRoot();

    root["power_grid"] = String(status->get_power(Status::POWER_METER_GRID));
    root["power_house"] = String(status->get_power(Status::POWER_METER_HOUSE));
    root["power_solar"] = String(status->get_power(Status::POWER_METER_SOLAR));
    root["duty_8bit"] = String(j1772->duty_8bit);
    root["pause"] = String(j1772->pause);
    root["pause_ts"] = String(j1772->pause_ts);
    root["pwm"] = String(j1772->pwm);
    root["adc_max"] = String(j1772->adc.max);
    root["adc_max_millivolt"] = String(j1772->adc.max_millivolt);
    root["adc_max_millivolt_controlpilot"] = String(j1772->adc.max_millivolt_controlpilot);

    root["evse_mode"] = String(status->get_evse_mode().mode_c);

    root["relay"] = String(j1772->get_ac_relay());
    root["J1772"] = String(j1772->print_state());

    root["grid_power"] = String(status->get_power_avg(Status::POWER_METER_GRID, 1));
    root["grid_power_60"] = String(status->get_power_avg(Status::POWER_METER_GRID, 60));
    root["grid_power_600"] = String(status->get_power_avg(Status::POWER_METER_GRID, 600));

     
     

    response->setLength();
    request->send(response);
  }

  void WebServer::system(AsyncWebServerRequest *request) {
    AsyncJsonResponse *response = new AsyncJsonResponse();
    JsonObject root = response->getRoot();

    // Hardware
    root["model"] = String(ESP.getChipModel());
    root["cores"] = String(ESP.getChipCores());
    root["frequency"] = String(ESP.getCpuFreqMHz());
    root["revision"] = String(ESP.getChipRevision());
    // Flash
    root["flash_chip_mode"] = String(ESP.getFlashChipMode());
    root["flash_chip_size"] = String(ESP.getFlashChipSize());
    root["flash_chip_speed"] = String(ESP.getFlashChipSpeed());
    // HEAP
    root["heap_size"] = String(ESP.getHeapSize());
    root["heap_free_min"] = String(ESP.getMinFreeHeap());
    root["heap_used_max"] = String(ESP.getMaxAllocHeap());
    // PSRAM
    root["psram_size"] = String(ESP.getPsramSize());
    root["psram_free_min"] = String(ESP.getMinFreePsram());
    root["psram_used_max"] = String(ESP.getMaxAllocPsram());
    // Program 
    root["sdk_version"] = String(ESP.getSdkVersion());
    root["app_size"] = String(ESP.getSketchSize());
    root["app_hash"] = String(ESP.getSketchMD5());
    
    //response->setLength();
    char buffer[1024];
    serializeJsonPretty(root, buffer);
    request->send(200, "text/plain", String(buffer));   



  }

  /*
  void WebServer::config_get(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/config.json", "text/txt");
  }

  void WebServer::config_put(AsyncWebServerRequest *request) {
    
  }
  */

  void WebServer::index_post(AsyncWebServerRequest *request) {
    EVSE::Config *config = EVSE::Config::getInstance();

    /*
    if (request->hasParam("power_max_grid", true)) {
      config->power_max_grid = request->getParam("power_max_grid", true)->value().toInt();
    }
    */
   /*
    if (request->hasParam("power_max_evse", true)) {
      config->power_max_evse = request->getParam("power_max_evse", true)->value().toInt();
    }
*/
    if (request->hasParam("power_capacity_grid", true)) {
      config->power_capacity_grid = request->getParam("power_capacity_grid", true)->value().toInt();
    }

    /*
    if (request->hasParam("power_capacity_evse", true)) {
      config->power_capacity_evse = request->getParam("power_capacity_evse", true)->value().toInt();
    }
    */
    if (request->hasParam("mode_eco_evse_power_limit", true)) {
      config->mode_eco_evse_power_limit = request->getParam("mode_eco_evse_power_limit", true)->value().toInt();
    }
    if (request->hasParam("mode_solar_max_evse_power_limit", true)) {
      config->mode_eco_evse_power_limit = request->getParam("mode_solar_max_evse_power_limit", true)->value().toInt();
    }
    if (request->hasParam("mode_slow_evse_power_limit", true)) {
      config->mode_slow_evse_power_limit = request->getParam("mode_slow_evse_power_limit", true)->value().toInt();
    }
    if (request->hasParam("mode_slow_evse_power_limit", true)) {
      config->mode_slow_evse_power_limit = request->getParam("mode_slow_evse_power_limit", true)->value().toInt();
    }
    if (request->hasParam("mode_fast_evse_power_limit", true)) {
      config->mode_fast_evse_power_limit = request->getParam("mode_fast_evse_power_limit", true)->value().toInt();
    }
    /*
    if (request->hasParam("mode_manual_evse_power_limit", true)) {
      config->mode_manual_evse_power_limit = request->getParam("mode_manual_evse_power_limit", true)->value().toInt();
    }
    */
    if (request->hasParam("mode_evse_default", true)) {
      config->mode_evse_default = request->getParam("mode_evse_default", true)->value().toInt();
    }

    /*
    if (request->hasParam("dev_mode", true)) {
      config->dev_mode = request->getParam("dev_mode", true)->value().toInt();
    }
    */
    

    
    /*
    config->power_meter_ignore = false;
    if (request->hasParam("power_meter_ignore", true)) {
      config->power_meter_ignore = true;
    }
    */
   
    config->save();
    WebServer::index(request);



  }


  void WebServer::index(AsyncWebServerRequest *request) {
    //request->send(200, "text/plain", "ciaooone");
    request->send(SPIFFS, "/index.html", String(), false, [](const String &var) {
      EVSE::Config *config = EVSE::Config::getInstance();

      /*
      if (var == "POWER_MAX_GRID") {
        return String(config->power_max_grid);
      }
      */
     /*
      if (var == "POWER_MAX_EVSE") {
        return String(config->power_max_evse);
      }
*/

      if (var == "POWER_CAPACITY_GRID") {
        return String(config->power_capacity_grid);
      }

      /*
      if (var == "POWER_CAPACITY_EVSE") {
        return String(config->power_capacity_evse);
      }
      */

      if (var == "MODE_ECO_EVSE_POWER_LIMIT") {
        return String(config->mode_eco_evse_power_limit);
      }
      if (var == "MODE_SOLAR_MAX_EVSE_POWER_LIMIT") {
        return String(config->mode_solar_evse_power_limit);
      }
      if (var == "MODE_SLOW_EVSE_POWER_LIMIT") {
        return String(config->mode_slow_evse_power_limit);
      }
      if (var == "MODE_FAST_EVSE_POWER_LIMIT") {
        return String(config->mode_fast_evse_power_limit);
      }
      /*
      if (var == "MODE_MANUAL_EVSE_POWER_LIMIT") {
        return String(config->mode_manual_evse_power_limit);
      }
      */
      if (var == "EVSE_MODE_OFF") {
        return String(EvseModes::OFF);
      }
      /*
      if (var == "EVSE_MODE_LAST") {
        return String(EVSE::Status::evse_mode_to_int(EVSE::Status::evse_modes_t::LAST));
      }
      */
      if (var == "EVSE_MODE_ECO") {
        return String(EvseModes::ECO);
      }
      if (var == "EVSE_MODE_SOLAR_MAX") {
        return String(EvseModes::SOLAR);
      }
      if (var == "EVSE_MODE_SLOW") {
        return String(EvseModes::SLOW);
      }
      if (var == "EVSE_MODE_FAST") {
        return String(EvseModes::FAST);
      }
      /*
      if (var == "EVSE_MODE_MANUAL") {
        return String(EVSE::Status::evse_mode_to_int(EVSE::Status::evse_modes_t::MANUAL));
      }
      */
      if (var == "EVSE_MODE_OFF_SELECTED") {
        if (config->mode_evse_default == EvseModes::OFF) {
          return String("selected");
        }
      }
      if (var == "EVSE_MODE_ECO_SELECTED") {
        if (config->mode_evse_default == EvseModes::ECO) {
          return String("selected");
        }
      }
      if (var == "EVSE_MODE_SOLAR_MAX_SELECTED") {
        if (config->mode_evse_default == EvseModes::SOLAR) {
          return String("selected");
        }
      }
      if (var == "EVSE_MODE_SLOW_SELECTED") {
        if (config->mode_evse_default == EvseModes::SLOW) {
          return String("selected");
        }
      }
      if (var == "EVSE_MODE_FAST_SELECTED") {
        if (config->mode_evse_default == EvseModes::FAST) {
          return String("selected");
        }
      }
      /*
      if (var == "EVSE_MODE_MANUAL_SELECTED") {
        if (config->mode_evse_default == EvseModes::MANUAL)) {
          return String("selected");
        }
      }
      */

      /*
      if (var == "DEV_MODE_SELECTED_ON") {
        if (config->dev_mode == 1) {
          return String("selected");
        }
      }

      if (var == "DEV_MODE_SELECTED_OFF") {
        if (config->dev_mode == 0) {
          return String("selected");
        }
      }
      */





      return String();

    });
  }

  void WebServer::error_404(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
  }

};