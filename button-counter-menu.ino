//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
  byte bP = 0;
  byte rBp = 0;
  int counterup;
void setup()
{
pinMode(7,INPUT_PULLUP);
  
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
}


void loop()
{

  
 bP=digitalRead(7);
 
 while(bP==0){
 bP=digitalRead(7);
  
 if(bP==0){
   rBp=1;  
  }else{
// bP=digitalRead(7);
    if((rBp==1)&&(bP==1)){
      counterup++;
   lcd.setCursor(2,3);
   lcd.print(counterup);
      rBp=0;
    }  
  }
 } 
}
