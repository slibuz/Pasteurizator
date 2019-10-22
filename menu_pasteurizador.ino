#include <Relay.h>
#include <DS18B20.h>

#include <SPI.h>
#include <SD.h>

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino

// Pin Definitions
#define DS18B20WP_PIN_DQ 22
#define RELAYMODULE_PIN_SIGNAL 23

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

const int TS_LEFT = 109, TS_RT = 927, TS_TOP = 72, TS_BOT = 893;
int  SET_PT = 62;

// object initialization
DS18B20 ds18b20wp(DS18B20WP_PIN_DQ);
Relay light(RELAYMODULE_PIN_SIGNAL, true);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button Menu_btn, Temp_btn, Reset_btn, UP_btn, DWN_btn;

int pixel_x, pixel_y, MENU;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Teste Menu");
   
    
    if (ID == 0xD3) ID = 0x9481;
    tft.begin(ID);
    tft.setRotation(0);
    
    tft.fillScreen(BLACK);
    tft.setTextColor(RED);
    tft.setTextSize(3);
    tft.setCursor(160, 0);
    tft.print("Teste de Leitura");
    tft.print("DS18B20");
    delay(100);      
    light.begin();

    tft.begin(ID);
    tft.setRotation(0);            //PORTRAIT
    tft.fillScreen(BLACK);
    Menu_btn.initButton(&tft, 180, 140, 100, 40, WHITE, CYAN, BLACK, "MENU", 2);
    Temp_btn.initButton(&tft, 180, 200, 100, 40, WHITE, CYAN, BLACK, "TEMP", 2);
    Reset_btn.initButton(&tft, 180, 260, 100, 40, WHITE, CYAN, BLACK, "RESET", 2);
    UP_btn.initButton(&tft, 180, 140, 60, 40, WHITE, BLUE, BLACK, "+", 2);
    DWN_btn.initButton(&tft, 180, 200, 60, 40, WHITE, BLUE, BLACK, "-", 2);
    
    Reset_btn.drawButton(false);

    UP_btn.drawButton(false);
    DWN_btn.drawButton(false);

        
}

/* two buttons are quite simple
 */
void loop(void)
{       
     
    bool down = Touch_getXY();

    //Menu_btn.press(down && Menu_btn.contains(pixel_x, pixel_y));
    //Temp_btn.press(down && Temp_btn.contains(pixel_x, pixel_y));
    Reset_btn.press(down && Reset_btn.contains(pixel_x, pixel_y));
    UP_btn.press(down && UP_btn.contains(pixel_x, pixel_y));
    DWN_btn.press(down && DWN_btn.contains(pixel_x, pixel_y));
    
    if (Reset_btn.justReleased())
        Reset_btn.drawButton(); 
    if (UP_btn.justReleased())
        UP_btn.drawButton();
    if (DWN_btn.justReleased())
        DWN_btn.drawButton();       
                
         
    float ds18b20wpTempC = ds18b20wp.getTempC();
    Serial.print(F("Temp: ")); Serial.print(ds18b20wpTempC); Serial.println(F(" [C]"));

    tft.fillRect(130, 20, 90, 40, WHITE);
    tft.fillRect(130, 65, 90, 40, WHITE);
    
    tft.setTextColor(WHITE);
    tft.setTextSize(2);  
    tft.setCursor(10, 30);
    tft.print("Temp.");
    tft.setCursor(10, 75);
    tft.print("SetPt");
     
   
    
    if (ds18b20wpTempC < SET_PT){
      light.turnOn();  
      bool isLightOn = light.getState();
    }
      else {
          light.turnOff();  
      }      
        
    
    if (UP_btn.justPressed()) {
        UP_btn.drawButton(true);
        SET_PT = SET_PT + 1;        
        }          
    if (DWN_btn.justPressed()) {
        DWN_btn.drawButton(true);
        SET_PT = SET_PT - 1;      
        }

    if (Reset_btn.justPressed()) {
        Reset_btn.drawButton(true);
        SET_PT = 62;
        }

    tft.setTextColor(BLACK);
    tft.setTextSize(2);  
    tft.setCursor(135, 30);
    tft.print(ds18b20wpTempC);
    tft.print(" C");
    tft.setTextColor(BLACK);
    tft.setTextSize(2);  
    tft.setCursor(135, 75);
    tft.print(SET_PT);
    tft.print(" C");
  
    }
     
