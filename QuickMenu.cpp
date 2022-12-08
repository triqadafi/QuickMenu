#include "QuickMenu.h"

QuickMenu::QuickMenu(Keypad *_keypad, LiquidCrystal_I2C *_lcd_i2c, QMenu *_main_menu, uint8_t _variable_monitor_length){
  TRQDF_keypad = _keypad;
  TRQDF_lcd = _lcd_i2c;
  TRQDF_mainMenu = _main_menu;
  if (_variable_monitor_length > 255) _variable_monitor_length = 255;
  if (_variable_monitor_length < 1) _variable_monitor_length = 1;
  VAR_Monitor_temp = (unsigned long *) malloc(_variable_monitor_length * sizeof(unsigned long));
}

void QuickMenu::begin(){
  TRQDF_lcd->clear();
}

void QuickMenu::idle(){
  if(MENU_Idle_enable){
    TRQDF_lcd->clear();
    TRQDF_lcd->setCursor(0, 0);
    TRQDF_lcd->print("Press # to");
    TRQDF_lcd->setCursor(0, 1);
    TRQDF_lcd->print("enter the QMenu"); 
  }
}

QMLevel* QuickMenu::MENULevel_now(){
  return &MENULevel[MENULevel_index];
}

QMenu* QuickMenu::MENU_now(int index){
  if(index == 0){
    index = MENULevel_index;
  }else{
    index = MENULevel_index + index;
  }
  return &MENULevel[index].menu[MENULevel[index].index];
}


bool QuickMenu::eventChange(u_int16_t var){
  bool _return = false;
  if(VAR_Monitor_temp[VAR_Monitor_index] != var){
    _return = true;
  }
  VAR_Monitor_temp[VAR_Monitor_index] = var;
  VAR_Monitor_index++;
  return _return;
}

void QuickMenu::displayMenu(){
  MENU_Display_initial = true;
  if(MENULevel_index == -1){
    idle();
  }else if(MENULevel_index == 0){
    MENULevel_now()->index = 0;
    MENULevel_now()->menu = TRQDF_mainMenu;
    MENULevel_now()->menuLength = sizeof(&TRQDF_mainMenu);
    MENULevel_now()->dropDownLength = 0;
    MENU_Title_index = 0;
 
    TRQDF_lcd->clear();
  }else{
    TRQDF_lcd->clear();
    TRQDF_lcd->setCursor(0, 0);
    TRQDF_lcd->print(MENU_now(-1)->title);
    MENULevel_now()->dropDownLength = 0;
  }
}


void QuickMenu::displayText(){
  if(MENULevel_index >= 0){
    if((millis() - MENU_DisplayRow0_millis > 1000) || MENU_Display_initial){
      MENU_DisplayRow0_millis = millis();
      MENU_DisplayRow1_millis = millis();
      MENU_DisplayRow1_begin = true;

      if(KEYPAD_isKeyInput){
        TRQDF_lcd->setCursor(0, 0);
        if(((MENU_TitleRotation_index % 8) == 0) || ((MENU_TitleRotation_index % 8) == 1)){
          TRQDF_lcd->print(MENU_Title_text);
        }else if(((MENU_TitleRotation_index % 8) == 2) || ((MENU_TitleRotation_index % 8) == 3)){
          TRQDF_lcd->print(MENU_Title_desc);
        }else if(((MENU_TitleRotation_index % 8) == 4) || ((MENU_TitleRotation_index % 8) == 5)){
          TRQDF_lcd->print("# to Save       ");
        }else{
          TRQDF_lcd->print("* to Cancel     ");
        }
        MENU_TitleRotation_index++;
      }else if(MENULevel_index == 0){
        TRQDF_lcd->setCursor(0, 0);
        if(((MENU_TitleRotation_index % 4) == 0) || ((MENU_TitleRotation_index % 4) == 1)){
          TRQDF_lcd->setCursor(0, 0);
          TRQDF_lcd->print("Choose [1..");
          TRQDF_lcd->print(sizeof(&TRQDF_mainMenu));
          TRQDF_lcd->print("]   ");
        }else{
          TRQDF_lcd->print("#/* to close    ");
        }
        MENU_TitleRotation_index++;
      }
       
       
    }

    if(((millis() - MENU_DisplayRow1_millis > 200) || MENU_Display_initial) && MENU_DisplayRow1_begin ){
      MENU_Display_initial = false;
      MENU_DisplayRow1_begin = false;
      if(!KEYPAD_isKeyInput){
        if(MENULevel_index != -1){
          if(MENU_Title_index >= MENULevel_now()->menuLength)
          {
            MENU_Title_index = 0;
          }
          TRQDF_lcd->setCursor(0, 1);
          TRQDF_lcd->print(MENULevel_now()->menu[MENU_Title_index++].title);
        }
      }
      switch(MENU_now()->dataType){
        case textDropDown:
          if(MENULevel_now()->dropDownLength > 0){
            if(MENU_Title_index >= MENULevel_now()->dropDownLength)
            {
              MENU_Title_index = 0;
            }
            TRQDF_lcd->setCursor(0, 1);
            char *dropdown_item = MENULevel_now()->dropDown + (MENU_Title_index++ * (LCDString_WIDTH));
            *(dropdown_item + 14) = '\0';
            TRQDF_lcd->print(dropdown_item);
          }
          break;
      }
    }
  }
}

void QuickMenu::loop(){
  char key = TRQDF_keypad->getKey();
  if (key){
    if(key == '#'){
      if(KEYPAD_isKeyInput){
        KEYPAD_isKeyInput = false;

        uint16_t KEYPAD_KeyInput_int = KEYPAD_KeyInput_char.toInt();
 
        if((KEYPAD_KeyInput_int >= MENU_now()->valueMax) && (KEYPAD_KeyInput_int <= MENU_now()->valueMin)){
          switch(MENU_now()->dataType){
            case UInt8:
              *(uint8_t*)MENU_now()->dataVariable = KEYPAD_KeyInput_int;
              break;
            case UInt16:
              *(uint16_t*)MENU_now()->dataVariable = KEYPAD_KeyInput_int;
              break;
            case textDropDown:
              MENULevel_now()->dropDownLength = 0;
              *(byte*)MENU_now()->dataVariable = KEYPAD_KeyInput_int;
              MENU_Title_index = 0;
              break;
          }
          TRQDF_lcd->clear();
          TRQDF_lcd->print(MENU_Title_text);
          TRQDF_lcd->setCursor(0, 1);
          TRQDF_lcd->print(KEYPAD_KeyInput_int);
          TRQDF_lcd->print(" saved!");
          delay(1000);
        }else{
          TRQDF_lcd->clear();
          TRQDF_lcd->print(MENU_Title_text);
          TRQDF_lcd->setCursor(0, 1);
          TRQDF_lcd->print("*    !Out Range!");
          delay(1000);

        }
      }else if(MENULevel_index < 0){
        MENULevel_index = 0; // enter main menu
      }else{
        MENULevel_index--;
      }
      displayMenu();
    }else if(key == '*'){
      if(KEYPAD_isKeyInput){
        KEYPAD_isKeyInput = false;
        displayMenu();
      }else if(MENULevel_index >= 0){
        MENULevel_index--;
        displayMenu();
      }
    }else if((MENULevel_index >= 0) && ((key >= '0') || (key <= '9'))){
      if(KEYPAD_isKeyInput){
        switch(MENU_now()->dataType){
          case UInt8:
          case UInt16:
            KEYPAD_KeyInput_char += key;
            TRQDF_lcd->setCursor(KEYPAD_KeyInput_position++, 1);
            TRQDF_lcd->print(key);
            break;
          case textDropDown:
            KEYPAD_KeyInput_char = key;
            TRQDF_lcd->setCursor(KEYPAD_KeyInput_position, 1);
            TRQDF_lcd->print(key);
            break;
        }
      }else{ // if input not required
        // we are selecting the menu
        if((key != '0') && (key - '0' <= MENULevel_now()->menuLength)){ // just check that the key input between menuLength
          MENULevel_now()->index = key - '1'; // change the level index parameter
          MENU_Title_text = MENU_now()->title;
          MENU_Title_desc = MENU_now()->desc;

          TRQDF_lcd->clear();
          TRQDF_lcd->print(MENU_Title_text);
          TRQDF_lcd->setCursor(0, 1);

          MENU_TitleRotation_index = 0;
          KEYPAD_KeyInput_position = 8;
          MENU_Display_initial = true;
  
          uint16_t nilaiUInt8;
          uint16_t nilaiUInt16;
          uint16_t nilaiUInt32;
          int16_t nilaiFloat;
  
          KEYPAD_KeyInput_char = "";
  
          switch(MENU_now()->dataType){
            case UInt8:
              KEYPAD_isKeyInput = true;
              TRQDF_lcd->print(*(byte*)MENU_now()->dataVariable);
              TRQDF_lcd->setCursor(6, 1);
              TRQDF_lcd->print("=>0");
              break;
            case UInt16:
              KEYPAD_isKeyInput = true;
              TRQDF_lcd->print(*(uint16_t*)MENU_now()->dataVariable);
              TRQDF_lcd->setCursor(6, 1);
              TRQDF_lcd->print("=>0");
              break;
            case textDropDown:
              KEYPAD_isKeyInput = true;
              MENULevel_now()->dropDownLength = MENU_now()->param_a;
              MENULevel_now()->dropDown = MENU_now()->menuSub;
              MENU_Title_index = 0;
              TRQDF_lcd->print(*(uint8_t*)MENU_now()->dataVariable);
              TRQDF_lcd->setCursor(14, 1);
              TRQDF_lcd->print(">0");
              KEYPAD_KeyInput_position = 15;
              // delay(1000);
              break;
            case subMenu:
              KEYPAD_isKeyInput = false;
              MENULevel_index++;
              MENULevel_now()->index = 0; // reset the next level index, current level index (set above) remain the same
              MENULevel_now()->menu = (QMenu *)MENU_now(-1)->menuSub;
              MENULevel_now()->menuLength = MENU_now(-1)->param_a;
              MENULevel_now()->dropDownLength = 0;
              MENU_Title_index = 0;
              break;
            case systemState:
              KEYPAD_isKeyInput = false;
              SYS_State = MENU_now()->param_a;
              break;
          }
        }
      }
    }
  }
  displayText();
  VAR_Monitor_index = 0;
}

bool QuickMenu::isMenuActive(){
  return (MENULevel_index != -1);
}
int QuickMenu::getState(){
  return SYS_State;
}
void QuickMenu::showIdleText(bool opt){
  MENU_Idle_enable = opt;
}

bool QuickMenu::resetState(){
  char key = TRQDF_keypad->getKey();
  if(key == '#'){
    SYS_State = 0;
    MENULevel_index = -1;
    
    TRQDF_lcd->clear();
    TRQDF_lcd->setCursor(0, 1);
    TRQDF_lcd->print("Stopping...");
    delay(1000);
    displayMenu();

    return true;
  }
  return false;
}



/*
void QuickMenu::begin(){
  TRQDF_lcd->clear();
}
  char msg[17];

  Switch menu mechanism
  MENULevel_index = 0;
  MENULevel[MENULevel_index].index = 0;
  MENULevel[MENULevel_index].menu = TRQDF_mainMenu;
  MENULevel[MENULevel_index].menuLength = array_sizeof(TRQDF_mainMenu);
  MENULevel[MENULevel_index].dropDownLength = 0;

  MENULevel_index++;
  MENULevel[MENULevel_index].index = 0;
  // MENULevel[MENULevel_index].menu = (QMenu *)MENULevel[MENULevel_index-1].menu[MENULevel[MENULevel_index-1].index].menuSub;
  MENULevel[MENULevel_index].menu = (QMenu *) MENU_now(-1)->menuSub;
  MENULevel[MENULevel_index].menuLength =  MENU_now(-1)->param_a;
  MENULevel[MENULevel_index].dropDownLength = 0;

  sprintf(msg, "%p", MENULevel[MENULevel_index].menuLength);
  TRQDF_lcd->setCursor(0, 0);
  TRQDF_lcd->print(msg);
  sprintf(msg, "%p", MENU_now(-1)->param_a);
  TRQDF_lcd->setCursor(0, 1);
  TRQDF_lcd->print(msg);
  LOG
  0x200000d0 &MENUAlarm_items
  0x200000ac &MENUActive_items
  0x200000d0 MENUAlarm_items
  0x200000ac MENUActive_items
  0x2000016c &MENUMain_items
  0x2000016c MENUMain_items
  
  0x200000ac MENUMain_items[0].menuSub <- Real pointer to MENUActive_items
  0x20000198 &MENUMain_items[0].menuSub
  0x20000000 MENUMain_items[1].menuSub <- Real pointer
  0x200001cc &MENUMain_items[1].menuSub
  0x200000d0 MENUMain_items[2].menuSub <- Real pointer to MENUAlarm_items
  0x20000200 &MENUMain_items[2].menuSub
  0x20000000 MENUMain_items[3].menuSub <- Real pointer
  0x20000234 &MENUMain_items[3].menuSub


  0x200000ac TRQDF_mainMenu[0].menuSub <- Real pointer
  0x20000198 &TRQDF_mainMenu[0].menuSub
  0x00000000 TRQDF_mainMenu[1].menuSub <- Real pointer
  0x200001cc &TRQDF_mainMenu[1].menuSub
  0x200000d0 TRQDF_mainMenu[2].menuSub <- Real pointer
  0x20000200 &TRQDF_mainMenu[2].menuSub
  0x00000000 TRQDF_mainMenu[3].menuSub <- Real pointer
  0x20000234 &TRQDF_mainMenu[3].menuSub

  0x2000016c TRQDF_mainMenu <- correct!

  QMenu level debug
  0x6341RAND MENULevel[0].menu[MENULevel[0].index]  !thats the pointer address
  0x2000016c &MENULevel[0].menu[MENULevel[0].index] <- means that level 0 has TRQDF_mainMenu items

  0x2000016c &MENULevel[0].menu[0]
  0x200001a0 &MENULevel[0].menu[1]
  0x200001d4 &MENULevel[0].menu[2]

  0x200000ac MENULevel[0].menu[0].menuSub <- Real pointer
  0x20000198 &MENULevel[0].menu[0].menuSub
  0x20000000 MENULevel[0].menu[1].menuSub <- Real pointer
  0x200001cc &MENULevel[0].menu[1].menuSub
  0x200000d0 MENULevel[0].menu[2].menuSub <- Real pointer
  0x200001d4 &MENULevel[0].menu[2].menuSub
  0x20000000 MENULevel[0].menu[3].menuSub <- Real pointer
  0x20000234 &MENULevel[0].menu[3].menuSub


  0x200000ac &MENULevel[1].menu[MENULevel[1].index]
  0x200000ac &MENULevel[1].menu[0]
  0x200000ac (QMenu *)MENULevel[0].menu[MENULevel[0].index].menuSub

  0x200006e0 &MENULevel[MENULevel_index].menu
  0x200000ac MENULevel[MENULevel_index].menu

  delay(3000);

  MENULevel_index++;
  MENULevel[MENULevel_index].index = 0;
  MENULevel[MENULevel_index].menu = (QMenu *)MENULevel[MENULevel_index-1].menu[MENULevel[MENULevel_index-1].index].subMenu;
  MENULevel[MENULevel_index].menuLength = MENULevel[MENULevel_index-1].menu[MENULevel[MENULevel_index-1].index].jumlahBaris;
  MENULevel[MENULevel_index].dropDownLength = 0;

  TRQDF_lcd->clear();
  sprintf(msg, "%p", TestMENU_now(1));
  TRQDF_lcd->setCursor(0, 0);
  TRQDF_lcd->print(msg);
  sprintf(msg, "%p", MENULevel[1].menu[MENULevel[1].index]);
  TRQDF_lcd->setCursor(0, 1);
  TRQDF_lcd->print(msg);
*/

