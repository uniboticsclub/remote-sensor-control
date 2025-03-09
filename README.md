# Remote ESP32C6 Controller

This is a small project created by ULSIT's robotics club Unibotics. Its purpose is to learn how to connect to an ESP32 through bluetooth and send/receive commands. This particular code is able to receive commands that play animations with LEDs.


## Hardware for project:

1. One breadboard.
2. One red F5 LED.
3. One 41 Ohm resistor.
4. One 22 Ohm resistor.
5. Two male to female cables.
6. One Type-C ESP32-C6 WIFI Bluetooth Development Core Board.


### Connections 

|Component        |Connection Description                       |ESP32-C6/ Breadboard                 |
|-----------------|---------------------------------------------|-------------------------------------|
|Red LED (Anode)  |Positive leg (longer) of LED                 |Connect to **Pin-2** (ESP32-C6)      |
|Red LED (Cathode)|Negative leg (shorter) of LED                |Connect to **Serial Resistor Connection**     |
|SRC(41Ω + 22Ω)   |Limits the current to the LED                |One end to the **Cathode of the LED**|
|SRC(41Ω + 22Ω)   |Connect the other end of the resistor to GND |Connect to **GND** on ESP32-C6       |
|ESP32-C6 GND     |Ground of the ESP32-C6 board                 |Connect to **Resistor**              |

<br>

![alt text](./media/image.png)


## Software for the project 

1. An arduino IDE for code writing/compilation
- ESP32C6 Dev Module [arduino board]
- BLE [library for bluetooth conneciton]

2. Android Apps for connection
- Serial Bluetooth terminal - https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal [used for connecting to the ESP and sending commands]
- nRF Connect - https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp [used for creating a Bluetooth LE profile that the app above can use]

## Usage 

Once connected to the ESP32-C6, you can begin sending commands. Every command is made out of a fixed width string in the form "\<command\> \<value\>", where \<command\> is a string of 3 characters indicating a command and \<value\> is a string with a numeric value (such as 001, 034, 211). The numeric value must be padded to contain 3 digits. The whole command should be exactly 7 characters long. 


### Example

Mobile> `ra1 005`  
[The ESP now runs the blinking animation 5 times]  
ESP> `Command Executed`  


### Further Usage

The source code contains the available commands (only `ra1` for now) and the different error codes that might be returned to the bluetooth device if the command is not formatted properly.