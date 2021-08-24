#ifndef _EVSE_UI_H_
#define _EVSE_UI_H_

//#include <EVSE.h>
#include <TFT_eSPI.h>
#include <SimpleButton.h>
#include "Config.h"
#include "Status.h"


namespace EVSE {
  class UI {
    public:
      /**
       * @brief Construct a new UI object
       * 
       * @param encoder_data 
       * @param encoder_clock 
       * @param encoder_switch 
       */
      UI(int encoder_data, int encoder_clock, int encoder_switch);
      ~UI();

      void begin();
      void handle();


      enum pages {UNKNOWN, BOOT, HOME, DETAILS};
      pages page = BOOT;
      pages page_last = BOOT; 
      
    private:
      
      TFT_eSPI display = TFT_eSPI();
      //Adafruit_SSD1306 display;

      EVSE::Config *config;
      EVSE::Status *status;
      void update();

      void _update_display();
      unsigned long _update_display_ts = 0;
      int display_refresh = 500;

      int cursor = 1;
      int cursor_last = -1;
      simplebutton::RotaryEncoder *encoder;

      void draw_icon(const unsigned char *icon, int x, int y, bool active, bool selected);
      void render_home_sliding_menu(int x, int y);

      


  };
}


#endif
