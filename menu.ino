#include <LiquidCrystal.h>
#include <EEPROM.h>
//LiquidCrystal lcd(12, 11, 10, 5, 4, 3, 2); // ARDUINO UNO
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //mega

byte buttons[] = {9,10,11,12,13}; //10 for additional pin
const byte nrButtons = 5; // change to 4 if additional button added
int t;
byte pressedButton;//, currentPos,currentPosParent, possiblePos[20], possiblePosCount, possiblePosScroll = 0;
String menu[]={
  "Over Voltage",
  "Under Voltage",
  "Over Curent"
  };
byte UVVal=0,OVVal=0,OCVal=0;
int editvolt=1,
    editcurrent=2,
    eUVVal=1,
    eOVVal=2,
    eOCVal=3,
    isview=3,statusmenu;
    

//=================================================================================================================================================================================================================================

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4095;
boolean eeprom_is_addr_ok(int addr) {
return ((addr >= EEPROM_MIN_ADDR) && (addr <= EEPROM_MAX_ADDR));
}

boolean eeprom_write_bytes(int startAddr, const byte* array, int numBytes)
{
int i;
if (!eeprom_is_addr_ok(startAddr) || !eeprom_is_addr_ok(startAddr + numBytes)) {
return false;
}
for (i = 0; i < numBytes; i++) {
EEPROM.write(startAddr + i, array[i]);}
return true;
}

boolean eeprom_write_string(int addr, const char* string) {
int numBytes; // actual number of bytes to be written
//write the string contents plus the string terminator byte (0x00)
numBytes = strlen(string) + 1;
return eeprom_write_bytes(addr, (const byte*)string, numBytes);
}

boolean eeprom_read_string(int addr, char* buffer, int bufSize) {
byte ch; // byte read from eeprom
int bytesRead; // number of bytes read so far
if (!eeprom_is_addr_ok(addr)) { // check start address
return false;
}

if (bufSize == 0) { // how can we store bytes in an empty buffer ?
return false;
}
// is there is room for the string terminator only, no reason to go further
if (bufSize == 1) {
buffer[0] = 0;
return true;
}
bytesRead = 0; // initialize byte counter
ch = EEPROM.read(addr + bytesRead); // read next byte from eeprom
buffer[bytesRead] = ch; // store it into the user buffer
bytesRead++; // increment byte counter

while ( (ch != 0x00) && (bytesRead < bufSize) && ((addr + bytesRead) <= EEPROM_MAX_ADDR) ) {
// if no stop condition is met, read the next byte from eeprom
ch = EEPROM.read(addr + bytesRead);
buffer[bytesRead] = ch; // store it into the user buffer
bytesRead++; // increment byte counter
}
// make sure the user buffer has a string terminator, (0x00) as its last byte
if ((ch != 0x00) && (bytesRead >= 1)) {
buffer[bytesRead - 1] = 0;
}
return true;
}
  int address = 0;


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

void setup() {
  // put your setup code here, to run once:
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


  pinMode(8,OUTPUT);
  digitalWrite(8,0); 
  
  lcd.begin(16,2); 
  lcd.clear();
  

  lcd.setCursor(0,0); lcd.print("Hellow");
  //delay(1000);
OVVal=EEPROM.read(1);
UVVal=EEPROM.read(2);
OCVal=EEPROM.read(3);

}

void loop() {
  // put your main code here, to run repeatedly:
  pressedButton = checkButtonPress();
  if (pressedButton !=0) {
    switch (pressedButton) {
      case 1:
        //lcd.setCursor(0,0); lcd.print("Button 1"); //up +++
        
        switch (statusmenu){
          case 1:if(OVVal<100){ OVVal++;}
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(OVVal);          
          break;
          case 2:if (UVVal<100){ UVVal++;}
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(UVVal);          
          break;
          lcd.setCursor(0,1);lcd.print("                ");
          case 3:if (OCVal<100){OCVal++;}
          lcd.setCursor(0,1); lcd.print(OCVal);          
          break;
          
         }
      break;
      // If I wanted a 4 button controll of the menu
      case 2:
//        lcd.setCursor(0,0); lcd.print(
        switch (statusmenu){
          case 1:if(OVVal>0){ OVVal--;}
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(OVVal);          
          break;
          case 2:if (UVVal>0){ UVVal--;}
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(UVVal);          
          break;
          case 3:if (OCVal>0){OCVal--;}
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(OCVal);          
          break;
          
         }
      break;
      case 3:
        //lcd.setCursor(0,0); lcd.print("Button 3");//right
      break;

//--------------------------------------------------------------------      
      
      case 4:
        //lcd.setCursor(0,0); lcd.print("Button 4");//left
        switch (statusmenu){
          case 1:
          EEPROM.write(1,OVVal);
          break;
          case 2:
          EEPROM.write(2,UVVal);
          break;
          case 3:
          EEPROM.write(3,OCVal);
          break;
        }
      break;

      
//--------------------------------------------------------------------      
      
      case 5:
          lcd.setCursor(0,1);lcd.print("                ");
        lcd.setCursor(0,0);
        
        if (statusmenu<3){statusmenu++;}else statusmenu=0;
        switch (statusmenu){
          case 1:
          lcd.print("Over Voltage  ");
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(OVVal);          
          break;
          case 2:
          lcd.print("Under Voltage  ");
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(UVVal);          
          break;
          case 3:
          lcd.print("Over Current  ");
          lcd.setCursor(0,1);lcd.print("                ");
          lcd.setCursor(0,1); lcd.print(OCVal);          
          
          break;
          case 0:
          lcd.print("Normal View      ");
          lcd.setCursor(0,1);lcd.print("                ");
          break;
        }
      break;
      
    }
  }



  

}
