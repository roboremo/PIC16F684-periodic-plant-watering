// code written by RoboRemo
// http://www.roboremo.com
// please don't remove this :)


// NOTE:
// delays are tuned for 9600 baud (for 8MHz XTAL)
// compiled with XC8 v1.34 free
// other versions may have different optimization, so you may need to re-tune


uint8_t kbhit() {
    return (uartRx==0);
}

uint8_t getc() {
    uint8_t res, a, i, j;
    uint16_t wait;

    res = 0;

    wait = 0;

    while(uartRx==1) {} // may happen when called directly


    // debug:
    //LATBbits.LATB0 = 1;

    //delay_104us();   // for 9600
    for(j=0; j<19; j++) {
        asm ("NOP");
    } 
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    //asm ("NOP");
    //asm ("NOP");

    for(i=0; i<8; i++) {

        for(j=0; j<8; j++) {
            asm ("NOP");
        }
        asm ("NOP");
        asm ("NOP");
        
        res = res << 1;
        res += uartRx;
        
        for(j=0; j<9; j++) {
            asm ("NOP");
        }
    }



    a = res;
    res = 0;

    for(i=0; i<8; i++) {
        res = res << 1;
        res += a%2;
        a = a >> 1;
    }




    while(uartRx==0) {}

    return res;

}


void putc(uint8_t data) {
    uint8_t i, j;

#ifdef forceCR
    if(data=='\n') data = '\r';
#endif

    uartTx = 0;
    //delay_104us();  // for 9600
    
    for(j=0; j<19; j++) {
        asm ("NOP");
    } 
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    asm ("NOP");
    //asm ("NOP");
    //asm ("NOP");
    //asm ("NOP");
    //asm ("NOP");
    

    for(i=0; i<8; i++) {
        
        uartTx = data & 0x01;
        data = data >> 1;
        
        for(j=0; j<16; j++) {
            asm ("NOP");
        }
    }

    uartTx = 1;
    
    
    for(j=0; j<18; j++) {
        asm ("NOP");
    }
}



void puts(const uint8_t *s) {
    uint8_t i;

    for(i=0; s[i]!=0; i++) {
        putc(s[i]);
        
        //delay_100us();
        //delay_100us();
    }
}



void putn(int32_t n) {  // -99999999 to 99999999
    uint8_t z;
    uint8_t dig, i;
    const uint32_t decaShift[] = {10000000, 1000000, 100000, 10000, 1000, 100, 10};

    if(n<0) {
        n = -n;
        putc('-');
    }

    z = 1;
    
    for(i=0; i<7; i++) {
        dig = ((n/decaShift[i])%10)+'0';
        if(z==1 && dig=='0') {}
        else {z = 0; putc(dig);}
    }
    
    dig = ((n)%10)+'0'; 
    //if(z==1 && dig=='0') {}
    //else {z = 0; uartPutc(dig);}
    putc(dig);

}


void putLF() {   // things that you start to do when out of memory
    putc('\n');
}