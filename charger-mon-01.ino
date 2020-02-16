#include <ADS1115.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Wire.h>

 ADS1115 adc;  /* Use this for the 16-bit version */
//LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2); // ARDUINO UNO
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //mega
LiquidCrystal_I2C lcd2(0x27, 20, 4);
const byte 
  btLeft=9,
  btRight=10,
  btUp=11,
  btDown=12,
  btOk=13;
byte buttons[] = {9,10,11,12,13}; //10 for additional pin
const byte nrButtons = 5; // change to 4 if additional button added
int t;
byte pressedButton;
String FirstLine="Voltage Current     ";
String      sMenu="Menu";//16
const byte  cMenu=16;
String      sOVP="OVP";
const byte  cOVP=17;
String      sUVP="UVP";
const byte  cUVP=17;
String      sOCP="OCP";
const byte  cOCP=17;
String      sVolt="VOLTAGE";//0
const byte  cVolt=0;
String      sCurrent="CURRENT";//8
const byte  cCurrent=8;
char sRun;
const byte  cpvVolt=0;
const byte  cpvCurrent=10;


int UVVal=0,OVVal=0,OCVal=0;
int editvolt=1,
    editcurrent=2,
    eUVVal=1,
    eOVVal=2,
    eOCVal=3,
    isview=4,
    statusmenu,cstatusmenu;
    

//=================================================================================================================================================================================================================================

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4095;
void EEPROMWriteInt(int address, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value /256) & 0xFF);
  
  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}

int EEPROMReadInt(int address)
{
  byte two = EEPROM.read(address);
  byte one = EEPROM.read(address + 1);

  return ((two ) & 0xFFFF) + ((one *256) & 0xFFFF);
}
//=================================================================================================================================================================================================================================


byte checkButtonPress() {
  byte bP = 0;
  byte rBp = 0;
  for (t = 0; t<nrButtons;t++) {
    if (digitalRead(buttons[t]) == 0) {bP = (t + 1);}
  }
  rBp = bP;
  while (bP != 0) { // wait while the button is still down
    bP = 0;
    for (t = 0; t<nrButtons;t++) {
      if (digitalRead(buttons[t]) == 0) {bP = (t + 1);}
    }
  }
  return rBp;
}
float aadc0, // Volt
      aadc1,  // current
      realcurrent,
      maxvoltref
      ;
int mVperAmp = 66,i=0;      
  float adc0, adc1, adc2, adc3,adc1_per_max,v,halfref,avolt,aamp;
  ADS1115ScaleFloat scale0,scale1;
byte customBackslash[8] = {
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};
void setup() {
  lcd2.createChar(7, customBackslash);
   
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Hello!");
  Serial.println("Getting single-ended readings from AIN0..3");
  Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");
//   ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default) // 
   //ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV// ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV// ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV// ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
 //  ads.setSPS(ADS1015_REG_CONFIG_DR_3300SPS)
  
//ads.begin();
 //  ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default) // 
 //  ads.setSPS(ADS1015_REG_CONFIG_DR_3300SPS)
//   ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
Wire.begin();
adc.setSpeed(ADS1115_SPEED_860SPS);//ADS1115_SPEED_8SPS 
 // adc.setSpeed(ADS1115_SPEED_250SPS );//ADS1115_SPEED_8SPS 

pinMode(9,INPUT_PULLUP);
pinMode(10,INPUT_PULLUP);
pinMode(11,INPUT_PULLUP);
pinMode(12,INPUT_PULLUP);
pinMode(13,INPUT_PULLUP);
//undervoltagevalue=0;overvoltagevalue=0;overcurrentvalue=0;
int show=1;
int edit=2;
int save=3;
int status;
  lcd2.init();
  lcd2.backlight();
  lcd2.setCursor(cVolt,0);
  //lcd2.print(sVolt);
  lcd2.setCursor(cCurrent,0);
  //lcd2.print(sCurrent);

  pinMode(8,OUTPUT);
  digitalWrite(8,0); 
  lcd2.createChar(7, customBackslash);
  
  lcd.begin(16,2); 
  lcd.clear();
  lcd2.setCursor(0,0);
  lcd2.print(FirstLine);
  //        EEPROMWriteInt(2,1300); //one time set //130.0 volt default value for initializing new hardware
  //        EEPROMWriteInt(4,1000); //100.0 volt
  //        EEPROMWriteInt(8,100); // 10.0 volt
  
  OVVal=EEPROMReadInt(2);
  UVVal=EEPROMReadInt(4);
  OCVal=EEPROMReadInt(8);
  cstatusmenu=isview;
}
int irun=0;
void loop() {
  if(irun>=3){irun=0;}else {irun++;}
  switch (irun){
    case 0:sRun='-';
    break;
    case 1:sRun=7;
    break;
    case 2:sRun='|';
    break;
    case 3:sRun='/';
    break;
  }
  
  if(cstatusmenu==isview){
  lcd2.setCursor(19,0);
  lcd2.print(sRun);

  v=0;
  avolt=0;
  maxvoltref=0;
  pressedButton = checkButtonPress();
  if (pressedButton ==0) { // jika ada press button abaikan proses
      for(int ii=0;ii<13;ii++){
        v        +=adc.convert(ADS1115_CHANNEL1, ADS1115_RANGE_6144);// +-6144
        avolt    +=adc.convert(ADS1115_CHANNEL0, ADS1115_RANGE_6144);// +-6144
        adc3     +=adc.convert(ADS1115_CHANNEL3, ADS1115_RANGE_6144);// +-6144
      maxvoltref +=adc.convert(ADS1115_CHANNEL2, ADS1115_RANGE_6144);
      } 
  }
  v=v/13;
  avolt=avolt/13;
  adc3=adc3/13;
  maxvoltref=maxvoltref/13;
  halfref=maxvoltref/2;
 
scale0.setRef(halfref+2.5, 0,maxvoltref, 30.000);    
scale1.setRef(0,0,maxvoltref,150.000);
  
  //adc1_per_max =adc.convertAutoScale(ADS1115_CHANNEL1, 10);//(ads.readADC_SingleEnded(1)*1000/ads.readADC_SingleEnded(2));
  //realcurrent=
     
  //adc1 = adc.convert(ADS1115_CHANNEL1, ADS1115_RANGE_6144);//adc.convertAutoScale(ADS1115_CHANNEL1, 10);
//  }
//20-02-16-21:25
//  if (cstatusmenu==isview){

  if (scale1.scale(avolt)<(UVVal/10.0)){ // check Over Voltage
    lcd2.setCursor(cVolt,3);
    lcd2.print("UV ALARM");
   }
   else{
    lcd2.setCursor(cVolt,3);
    lcd2.print("        ");
   }
   
  if (scale1.scale(avolt)>(OVVal/10.0)){ // check Under Voltage
    lcd2.setCursor(cVolt,1);
    lcd2.print("OV ALARM");
   }
   else{
    lcd2.setCursor(cVolt,1);
    lcd2.print("        ");
   }

  if (scale0.scale(v)>(OCVal/10.0)){ // check Over Current
    lcd2.setCursor(cCurrent,1);
    lcd2.print("OC ALARM");
   }
   else{
    lcd2.setCursor(cCurrent,1);
    lcd2.print("        ");
   }

    kirimmsg();
    lcd2.setCursor(cVolt,2);
    lcd2.print(scale1.scale(avolt),1); //new
    lcd2.setCursor(cCurrent,2);
    lcd2.print(scale0.scale(v),1); /////real
 }

  
  pressedButton = checkButtonPress();
  if (pressedButton !=0) {
    cstatusmenu=eOVVal;
    switch (pressedButton) {
      // If I wanted a 4 button controll of the menu
      case 1: //Left - Up button
        switch (statusmenu){
          case 1:if(OVVal>0){ OVVal--;}
            lcd2.setCursor(0,1);lcd2.print("                ");
            lcd2.setCursor(0,1);lcd2.print((OVVal/10.0),1);          
          break;
          case 2:if (UVVal>0){ UVVal--;}
            lcd2.setCursor(0,1);lcd2.print("                ");
            lcd2.setCursor(0,1);lcd2.print((UVVal/10.0),1);          
          break;
          case 3:if (OCVal>0){OCVal--;}
            lcd2.setCursor(0,1);lcd2.print("                ");
            lcd2.setCursor(0,1);lcd2.print((OCVal/10.0),1);          
          break;
         }
      break;
      case 2: // Right Down
        //lcd.setCursor(0,0); lcd.print("Button 1"); //up +++
        switch (statusmenu){
          case 1:if(OVVal<10000){ OVVal++;}
            lcd2.setCursor(0,1); lcd2.print("                ");
            lcd2.setCursor(0,1); lcd2.print((OVVal/10.0),1);          
          break;
          case 2:if (UVVal<10000){ UVVal++;}
            lcd2.setCursor(0,1); lcd2.print("                ");
            lcd2.setCursor(0,1); lcd2.print((UVVal/10.0),1);          
          break;
          case 3:if (OCVal<10000){OCVal++;}
            lcd2.setCursor(0,1); lcd2.print("                ");
            lcd2.setCursor(0,1); lcd2.print((OCVal/10.0),1);          
          break;
          
         }
      break;
      case 3:
        // saving an edited parameter
        switch (statusmenu){
          case 1:
          EEPROMWriteInt(2,OVVal);
          break;
          case 2:
          EEPROMWriteInt(4,UVVal);
          break;
          case 3:
          EEPROMWriteInt(8,OCVal);
          break;
        }
      break;
      //end saving / update parameter
      
      case 4:// Menu
          lcd2.setCursor(0,1);lcd2.print("                ");
          lcd2.setCursor(0,0);
          if (statusmenu<3){statusmenu++;}else statusmenu=0;

          switch (statusmenu){
            case 1:
              lcd2.print("Over Voltage Set ");
              lcd2.setCursor(0,1); lcd2.print("                ");
              lcd2.setCursor(0,1); lcd2.print((OVVal/10.0),1);          
              cstatusmenu=eOVVal;            
            break;
            case 2:
              lcd2.print("Under Voltage Set");
              lcd2.setCursor(0,1); lcd2.print("                ");
              lcd2.setCursor(0,1); lcd2.print((UVVal/10.0),1);          
              cstatusmenu=eUVVal;            
            break;
            case 3:
              lcd2.print("Over Current Set ");
              lcd2.setCursor(0,1); lcd2.print("                ");
              lcd2.setCursor(0,1); lcd2.print((OCVal/10.0),1);          
              cstatusmenu=eOCVal;            //
            break;
            case 0:
              lcd2.setCursor(0,0);
              lcd2.print(FirstLine);
              lcd2.setCursor(0,1);
              lcd2.print("                    ");
              lcd2.setCursor(0,2);
              lcd2.print("                    ");
              lcd2.setCursor(0,3);
              lcd2.print("                    ");
              cstatusmenu=isview;             //
/*
            eUVVal=1,
            eOVVal=2,
            eOCVal=3,
            isview=4,

  lcd2.setCursor(0,0);
  lcd2.print(FirstLine);


            
*/
            lcd2.setCursor(0,1);lcd2.print("                ");
            break;
          }
      break; // case 4 Main menu
      
    }
  }
}// loop

void kirimmsg(void){
  /*
  Serial.print("AIN0: raw  "); Serial.println(adc0);
  Serial.print("AIN1: raw  "); Serial.println(adc1);
  Serial.print("AIN0: calc "); Serial.println(aadc0);
  Serial.print("AIN1: real "); Serial.println(adc.convert(ADS1115_CHANNEL1, ADS1115_RANGE_6144));
  Serial.print("AIN1: mv   "); Serial.println(scale0.scale(v),4);
  Serial.print("AIN0: scle "); Serial.println(scale1.scale(avolt),4);
  Serial.print("AIN2:      "); Serial.println(adc2);
  Serial.print("AIN3:      "); Serial.println(adc3);
  Serial.print("Half Ref   "); Serial.println(halfref);
  */
  Serial.print("mvxref  [");  Serial.print(maxvoltref); Serial.println("]");
  Serial.print("mvhref  [");  Serial.print(halfref); Serial.println("]");
  Serial.print("AIN0 Raw[");  Serial.print(avolt); Serial.println("]");
  Serial.print("AIN1 Raw[");  Serial.print(v); Serial.println("]");
  Serial.print("AIN2 Raw[");  Serial.print(maxvoltref); Serial.println("]");
  Serial.print("AIN3 Raw[");  Serial.print(maxvoltref); Serial.println("]");
  Serial.print("Volt    [");  Serial.print(scale1.scale(avolt),1); Serial.println("]");
  Serial.print("Amp     [");  Serial.print(scale0.scale(v),2); Serial.println("]");
  Serial.print("AVolt   [");  Serial.print(v); Serial.println("]");

  
}
