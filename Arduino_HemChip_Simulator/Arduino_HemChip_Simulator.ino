//https://www.analog.com/media/en/technical-documentation/data-sheets/AD5241_5242.pdf

#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#define NUM_CIRCUITS 81

const int SD_SELECT_PIN = 4;
const int LCD_SELECT_PIN = 10;
const int CH_CIRCUIT_PIN = 7;

const int COM_INT_PIN = 2;
const int COM_PIN_1 = 12;
const int COM_PIN_2 = 13;

char data_chunk[NUM_CIRCUITS];

#define DIGIPOTS_ERROR_CODE     2011
#define DIGIPOTS_ADDRESS        0b101100                //0b101100 This is 44 in decimal. The last bit is reserved for ~WRITE or READ. This is the I2C address of the chip.

#define START 0b100
#define NEXT_CIRCUIT 0b001
#define NEXT_TIME_STEP 0b010
#define STOP 0b011

#define CMD_BUFFER_SIZE 10

int command_buf[CMD_BUFFER_SIZE];

void zero_cmd_buf(){
    for(int i = 0; i < CMD_BUFFER_SIZE; i++){
      command_buf[i] = 0;
    }
}
void print_cmd_buf(){
 for(int i = 0; i < CMD_BUFFER_SIZE; i++){
      Serial.println(command_buf[i]);
    }
}

void receive_command(){
  zero_cmd_buf();
  int index = 0;
  int command = 0; 
  Serial.println("HELLO\n");

while(command !=STOP && index < CMD_BUFFER_SIZE ){
  command = 0;
  //command += (digitalRead(COM_PIN_0)*4);
  command += (digitalRead(COM_PIN_1)*2);
  command += (digitalRead(COM_PIN_2)*1);
  Serial.println(command);

  switch(command){
      case START:
        Serial.println("START\n");
        break;
      case NEXT_CIRCUIT:
        Serial.println("NEXT_CIRCUIT\n");
        command_buf[index] = command;
        index++;
        break;
      case NEXT_TIME_STEP:
        Serial.println("NEXT_TIME_STEP\n");
        command_buf[index] = command;
        index++;
        break;
      case STOP:
        Serial.println("STOP\n");
        break;
      default:
        Serial.println("FAIL\n");   
  }
  delay(500);
}
  print_cmd_buf();
}

int writeToDigipots( uint8_t setting)
    { 
      Serial.println(setting);
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
    if(writeToDigipots(set)!= 1){
      Serial.println("fail\n");
      
    }
    
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
  //pinMode(COM_PIN_0, INPUT_PULLUP);
  pinMode(COM_PIN_1, INPUT_PULLUP);
  pinMode(COM_PIN_2, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(COM_INT_PIN), receive_command, HIGH);

  digitalWrite(LCD_SELECT_PIN, HIGH);
  digitalWrite(SD_SELECT_PIN, LOW);
  digitalWrite(CH_CIRCUIT_PIN, LOW);
 
  Serial.println("Starting SD card");
  //receive_command();
  
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
   //receive_command();
   delay(200);
  // put your main code here, to run repeatedly:

}
