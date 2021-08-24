//#include <EVSE.h>

#include <TFT_eSPI.h>
#include <SimpleButton.h>
#include <WiFi.h>

#include "UI.h"
#include "icons.h"
#include "Status.h"

#define TFT_GRAY 0xBDF7


using namespace simplebutton;


namespace EVSE {


  UI::UI(int encoder_data, int encoder_clock, int encoder_switch) {
    encoder = new RotaryEncoder(encoder_data, encoder_clock, encoder_switch);
  }

  UI::~UI() {

  }



  void UI::begin() {
    status = EVSE::Status::getInstance();
    config = EVSE::Config::getInstance();

    encoder->setPos(0);
    encoder->setInverted(true);
    encoder->setMin(-127);
    encoder->setMax(127);

    display.begin();
    display.init();
    display.setRotation(3);
    display.fillScreen(TFT_BLACK);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextSize(1);
    
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("BOOT...");
    
    display.drawBitmap(55, 32, icon_hourglass_start_49_64, 49, 64, TFT_WHITE, TFT_BLACK);
    page = HOME;
    page_last = BOOT;
    cursor = 1;

  }


  void UI::handle() {
    encoder->update();
    update();
  }

  void UI::update() {
    if ((millis() - _update_display_ts) > display_refresh) {
      _update_display();
      _update_display_ts = millis();
    }
  }


  void UI::_update_display() {
    
    int x = 0;
    int y = 0;
    char s[32];


    if (page != page_last) {
      display.fillScreen(TFT_BLACK);
      page_last = page;
    }

    
    display.setTextSize(1);
    display.setTextColor(TFT_WHITE, TFT_BLACK);


    display.setCursor(x, y);
    if (WiFi.status() == WL_CONNECTED) {
      display.printf("%s      ", status->wifi_ip_address());
      display.setCursor(display.width() - 24, 0);
      display.printf("%i  ", status->wifi_signal());
    } else {
      display.print("OFFLINE...                        ");
    }
   
    display.drawFastHLine(0, 10, 160, TFT_GREEN);
    
    y+=13;
    x = 0;
    display.setCursor(24, y);
    display.setTextSize(2);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    sprintf(s, "%s KW  ", status->get_power_char(EVSE::Status::POWER_METER_GRID));
    display.drawBitmap(x,y, ico_16x16_power ,16,16,TFT_BLACK, TFT_YELLOW);
    display.print(s);


    x = 6;
    y+= 22;
    render_home_sliding_menu(x,y);

    y+= 38;
    x = 10;
    display.setCursor(x, y);

    
    display.setTextSize(2);
    switch(status->state) {
      case Status::states_t::WAITING_POWER:
        display.setTextColor(TFT_YELLOW, TFT_BLACK);
        display.print("WAIT POWER");
        break;
      case Status::states_t::READY:
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.print("READY     ");
        break;
      case Status::states_t::PAUSE:
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.printf("PAUSE %i  ", int((status->pause_untill-millis())/1000));
        break;
      case Status::states_t::STANDBY:
        display.setTextColor(TFT_GREEN, TFT_BLACK);
        display.print("STANDBY   ");
        break;
      case Status::states_t::OFF:
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.print("OFF       ");
        break;
      case Status::states_t::RELOAD:
        display.setTextColor(TFT_WHITE, TFT_BLACK);
        display.print("reload    ");
        break;
      default:
        display.setTextColor(TFT_RED, TFT_BLACK);
        display.printf("BO %i    ", status->state);
    } 

    display.setTextSize(1);
    y+=18;
    x = 0;
    display.setCursor(x, y);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.printf("allocated %i   ", status->power_evse_allocated);


    if (encoder->incremented()) {
      cursor++;
    } else if (encoder->decremented()) {
      cursor--;
    } else {
      // do nothing...
    }
    


    

    y+=18;
    x = 0;
    display.setCursor(x, y);

    y+=16;
    x = 0;
    display.setCursor(x, y);

    
  }
  


  void UI::draw_icon(const unsigned char *icon, int x, int y, bool active, bool selected) {
    uint32_t color = TFT_WHITE;
    uint8_t size_x = 32;
    uint8_t size_y = 32;

    if (icon == ico_32x32_eco) {
      color = TFT_GREEN;
    } else if (icon == ico_32x32_solar) {
      color = TFT_YELLOW;
    } else if (icon == ico_32x32_turtle) {
      color = TFT_SKYBLUE;
    } else if (icon == ico_32x32_rocket) {
      color = TFT_BLUE;
    } else if (icon == ico_32x32_pause) {
      color = TFT_RED;
    } else if (icon == ico_32x32_info) {
      color = TFT_NAVY;
    } else if (icon == ico_32x32_restart) {
      color = TFT_PURPLE;
    } else if (icon == ico_32x32_save) {
      color = TFT_SILVER;
    }

    if (!active) {
      color = TFT_GRAY;
    }

    display.drawBitmap(x,y, icon ,32,32,TFT_BLACK, color);

    if (selected) {
      display.drawRect(x-2, y-2, size_x+4, size_y+4,  TFT_YELLOW);
    } else {
      display.drawRect(x-2, y-2, size_x+4, size_y+4, TFT_BLACK);
    }
  }


  void UI::render_home_sliding_menu(int x, int y) {
    
    int menu_items = 8;
    int menu_window = 4;
    int menu_item = 1;
    int menu_item_rendered = 0;
    bool render = true;
    
    bool clicked = encoder->clicked();

    // risposiziono il cursore all'inizio dopo l'ultimo item
    if (cursor > menu_items) {
      cursor = 1;
    }

    // evito il redraw se il cursore non è stato spostato
    if (cursor == cursor_last) {
      render = false;
    }
    cursor_last = cursor;

    

    // pause
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_pause, x, y, 
        status->get_evse_mode().mode_i == EvseModes::OFF ? true : false,
        cursor == menu_item ? true : false
      );
      x += 36;
      menu_item_rendered++;

    }
    if (cursor == menu_item && clicked) {
      status->set_evse_mode(EvseModes::OFF);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // eco
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
       draw_icon(ico_32x32_eco, x, y, 
          status->get_evse_mode().mode_i == EvseModes::ECO ? true : false,
          cursor == menu_item ? true : false
       );
       x+=36;
       menu_item_rendered++;
    }
    if (cursor == menu_item && clicked) {
      status->set_evse_mode(EvseModes::ECO);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // solar_max
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_solar, x, y, 
          status->get_evse_mode().mode_i == EvseModes::SOLAR ? true : false,
        cursor == menu_item ? true : false
      );
      x += 36;
      menu_item_rendered++;
    }
    if (cursor == menu_item && clicked) {
      status->set_evse_mode(EvseModes::SOLAR);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // slow
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_turtle, x, y, 
          status->get_evse_mode().mode_i == EvseModes::SLOW ? true : false,
        cursor == menu_item ? true : false
      );
      x += 36;
      menu_item_rendered++;

    }
    if (cursor == menu_item && clicked) {
      status->set_evse_mode(EvseModes::SLOW);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // fast
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_rocket, x, y, 
        status->get_evse_mode().mode_i == EvseModes::FAST ? true : false,
        cursor == menu_item ? true : false
      );
      x += 36;
      menu_item_rendered++;
    }
    if (cursor == menu_item && clicked) {
      status->set_evse_mode(EvseModes::FAST);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // save
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_save, x, y, 
        true,
        cursor == menu_item ? true : false
      );
      x += 36;
      menu_item_rendered++;
    }
    if (cursor == menu_item && clicked) {
      //status->set_evse_mode(Status::evse_modes_t::OFF);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // info
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_info, x, y, 
        true,
        cursor == menu_item ? true : false
      );
      x+=36;
      menu_item_rendered++;

    }
    if (cursor == menu_item && clicked) {
      //status->set_evse_mode(Status::evse_modes_t::OFF);
      cursor_last = -1;
      return;
    }
    menu_item++;

    // reboot
    if (cursor < menu_item+menu_window && menu_item_rendered < menu_window && render) {
      draw_icon(ico_32x32_restart, x, y, 
        true,
        cursor == menu_item ? true : false
      );
      x += 36;
      menu_item_rendered++;

    }
    if (cursor == menu_item && clicked) {
      //status->set_evse_mode(Status::evse_modes_t::OFF);
      cursor_last = -1;
      return;
    }
    menu_item++;

  }

}
