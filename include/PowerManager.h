#ifndef _EVSE_POWERMANAGER_H_
#define _EVSE_POWERMANAGER_H_

#include <Arduino.h>
#include "Config.h"
#include "Status.h"
#include "J1772.h"

namespace EVSE {
  /**
   * @brief classe per la gestione della potenza allocabile e gli stati del sistema
   * 
   */
  class PowerManager {
    /*
    typedef struct {
      int buffer[60];
      int p;
      bool ready;
    } power_buffer_t;
    */

    public:
      PowerManager();
      ~PowerManager();

      void begin();

      /**
       * @brief è il methodo che deve essere eseguito ad ogni loop
       * 
       */
      void handle();
      unsigned long handle_last_ts = 0;   // timestamp ultima esecuzione del metodo handle


      unsigned long ts_on = 0;            // **eliminabili forse timestamp ultima attivazione PWM
      unsigned long ts_off = 0;           // **eliminabili forse timestamp ultima disattivazione PWM

      
    private:
      EVSE::Config *config;
      EVSE::Status *status;
      EVSE::J1772 *j1772;

      /**
       * @brief esegue tutti i controlli per allocare la potenza in base al profilo, gestire i sovraccarichi e mettere in pause il sistema
       * 
       */
      void power_tracker();
      unsigned long power_tracker_next_run_ts = 0;   // timestamp prossima esecuzione del power_tracker


      boolean overpower = false;              // everpower dalla rete
      unsigned long overpower_ts = 0;         // timestamp ultimo evento overpower
      
      boolean grid_over_target = false;       // over target di potenza dalla rete
      unsigned long grid_over_target_ts = 0;  // timestamp ultimo evento overpower del target

      
      
      

      /**
       * @brief imposta la potenza allocata per l'evse in valore assoluto in watt
       * 
       * @param power 
       */
      void set_evse_power(int power);
      int evse_power = 0;   // potenza attualmente allocata per l'evse


      /**
       * @brief diminuisce la potenza allocata per l'evse
       * 
       * @param value valore di potenza da decrementare
       * @return int ritorna 1 se la potenza è stata aumentata o 0 se non è possibile aumentarla
       */
      int set_evse_down(int value = 100);
      unsigned long set_evse_down_ts = 0;   // timestamp ultima operazione di decremento potenza evse

      /**
       * @brief incrementa la potenza allocata per l'evse
       * 
       * @param value potenza in watt da incrementare
       * @param max potenza massima impostabile
       * @return int ritorna 1 se la potenza è stata aumentata o 0 se non è possibile aumentarla
       */
      int set_evse_up(int value = 100, int max = 1500);
      unsigned long set_evse_up_ts = 0;   // timestamp ultima operazione di incremento potenza evse





  };
}

#endif