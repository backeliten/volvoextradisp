#include <FlexCAN.h>
#include <kinetis_flexcan.h>

/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define OLED_RESET 2
Adafruit_SSD1306 display(OLED_RESET);

typedef struct
{ 
    uint16_t  adress;
    uint8_t   data_offset;
    uint8_t   data_length;
    float     multiplier;
    int16_t  value_offset;
}volvo_id_s;

FlexCAN can(125000);    //Init CAN driver for teensy

CAN_message_t   message_in;    //Structure for CAN message
CAN_message_t   message_out;    //Structure for CAN message

CAN_filter_t    filter, filter2, filter3, filter4;

int16_t       tempvalue;
uint8_t       KeyOn = 0;
uint8_t       button_count = 0;

#define VERSION       "2.0"

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2
#define BUTTON_PIN        A6

volvo_id_s  rpm {.adress=0x155, .data_offset=4, .data_length=2, .multiplier=1.0, .value_offset=0};
volvo_id_s  speed {.adress=0x145, .data_offset=6, .data_length=2, .multiplier=1.0, .value_offset=0};
volvo_id_s  clt {.adress=0x4C5, .data_offset=5, .data_length=1, .multiplier=1.0, .value_offset=60};

int16_t  volvo_convert_data(volvo_id_s conv_s, CAN_message_t message_in, int16_t *value);   

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

int16_t volvo_convert_data(volvo_id_s conv_s, CAN_message_t message_in, int16_t *value)
{
  float tempvalue = 0;
  if(message_in.id == conv_s.adress)
  {
      if(conv_s.data_length==1)
      {
        tempvalue = (float)message_in.buf[conv_s.data_offset];
      }
      if(conv_s.data_length==2)
      {
        tempvalue = (float)((message_in.buf[conv_s.data_offset] << 8) | message_in.buf[conv_s.data_offset+1]);   //Bitshift with 8 and or lower
      }
      tempvalue = tempvalue - (float)conv_s.value_offset;   //Correct with offset
      *value = (int16_t)(tempvalue*conv_s.multiplier);   //Correct with multiplier   

      return 0;
  }
  return -1;
}

void setup()   {                
  Serial.begin(1000000);
    pinMode(BUTTON_PIN, INPUT);    //A6

 // filter.ext = 0;
 // filter.id = rpm.adress;

//  filter2.ext = 0;
//  filter2.id = speed.adress;

//  filter3.ext = 0;
//  filter3.id = clt.adress;

  can.begin();

//  can.setFilter(filter3, 0);
//   can.setFilter(filter3, 1);
//    can.setFilter(filter3, 2);
//     can.setFilter(filter3, 3);
//      can.setFilter(filter3, 4);
//       can.setFilter(filter3, 5);
//        can.setFilter(filter3, 6);
//         can.setFilter(filter3, 7);
  

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Clear the buffer.
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Volvo ext.display..");
  display.println(VERSION);
  display.display();
  delay(2000);

 //display.setTextSize(7);
  display.setTextSize(1);
 display.setTextColor(WHITE);

 Serial.println("Volvo Extended display startup....");
}
void loop() {

 // Serial.println(message_in.id);

 // message_out.id = 0x201;
 // message_out.len = 8;
//  message_out.ext = 0;
//  message_out.buf[0] = 0x12;

 // can.write(message_out);

 // display.setCursor(0,20);
 // display.println(message_out.id);

   if(digitalRead(BUTTON_PIN) == LOW)
  {
      button_count++;

      if(button_count == 3)
      {
        button_count = 0;   //Clear button
      }
      delay(200);     //Bounce delay
  }
 
  if(can.read(message_in))
  {
      if(message_in.id == 0x035)   
      {
          if((message_in.buf[0] & 0x0F) == 0x04)
          {
            KeyOn = 1;      //Key is on!
          }
          else
          {
            KeyOn = 0;    //Key is off!
            
          }
      }
  } 

  //display.setCursor(0,0);
  //display.clearDisplay();
      
  if(button_count == 0)
  {
    //display.println("CLT");
    //display.display();
      if(volvo_convert_data(clt, message_in, &tempvalue) == 0)
      {
          if(KeyOn == 1)
          {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(7);
            display.print(tempvalue);
            display.display();
          }
          else
          {
            display.clearDisplay();
            display.display();
          }
       }
  }
  
  if(button_count == 1)
  {
    //display.println("RPM");
    //display.display();
      if(volvo_convert_data(rpm, message_in, &tempvalue) == 0)
      {
          if(KeyOn == 1)
          {
            display.clearDisplay();
            display.setCursor(0,0);
             display.setTextSize(3);
            display.print(tempvalue);
            display.display();
          }
          else
          {
            display.clearDisplay();
            display.display();
          }
       }
  }

   if(button_count == 2)
  {
    //display.println("SPEED");
   // display.display();
      if(volvo_convert_data(speed, message_in, &tempvalue) == 0)
      {
          if(KeyOn == 1)
          {
            display.clearDisplay();
            display.setCursor(0,0);
             display.setTextSize(4);
            display.print(tempvalue);
            display.display();
          }
          else
          {
            display.clearDisplay();
            display.display();
          }
       }
  }

  delay(1);
}
