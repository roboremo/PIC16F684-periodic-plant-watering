// code written by RoboRemo
// http://www.roboremo.com
// please don't remove this :)


void eeWrite8(uint8_t addr, uint8_t data) {
    EEADR = addr;
    EEDATA = data;
    //EECON1bits.EEPGD = 0; // point to data memory
    //EECON1bits.CFGS = 0;  // access eeprom
    EECON1bits.WREN = 1;  // write enable
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;    // start write
    while(EECON1bits.WR==1) {} // wait to write
    EECON1bits.WREN = 0;  // write disable
}


uint8_t eeRead8(uint8_t addr) {
    EEADR = addr;
    //EECON1bits.EEPGD = 0; // point to data memory
    //EECON1bits.CFGS = 0;  // access eeprom
    EECON1bits.RD = 1;
    return EEDATA;
}


void eeWrite16(uint8_t addr, int16_t data) {
    uint8_t d;
    d = (data >> 8) & 0xFF;
    eeWrite8(addr, d);
    d = data & 0xFF;
    eeWrite8(addr+1, d);
}


int16_t eeRead16(uint8_t addr) {
    int16_t res;
    res = eeRead8(addr);
    res = res << 8;
    res += eeRead8(addr+1);
    return res;
}


void eeWrite32(uint8_t addr, int32_t data) {
    uint8_t i;
    for(i=0; i<4; i++) {
        eeWrite8(addr+3-i, data /*& 0xFF*/ );
        data = data >> 8;
    }
        
    //eeWrite8(addr+3, data /*& 0xFF*/ );
    //data = data >> 8;
    //eeWrite8(addr+2, data /*& 0xFF*/ );
    //data = data >> 8;
    //eeWrite8(addr+1, data /*& 0xFF*/ );
    //data = data >> 8;
    //eeWrite8(addr+0, data /*& 0xFF*/ );
}


int32_t eeRead32(uint8_t addr) {
    int32_t res=0;
    uint8_t i;
    for(i=0; i<4; i++) {
        res = res << 8;
        res += eeRead8(addr+i);
    }
    
    //res = eeRead8(addr);
    //res = res << 8;
    //res += eeRead8(addr+1);
    //res = res << 8;
    //res += eeRead8(addr+2);
    //res = res << 8;
    //res += eeRead8(addr+3);
    
    return res;
}
