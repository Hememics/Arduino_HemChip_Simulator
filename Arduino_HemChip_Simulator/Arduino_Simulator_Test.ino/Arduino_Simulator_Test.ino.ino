const int COM_INT_PIN = 2;
const int COM_PIN_1 = 12; //MSB
const int COM_PIN_2 = 13; //LSB

#define NEXT_CIRCUIT 0b01
#define NEXT_TIME_STEP 0b10
#define STOP 0b11
#define WAIT 0b00

#define START_COMS digitalWrite(COM_INT_PIN, HIGH); delay(25); digitalWrite(COM_INT_PIN, LOW);

/*Communication scheme:
 * first: set the COM_INT_PIN to high to signal to the sim you are ready to send data
 * next: set the 2 COM pins to whatever instruction you want (there are only 2 right now)
 * finally: set both COM pins to high to signal that you are done transmitting a command
 * repeat as necesarry through the test file
 */

int incomingByte = 0;
void writeToPins(int command){
  if(command < 4 && command > -1){
    Serial.println(command);
      digitalWrite(COM_PIN_2, command & 1);
      digitalWrite(COM_PIN_1, (command >> 1) & 1);
  }
  
}

void setup() {
  Serial.begin(9600);
  while(!Serial){ //wait until serial is ready
    ;
  }
  pinMode(COM_PIN_1, OUTPUT);
  pinMode(COM_PIN_2, OUTPUT);
  pinMode(COM_INT_PIN, OUTPUT);

}
int i = 0;
void loop() {
          // send data only when you receive data:
        if (Serial.available() > 0) {
                // read the incoming byte:
                incomingByte = Serial.read();
                int command = (int)incomingByte-48;
                Serial.println(command);
                if(command == STOP){
                  i = 0;
                }
                START_COMS
                writeToPins(command);
                delay(10);
                writeToPins(WAIT);
                delay(10);

                i++;
        }
}
