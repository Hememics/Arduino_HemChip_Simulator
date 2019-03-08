
rduino_HemChip_Simulator
This simulator is meant to step through a test file and set resistance values based on a 3-wire command scheme. 

## Installation
For the simulator you will need:
an Arduino, an [ad5242 digipot](https://www.analog.com/media/en/technical-documentation/data-sheets/AD5241_5242.pdf), and [WSWire](https://github.com/steamfire/WSWireLib). WSWire is I2C bus code to replace what is commonly used now. Big difference is a breakout from endless loops that would otherwise lock up the Arduino.

## Usage
Currently the simulator requires a terminal connection to start working.
Any micro controller that has 5v GPIO pins or a level shifter should work for an input. The simulator can be configured in the Arduino_HemChip_Simulator.ino file,
```
const int COM_INT_PIN = 2; //pin input for starting coms
const int COM_PIN_1 = 19; //MSB pin
const int COM_PIN_2 = 18; //LSB pin
```

There is also a tester Arduino file that can be used to send commands to the simulator. Just open up a terminal attached to the tester and enter a number from 0-3.

## Command Scheme
1. set com int pin to high
2. set one of the com wires to high 
3. punctuate each command by setting both com wires to high
