// code written by RoboRemo
// http://www.roboremo.com
// please don't remove this :)


const uint8_t paramIds[] = {'p', 'i', 0};

struct {
    int32_t p, i;  // period, interval
    // long is 4-Byte, signed
} params;


void writeParam(uint8_t id, int32_t val) {
  for(uint8_t i=0; paramIds[i]!=0; i++) {
    if(id==paramIds[i]) {
      eeWrite32(i*4, val);
      return;
    }
  }     
}


int32_t readParam(uint8_t id) {
  for(uint8_t i=0; paramIds[i]!=0; i++)
    if(id==paramIds[i]) 
      return eeRead32(i*4);       
  return 0;
}

void initParams() {
  for(uint8_t i=0; paramIds[i]!=0; i++)
    ((int32_t*)(&params))[i] = readParam(paramIds[i]);
}


void updateParam(uint8_t id, int32_t val) {
  for(uint8_t i=0; paramIds[i]!=0; i++)
    if(id==paramIds[i]) {
      ((int32_t*)(&params))[i] = val;
      writeParam(id, val);
    }
}

void showParams() {
  initParams();
  
  //puts("\nparams:\n");
  for(uint8_t i=0; paramIds[i]!=0; i++) {
      putc(paramIds[i]);
      puts(" = ");
      putn( ((int32_t*)(&params))[i] );
      putLF();
  }
}
