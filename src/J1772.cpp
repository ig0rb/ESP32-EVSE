//#include <EVSE.h>

#include "J1772.h"
#include "MyLog.h"

namespace EVSE {

  J1772* J1772::instance = 0;

  J1772* J1772::getInstance() {
    if (instance == 0) {
      l_fatal("J1772 NOT inizialized RESTART");
      log_e("J1772 NOT inizialized RESTART");
      ESP.restart();
    }
    return instance;
  }

  
  J1772* J1772::setInstance(int gpio_control_pilot_pwm, int gpio_control_pilot_adc, int gpio_ac_relay) {
    if (instance != 0) {
      l_fatal("J1772 already inizialized RESTART");
      log_e("J1772 already inizialized RESTART");
      ESP.restart();
    }
    instance = new J1772();
    instance->_gpio_control_pilot_pwm = gpio_control_pilot_pwm;
    instance->_gpio_control_pilot_adc = gpio_control_pilot_adc;
    instance->_gpio_ac_relay = gpio_ac_relay;
    return instance;
  }
  

  J1772::J1772() {

  }

  void J1772::begin() {

    status = EVSE::Status::getInstance();
    config = EVSE::Config::getInstance();
    
    ledcSetup(0, 1000, 8);

    // Imposto CONTROL_PILOT a +12V
    pinMode(_gpio_control_pilot_pwm, OUTPUT);
    digitalWrite(_gpio_control_pilot_pwm, HIGH);

    // Spengo il relay AC
    pinMode(_gpio_ac_relay, OUTPUT);
    digitalWrite(_gpio_ac_relay, LOW);

    // Imposto l'ADC sul CONTROL_PILOT
    analogReadResolution(9);
    adcAttachPin(_gpio_control_pilot_adc);

    pause = true;


  }

  J1772::~J1772() {

  }

  void J1772::set_power(int power) {
    float watt = float(power);
    float ampere = watt/230;
    ampere = (ampere > 30) ? 30 : ampere;
    //ampere = (ampere < 6) ? 6 : ampere;
    float duty = ampere/0.6;
    int _duty_8bit = int(255*duty/100);
    _duty_8bit = (_duty_8bit < 25) ? 0 : _duty_8bit;

    if (power < 1000) {
      Serial.printf("set_power %i _duty %i duty %i\n", power, _duty_8bit, duty_8bit);
    }

    if (_duty_8bit == duty_8bit) {
      return;
    }
    
    if (pause == false && _duty_8bit < 25) {
      duty_8bit = _duty_8bit;
      pwm = 25;
      ledcWrite(0, pwm);
      control_pilot_off();
      pause = true;
      pause_ts = millis();
      return;
    } 
    
    if (pause == true && _duty_8bit >= 25) {
      duty_8bit = _duty_8bit;
      pwm = duty_8bit;
      ledcWrite(0, pwm);
      control_pilot_on();
      pause = false;
      pause_ts = millis();
      return;      
    }
    
    // Se sono qua vuol dire che sto lavorando e devo sollo aggiornare il PWM
    duty_8bit = _duty_8bit;
    pwm = duty_8bit;
    ledcWrite(0, pwm);
  }

  void J1772::relay_on() {

    digitalWrite(_gpio_ac_relay, HIGH);
  }

  void J1772::relay_off() {
    digitalWrite(_gpio_ac_relay, LOW);
  }

  void J1772::control_pilot_on() {
    ledcSetup(0, 1000, 8);
    ledcAttachPin(_gpio_control_pilot_pwm, 0);
    ledcWrite(0, pwm);
  }

  void J1772::control_pilot_off() {
    ledcSetup(0, 1000, 8);
    ledcDetachPin(_gpio_control_pilot_pwm);
    pinMode(_gpio_control_pilot_pwm, OUTPUT);
    digitalWrite(_gpio_control_pilot_pwm, HIGH);
  }

  int J1772::get_ac_relay() {
    return digitalRead(_gpio_ac_relay);
  }

  void J1772::handle() {
    control_pilot_read();

    if (adc_state == J1772::STANDBY) {
      if (state != J1772::STANDBY) {
        Serial.printf("STATO AA1 J1772_state: %s  adc_state: %s adc_max %i\n", print_state(), print_state(adc_state), adc.max);
        state = J1772::STANDBY;
        relay_off();
      }
      return;
    }

    if (adc_state == J1772::VEHICLE_DETECTED) {
      if (state != J1772::VEHICLE_DETECTED) {
        state = J1772::VEHICLE_DETECTED;
        relay_off();
        Serial.printf("STATO AA2 J1772_state: %s  adc_state: %s adc_max %i\n", print_state(), print_state(adc_state), adc.max);
        //relay_on();
      }
      return;
    }

    if (adc_state == J1772::READY || adc_state == J1772::VENTILATION) {
      if (state != J1772::READY && state != J1772::VENTILATION) {
        state = adc_state;
        Serial.printf("STATO AA3 J1772_state: %s  adc_state: %s adc_max %i\n", print_state(), print_state(adc_state), adc.max);
        relay_on();
      }
      return;
    }

    // Non dovrei mai arrivare qua...
    
    Serial.printf("STATO BOOO J1772: %s  adc: %s\n", print_state(), print_state(adc_state));
    Serial.printf("ADC %i %i %i - loop %i - ts %li %li %li\n", adc.cur, adc.min, adc.max, adc.loop, adc.ts, adc.ts_start, adc.ts_stop);
    //sleep(1000);
    delay(250);
    /*
    if (adc_state == J1772::ERROR) {
      relay_off();
      control_pilot_off();
    }
    */
    //relay_off();

 }

  void J1772::control_pilot_read() {
    unsigned long ts_now = micros();
    adc.ts_start = ts_now;
    int loop = 0;
    int min = 0;
    int max = 0;
    int cur = 0;
    while ((micros() - ts_now) < 1250) {
      cur = analogRead(_gpio_control_pilot_adc);
      if (cur > max) {
        max = cur;
      }

      if (min > cur) {
        min = cur;
      }
      loop++;
    }
    adc.ts_stop = micros();
    adc.ts = adc.ts_stop - adc.ts_start;
    adc.cur = cur;
    adc.min = min;
    adc.max = max;
    adc.loop = loop;

    float millivolt = 3.3/511*adc.max*1000;
    float millivolt_controlpilot = (millivolt * 24/3.3) - 12000;
    adc.max_millivolt = int(millivolt);
    adc.max_millivolt_controlpilot = int(millivolt_controlpilot);

    if (adc.max > 475) {
      adc_state = J1772::STANDBY;
    } else if (adc.max > 410) {
      adc_state = J1772::VEHICLE_DETECTED;
    } else if (adc.max > 350) {
      adc_state = J1772::READY;
    } else if (adc.max > 310) {
      adc_state = J1772::VENTILATION;
    } else {
      adc_state = J1772::ERROR;
    }


  }

  const char* J1772::print_state() {
    return print_state(state);
  }

  const char* J1772::print_state(states _state) {
    switch(_state) {
      case J1772::READY:
        return "READY";
      case J1772::VEHICLE_DETECTED:
        return "VEHICLE_DETECTED";
      case J1772::STANDBY:
        return "STANDBY";
      case J1772::VENTILATION:
        return "VENTILATION";
      case J1772::NOPOWER:
        return "NOPOWER";
      case J1772::ERROR:
        return "ERROR";
      default:
        return "UNKNOWN";
    }
  }


}
