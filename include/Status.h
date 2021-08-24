#ifndef _EVSE_STATUS_H
#define _EVSE_STATUS_H

//#include <EVSE.h>

#include "Arduino.h"
#include "Config.h"
#include "EvseModes.h"


namespace EVSE {
  /**
   * @brief struttura per la lettura delle potenze di rete
   * 
   */
  typedef struct {
    int watt = 0;           /*! lettura corrente potenza in watt */
    int watt_last = 0;      /*! ultima lettura valida in watt */
    int ring_buffer[120];    /*! array delle letture da usare come buffer circolare, per calcolare le medie */
    int ring_pointer = 0;   /*! posizione dell'ultima lettura inserita */
    boolean ready = false;  /*! il buffer di tutte le letture è pieno */
    unsigned long ts = 0;   /*! timestamp dell'ultima lettura */
  } power_meter_t;


  
/**
 * @brief Classe Singleton per la gestione dello stato del sistema
 * 
 */
  class Status {
  public:
    /**
     * @brief Get the Instance object
     * 
     * @return Status* 
     */
    static Status* getInstance();

    /**
     * @brief ritorna il livello del segnale WiFi
     * 
     * @return int livello del segnale WiFi
     */
    int wifi_signal();

    /**
     * @brief ritorna l'indirizzo ip del dispositivo
     * 
     * @return const char* indirizzo ip
     */
    const char* wifi_ip_address();

    /**
     * @brief struttura per il salvataggio dell'ora del sistema
     * 
     */
    struct tm localtime;

    /**
     * @brief definizione dei vari power meters installati, per il funzionamento viene usato solo il POWER_METER_GRID
     * 
     */
    enum power_meters_t {
      POWER_METER_SOLAR, // produzione fotovoltaica
      POWER_METER_HOUSE, // consumo della casa
      POWER_METER_GRID,  // consumo dalla rete
      POWER_METER_EVSE   // consumo dell'evse
    };
    power_meter_t power_meter[4];


 
    bool reboot = false;              // è stato richiesto il reboot... 
    



    /**
     * @brief imposta il modo di funzionamento
     * 
     * @param mode 
     * @return evse_modes_t 
     */
    //evse_modes_t set_evse_mode(evse_modes_t mode);

    /**
     * @brief ritorna il modo di funzionamento corrente
     * 
     * @return evse_modes_t 
     */
    //evse_modes_t get_evse_mode();
    

    EvseModes *evse_modes;
    EvseMode get_evse_mode();
    EvseMode set_evse_mode(EvseModes::modes_t mode);

    

    

    /**
     * @brief 
     * Stati di funzionamento del sistema
     */
    enum states_t {
      ERROR,          // Errore, il sistema deve togliere il PWM, spegnere il rele AC
      BOOTING,        // Il sistema è in fase di avvio
      RELOAD,         // Viene ricaricata la macchina a stati
      WAITING_POWER,  // Il sistema sta aspettando di avere potenza disponibile
      STANDBY,        // Il sistema è pronto ed aspetta che l'auto sia connessa e e richieda potenza
      READY,          // Il sistema sta caricando (od almeno l'auto chiede potenza)
      PAUSE,           // Il sistema è in pausa per mancanza di potenza disponibile
      OFF            // Non voglio caricare
    };
    states_t state = BOOTING;

    /**
     * @brief ritorna la potenza istantanea in formato char
     * 
     * @param type misuratore
     * @return char* potenza istantanea
     */
    char* get_power_char(power_meters_t type);

    /**
     * @brief ritorna la potenza istantanea in formato in
     * 
     * @param type misuratore
     * @param valid puntatore ad un bool, se settato a true la misura è valida, se false la misura è vecchia
     * @return int  potenza istantanea
     */
    int get_power(power_meters_t type, boolean *valid = NULL);


    /**
     * @brief ritorna la potenzia media in watt
     * @attention nel caso implementare una cache???
     * 
     * @param type misuratore
     * @param seconds secondi di media (il buffer contiene 1 campione ogni 5 secondi)
     * @param valid puntatore ad un bool, se true il valore è valido se false non è valido
     * @param ts ultima lettura valida
     * @return int 
     */
    int get_power_avg(power_meters_t type, int seconds, boolean *valid = NULL, unsigned long *ts = NULL);


    /**
     * @brief salva la lettura della potenza istantanea del misuratore
     * 
     * @param value valore in watt
     * @param type misuratore
     */
    void set_power_buffer(int value, power_meters_t type);

    unsigned long pause_untill = 0;

    boolean power_meter_fail = false;
    boolean grid_over_power = false;
    boolean limit_over_power = false;


    int power_evse_allocated = 0;






  private:
    Status();
    static Status *instance;
    

    

    const unsigned long _power_meter_timeout = 30000;

    char _power_meter_buffer[8];

    char _buffer[64];

    /**
     * @brief inzializza il power_meter_t
     * 
     * @param p puntatore alla struttura
     */
    void power_meter_init(power_meter_t *p);





  };

};

#endif
