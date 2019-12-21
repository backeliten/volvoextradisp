#include <FlexCAN.h>
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

volvo_id_s  rpm {.adress=0x155, .data_offset=4, .data_length=2, .multiplier=1.0, .value_offset=0};
volvo_id_s  speed {.adress=0x145, .data_offset=6, .data_length=2, .multiplier=1.0, .value_offset=0};
volvo_id_s  clt {.adress=0x4C5, .data_offset=5, .data_length=1, .multiplier=1.0, .value_offset=60};

int16_t       tempvalue;
int16_t       clt_temp;
int16_t       rpm_temp;
int16_t       spd_temp;
uint8_t       KeyOn = 0;
uint8_t       button_count = 0;

#define VERSION       "2.4"
#define BUTTON_PIN    A6


int16_t  volvo_convert_data(volvo_id_s conv_s, CAN_message_t message_in, int16_t *value);  

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

class ExampleClass : public CANListener 
{
public:
   void printFrame(CAN_message_t &frame, int mailbox);
   void gotFrame(CAN_message_t &frame, int mailbox); //overrides the parent version so we can actually do something
};

void ExampleClass::printFrame(CAN_message_t &frame, int mailbox)
{
   Serial.print(frame.id);
   Serial.print(" ");
   Serial.print(frame.len);
   Serial.print(" ");
   for (int c = 0; c < frame.len; c++) 
   {
      Serial.print(frame.buf[c], HEX);
      Serial.write(' ');
   }
   Serial.write('\r');
   Serial.write('\n');
}

void ExampleClass::gotFrame(CAN_message_t &frame, int mailbox)
{
    printFrame(frame, mailbox);

      if(frame.id == 0x035)   
      {
          if((frame.buf[0] & 0x0F) == 0x04)
          {
            KeyOn = 1;      //Key is on!
          }
          else
          {
            KeyOn = 0;    //Key is off!
            
          }
      }

  //display.setCursor(0,0);
  //display.clearDisplay();
      
  if(button_count == 0)
  {
    //display.println("CLT");
    //display.display();
      if(volvo_convert_data(clt, frame, &tempvalue) == 0)
      {
          if(KeyOn == 1)
          {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.println("CLT");
            display.setTextSize(5);
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
      if(volvo_convert_data(rpm, frame, &tempvalue) == 0)
      {
          if(KeyOn == 1)
          {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.println("RPM");
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
      if(volvo_convert_data(speed, frame, &tempvalue) == 0)
      {
          if(KeyOn == 1)
          {
            display.clearDisplay();
            display.setCursor(0,0);
            display.setTextSize(2);
            display.println("SPEED");
             display.setTextSize(3);
            //display.print(tempvalue);
            display.print(tempvalue/100);   //Since we got the speed in millispeed, we need to divide by 100 to get real km/h.
            display.display();
          }
          else
          {
            display.clearDisplay();
            display.display();
          }
       }
  }

  if(button_count == 3)
  {
    //display.println("SPEED");
   // display.display();
      if(volvo_convert_data(speed, frame, &spd_temp) == 0)
      {}
      if(volvo_convert_data(clt, frame, &clt_temp) == 0)
      {}
      if(volvo_convert_data(rpm, frame, &rpm_temp) == 0)
      {}
        
      if(KeyOn == 1)
      {
        display.clearDisplay();
        display.setCursor(0,0);
        
        display.setTextSize(2);
        display.println("CLT");
        display.setTextSize(3);
        display.println(clt_temp);

        display.setTextSize(2);
        display.println("RPM");
        display.setTextSize(3);
        display.println(rpm_temp);

        display.setTextSize(2);
        display.println("Speed");
        display.setTextSize(3);
        display.println(spd_temp);
        
        display.display();
      }
      else
      {
        display.clearDisplay();
        display.display();
      }
  }

    
}

ExampleClass exampleClass;

// -------------------------------------------------------------
void setup(void)
{
  delay(1000);
  Serial.println(F("Volvodisp at your service"));

  pinMode(BUTTON_PIN, INPUT);    //A6

  Can0.begin(125000);  

  //if using enable pins on a transceiver they need to be set on
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  // Clear the buffer.
  display.clearDisplay();
  
  display.setTextSize(0);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Volvo ext.display..");
  display.println(VERSION);

  //display.drawFastHLine(0,128,30,WHITE);

//  for(int p=0;p<128;p++)
//  {
//    display.drawPixel(p,30,WHITE);
//  }

 //  display.drawLine(30, 30, 40, 40, WHITE);
  
  display.display();
  delay(2000);

 //display.setTextSize(7);
  display.setTextSize(1);
 display.setTextColor(WHITE);

  Can0.attachObj(&exampleClass);
  exampleClass.attachGeneralHandler();
}


// -------------------------------------------------------------
void loop(void)
{
  delay(10);
  //Serial.write('.');
     if(digitalRead(BUTTON_PIN) == LOW)
  {
      button_count++;

      if(button_count == 4)
      {
        button_count = 0;   //Clear button
      }
      delay(200);     //Bounce delay
  }
}
