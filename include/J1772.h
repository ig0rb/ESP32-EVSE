#ifndef _EVSE_J1772_H_
#define _EVSE_J1772_H_

//#include <EVSE.h>
#include <Arduino.h>
#include "Config.h"
#include "Status.h"


namespace EVSE {

  typedef struct {
    unsigned long ts_start;
    unsigned long ts_stop;
    unsigned long ts;
    int max_millivolt = 0;
    int max_millivolt_controlpilot = 0;
    int cur = 0;
    int max = 0;
    int min = 0;
    int loop = 0;
  } adc_t;

  class J1772 {
    public:
      enum states {STANDBY, VEHICLE_DETECTED, READY, VENTILATION, NOPOWER, ERROR};
      states state = STANDBY;
      
      /**
       * @brief Get the Instance object
       * @attention must be called AFTER setInstance
       * 
       * @return J1772* 
       */
      static J1772* getInstance();

      /**
       * @brief Set the Instance object
       * 
       * @param gpio_control_pilot_pwm pin del control pilot
       * @param gpio_control_pilot_adc pin del adc per la lattura del control pilot
       * @param gpio_ac_relay pin del relay 
       * @return J1772* 
       */
      static J1772* setInstance(int gpio_control_pilot_pwm, int gpio_control_pilot_adc, int gpio_ac_relay);

      /**
       * @brief inizializza l'hardware ed imposta l'EVSE in stadby
       * 
       */
      void begin();


      const char* print_state();
      const char* print_state(states _state);
      int get_ac_relay();


      
      void handle();



      adc_t adc;

      void set_power(int power);

      int pwm = 0;
      int duty_8bit = 0;
      bool pause = true; 
      unsigned long pause_ts = 0;


    private:
      static J1772* instance;

      int _gpio_control_pilot_adc;
      int _gpio_control_pilot_pwm;
      int _gpio_ac_relay;



      //J1772(int gpio_control_pilot_pwm, int gpio_control_pilot_adc, int gpio_ac_relay);
      J1772();
      ~J1772();


      void relay_on();
      void relay_off();
      void control_pilot_on();
      void control_pilot_off();


      void control_pilot_read();
      
      states adc_state = STANDBY;



      EVSE::Status *status;
      EVSE::Config *config;

  };
}

#endif
