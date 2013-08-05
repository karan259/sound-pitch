#pragma config(Sensor, S1,TIR,sensorI2CCustom)

/*
How to connect your LEGO MINDSTORMS NXT with an Arduino.
http://www.dexterindustries.com/howto/
*/


// First, define the Arduino Address
// Address is 0x0A on the Arduino:  					(Binary)  1010
// Bit shifted out with one 0, that becomes: 	(Binary) 10100
// Which is 0x14
#define ARDUINO_ADDRESS	0x14
#define ARDUINO_PORT S1

ubyte I2Cmessage[22];
char I2Creply[20];

void i2c_write_registers(ubyte register_2_read, int message_size, int return_size,  ubyte byte1, ubyte byte2, ubyte byte3){
  memset(I2Creply, 0, sizeof(I2Creply));
	message_size = message_size+3;

  I2Cmessage[0] = message_size;    // Messsage Size
  I2Cmessage[1] = ARDUINO_ADDRESS;
  I2Cmessage[2] = register_2_read; // Register

  I2Cmessage[3] = byte1;
  I2Cmessage[4] = byte2;
  I2Cmessage[5] = byte3;

  sendI2CMsg(S1, &I2Cmessage[0], return_size);
  wait1Msec(20);

  readI2CReply(ARDUINO_PORT, &I2Creply[0], return_size);

  int i = 0;
  //Sound Power Level
  if(byte1==2)
  	writeDebugStream("%i", (int)I2Creply[0]);
 	//Pitch
 	if(byte1==1)
 	{
 		//writeDebugStreamLine("%d", (int)I2Creply[0]+((int)I2Creply[0])*256);
  	for(int x = 0; x <= return_size-1; x++)
  		{
  		writeDebugStream("%d", I2Creply[x]);
  		writeDebugStream(" ");
 			}
  }
  //else if(byte1==1)
  //{
  //	writeDebugStream("%i", (int)I2Creply[0]);
  //}
  //	while(true){
	//  	writeDebugStream("%c", I2Creply[i]);
	//  	i++;
	//  	if(I2Creply[i] == 0) break;
	//  }
  writeDebugStreamLine(" ");
}

task main()
{
	while(true){

		//i2c_write_registers(0x01, 0x02, 10, 1, 10,0);
		//Pitch
		i2c_write_registers(0x01, 0x02, 2, 1, 1, 10);
		//Sound
		//i2c_write_registers(0x01, 0x02, 2, 2, 1, 10);

		wait1Msec(50);
		//i2c_write_registers(0x01, 0x02, 10, 3, 1, 10);
		//i2c_write_registers(0x01, 0x02, 10, 4, 1, 10);
		//i2c_read_registers_text(0x01, 0, 10);		// Here we're going to get back "Industries" because we're weriting to the 0x00 register.
		//i2c_read_registers_text(0x00, 0, 10);
		//wait1Msec(1000);
	}

}
