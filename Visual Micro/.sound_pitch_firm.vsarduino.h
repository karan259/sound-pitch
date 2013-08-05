//Board = Arduino Uno
#define __AVR_ATmega328P__
#define ARDUINO 105
#define F_CPU 16000000L
#define __AVR__
extern "C" void __cxa_pure_virtual() {;}

void printb(uint16_t in[]);
void fft_run();
int sum(uint16_t in[]);
void sample(uint16_t *in,int tot,int opt);
void coll_sample(uint16_t *in,char *s,int tot,int opt);
int d(uint16_t *a,uint16_t *b);
int peak(uint16_t a[]);
void establishContact();
void adcInit(int mode);
void adcCalb();
int pitch (int mode,int del);
unsigned char s_power(int del);
void run_fft(int mode,int del);
//
//
void receiveI2C(int bytesIn);
void requestEvent();

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\standard\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\John\Documents\Arduino\sound_pitch_firm\sound_pitch_firm.ino"
