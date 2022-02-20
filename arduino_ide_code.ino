#include <Adafruit_GFX.h>       
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"

#define REPORTING_PERIOD_MS     1000
#define SAMPLING_RATE                       MAX30100_SAMPRATE_100HZ

#define IR_LED_CURRENT                      MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT                     MAX30100_LED_CURR_7_6MA

#define PULSE_WIDTH                         MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE                        true

MAX30100 sensor;
PulseOximeter pox;

uint32_t tsLastReport = 0;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //Declaring the display name (display)

static const unsigned char PROGMEM logo2_bmp[] =
{ 0x03, 0xC0, 0xF0, 0x06, 0x71, 0x8C, 0x0C, 0x1B, 0x06, 0x18, 0x0E, 0x02, 0x10, 0x0C, 0x03, 0x10,              //Logo2 and Logo3 are two bmp pictures that display on the OLED if called
0x04, 0x01, 0x10, 0x04, 0x01, 0x10, 0x40, 0x01, 0x10, 0x40, 0x01, 0x10, 0xC0, 0x03, 0x08, 0x88,
0x02, 0x08, 0xB8, 0x04, 0xFF, 0x37, 0x08, 0x01, 0x30, 0x18, 0x01, 0x90, 0x30, 0x00, 0xC0, 0x60,
0x00, 0x60, 0xC0, 0x00, 0x31, 0x80, 0x00, 0x1B, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x04, 0x00,  };

static const unsigned char PROGMEM logo3_bmp[] =
{ 0x01, 0xF0, 0x0F, 0x80, 0x06, 0x1C, 0x38, 0x60, 0x18, 0x06, 0x60, 0x18, 0x10, 0x01, 0x80, 0x08,
0x20, 0x01, 0x80, 0x04, 0x40, 0x00, 0x00, 0x02, 0x40, 0x00, 0x00, 0x02, 0xC0, 0x00, 0x08, 0x03,
0x80, 0x00, 0x08, 0x01, 0x80, 0x00, 0x18, 0x01, 0x80, 0x00, 0x1C, 0x01, 0x80, 0x00, 0x14, 0x00,
0x80, 0x00, 0x14, 0x00, 0x80, 0x00, 0x14, 0x00, 0x40, 0x10, 0x12, 0x00, 0x40, 0x10, 0x12, 0x00,
0x7E, 0x1F, 0x23, 0xFE, 0x03, 0x31, 0xA0, 0x04, 0x01, 0xA0, 0xA0, 0x0C, 0x00, 0xA0, 0xA0, 0x08,
0x00, 0x60, 0xE0, 0x10, 0x00, 0x20, 0x60, 0x20, 0x06, 0x00, 0x40, 0x60, 0x03, 0x00, 0x40, 0xC0,
0x01, 0x80, 0x01, 0x80, 0x00, 0xC0, 0x03, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x30, 0x0C, 0x00,
0x00, 0x08, 0x10, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x01, 0x80, 0x00  };

void onBeatDetected()
{
        display.clearDisplay();                                //Clear the display
        display.drawBitmap(0, 0, logo3_bmp, 32, 32, WHITE);    //Draw the second picture (bigger heart) 
        display.setTextSize(1);                                //And still displays the average BPM
        display.setTextColor(WHITE);             
        display.setCursor(50,0);                
        display.println("BPM");             
        display.setCursor(80,0);                
        display.println(pox.getHeartRate()); 
        display.setCursor(50,18);
        display.println("SpO2");
        display.setCursor(80,18);
        display.println(pox.getSpO2());
        display.setCursor(95,18);
        display.println("%");
        display.display();
}

void setup()
{
    Serial.begin(9600);

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //Start the OLED display
    display.display();
    delay(3000);
    pinMode(10, INPUT); //for L0+
    pinMode(11, INPUT); //for L0-
    
    sensor.begin(); //Get raw values
    sensor.setMode(MAX30100_MODE_SPO2_HR);
    sensor.setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
    sensor.setLedsPulseWidth(PULSE_WIDTH);
    sensor.setSamplingRate(SAMPLING_RATE);
    sensor.setHighresModeEnabled(HIGHRES_MODE);
    
    pox.begin();
 // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    
}

void loop()
{
    Serial.println(analogRead(A0));
    uint16_t irValue, red;

    sensor.update();
    pox.update();
    
    sensor.getRawValues(&irValue, &red);
    if (irValue < 7000){       //If no finger is detected it inform the user and put the average BPM to 0 or it will be stored for the next measure
     display.clearDisplay();
     display.setTextSize(1);                    
     display.setTextColor(WHITE);             
     display.setCursor(30,5);                
     display.println("Please Place "); 
     display.setCursor(30,15);
     display.println("your finger ");  
     display.display();
    }
        
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

          if(irValue > 7000){                                           //If a finger is detected
        display.clearDisplay();                                   //Clear the display
        display.drawBitmap(5, 5, logo2_bmp, 24, 21, WHITE);       //Draw the first bmp picture (little heart)
        display.setTextSize(1);                                //And still displays the average BPM
        display.setTextColor(WHITE);             
        display.setCursor(50,0);                
        display.println("BPM");             
        display.setCursor(80,0);                
        display.println(pox.getHeartRate()); 
        display.setCursor(50,18);
        display.println("SpO2");
        display.setCursor(80,18);
        display.println(pox.getSpO2());
        display.setCursor(95,18);
        display.println("%");
        display.display();
        
        pox.setOnBeatDetectedCallback(onBeatDetected);                        //If a heart beat is detected
          
        Serial.print("\t Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
        tsLastReport = millis();
      }//inner if for finger detect
  }//outer if for Report
}// loop end
