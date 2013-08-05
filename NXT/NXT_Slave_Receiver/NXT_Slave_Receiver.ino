#include <Wire.h>

byte read_register = 0x00;

void setup()
{
  Wire.begin(0x0A);                // join i2c bus with address #2
  Wire.onRequest(requestEvent);  // Sending information back to the NXT!
  Wire.onReceive(receiveI2C);    // Receiving information!
  Serial.begin(9600);
}

void loop()
{
}
void receiveI2C(int bytesIn)
{
  read_register = bytesIn;
  while(0 < Wire.available())       // loop through all but the last
    Wire.read();
}
void requestEvent()
{
    //unsigned char a=map(analogRead(0),0,1023,0,255);
    delay(1);
    Wire.write('a');   // respond with message of 10 bytes
                                   // as expected by master
}
