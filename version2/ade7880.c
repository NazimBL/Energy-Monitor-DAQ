 #include "ade7880.h"
 #define IAoffset 0xFFFE000
 #define VAoffset 0xFF60000
 
 unsigned char ADE_Read8(unsigned int reg){
     unsigned char b0;
     char r1,r0;
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
    
     GPIOA_ODRbits.ODR15=0;
     Delay_us(10);
     
     SPI_Write(0x01);
      SPI_Write(r1);
     SPI_Write(r0);
     b0=SPI_Read(0x00);
     
     GPIOA_ODRbits.ODR15=1; 
     Delay_us(10);
     return b0;
}

unsigned int ADE_Read16(unsigned int reg){
    
    unsigned char b1,b0;
    char r1,r0;
    
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
   
    GPIOA_ODRbits.ODR15=0;
    Delay_us(10);

    SPI_Write(0x01);
    SPI_Write(r1);
    SPI_Write(r0);
    b1=SPI_Read(0x00);
    b0=SPI_Read(0x00);
   
    GPIOA_ODRbits.ODR15=1; 
    Delay_us(10);
    
    return (unsigned int)b1<<8 | (unsigned int)b0;
}

unsigned long ADE_Read24(unsigned int reg){
    unsigned char b2,b1,b0;
    char r1,r0;
    
    r0=(unsigned char)(reg & 0xFF);
    r1=(unsigned char)(reg >> 8);
     
    GPIOA_ODRbits.ODR15=0;
    Delay_us(10);
    
    SPI_Write(0x01);
    SPI_Write(r1);
    SPI_Write(r0);
    b2=SPI_Read(0x00);
    b1=SPI_Read(0x00);
    b0=SPI_Read(0x00);
    
    GPIOA_ODRbits.ODR15=1; 
    Delay_us(10);
    return (unsigned long)b2<<16 | (unsigned long)b1<<8 | (unsigned long)b0;
}

unsigned long ADE_Read32(unsigned int reg){

    unsigned char b3,b2,b1,b0;
    char r1,r0;
    
    r0=(unsigned char)(reg & 0xFF);
    r1=(unsigned char)(reg >> 8);
   
    GPIOA_ODRbits.ODR15=0;
    Delay_us(10);
    
    SPI_Write(0x01);
    SPI_Write(r1);
    SPI_Write(r0);
    b3=SPI_Read(0x00);
    b2=SPI_Read(0x00);
    b1=SPI_Read(0x00);
    b0=SPI_Read(0x00);
    
    GPIOA_ODRbits.ODR15=1; 
    Delay_us(10);
    
    return (unsigned long)b3<<24 | (unsigned long)b2<<16 | (unsigned long)b1<<8 | (unsigned long)b0;
}

void ADE_Write8(unsigned int reg ,unsigned char dat){

     char r1,r0;
     
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
     
     GPIOA_ODRbits.ODR15=0;
     Delay_us(10);
     
     SPI_Write(0x00);
     SPI_Write(r1);
     SPI_Write(r0);
     SPI_Write(dat);
     
     GPIOA_ODRbits.ODR15=1; 
     Delay_us(10);
}

void ADE_Write16(unsigned int reg, unsigned int dat){
     
     char b0,b1;
     char r1,r0;
     
     b0=(unsigned char)(dat & 0xFF);
     b1=(unsigned char)(dat >> 8);
     
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
     
     GPIOA_ODRbits.ODR15=0;
     Delay_us(10);
     
     SPI_Write(0x00);
     SPI_Write(r1);
     SPI_Write(r0);
     SPI_Write(b1);
     SPI_Write(b0);
     
     GPIOA_ODRbits.ODR15=1; 
     Delay_us(10);

}

 void ADE_Write24(unsigned int reg, unsigned long dat){
 
     char b0,b1,b2;
     char r1,r0;
     
     b0=(unsigned char)(dat & 0xFF);
     b1=(unsigned char)(dat >> 8);
     b2=(unsigned char)(dat >> 16);
     
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
   
     GPIOA_ODRbits.ODR15=0;
     Delay_us(10);
     
     SPI_Write(0x00);
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
     SPI_Write(b2);
     SPI_Write(b1);
     SPI_Write(b0);
     
     GPIOA_ODRbits.ODR15=1; 
     Delay_us(10);
 }
 
 void ADE_Write32(unsigned int reg, unsigned long dat){
      char b0,b1,b2,b3;
      char r1,r0;
      
     b0=(unsigned char)(dat & 0xFF);
     b1=(unsigned char)(dat >> 8);
     b2=(unsigned char)(dat >> 16);
     b3=(unsigned char)(dat >> 24);
     
     r0=(unsigned char)(reg & 0xFF);
     r1=(unsigned char)(reg >> 8);
     
     GPIOA_ODRbits.ODR15=0;
     Delay_us(10);
     
     SPI_Write(0x00);
     SPI_Write(r1);
     SPI_Write(r0);
     SPI_Write(b3);
     SPI_Write(b2);
     SPI_Write(b1);
     SPI_Write(b0);
     
     GPIOA_ODRbits.ODR15=1; 
     Delay_us(10);
 
 }  
 
 void ADE_Init(){
 
         ADE_Write16(CFMODE,0x00A0);
         
         ADE_Write8(0xE7FE,0xAD);
         ADE_Write8(0xE7E3,0x00);
          
        ADE_Write32(AIRMSOS,IAoffset);
        ADE_Write32(AVRMSOS,VAoffset);
        ADE_Write32(AWATTOS,VAoffset);
        
         ADE_Write16(HCONFIG,0x00); 
         ADE_Write8(HX,0x02);
         ADE_Write8(HY,0x03);
         ADE_Write8(HZ,0x04);
         ADE_Write8(HZ,0x04);
         ADE_Write8(HZ,0x04);
                        
         //Write protection
        ADE_Write8(0xE7FE,0xAD);
        ADE_Write8(0xE7E3,0x80);

        //start dsp
        ADE_Write16(RUNREG,0x0001);
        ADE_Read32(0xE400); 
        ADE_Read32(0xE403); 
        ADE_Read32(0xE409);
        ADE_Read32(0xE40C);
    
 } 
 
 void ADE_SpiEnable(){
 
   char d=0;
   //toggle cs 3 times by writing 0x0 to a reserved reg address
   for(d=0;d<3;d++)ADE_Write8(0xEBFF,0x00);
   //lock spi
   ADE_Write8(CONFIG2,0x00);   
}

 unsigned long getVRMS(char phase){
 
 if(phase==0)return ADE_Read32(AVRMS);
 else if (phase==1)return ADE_Read32(BVRMS);
 else return ADE_Read32(CVRMS);
 
 }
 
 unsigned long getIRMS(char phase){
 
 if(phase==0)return ADE_Read32(AIRMS);
 else if (phase==1)return ADE_Read32(BIRMS);
 else return ADE_Read32(CIRMS);
 
 }
 
 unsigned long getWatt(char phase){
 
 if(phase==0)return ADE_Read32(AWATT);
 else if (phase==1)return ADE_Read32(BWATT);
 else return ADE_Read32(CWATT);
 
 }
 unsigned long getVA(char phase){
 
 if(phase==0)return ADE_Read32(AVA);
 else if (phase==1)return ADE_Read32(BVA);
 else return ADE_Read32(CVA);
 
 }
 
 unsigned long getVAR(char phase){
 
 if(phase==0)return ADE_Read32(AVA);
 else if (phase==1)return ADE_Read32(BVA);
 else return ADE_Read32(CVA);
 
 }
 
 unsigned int getPhaseShift(char phase){
 
 if(phase==0)return ADE_Read16(ANGLE0);
 else if (phase==1)return ADE_Read16(ANGLE1);
 else return ADE_Read16(ANGLE2);
 
 }
 
 unsigned long getFVRMS(char phase){
 
 /*
  ADE_Write8(0xE7FE,0xAD);
  ADE_Write8(0xE7E3,0x00);
  
if(phase==0){
 ADE_Write16(HCONFIG,0x00);
 
 }
else if(phase==1){
ADE_Write16(HCONFIG,0x02);

}
else {
ADE_Write16(HCONFIG,0x04);

} 
 Delay_us(130);
 
  ADE_Write8(0xE7FE,0xAD);
  ADE_Write8(0xE7E3,0x80);
  */
 
return ADE_Read32(FVRMS);
 
}

unsigned long getFIRMS(char phase){
 
    //write enable
   //ADE_Write8(0xE7FE,0xAD);
   //ADE_Write8(0xE7E3,0x00);
 
 // if(phase==0)ADE_Write16(HCONFIG,0x08);
// else if(phase==1)ADE_Write16(HCONFIG,0x0A);
// else ADE_Write16(HCONFIG,0x0C);
// Delay_ms(120);

 //Write protection
 //ADE_Write8(0xE7FE,0xAD);
 //ADE_Write8(0xE7E3,0x80);
 
 return ADE_Read32(FIRMS);
 
}

unsigned int getPeriode(char phase){

if(phase==0)return ADE_Read16(APERIODE);
else if(phase==1)return ADE_Read16(BPERIODE);
else return ADE_Read16(CPERIODE);
/*
 checksum=getPeriode(0);
     IntToHex(checksum, txt);
     UART1_Write_Text("periode reg : ");
     UART1_Write_Text(txt);
     UART1_Write('\n');
     UART1_Write('\r');
     
     ff3=(float)checksum*3.90625;
     ff3/=1000;
     
     sprintf(txt, "%12f", ff3);             
     UART1_Write_Text("periode in mili sec: ");
     UART1_Write_Text(txt);
     UART1_Write('\n');
     UART1_Write('\r');

*/

}

unsigned long getVHarmonics(unsigned char comp){
 
 //write enable
ADE_Write8(0xE7FE,0xAD);
 ADE_Write8(0xE7E3,0x00);
  ADE_Write16(HCONFIG,0x00);
 ADE_Write8(HX,comp);
  ADE_Write8(HX,comp);
  ADE_Write8(HX,comp); 

 //Write protection
 ADE_Write8(0xE7FE,0xAD);
 ADE_Write8(0xE7E3,0x80);
  
   Delay_us(115);
 return ADE_Read32(HXVRMS);
 
}

unsigned long getIHarmonics(unsigned char comp){
 
 //write enable
 ADE_Write8(0xE7FE,0xAD);
 ADE_Write8(0xE7E3,0x00);
 
 ADE_Write8(HX,comp);
 
 Delay_us(50);
 //Write protection
 ADE_Write8(0xE7FE,0xAD);
 ADE_Write8(0xE7E3,0x80);
 
 return ADE_Read32(HXIRMS);
 
}

 void ADE_SpiMode(char n){
 //Write shift on first clock edge
 if(n==0){
  SPI1_Init_Advanced(_SPI_FPCLK_DIV32, _SPI_MASTER | _SPI_8_BIT |_SPI_CLK_IDLE_HIGH |
    _SPI_FIRST_CLK_EDGE_TRANSITION | _SPI_MSB_FIRST | _SPI_SS_ENABLE | _SPI_SSM_DISABLE |
    _SPI_SSI_0, &_GPIO_MODULE_SPI1_PB345);
 }
 //Read shift on 2nd clock edge
 else {
 
   SPI1_Init_Advanced(_SPI_FPCLK_DIV32, _SPI_MASTER | _SPI_8_BIT |_SPI_CLK_IDLE_HIGH |
    _SPI_SECOND_CLK_EDGE_TRANSITION | _SPI_MSB_FIRST | _SPI_SS_ENABLE | _SPI_SSM_DISABLE |
    _SPI_SSI_0, &_GPIO_MODULE_SPI1_PB345);
 }
 
 }