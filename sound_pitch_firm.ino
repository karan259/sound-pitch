/*
This Example acquire analog signal from A0 of Arduino, and Serial out to Processing application to visualize.
Tested with preamplified audio data. Take a look at http://www.youtube.com/watch?v=drYWullBWcI

Analog signal is captured at 9.6 KHz, 64 spectrum bands each 150Hz which can be change from adcInit()


Original Fixed point FFT library is from ELM Chan, http://elm-chan.org/works/akilcd/report_e.html
A way to port it to the Arduino library and most demo codes are from AMurchick http://arduino.cc/forum/index.php/topic,37751.0.html
Processing app is based on codes from boolscott http://boolscott.wordpress.com/2010/02/04/arduino-processing-analogue-bar-graph-2/
*/


#include <stdint.h>
#include <ffft.h>
#include <MemoryFree.h>
#include <Wire.h>

#define  IR_AUDIO  0 // ADC channel to capture


volatile  byte  position = 0;
volatile  long  zero = 0;

int cmd[]={0,0,0,0};

int16_t capture[FFT_N];			/* Wave captureing buffer */
complex_t bfly_buff[FFT_N];		/* FFT buffer */
uint16_t spektrum[FFT_N/2];		/* Spectrum output buffer */
uint16_t base_buf[FFT_N/2];

unsigned int s2[]={300,600,900,1200,1500,1800,2100,2400,2700,3000,3300,3600,3900,4200,4500,4800,5100,5400,5700,6000,6300,6600,6900,7200,7500,7800,8100,8400,8700,9000,9300,9600,9900,10200,10500,10800,11100,11400,11700,12000,12300,12600,12900,13200,13500,13800,14100,14400,14700,15000,15300,15600,15900,16200,16500,16800,17100,17400,17700,18000,18300,18600,18900,19200};
unsigned int s1[]={150,300,450,600,750,900,1050,1200,1350,1500,1650,1800,1950,2100,2250,2400,2550,2700,2850,3000,3150,3300,3450,3600,3750,3900,4050,4200,4350,4500,4650,4800,4950,5100,5250,5400,5550,5700,5850,6000,6150,6300,6450,6600,6750,6900,7050,7200,7350,7500,7650,7800,7950,8100,8250,8400,8550,8700,8850,9000,9150,9300,9450,9600};
unsigned int s0[]={75,150,225,300,375,450,525,600,675,750,825,900,975,1050,1125,1200,1275,1350,1425,1500,1575,1650,1725,1800,1875,1950,2025,2100,2175,2250,2325,2400,2475,2550,2625,2700,2775,2850,2925,3000,3075,3150,3225,3300,3375,3450,3525,3600,3675,3750,3825,3900,3975,4050,4125,4200,4275,4350,4425,4500,4575,4650,4725,4800};

unsigned int *spec[]={s0,s1,s2};
int i,j,cur=0;
int n=64,tot=50;
int diff[5];

//Print the FFT_N/2 bytes array of the FFT output
void printb(uint16_t in[])
{
  for(int i=0;i<n;i++){
    Serial.print(in[i]);
    Serial.print(" ");
  }
  Serial.println();
}

//Runs the FFT and gives the output in spektrum
void fft_run()
{
  fft_input(capture, bfly_buff);
  fft_execute(bfly_buff);
  fft_output(bfly_buff, spektrum);
}

//Sums up a spectrum and returns the sum
int sum(uint16_t in[])
{
  int s=0;
  for(i=0;i<n;i++)
    s+=in[i];
  return s;
}
//Takes a sample using FFT and stores it in 'in'
void sample(uint16_t *in,int tot,int opt)
{
  int maxi=0,maxa=0,f=0;
  uint16_t s;
  for(int i=0;i<n;i++)
    *(in+i)=0;

  memset(in,0,n);
  while(cur<tot)
    while(1)
    {
      if (position == FFT_N)
      {
        fft_run();
		//if(opt!=0)
		//	for(i=0;i<n;i++)
		//		spektrum[i]=spektrum[i]<base_buf[i]?0:spektrum[i]-base_buf[i];
		//printb(spektrum);
		
        maxi=0;
        if(cur!=0)
        {
          if(peak(spektrum)>peak(in))
            for(i=0;i<n;i++)
              in[i]=spektrum[i];
        }
        cur++;
        position = 0; //This is necessary
        delay(10);
        break;
      }
    }
}

//Output to be shown when taking a sample
//opt=0: baseline, opt=1: other samples-baseline
void coll_sample(uint16_t *in,char *s,int tot,int opt)
{
  Serial.println(" ");
  Serial.print("Give input for ");
  Serial.println(s);
  delay(1500);
  Serial.print("Say ");
  Serial.println(s);
  delay(20);
  cur=0;
  sample(&in[0],tot,opt);
  printb(in);  
  Serial.print(s);
  Serial.println(" Completed");
}

//Difference between two samples
int d(uint16_t *a,uint16_t *b)
{
  int s=0,t;
  ///printb(a);
  //printb(b);
  
  for(int i=0;i<n;i++)
  {
    t=*(a+i)-*(b+i);
    s+=abs(t);
  }
  return s;
}

//Finds the peak values 
int peak(uint16_t a[])
{
  int i,pk=0;
  for(i=0;i<n;i++)
    if(a[i]>pk)
      pk=a[i];
  return pk;
}

void establishContact() {
	while (Serial.available() <= 0) {
		Serial.write('A');   // send a capital A
		delay(300);
	}
}

// free running ADC fills capture buffer
ISR(ADC_vect)
{
	if (position >= FFT_N)
	return;
	
	capture[position] = ADC + zero;
	if (capture[position] == -1 || capture[position] == 1)
	capture[position] = 0;

	position++;
}
void adcInit(int mode){
	/*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
	// free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion
	ADMUX = _BV(REFS0) | IR_AUDIO; // | _BV(ADLAR);
	if(mode==2)
		ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS0);// prescaler 32 38462 Hz 600Hz per division upto 18000Hz
	else if(mode==1)
		ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1); //prescaler 64 : 19231 Hz - 300Hz per 64 divisions 10000Hz
	else if(mode==0)
		ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music 5000Hz
	sei();
}
void adcCalb(){
	Serial.println("Start to calc zero");
	long midl = 0;
	// get 2 meashurment at 2 sec
	// on ADC input must be NO SIGNAL!!!
	for (byte i = 0; i < 2; i++)
	{
		position = 0;
		delay(100);
		midl += capture[0];
		delay(900);
	}
	zero = -midl/2;
	Serial.println("Done.");
}

int pitch (int mode)
{
	int pitc=0;
	if(mode<0 ||mode>2)
		return 0;
	adcInit(mode);
	adcCalb();
	int s=0,pk=0;
	while(1)
	{
		if (position == FFT_N)
		{
			s=0;
			pk=0;
			fft_run();
			for(i=2;i<64;i++)
			{
				if(pk<spektrum[i])
				{
					pk=spektrum[i];
					s=i;
				}
			}
			//Data having the frequency
			//For multiple frequencies
			//Serial.println(spec[mode][s]);
			
			//For one call 
			pitc=spec[mode][s];
			//Serial.println(pitc);
			goto l1;
			//return pitc;
			
			//delay(del);
			position = 0;
		}
	}
	l1: return pitc;
}
unsigned char s_power(int del)
{
	unsigned char b;
	b=map(analogRead(0),0,1023,0,255);
	delay(del);
	return(b);
}

void run_fft(int mode,int del)
{
	if(mode<0 ||mode>2)
		return;
	adcInit(mode);
	adcCalb();
	int s=0,pk=0;
	while(1)
	{
		if (position == FFT_N)
		{
			s=0;
			pk=0;
			fft_run();
			//FFT Spectrum
			printb(spektrum);
			delay(del);
			position = 0;
		}
	}
}
void setup()
{
  Wire.begin(0x0A);             // Start I2C on Address 0x0A
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveI2C);   // Receive Event from Master
  Serial.begin(9600);
  //Show Pitch
  //pitch(2,10);
  
  //Show power level
  //while(1)
  //	Serial.println(s_power(10));
  
  //Run FFT
  //run_fft(0,10);
  
}
 uint8_t sp,sp1,buf[16];
int ptc;
void loop()
{
	if(cmd[1]!=0)
	{
		if(cmd[1]==2)
			buf[0]=map(analogRead(0),0,1023,0,255);
		if(cmd[1]==1)
		{
			ptc=pitch(1);
			buf[0]=ptc%256;
			buf[1]=ptc/256;
			Serial.print(ptc);
			Serial.print(" ");
			Serial.print((int)buf[0]);
			Serial.print(" ");
			Serial.println((int)buf[1]);
		}
	}
}

void receiveI2C(int bytesIn)
{
	int i=0;
	//  Serial.print("Bytes: ");
	//  Serial.println(bytesIn);
	while(0 < Wire.available()) // loop through all but the last
		cmd[i++]=(int)Wire.read();
	/*if(cmd[1]==1)
	{
		ptc=pitch(1);
		sp=map(ptc,0,1023,0,255);	;
		//sp1=ptc/256;
	}
	//else if(cmd[1]==2)
	//	sp=map(analogRead(0),0,1023,0,255);	
		
	/*Serial.print(cmd[1]);
	Serial.print(" ");
	Serial.print(cmd[2]);
	Serial.print(" ");
	Serial.print(cmd[3]);
	Serial.println(" ");
	*/
	
}
void requestEvent()
{
	if(cmd[1]==1)
		Wire.write(buf,2);	
	
	if(cmd[1]==2)
		Wire.write(buf,1);
	//Wire.write("Pitch     ");
	/*/char buf[10];
	if(cmd[1] == 1)
	{

		//ptc=pitch(2,10);
		//buf[0]=ptc%256;
		//buf[1]=ptc/256;
		Wire.write(sp);   // respond with message of 10 bytes
		//Wire.write(sp1);
	}
	else if(cmd[1] == 2)
	{
		sp=map(analogRead(0),0,1023,0,255);
		Wire.write(sp);   // respond with message of 10 bytes
	}                                     // as expected by master
	else if(cmd[1] == 3){
		Wire.write("FFT       ");   // respond with message of 10 bytes
	}
	else {
		Wire.write("Wrong Cmd ");   // respond with message of 10 bytes
	}
	*/
}