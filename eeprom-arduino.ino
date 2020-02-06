#include <EEPROM.h>
// Absolute min and max eeprom addresses. Actual values are hardware-dependent.
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

void setup()
{
  byte ch;
  ch=EEPROM.read(0);
  Serial.begin(9600);
  pinMode(PA4,INPUT_PULLUP);
  if (ch!='H'){
  Serial.println("Saving string to address 0");
  Serial.println("Write data to EEPROM");
  EEPROMwrite(address,"Hello World");
  }
  delay(1000);
}

void loop()
{
  if(digitalRead(PA4)==HIGH){
//  Serial.println("Write data to EEPROM");
//  EEPROMwrite(address,"Hello World");
  delay(1000);
  }
  Serial.println("Read data From EEPROM");
  Serial.println(EEPROMread(address,11));
  delay(1000);
}

String EEPROMread(int StartAddr, int StringLength){
   char buf[StringLength + 1];
   eeprom_read_string(StartAddr, buf, StringLength + 1);
   String dataStr = buf;
   return dataStr;
}

void EEPROMwrite(int StartAddr, String DataString){
   int val = DataString.length()+ 1;
   char StringChar[val];
   char buff[val];
   //convert string to char array
   DataString.toCharArray(StringChar, val);
   strcpy(buff, StringChar);
   eeprom_write_string(StartAddr, buff); 
}
