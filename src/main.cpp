

//#include <EVSE.h>
#include <Arduino.h>
#include <freertos/task.h>
#include <Preferences.h>

#include "Config.h"
#include "Status.h"
#include "Networking.h"
#include "WebServer.h"
#include "PowerManager.h"
#include "UI.h"
#include "OTA.h"
#include "PowerMonitor.h"
#include "J1772.h"

#define CONFIG_ASYNC_TCP_RUNNING_CORE 0

#define GPIO_CONTROL_PILOT_PWM 16
#define GPIO_CONTROL_PILOT_ADC 34
#define GPIO_AC_RELAY 12
//#define GPIO_I2C_SCL 22
//#define GPIO_I2C_SDA 21
#define GPIO_ENCODER_DATA 15
#define GPIO_ENCODER_CLK 13
#define GPIO_ENCODER_SW 27
#define GPIO_TFT_RST 14
#define GPIO_TFT_CS 17
#define GPIO_TFT_DC 2
#define GPIO_TFT_MOSI 23
#define GPIO_TFT_SCLK 18
#define GPIO_SD_CS 5


EVSE::Networking networking;
EVSE::OTA ota;
EVSE::UI ui(GPIO_ENCODER_DATA, GPIO_ENCODER_CLK, GPIO_ENCODER_SW);
EVSE::Config *config;
EVSE::Status *status;
EVSE::PowerMonitor powermonitor;
EVSE::J1772 *j1772;
EVSE::WebServer webserver;
EVSE::PowerManager powermanager;

#include <WiFi.h>
#include <MyLog.h>



void setup() {
  
  Serial.begin(115200);

  Preferences preferences;

  status = EVSE::Status::getInstance();
  config = EVSE::Config::getInstance();

  j1772 = EVSE::J1772::setInstance(GPIO_CONTROL_PILOT_PWM, GPIO_CONTROL_PILOT_ADC, GPIO_AC_RELAY);
  j1772->begin();

  ui.begin();

  networking.begin(); // Handle non necessario, gira in background
  MyLog::server(config->syslog);

  ota.begin();
  powermonitor.begin(); // Handle non necessario, gira in background
  SPIFFS.begin();
  webserver.begin();
  networking.ntp();
  networking.mdns();
  powermanager.begin();

  l_notice("EVSE started %s", status->wifi_ip_address());
}

unsigned long ts = 0;
void loop() {
  if ((millis() - ts) > 5000) {
    Serial.println(ts);
    ts = millis();
  }
 
 
  if (status->reboot) {
    //log_i("Rebooting...\n");
    delay(2000);
    ESP.restart();
  }

  

  ota.handle();
  ui.handle();

  
  powermanager.handle();
  j1772->handle();

  //yield();

}
