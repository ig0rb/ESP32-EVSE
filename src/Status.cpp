//#include <EVSE.h>

#include "Status.h"
#include "WiFi.h"
#include "MyLog.h"
#include "EvseModes.h"

namespace EVSE {

  Status* Status::instance = 0;


  Status* Status::getInstance() {
    if (instance == 0) {
      instance = new Status();

    }
    return instance;
  }

  int Status::wifi_signal() {
    return WiFi.RSSI();
  }

  const char* Status::wifi_ip_address() {
    strncpy(_buffer, WiFi.localIP().toString().c_str(), sizeof(_buffer));
    return _buffer;
  }


  void Status::set_power_buffer(int value, power_meters_t type) {
    power_meter_t *pm = &power_meter[type];
    if (pm == NULL) {
      return;
    }

    //l_debug("value %i pm %i %p ring_pointer %i pmready %i", value, type, pm, pm->ring_pointer, pm->ready);

    pm->watt = value;
    pm->ts = millis();

    pm->watt_last = pm->ring_buffer[pm->ring_pointer];
    pm->ring_pointer++;
    if (pm->ring_pointer > (sizeof(pm->ring_buffer)/sizeof(int))-1) {
      pm->ring_pointer = 0;
      pm->ready = true;
    }
    pm->ring_buffer[pm->ring_pointer] = value;
  }


  int Status::get_power(power_meters_t type, boolean *valid) {
    return get_power_avg(type, 1, valid);
  }


  int Status::get_power_avg(power_meters_t type, int seconds, boolean *valid, unsigned long *ts) {
    const int N = 5;
    int samples = 1;
    boolean _valid = false;
    power_meter_t *pm = &power_meter[type];

    _valid = pm->ready;

    if (seconds > N) {
      samples = seconds/N;
    } 

    if (pm->ring_pointer >= samples) {
      _valid = true;
    }

    if (_valid == false) {
      samples = pm->ring_pointer;
    }

    int i = samples;
    int p = pm->ring_pointer;
    long int sum = 0;
    while (i > 0) {
      sum += pm->ring_buffer[p];
      i--;
      p--;
      if (p <= 0) {
        p = sizeof(pm->ring_buffer)/sizeof(int);
      }
    }
    
    if (valid != NULL) {
      *valid = _valid;
    }

    if (ts != NULL) {
      *ts = pm->ts;
    }

    if (samples < 1) {
      return 0;
    }
    return int(sum/samples);
  /*
    // torna la potenza istantanea
    if (seconds <= 5) {
        seconds = 5;
    }

    power_meter_t *pm = &power_meter[type];
    
    long sum = 0;             // accumulatore per il calcolo della media
    int samples = seconds/5;  // numero di campioni da prendere nel buffer
    int idx_count = samples;  // contatore per i campioni mancanti nel ciclo
    int idx_start = pm->ring_pointer;   // valore iniziale del contatore
    int idx = idx_start;                // valore iniziale del contatore
    boolean _valid;                     // server per evitare un crash 

    // Se il buffer non è del tutto inizializzato riduco i campioni per il calcolo della media, e setto valid a false
    if (pm->ready == false) {
      if (samples <= pm->ring_pointer) {    // se anche il buffer non è pieno ma ho + campioni della finestra richiesta valid va a true
        _valid = true;
      } else {
        idx_count = pm->ring_pointer;
        samples = pm->ring_pointer;
        _valid = false;
      }
    } else {
      _valid = true;
    }

    

    if (valid != NULL) {
      *valid = _valid;
    }

    
    Serial.printf("seconds %i samples %i idx_count %i idx_start %i ring_pointer %i ready %i\n",
      seconds, samples, idx_count, idx_start, pm->ring_pointer, pm->ready
    );
    

    while (idx_count > 0) {
      sum += pm->ring_buffer[idx_count];
      idx_count--;
      if (idx < 0) {
        idx = (sizeof(pm->ring_buffer)/sizeof(int)) -1;
      }
    }

    if (sum == 0 || samples == 0) {
      return 0;
    }

    return int(sum/samples);
    */
  }





  char* Status::get_power_char(power_meters_t type) {
    boolean valid;
    float watt;
    
    watt = static_cast<float>(get_power_avg(type, 1, &valid));
    if (valid == true) {
      snprintf(_power_meter_buffer, sizeof(_power_meter_buffer), "%0.2f", (watt/1000));
    } else {
      strncpy(_power_meter_buffer, "n/a", sizeof(_power_meter_buffer));
    }

    return _power_meter_buffer;
  }

  EvseMode Status::get_evse_mode() {
    return *evse_modes->mode;
  }

  EvseMode Status::set_evse_mode(EvseModes::modes_t mode) {
    evse_modes->mode = evse_modes->modes[mode];
    state = Status::states_t::RELOAD;
    return *evse_modes->mode;
  }

  Status::Status() {
    power_meter_init(&power_meter[POWER_METER_SOLAR]);
    power_meter_init(&power_meter[POWER_METER_HOUSE]);
    power_meter_init(&power_meter[POWER_METER_GRID]);
    power_meter_init(&power_meter[POWER_METER_EVSE]);

    state = BOOTING;
    evse_modes = new EvseModes();
    evse_modes->mode = evse_modes->modes[EvseModes::OFF];
  }


  void Status::power_meter_init(power_meter_t *pm) {
    pm->ready = false;
    pm->ring_pointer = 0;
    pm->ts = 0;
    pm->watt = 0;
    pm->watt_last = 0;

    for (int i = 0; i < sizeof(pm->ring_buffer)/sizeof(int); i++) {
      pm->ring_buffer[i] = 0;
    }
  }






};
