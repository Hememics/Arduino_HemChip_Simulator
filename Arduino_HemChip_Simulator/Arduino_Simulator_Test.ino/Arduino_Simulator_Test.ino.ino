const int COM_INT_PIN = 2;
const int COM_PIN_1 = 12; //MSB
const int COM_PIN_2 = 13; //LSB

#define NEXT_CIRCUIT 0b01
#define NEXT_TIME_STEP 0b10
#define STOP 0b11
#define WAIT 0b00

#define START_COMS digitalWrite(COM_INT_PIN, HIGH); delay(25); digitalWrite(COM_INT_PIN, LOW);

int incomingByte = 0;
void writeToPins(int command){
  if(command < 4 && command > -1){
    Serial.println(command);
      digitalWrite(COM_PIN_2, command & 1);
      digitalWrite(COM_PIN_1, (command >> 1) & 1);
  }
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){
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
                //if(i%9 == 0){
                START_COMS
               // }
                writeToPins(command);
                delay(10);
                writeToPins(WAIT);
                delay(10);
                /*writeToPins(STOP);
                delay(50);
                writeToPins(WAIT);
                 delay(10);
                writeToPins(STOP);
                delay(50);
                writeToPins(WAIT);
                */
                i++;
        }
}
