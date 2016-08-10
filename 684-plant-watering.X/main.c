// code written by RoboRemo
// http://www.roboremo.com
// please don't remove this :)


#include "pic16f684.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#define int1_t bit


//#define forceCR



// PIC16F684 Configuration Bit Settings


// CONFIG
#pragma config FOSC = HS
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection OFF
#pragma config CPD = OFF        // Data Memory Code Protection OFF
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)


//    Vdd       |--v--|  Vss
//    RA5       |     |  RA0 / AN0
//    RA4 / AN3 | 16F |  RA1 / AN1
//    RA3       | 684 |  RA2 / AN2 / INT
//    RC5       |     |  RC0 / AN4
//    RC4       |     |  RC1 / AN5
//    RC3 / AN7 |-----|  RC2 / AN6


#define led PORTCbits.RC4
#define btEn PORTCbits.RC1
#define valveOpen PORTCbits.RC2

#define btnBTen PORTAbits.RA0
#define uartTx PORTAbits.RA1
#define uartRx PORTAbits.RA2


#include "soft_uart_8_9600.c"   // 9600 baud for 8 MHz
#include "eeprom.c"
#include "plant_watering_params.c"


uint8_t btEnCounter = 0; // counter to turn off Bluetooth after a few seconds

#define btEnSeconds 30 // keep ON for 30 seconds after last command

void initPORTS() {
    
    CMCON0 = 0b00000111; // comparators off
    ANSEL = 0b00000000;   // all digital
    
    TRISA = 0b11111101;
    OPTION_REGbits.nRAPU = 0; // global PORTA pull-ups enabled
    WPUA = 0b00000101; // Rx and btnBTen pull-ups enabled
    
    uartTx = 1; // idle
    
    TRISC = 0b11101001;
    
    led = 0; // OFF
    btEn = 0;
    valveOpen = 0;
    
}



void openValve() {
    valveOpen = 1;
}


void closeValve() {
    valveOpen = 0;
}



uint8_t tCounter;
uint32_t sec; // overflows after 136 years :)

uint8_t cmd[32];
uint8_t cmdIndex;


uint8_t cmdStartsWith(uint8_t *st) {
  for(uint8_t i=0; ; i++) {
    if(st[i]==0) return 1;
    if(cmd[i]==0) return 0;
    if(cmd[i]!=st[i]) return 0;
  }
}



void exeCmd() { 


  if( cmdStartsWith("get") ) {
    showParams();
  }
  
  if( cmdStartsWith("tim") ) {
      puts("up time: "); putn(sec); putc('s'); putLF();
  }
  

  if( cmdStartsWith("open") ) {
    openValve();
  }

  if( cmdStartsWith("close") ) {
    closeValve();
  }

  if( cmdStartsWith("set ") ) { // set p(eriod) / set i(nterval)
    int32_t val = atol(cmd+6);
    updateParam(cmd[4], val);
    showParams();
    //sec = 0;
  }
  
  btEnCounter = btEnSeconds;

}


uint8_t cmdReady;

void main(void) {
    
    uint8_t c;
    uint32_t r;

    initPORTS();
   
    
    //OSCCONbits.IRCF0 = 1;
    //OSCCONbits.IRCF1 = 1;
    //OSCCONbits.IRCF2 = 1;  // FOR INTERNAL 8 MHz
    
    cmdIndex = 0;
    initParams();
    
    //puts("hi :)\n");
    
    T2CONbits.TMR2ON = 0;
    T2CONbits.T2CKPS = 0b11; // prescaler = 16, T2 CLK = 125 KHz (for 8 MHz)
    T2CONbits.TOUTPS = 9; // 1:10 postscale
    PR2 = 249; // 0.05 KHz -> 20 ms interrupt (period is PR2+1)
    TMR2 = 0;
    
    //puts("PR2 = "); putn(PR2); putc('\n');
    
    
    tCounter = 0;
    sec = 0;
    T2CONbits.TMR2ON = 1; // Start timer
    
    
    // configure external interrupt:
    IOCAbits.IOC2 = 1; // interrupt only for RA2
    INTCONbits.RAIE = 1; // enable interrupt
    INTCONbits.RAIF = 0; // clear flag
    
    // enable global interrupts:
    INTCONbits.GIE = 1;
    
    
    
    cmdReady = 0;
    

    while(1) {
        
        if(cmdReady) {  // set by isr
            exeCmd();  // execute the command
            cmdIndex = 0; // reset the cmdIndex
            cmdReady = 0;
        }
        
        
        if(!btnBTen) { // "Bluetooth enable" button pressed
            btEn = 1; // enable Bluetooth
            btEnCounter = btEnSeconds; // disable after a few seconds
        }
        
        
        if(PIR1bits.TMR2IF) {
            PIR1bits.TMR2IF = 0;
            tCounter++;
            if(tCounter==50) {
                tCounter = 0;
                sec++;
                
                
                
                /////////////////////////////////////////////////////////
                
                
                //if(sec==20) {
                //    puts("hi :)\n"); // for debug
                //}
                
                if(btEnCounter) {
                    btEnCounter--;
                    if(btEnCounter==0) {
                        btEn = 0; // disable Bluetooth (to save power)
                    }
                }
                
                
                
                /*if(sec==params.p) {
                    openValve();
                }

                if(sec==params.p + params.i) {
                    closeValve();
                    sec = params.i; // to start from there
                }*/
                
                r = sec % params.p;
                
                if(r == 0) {
                    openValve();
                }

                if(r == params.i) {
                    closeValve();
                    //sec = params.i; // to start from there
                }
                
                /////////////////////////////////////////////////////////
                
            }
            
            
            if( (sec & 0x01) == 0) {
                if(tCounter==0) led = 1;
                if(tCounter==5) led = 0;
            }
            
            //led = 1 - led;
        }
        
    }
    
}


uint8_t c;

void interrupt isr() {  // interrupt serving routine
    
    // external interrupt used as kbhit for software serial
    
    c = getc();

    if(c=='\n' || c=='\r') {
      cmd[cmdIndex] = 0;
      cmdReady = 1;
    } else {      
      cmd[cmdIndex] = c;
      if(cmdIndex<31) cmdIndex++;
    }
    
    INTCONbits.RAIF = 0; // clear flag
    
}