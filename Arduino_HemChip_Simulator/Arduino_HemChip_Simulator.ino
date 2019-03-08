#include <WSWire.h> 
//https://github.com/steamfire/WSWireLib
//please use this wire library for I2C. the default one has many issues
//https://www.analog.com/media/en/technical-documentation/data-sheets/AD5241_5242.pdf
//this is the data sheet for the specific digipot we are using
#include <SPI.h>
#include <SD.h>
  
#define NUM_CIRCUITS 81
#define CMD_BUFFER_SIZE 1

const int SD_SELECT_PIN = 4;
const int LCD_SELECT_PIN = 10;
const int CH_CIRCUIT_PIN = 7;

const int COM_INT_PIN = 2;
const int COM_PIN_1 = 19;
const int COM_PIN_2 = 18;

uint8_t data_chunk[NUM_CIRCUITS];
// the command_buffer and data_chunk are left as arrays for possible extension
int command_buf[CMD_BUFFER_SIZE];


#define DIGIPOTS_ERROR_CODE     2011
#define DIGIPOTS_ADDRESS        0b101100                //0b101100 This is 44 in decimal. The last bit is reserved for ~WRITE or READ. This is the I2C address of the chip.

#define NEXT_CIRCUIT 0b01
#define NEXT_TIME_STEP 0b10
#define STOP 0b11
#define WAIT 0


 
void interpret_cmds(void){
  File dataFile = SD.open("test.bin"); //open up our test file 
  static uint8_t data_index = 0; // static so we can keep track of where we are in the data
  dataFile.read(data_chunk, sizeof(data_chunk));
  for(int i = 0; i < CMD_BUFFER_SIZE; i++){
  
    //Serial.print("data_index: ");
    //Serial.println(data_index);
    //Serial.println(data_chunk[data_index]);
    writeToDigipots(data_chunk[data_index]);
    switch(command_buf[i]){
      case NEXT_CIRCUIT:
        if(data_index < NUM_CIRCUITS)
         data_index++;
        else{
          data_index = 0;
          dataFile.read(data_chunk, sizeof(data_chunk));
        }
        break;
      case NEXT_TIME_STEP:
        dataFile.read(data_chunk, sizeof(data_chunk));
        data_index = 0;
        break;
    }
    delay(150);
    
  }
  
}
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
  Serial.println("ready to receive\n");
  //delay(500);
  while(digitalRead(COM_INT_PIN) != HIGH){
    ;             // wait until the communications are ready
  }
  while(command != STOP && index < CMD_BUFFER_SIZE ){
    command = 0;
    command += (digitalRead(COM_PIN_1)*2);
    command += (digitalRead(COM_PIN_2)*1);
    Serial.println(command);
    //delay(500);
    switch(command){
      case NEXT_CIRCUIT:
        //Serial.println("NEXT_CIRCUIT\n");
        command_buf[index] = command;
        index++;
        break;
      case NEXT_TIME_STEP:
        //Serial.println("NEXT_TIME_STEP\n");
        command_buf[index] = command;
        index++;
        break;
      default:
        //Serial.println("FAIL\n");   
  }
}
  interpret_cmds(); // start insterpretting commands after they have been received
}

int writeToDigipots( uint8_t setting)
    { 
    //Serial.println(setting);
    uint8_t instructionByte; 
 
    
    
    instructionByte = 0b00000000;
     
 
    if (setting > 255) return (DIGIPOTS_ERROR_CODE);                     //failure
    if (setting < 0) return (DIGIPOTS_ERROR_CODE);                       //failure
 
    Wire.beginTransmission(DIGIPOTS_ADDRESS);     // transmit to device # 47
    Wire.write((uint8_t)instructionByte);         // sends instruction byte
    Wire.write((uint8_t)setting);                 // sends potentiometer value byte
    //Serial.print("WSWire exit status: ");
    Serial.println(Wire.endTransmission());       // stop transmitting
   return (1);                                    // success
}
// digipot testing function. useful for testing digipots
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
  
  //setting up pins
  pinMode(LCD_SELECT_PIN, OUTPUT);
  pinMode(SD_SELECT_PIN, OUTPUT);
  pinMode(CH_CIRCUIT_PIN, OUTPUT);
  pinMode(COM_PIN_1, INPUT_PULLUP);
  pinMode(COM_INT_PIN, INPUT_PULLUP);
  pinMode(COM_PIN_2, INPUT_PULLUP);
  digitalWrite(LCD_SELECT_PIN, HIGH);
  digitalWrite(SD_SELECT_PIN, LOW);
  digitalWrite(CH_CIRCUIT_PIN, LOW);
 
  Serial.println("Starting SD card");
  
  while (!SD.begin(SD_SELECT_PIN)) {
    Serial.println("Card failed!");
    delay(1000);
  }
  Serial.println("SD card ready");
}
  
void loop() {
   receive_command();
}
