#include <Arduino.h>
#include <SPI.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "QuickMenu.h"



//========================================================
// LCD
//========================================================
#define LDC_I2C_ADDRESS 0x27
#define LDC_ROWS 2
#define LDC_COLS 16
LiquidCrystal_I2C lcd_i2c(LDC_I2C_ADDRESS, LDC_COLS, LDC_ROWS);  // set the LCD address to 0x27 for a 16 chars and 2 line display


//========================================================
// Keypad
//========================================================
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[KEYPAD_ROWS] = {PB1, PB0, PA7, PA6};
byte colPins[KEYPAD_COLS] = {PA5, PA4, PA3, PA2};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS );


 
//========================================================
#define MAIN_PERIOD 1000
unsigned long TQDF_MAIN_millis = 0;
unsigned long TQDF_MAIN_period = MAIN_PERIOD;

unsigned long TQDF_STATE1_millis = 0;

//========================================================
// QuickMenu Variables
//========================================================
bool SYS_State;
bool SYS_Active;
uint16_t SYS_Frequency = 50;

byte SYS_Alarm_hour;
byte SYS_Alarm_minute;
byte SYS_Alarm_second;
 
//========================================================
// QuickMenu
// Note: String width is 16 characters
//========================================================
//Dropdown menu
QMDropdown MENUActive_items[]  = 
{
//|"     TEXT       "         |
  {"0.Deactivate    "         },
  {"1.Active        "         },
};

//Sub menu
QMenu MENUAlarm_items[] = 
{
//|"     TEXT       "         | "      DESC      "  |      dataType     |     dataVariable        |    ValueMax     |    valueMin     | submenu                     |    param_A  |     COMMENT
  {"1.Hour          "         , ""                  , UInt8             , &SYS_Alarm_hour          , 1              , 24              , 0                           , 0           },
  {"2.Minute        "         , ""                  ,  UInt8            , &SYS_Alarm_minute        , 0              , 59              , 0                           , 0           },
  {"3.Second        "         , ""                  ,  UInt8            , &SYS_Alarm_second        , 0              , 59              , 0                           , 0           },
};
 

//Main menu
QMenu MENUMain_items[] = 
{
//|"     TEXT       "         | "      DESC      "  |      dataType     |     dataVariable        |    ValueMax     |    valueMin     | submenu                     |    param_A  |     COMMENT
  {"1.Active        "         , ""                  ,  textDropDown     , &SYS_Active             , false           , true            , (char *)MENUActive_items    , 2           }, // this is menu comment
  {"2.Frequency     "         , "0 - 100 Hz      "  ,  UInt16           , &SYS_Frequency          , 0               , 100             , 0                           , 0           },
  {"3.Alarm        >"         , ""                  ,  subMenu          , 0                       , 1               , 3               , (char *)MENUAlarm_items     , 3           },
  {"4.RUN!          "         , ""                  ,  systemState      , 0                       , 0               , 0               , 0                           , 1           },
  //{"5.Custom QMenu   "         , ""                  ,  systemState      , 0                       , 0               , 0               , 0                           , 1           },
};
//========================================================
QuickMenu menu(&keypad, &lcd_i2c, MENUMain_items);
//========================================================

 
void setup() {
  Serial.begin(11500);
  Serial.println("Dynamic infinite menu with keypad 4x4 by www.triqada.fi");
  
  // INIT LCD
  lcd_i2c.init();
  lcd_i2c.backlight();

  // INIT QuickMenu
  menu.begin();
  menu.showIdleText(false); // disable idle text
}
 
 
void loop() {
  menu.loop(); // menu update loop

  // EXAMPLE: using state
  // print "#" to serial monitor every second
  if(menu.getState() == 1){
    lcd_i2c.clear();
    lcd_i2c.setCursor(0, 0);
    lcd_i2c.print("QMenu State 1");
    while(1){
      if(millis() - TQDF_STATE1_millis > 1000){
        TQDF_STATE1_millis = millis();
        Serial.println("#");
      }

      if(menu.resetState() == true) break;
    }
  };
  // just extend this if you need more state
  // if(menu.getState() == 2 ){ ...
  
  // EXAMPLE: main loop
  // PLEASE DONT use delay()! use millis base delay!
  if(millis() - TQDF_MAIN_millis > TQDF_MAIN_period){
    TQDF_MAIN_millis = millis();
    
    // EXAMPLE: update main screen when not in menu
    //          Data is a random number between 0-100
    if(menu.isMenuActive() == false){
      // lcd_i2c.clear();
      lcd_i2c.setCursor(0, 0);
      lcd_i2c.print("Data: ");
      lcd_i2c.print(random(0,100));
      lcd_i2c.print("       "); // just fill the LCD
      lcd_i2c.setCursor(0, 1);
      lcd_i2c.print("Freq: ");
      if(SYS_Frequency < 100) lcd_i2c.print("_");
      lcd_i2c.print(SYS_Frequency);
      lcd_i2c.print("Hz");
      lcd_i2c.print("    #");

    }

    // EXAMPLE: display data to serial
    Serial.print("SYS_active = ");
    Serial.println(SYS_Active);
    Serial.print("SYS_frequency = ");
    Serial.println(SYS_Frequency);

    Serial.print("SYS_Alarm_hour = ");
    Serial.println(SYS_Alarm_hour);
    Serial.print("SYS_Alarm_minute = ");
    Serial.println(SYS_Alarm_minute);
    Serial.print("SYS_Alarm_second = ");
    Serial.println(SYS_Alarm_second);
    Serial.println();
  }
}

 

