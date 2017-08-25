

/*
 Demonstration sketch for Adafruit i2c/SPI LCD backpack
 using MCP23008 I2C expander
 ( http://www.ladyada.net/products/i2cspilcdbackpack/index.html )

 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * 5V to Arduino 5V pin
 * GND to Arduino GND pin
 * CLK to Analog #5
 * DAT to Analog #4
*/

// include the library code:
#include <EEPROM.h>
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"


#define MATRIX_BAUDRATE 0x39       // 1 arg

/******** text commands */
#define MATRIX_AUTOSCROLL_ON 0x51
#define MATRIX_AUTOSCROLL_OFF 0x52
#define MATRIX_CLEAR 0x58
#define MATRIX_CHANGESPLASH 0x40  // COL * ROW chars!
#define MATRIX_AUTOWRAPLINE_ON 0x43
#define MATRIX_AUTOWRAPLINE_OFF 0x44
#define MATRIX_SETCURSOR_POSITION 0x47    // 2 args: col, row
#define MATRIX_HOME 0x48
#define MATRIX_MOVECURSOR_BACK 0x4C
#define MATRIX_MOVECURSOR_FORWARD 0x4D
#define MATRIX_UNDERLINECURSOR_ON 0x4A
#define MATRIX_UNDERLINECURSOR_OFF 0x4B
#define MATRIX_BLOCKCURSOR_ON 0x53
#define MATRIX_BLOCKCURSOR_OFF 0x54
#define MATRIX_DISPLAY_SHIFT_R 0x5A
#define MATRIX_DISPLAY_SHIFT_L 0x5B

#define MATRIX_STARTL_COMMAND 0xFE

#define MATRIX_SETDDRAMADDR 0x80
#define LCD_SPACE 0x20
#define SatLeftBM 0
#define SatRightBM 1


/****** special chars */
#define MATRIX_CUSTOM_CHARACTER 0x4E  // 9 args: char #, 8 bytes data
#define MATRIX_SAVECUSTOMCHARBANK 0xC1  // 9 args: char #, 8 bytes data
#define MATRIX_LOADCUSTOMCHARBANK 0xC0  // 9 args: char #, 8 bytes data

/***** display func */
#define MATRIX_DISPLAY_ON  0x42    // backlight. 1 argument afterwards, in minutes
#define MATRIX_DISPLAY_OFF  0x46
#define MATRIX_SET_BRIGHTNESS 0x99 // 1 arg: scale
#define MATRIX_SETSAVE_BRIGHTNESS 0x98 // 1 arg: scale
#define MATRIX_SET_CONTRAST 0x50 // 1 arg
#define MATRIX_SETSAVE_CONTRAST 0x91 // 1 arg

// storage of the current backlight state
#define COLS_ADDR 0
#define ROWS_ADDR 1
#define SPLASH_ADDR 2
#define BACKLIGHT_R_ADDR 100
#define BACKLIGHT_G_ADDR 101
#define BACKLIGHT_B_ADDR 102
#define BACKLIGHT_STATE_ADDR 103
#define BACKLIGHT_BRIGHTNESS_ADDR 104
#define AUTOSCROLL_ADDR 105
#define BAUDRATE_ADDR 106  // and 107, 108
#define CONTRAST_ADDR 109
#define CUSTOMCHARBANKS_ADDR 110
// 110 thru 430 !
#define GPO_1_START_ONOFF_ADDR 431
#define GPO_2_START_ONOFF_ADDR 432
#define GPO_3_START_ONOFF_ADDR 434
#define GPO_4_START_ONOFF_ADDR 435
#define LAST_ADDR 436
#define EEPROM_END 1023

uint8_t COLS = EEPROM.read(COLS_ADDR);
uint8_t ROWS = EEPROM.read(ROWS_ADDR);
uint8_t x, y;


unsigned char sdcjp[] = { 0xbd,0xc3,0xa8,0xb0,0xcc,0xde,0xdd };

//char* sdcjp[] = { 0xbd,0xc3,0xa8,0xb0,0xcc,0xde,0xdd };

unsigned char bmSatLeft[] = { 0, 20, 21, 21, 31, 21, 20, 20 };
unsigned char bmSatright[] = { 0, 5, 21, 21, 31, 21, 5, 5 };
char written; 

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

void setup() 
{

  if (COLS > 20) COLS = 20;
  if (ROWS > 4) ROWS = 4;
  // set up the LCD's number of rows and columns: 
  //lcd.begin(16, 2);

  //lcd.begin(20,4); 
  lcd.begin(COLS, ROWS);   
  
  // Print a message to the LCD.
  lcd.print("hello, world!");
  lcd.createChar(0, bmSatLeft);
  lcd.createChar(1, bmSatright);
  Serial.begin(9600); 
  while(! Serial)
    Serial.println(F("Hello from LCD"));

  Serial.println(EEPROM.read(0));
  for(int i=0; i!=7; i++)
  {
      EEPROM.write(i,sdcjp[i]);
  }


    lcd.clear();   
    lcd.home(); 
    lcd.print(F("USB/Serial LCD"));
    lcd.setCursor(0,1);   
    lcd.blink();
    
//    for(int i=0; i!=30; i++)
//    {
//        lcd.command(MATRIX_SETDDRAMADDR+i);
//        lcd.write('A');
//        delay(200);
//      
//    }

    walkSatbm();
    
}

char displaybuff[20][4];

/*****************************************/
void loop() 
{
  byte a, b, c;
  //parse command loop
  lcd.display();
  if (Serial.available()) 
  {
    c = serialBlockingRead();
    //lcd.print("cmd= ");
    // lcd.print(c);
    //Serial.print(c);
    //delay(1000);

    if (c != MATRIX_STARTL_COMMAND)     // not a command, just print the text!
    {
      if (c == 0x0D) 
      {
        // special exception for new lines
        while (x <= COLS-1) 
        {
          lcd.print(' '); // fill with spaces
          displaybuff[x][y] = ' ';
          x++;
        }
      }
      else if (c == 0x0A) 
      {
       // do nothing
       return;
      } 
      else if (c == 0x08) 
      {
       if (x > 0) x--;
         else 
         {
           x = COLS - 1;
          if (y > 0) y--;
          else y = ROWS - 1;
         }
         lcd.setCursor(x, y); 
         displaybuff[x][y] = ' ';
         lcd.print(' ');
         lcd.setCursor(x, y); 
         return;
      }
      else 
      {
        lcd.print((char)c);
        displaybuff[x][y] = c;
      }

      // wrap the cursor
      x++;
      if (x >= COLS) {
        // go to the next row
        x = 0;
        y++;
        if (y >= ROWS) {
          // ran out of display space!
          if (! getAutoScroll()) {
            // go to the top left
            y = 0;
          } else {
            // shift all data up by one line!
            for (int8_t i=0; i < ROWS; i++) {
              for (uint8_t j=0; j<COLS; j++) {
                if (i == ROWS-1) {
                  //Serial.print("spaces");
                  displaybuff[j][ROWS-1] = ' ';
                } else {
                  displaybuff[j][i] = displaybuff[j][i+1];
                }
              }
            }
            for (int8_t i=0; i < ROWS; i++) {
              lcd.setCursor(0, i);                  /// sdc Liquid Crystal call
              for (uint8_t j=0; j<COLS; j++) {
                lcd.write(displaybuff[j][i]);       /// sdc Liquid Crystal call
              }
            }
            // set the x & y
            x = 0;
            y = ROWS-1;
            printbuffer();
          }
        }
        lcd.setCursor(x, y);                    /// sdc Liquid Crystal call
      }
       printbuffer();
      return;
    }
    
    //Serial.print(F("\ncommand: "));
    // it is a command! get the command byte
    
    c = serialBlockingRead();
    //     lcd.print("cmd1= ");
    //    lcd.print(c);
    //Serial.println(c, HEX);
    switch (c) 
    {
      case MATRIX_DISPLAY_ON:
        displayOn();
        c = serialBlockingRead();   // todo - add timeout!
        //    lcd.print("cmd2= ");
        //   lcd.print(c);
        break;
      case MATRIX_DISPLAY_OFF:
        displayOff();
        //displayOn();
        break;
 
      case MATRIX_HOME:
        lcd.home();                 /// sdc Liquid Crystal call
        x = 0; y = 0;
        break;
      case MATRIX_CLEAR:
        lcd.clear();                /// sdc Liquid Crystal call
        clearbuffer();
        x = 0; y = 0;
        //   lcd.print("cmd3= ");
        //   lcd.print(c);
        break;
      case MATRIX_AUTOSCROLL_OFF:
        setAutoScroll(0);
        break;
      case MATRIX_AUTOSCROLL_ON:
        setAutoScroll(1);
        break;
      case MATRIX_AUTOWRAPLINE_OFF:
      case MATRIX_AUTOWRAPLINE_ON:
         // not implemented
         break;
      case MATRIX_SETCURSOR_POSITION:
         x = serialBlockingRead();
         y = serialBlockingRead();
         x--; y--;// matrix orbital starts at (1,1) not (0,0)
         if (x >= COLS) { y += x / COLS; x %= COLS; }
         y %= ROWS;
         lcd.setCursor(x, y);
         //Serial.print(F("\nMoving to (")); Serial.print(x, DEC);
         //Serial.print(','); Serial.print(y, DEC); Serial.println(')');
         break;
       case MATRIX_MOVECURSOR_BACK:
         if (x > 0) x--;
         else {
           x = COLS - 1;
          if (y > 0) y--;
          else y = ROWS - 1;
         }
         lcd.setCursor(x, y);
         break;
       case MATRIX_MOVECURSOR_FORWARD:
         if (x < COLS - 1) x++;
         else {
           x = 0;
          if (y < ROWS - 1) y++;
          else y = 0;
         }
         lcd.setCursor(x, y);
         break;
       case MATRIX_UNDERLINECURSOR_ON:
         lcd.blink();                      /// sdc Liquid Crystal call
         break;
       case MATRIX_BLOCKCURSOR_OFF:
       case MATRIX_UNDERLINECURSOR_OFF:
         lcd.noCursor();                    /// sdc Liquid Crystal call
         lcd.noBlink();                     /// sdc Liquid Crystal call
        //lcd.print("cmdoff= ");
       // lcd.print(c);
         break;
       case MATRIX_BLOCKCURSOR_ON:
          lcd.blink();                       /// sdc Liquid Crystal call
          lcd.cursor();
        //lcd.print("cmdon= ");
        //lcd.print(c);
         break;
       case MATRIX_CHANGESPLASH:
         for (uint8_t i=0; i < ROWS; i++) {
            for (uint8_t j=0; j<COLS; j++) {
              c = serialBlockingRead();
              EEPROM.write(SPLASH_ADDR + j * 40 + i, c);
            }
          }
         break;
      
       case MATRIX_CUSTOM_CHARACTER:
         c = serialBlockingRead();
         readCustom(c, 255);
         break;
       case MATRIX_LOADCUSTOMCHARBANK:
         c = serialBlockingRead();
         loadCustom(c);
         break;
       case MATRIX_SAVECUSTOMCHARBANK:
         //Serial.println(F("Custom char"));
         b = serialBlockingRead();
         c = serialBlockingRead();
         readCustom(c, b);
         break;
      case MATRIX_DISPLAY_SHIFT_R:
        lcd.scrollDisplayRight();
        break;
      case MATRIX_DISPLAY_SHIFT_L:
        lcd.scrollDisplayRight();
        break;
     }
  }

  
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loadCustom(uint8_t bank) 
{
  uint8_t newChar[8];

  if (bank > 4) return;

  int16_t addr = bank;
    addr *= 64;
    addr += CUSTOMCHARBANKS_ADDR;

  for (uint8_t loc = 0; loc < 8; loc++) {
    for (uint8_t i=0; i<8; i++) {
     newChar[i] = EEPROM.read(addr);
     //Serial.print(F("Loaded ")); Serial.print(newChar[i], HEX); Serial.print(F(" from $"));
     //Serial.println(addr);
     addr++;
    }
    lcd.createChar(loc, newChar);   /// sdc Liquid Crystal call
    //lcd.write(loc);
  }
  lcd.setCursor(x, y);              /// sdc Liquid Crystal call
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void readCustom(uint8_t loc, uint8_t bank) 
{
/*
  Serial.print(F("Saving char to bank #")); Serial.print(bank, DEC);
  Serial.print(F(" : "));
  Serial.println(loc, DEC);
  */

  uint8_t newChar[8];
  for (uint8_t i=0; i<8; i++) {
     newChar[i] = serialBlockingRead();
  }
  if (bank > 4) {
    // instant display
    lcd.createChar(loc, newChar);       /// sdc Liquid Crystal call
    lcd.setCursor(x, y);                /// sdc Liquid Crystal call
  } else {
    // save it to EEPROM
    int16_t addr = bank;
    addr *= 64;
    addr += loc * 8;
    addr += CUSTOMCHARBANKS_ADDR;

    for (uint8_t i=0; i<8; i++) {
      EEPROM.write(addr + (int16_t)i, newChar[i]);
    }
  }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int serialAvailable() 
{
  return Serial.available();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
char serialBlockingRead() 
{
   while (!serialAvailable());
   char c = 0;

   if (Serial.available()) 
     c = Serial.read();

  return c;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setAutoScroll(uint8_t x) 
{
  EEPROM.write(AUTOSCROLL_ADDR, x);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint8_t getAutoScroll() 
{
  return EEPROM.read(AUTOSCROLL_ADDR);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void displayOn() 
{
    lcd.setBacklight(HIGH);
    lcd.display();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void displayOff() 
{
     lcd.setBacklight(LOW);
     lcd.noDisplay();

}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setSize(uint8_t c, uint8_t r) 
{
  if (r != EEPROM.read(ROWS_ADDR))
    EEPROM.write(ROWS_ADDR, r);
  if (c != EEPROM.read(COLS_ADDR))
    EEPROM.write(COLS_ADDR, c);

  COLS = c;
  ROWS = r;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void clearbuffer() 
{
    for (uint8_t i=0; i <ROWS; i++) {
       for (uint8_t j=0; j<COLS; j++) {
         displaybuff[j][i] = ' ';
       }
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printbuffer() 
{
#ifdef USBECHOBUFFER
  Serial.println(F("\n\r------------------"));
  for (uint8_t i=0; i <ROWS; i++) {
    Serial.print('|');
    for (uint8_t j=0; j<COLS; j++) {
      Serial.print(displaybuff[j][i], BYTE);
    }
    Serial.println('|');
   }
   Serial.println(F("------------------"));
#endif
}


void walkSatbm()
{
   for(int ddcount=0; ddcount<40; ddcount++)
    {
        lcd.command(LCD_SETDDRAMADDR+ddcount); // direct to DDRAM location 80+i on display. 0,0
        lcd.write(SatLeftBM);
        lcd.command(LCD_SETDDRAMADDR+ddcount+1);
        lcd.write(SatRightBM);
        delay(500);
        lcd.command(0x80+ddcount);
        lcd.write(LCD_SPACE);              // blank space to overwrite sat bmp
    }
}



















