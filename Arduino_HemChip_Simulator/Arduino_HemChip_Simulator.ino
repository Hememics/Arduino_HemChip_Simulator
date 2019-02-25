
#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#define NUM_CIRCUITS 81

const int SD_SELECT_PIN = 4;
const int LCD_SELECT_PIN = 10;
const int CH_CIRCUIT_PIN = 7;

char data_chunk[NUM_CIRCUITS];

#define DIGIPOTS_ERROR_CODE     2011
#define DIGIPOTS_ADDRESS        0b101100                //0b101100 This is 44 in decimal. The last bit is reserved for ~WRITE or READ. This is the I2C address of the chip.

int writeToDigipots( uint8_t setting)
    { 
    uint8_t instructionByte; 
 
    
    
    instructionByte = 0b00000000;
     
 
    if (setting > 255) return (DIGIPOTS_ERROR_CODE);                     //failure
    if (setting < 0) return (DIGIPOTS_ERROR_CODE);                       //failure
 
    Wire.beginTransmission(DIGIPOTS_ADDRESS);     // transmit to device # 47
    Wire.write((uint8_t)instructionByte);         // sends instruction byte
    Wire.write((uint8_t)setting);                 // sends potentiometer value byte
 
    Wire.endTransmission();                       // stop transmitting
 
   return (1);                                   // success
}

void test_digipot(){
  uint8_t set = 255;
  while(true){
     digitalWrite(CH_CIRCUIT_PIN, HIGH);
    unsigned long digipot_start_time = micros();
    Serial.println(set);
    writeToDigipots(set);
    
    unsigned long digipot_end_time = micros();
    digitalWrite(CH_CIRCUIT_PIN, LOW);
    set = ~set;
    delay(10);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  while(!Serial) {
    ;
  }


  //test_digipot();lo,l

  pinMode(LCD_SELECT_PIN, OUTPUT);
  pinMode(SD_SELECT_PIN, OUTPUT);
  pinMode(CH_CIRCUIT_PIN, OUTPUT);
  digitalWrite(LCD_SELECT_PIN, HIGH);
  digitalWrite(SD_SELECT_PIN, LOW);
  digitalWrite(CH_CIRCUIT_PIN, LOW);
 
  Serial.println("Starting SD card");
  
  while (!SD.begin(SD_SELECT_PIN)) {
    Serial.println("Card failed!");
    delay(1000);
  }
  Serial.println("SD card ready");

  File dataFile = SD.open("test.bin");

  int i = 0;
  if (dataFile) {
    char buff;
    bool done = false;

    dataFile.read(data_chunk, sizeof(data_chunk));
    while (!done) {

      for (int iCircuit = 0; iCircuit < NUM_CIRCUITS; iCircuit++) {
        digitalWrite(CH_CIRCUIT_PIN, HIGH);
        unsigned long digipot_start_time = micros();
        writeToDigipots( data_chunk[i] );
        unsigned long digipot_end_time = micros();
        digitalWrite(CH_CIRCUIT_PIN, LOW);

        //Serial.print("Time to write to digipot: ");
        //Serial.println(digipot_end_time - digipot_start_time);
        
        //delay(100);        
      }

      unsigned long read_start_time = micros();
      if (dataFile.read(data_chunk, sizeof(data_chunk)) <= 0) {
        done = true;
      }else{
        unsigned long read_end_time = micros();
        Serial.print("Time to read 81 bytes from file: ");
        Serial.println(read_end_time - read_start_time);
        //delay(1000);
      }
      
    }
    Serial.println("\r\nDone reading file");
  }else{
    Serial.println("File open error!");
  }

  dataFile.close();
}

void loop() {
  // put your main code here, to run repeatedly:

}
