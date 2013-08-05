// I2C Slave Receiver
// How to send data from the LEGO Mindstorms NXT to the Arduino.
// For LEGO Mindstorms

// Demonstrates how to connect a LEGO MINDSTORMS to an Arduino and Send commands.
// A4 - SDA
// A5 - SCL
// See our website, www.dexterindustries.com/howto for more information on the physical setup.

#include <Wire.h>
int cmd[4];
void setup()
{
  Wire.begin(0x0A);             // Start I2C on Address 0x0A
  Wire.onReceive(receiveI2C);   // Receive Event from Master
  Serial.begin(9600);           // We will spit it back out on the serial line.
}

void loop()
{
  delay(1);
}

// When data is received, this function is called.
void receiveI2C(int bytesIn)
{
  int i=0;
//  Serial.print("Bytes: ");
//  Serial.println(bytesIn);
  while(0 < Wire.available()) // loop through all but the last
    cmd[i++]=(int)Wire.read();
  
  Serial.print(cmd[1]);
  Serial.print(" ");
    Serial.print(cmd[2]);
  Serial.print(" ");
    Serial.print(cmd[3]);
  Serial.println(" ");
}
