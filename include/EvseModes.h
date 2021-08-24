#ifndef _EVSE_MODES_H_
#define _EVSE_MODES_H_


namespace EVSE {
  /**
   * @brief profilo di un modo di funzionamento
   * 
   */
  class EvseMode {
    public: 
    /**
     * @brief Construct a new profilo di funzionamento
     * 
     * @param mode_i char del modo di funzionamento
     * @param mode_c integer del modo di funzionamento
     * @param parameter-name description
     * @param evse_max_power massima potenza usabile dall'evse
     * @param grid_max_power massima potenza assorbile dalla rete
     * @param hysteresis_on  tempo in secondi da attendere dopo una pausa per sovraconsumo dalla rete
     * @param hysteresis_off tempo in secondi da attendere prima di mettere in pausa per sovra consumo dalla rete
     */

      EvseMode(const char* mode_c, int mode_i, int *evse_max_power, int *grid_max_power, int *hysteresis_on, int *hysteresis_off);
      ~EvseMode();

      int *evse_max_power;
      int *grid_max_power;
      int *grid_tolerance;
      int *hysteresis_on;
      int *hysteresis_off;

      int mode_i;
      char mode_c[32];
      

  };


  class EvseModes {
    public:
      EvseModes();
      ~EvseModes();
      int zero = 0;
      int cinque = 0;
      enum modes_t {OFF = 0, ECO = 1, SOLAR = 2, SLOW = 3, FAST = 4};

      EvseMode *modes[5];    // Profili dei modi di funzionamento
      EvseMode *mode;       // Profilo del modo di funzionamento corrente

      
      void init();

    private:

  };
}

#endif