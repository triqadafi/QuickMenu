//
// QuickMenu Library by @triqadafi
// TQDF is a shorthand of triqadafi
// 2022 (C) www.triqada.fi
//
//
// Big thanks to the source of inspiration: 
//    https://www.semesin.com/project/2018/04/13/dinamik-menu-dan-submenu-dengan-keypad-dan-lcd-16x2-menggunakan-arduino/
//

#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

#ifndef TQDF_QuickMenu_h
#define TQDF_QuickMenu_h

#define MENULevel_MAX 2
#define LCDString_WIDTH 16+1
#define QuickMenu_Size(type) sizeof(type)/sizeof(type[0])

 
enum QMMode {
  UInt8,
  UInt16,
  textDropDown,
  subMenu,
  systemState
};
 
struct QMDropdown{
  char title[LCDString_WIDTH];
};

struct QMenu{
  char title[LCDString_WIDTH];
  char desc[LCDString_WIDTH];
  byte dataType;
  void *dataVariable;
  uint16_t valueMax;
  uint16_t valueMin;
  char *menuSub;
  byte param_a;
};
 
struct QMLevel
{
  byte index;
  QMenu *menu;
  byte menuLength;
  char *dropDown;
  byte dropDownLength;
};

/**
 * @brief test coba coba tentang brief
 * 
 * dibawahnya ini harusnya comment
 * 
 */
class QuickMenu
{
  public:
    /**
     * @brief Construct a new TQDF_QuickMenu object
     * 
     * @param _keypad keypad instance
     * @param _lcd_i2c LCD i2c instance
     * @param _main_menu main menu variable
     */
    QuickMenu(Keypad *_keypad, LiquidCrystal_I2C *_lcd_i2c, QMenu *_main_menu, uint8_t _menu_size, uint8_t _variable_monitor_length = 5);

    /**
     * @brief Menu idle state
     * 
     */
    void idle();
    /**
     * @brief Init
     * 
     */
    void begin();

    /**
     * @brief TODO:
     * 
     */
    void showIdleText(bool opt);
    
    /**
     * @brief TODO:
     * 
     */
    void loop();

    /**
     * @brief Loop
     * 
     */
    bool isMenuActive();

    /**
     * @brief TODO:
     * 
     */
    void displayMenu();

    /**
     * @brief TODO:
     * 
     */
    int getState();

    /**
     * @brief TODO:
     * 
     */
    bool resetState();

    bool eventChange(u_int16_t var);
    
  private:
    LiquidCrystal_I2C* TQDF_lcd;
    Keypad* TQDF_keypad;
    QMenu* TQDF_mainMenu;
    uint8_t TQDF_mainMenu_size;
    
    QMLevel* MENULevel_now();
    QMenu* MENU_now(int index = 0);

    QMLevel MENULevel[MENULevel_MAX];
    
    int SYS_State;
    bool MENU_Idle_enable = true;
    int8_t MENULevel_index = -1;

    bool MENU_Display_initial = true;
    unsigned long MENU_DisplayRow0_millis;
    unsigned long MENU_DisplayRow1_millis;
    bool MENU_DisplayRow1_begin = false;

    byte MENU_Title_index;
    char *MENU_Title_text;
    char *MENU_Title_desc;
    byte MENU_TitleRotation_index;
    
    bool KEYPAD_isKeyInput;
    byte KEYPAD_KeyInput_position;
    String KEYPAD_KeyInput_char;

    uint8_t VAR_Monitor_index = 0;
    unsigned long* VAR_Monitor_temp;

    void displayText();
};
#endif