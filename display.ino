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


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

volvo_id_s  rpm {.adress=0x155, .data_offset=4, .data_length=2, .multiplier=1.0, .value_offset=0};
volvo_id_s  speed {.adress=0x145, .data_offset=6, .data_length=2, .multiplier=1.0, .value_offset=0};
volvo_id_s  clt {.adress=0x4C5, .data_offset=5, .data_length=1, .multiplier=1.5, .value_offset=60};

int16_t  volvo_convert_data(volvo_id_s conv_s, CAN_message_t message_in);   

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

int16_t volvo_convert_data(volvo_id_s conv_s, CAN_message_t message_in)
{
  int16_t converted = 0;
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
      converted = (int16_t)(tempvalue*conv_s.multiplier);   //Correct with multiplier   
  }
  return converted;
}

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

//#if (SSD1306_LCDHEIGHT != 64)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

void setup()   {                
  Serial.begin(1000000);

  filter.ext = 0;
  filter.id = rpm.adress;

  filter2.ext = 0;
  filter2.id = speed.adress;

  filter3.ext = 0;
  filter3.id = clt.adress;

  can.begin(filter);

  can.setFilter(filter, 0);
   can.setFilter(filter2, 1);
    can.setFilter(filter3, 2);
     can.setFilter(filter, 3);
      can.setFilter(filter, 4);
       can.setFilter(filter, 5);
        can.setFilter(filter, 6);
         can.setFilter(filter, 7);
  

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
 // display.display();
 // delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // draw a single pixel
//  display.drawPixel(10, 10, WHITE);
  // Show the display buffer on the hardware.
  // NOTE: You _must_ call display after making any drawing commands
  // to make them visible on the display hardware!

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Volvo ext.display..");
  display.display();
  delay(2000);
//  display.clearDisplay();

  // draw many lines
 // testdrawline();
 // display.display();
 // delay(2000);
 // display.clearDisplay();

  // draw rectangles
 // testdrawrect();
//  display.display();
///  delay(2000);
 // display.clearDisplay();

  // draw multiple rectangles
 // testfillrect();
 // display.display();
 // delay(2000);
 // display.clearDisplay();

  // draw mulitple circles
//  testdrawcircle();
//  display.display();
//  delay(2000);
///  display.clearDisplay();

  // draw a white circle, 10 pixel radius
///  display.fillCircle(display.width()/2, display.height()/2, 10, WHITE);
 // display.display();
 // delay(2000);
 // display.clearDisplay();

 // testdrawroundrect();
 // delay(2000);
 // display.clearDisplay();

 // testfillroundrect();
 // delay(2000);
 // display.clearDisplay();

 // testdrawtriangle();
//  delay(2000);
 // display.clearDisplay();
   
 // testfilltriangle();
 // delay(2000);
 // display.clearDisplay();

  // draw the first ~12 characters in the font
//  testdrawchar();
//  display.display();
//  delay(2000);
//  display.clearDisplay();

  // draw scrolling text
 // testscrolltext();
 // delay(2000);
 // display.clearDisplay();

  // text display tests
//  display.setTextSize(1);
//  display.setTextColor(WHITE);
//  display.setCursor(0,0);
//  display.println("Hello, world!");
//  display.setTextColor(BLACK, WHITE); // 'inverted' text
 // display.println(3.141592);
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.print("0x"); display.println(0xDEADBEEF, HEX);
//  display.display();
//  delay(2000);

  // miniature bitmap display
 // display.clearDisplay();
//  display.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
//  display.display();

  // invert the display
//  display.invertDisplay(true);
//  delay(1000); 
//  display.invertDisplay(false);
//  delay(1000); 

  // draw a bitmap icon and 'animate' movement
//  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
//  display.clearDisplay();
 display.setTextSize(7);
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
 
  if(can.read(message_in))
  {
    
    
    display.clearDisplay();

  //    if(message_in.id = 0x155)   
  //    {
  //      
   //   }
  
  //  display.setCursor(0,0);
  //  display.print("ID:");
  //  display.print(message_in.id);

    //display.setCursor(40,0);
    //display.print("Len:");
    //display.print(message_in.len);

    display.setCursor(0,0);
    //display.print("D:");
    display.print(volvo_convert_data(clt, message_in));
    
   // Serial.println(message_in.id);
    display.display();
  }

  Serial.println("Loop...");     // string

  delay(1);
}


void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    display.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display.height()) {
	icons[f][XPOS] = random(display.width());
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
      display.println();
  }    
  display.display();
}

void testdrawcircle(void) {
  for (int16_t i=0; i<display.height(); i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, WHITE);
    display.display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display.height()/2; i+=3) {
    // alternate colors
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, color%2);
    display.display();
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    display.display();
  }
}

void testfilltriangle(void) {
  uint8_t color = WHITE;
  for (int16_t i=min(display.width(),display.height())/2; i>0; i-=5) {
    display.fillTriangle(display.width()/2, display.height()/2-i,
                     display.width()/2-i, display.height()/2+i,
                     display.width()/2+i, display.height()/2+i, WHITE);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, WHITE);
    display.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = WHITE;
  for (int16_t i=0; i<display.height()/2-2; i+=2) {
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i, display.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display.display();
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
  }
}

void testdrawline() {  
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, WHITE);
    display.display();
  }
  delay(250);
  
  display.clearDisplay();
  for (int16_t i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, WHITE);
    display.display();
  }
  for (int16_t i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, WHITE);
    display.display();
  }
  delay(250);

  display.clearDisplay();
  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, WHITE);
    display.display();
  }
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, WHITE); 
    display.display();
  }
  delay(250);
}

void testscrolltext(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println("scroll");
  display.display();
 
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);    
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}
