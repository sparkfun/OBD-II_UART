/*
* OBD-II-UART Quickstart Sketch
* Written by Ryan Owens for SparkFun Electronics 7/5/2011
* Updates for Arduino 1.0+ by Toni Klopfenstein
*
* Released under the 'beerware' license
* (Do what you want with the code, but if we ever meet then you buy me a beer)
*
* This sketch will grab RPM and Vehicle Speed data from a vehicle with an OBD port
* using the OBD-II-UART board from SparkFun electronics. The data will be displayed
* on a serial 16x2 LCD. See the tutorial at https://www.sparkfun.com/tutorials/294
* to learn how to hook up the hardware:
*
*/

#include <SoftwareSerial.h>

//Create an instance of the new soft serial library to control the serial LCD
//Note, digital pin 3 of the Arduino should be connected to Rx of the serial LCD.

SoftwareSerial lcd(2,3);

//This is a character buffer that will store the data from the serial port
char rxData[20];
char rxIndex=0;

//Variables to hold the speed and RPM data.
int vehicleSpeed=0;
int vehicleRPM=0;

void setup(){
  //Both the Serial LCD and the OBD-II-UART use 9600 bps.
  lcd.begin(9600);
  Serial.begin(9600);
  
  //Clear the old data from the LCD.
  lcd.write(254);
  lcd.write(1);  
  
  //Put the speed header on the first row.
  lcd.print("Speed: ");
  lcd.write(254);
  //Put the RPM header on the second row.
  lcd.write(128+64);
  lcd.print("RPM: ");
  
  //Wait for a little while before sending the reset command to the OBD-II-UART
  delay(1500);
  //Reset the OBD-II-UART
  Serial.println("ATZ");
  //Wait for a bit before starting to send commands after the reset.
  delay(2000);
  
  //Delete any data that may be in the serial port before we begin.
  Serial.flush();
}

void loop(){
  //Delete any data that may be in the serial port before we begin.  
  Serial.flush();
  //Set the cursor in the position where we want the speed data.
  lcd.write(254);
  lcd.write(128+8);
  //Clear out the old speed data, and reset the cursor position.
  lcd.print("        ");
  lcd.write(254);
  lcd.write(128+8);
  //Query the OBD-II-UART for the Vehicle Speed
  Serial.println("010D");
  //Get the response from the OBD-II-UART board. We get two responses
  //because the OBD-II-UART echoes the command that is sent.
  //We want the data in the second response.
  getResponse();
  getResponse();
  //Convert the string data to an integer
  vehicleSpeed = strtol(&rxData[6],0,16);
  //Print the speed data to the lcd
  lcd.print(vehicleSpeed);
  lcd.print(" km/h");
  delay(100);
  
  //Delete any data that may be left over in the serial port.
  Serial.flush();
  //Move the serial cursor to the position where we want the RPM data.
  lcd.write(254);
  lcd.write(128 + 69);
  //Clear the old RPM data, and then move the cursor position back.
  lcd.print("          ");
  lcd.write(254);
  lcd.write(128+69);

  //Query the OBD-II-UART for the Vehicle rpm
  Serial.println("010C");
  //Get the response from the OBD-II-UART board
  getResponse();
  getResponse();
  //Convert the string data to an integer
  //NOTE: RPM data is two bytes long, and delivered in 1/4 RPM from the OBD-II-UART
  vehicleRPM = ((strtol(&rxData[6],0,16)*256)+strtol(&rxData[9],0,16))/4;
  //Print the rpm data to the lcd
  lcd.print(vehicleRPM); 
  
  //Give the OBD bus a rest
  delay(100);
  
}

//The getResponse function collects incoming data from the UART into the rxData buffer
// and only exits when a carriage return character is seen. Once the carriage return
// string is detected, the rxData buffer is null terminated (so we can treat it as a string)
// and the rxData index is reset to 0 so that the next string can be copied.
void getResponse(void){
  char inChar=0;
  //Keep reading characters until we get a carriage return
  while(inChar != '\r'){
    //If a character comes in on the serial port, we need to act on it.
    if(Serial.available() > 0){
      //Start by checking if we've received the end of message character ('\r').
      if(Serial.peek() == '\r'){
        //Clear the Serial buffer
        inChar=Serial.read();
        //Put the end of string character on our data string
        rxData[rxIndex]='\0';
        //Reset the buffer index so that the next character goes back at the beginning of the string.
        rxIndex=0;
      }
      //If we didn't get the end of message character, just add the new character to the string.
      else{
        //Get the new character from the Serial port.
        inChar = Serial.read();
        //Add the new character to the string, and increment the index variable.
        rxData[rxIndex++]=inChar;
      }
    }
  }
}