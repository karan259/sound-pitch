#pragma config(Sensor, S1,TIR,sensorI2CCustom)

#define ARDUINO_ADDRESS	0x14
#define ARDUINO_PORT S1

ubyte I2Cmessage[22];
char I2Creply[20];
int cmd=0,mode=0;

//Retrieve data from the sound sensor
void i2c_read_registers_text(ubyte register_2_read, int message_size, int return_size)
{
  memset(I2Creply, 0, sizeof(I2Creply));
    message_size = message_size+3;

  I2Cmessage[0] = message_size;    // Messsage Size
  I2Cmessage[1] = ARDUINO_ADDRESS;
  I2Cmessage[2] = register_2_read; // Register
  sendI2CMsg(S1, &I2Cmessage[0], return_size);
  wait1Msec(20);

  readI2CReply(ARDUINO_PORT, &I2Creply[0], return_size);

  int i = 0,top=0;
  //Pitch
 	if(cmd==1)
 		writeDebugStreamLine("%d", (int)I2Creply[0]+(int)I2Creply[1]*256);

 	//Sound Power Level
  else if(cmd==2)
  	//For detecting clap
  	//if((int)I2Creply>85)
  	writeDebugStreamLine("%i", (int)I2Creply[0]);

  //FFT
  else if(cmd==3)
  {
  	for(int x = 0; x <return_size; x++)
  		{
  		writeDebugStream("%i", I2Creply[x]);
  		writeDebugStream(" ");
 			}
 		writeDebugStreamLine(" ");

 		//Graphic Equalizer
 		nxtEraseRect(6,62,99,6);
 		nxtDrawLine(5,2,5,61);
 		nxtDrawLine(2,5,95,5);
 		for(int j=0;j<return_size;j++)
 		{
 			top=I2Creply[j]+5;
 			if(top>61)
 				top=61;
 			nxtDrawRect(5+6*j,top,5+6*(j+1),5);
 		}

  }

}

//Send command to the processor
void i2c_write_registers(ubyte register_2_write, int message_size, int return_size, ubyte byte1, ubyte byte2){
  memset(I2Creply, 0, sizeof(I2Creply));

  message_size = message_size+3;

  I2Cmessage[0] = message_size;    // Messsage Size
  I2Cmessage[1] = ARDUINO_ADDRESS;
  I2Cmessage[2] = register_2_write; // Register

  I2Cmessage[3] = byte1;
  I2Cmessage[4] = byte2;
  //I2Cmessage[5] = byte3;

  sendI2CMsg(ARDUINO_PORT, &I2Cmessage[0], return_size);
  wait1Msec(20);
}

int st=0;
task main()
{
	while(true)
		{
		//Cmd:
		//		1- pitch detect
		//		2- sound power level
		//		3- FFT
		cmd=3;
		mode=0;
		if(st==0)
		{
			i2c_write_registers(0x01, 0x02, 0,cmd,mode);
			st=1;
			writeDebugStreamLine("Starting");
			//wait1Msec(4000);
			writeDebugStreamLine("Ok");
		}
		else
			if(cmd==1)
				i2c_read_registers_text(0x01, 0, 2);

			else if(cmd==2)
				i2c_read_registers_text(0x01, 0, 1);

			else if(cmd==3)
				i2c_read_registers_text(0x01, 0, 15);
			wait1Msec(2);
	}
}
