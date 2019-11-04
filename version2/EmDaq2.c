#include "ade7880.h"

void InitTimer2();
void fetchData();
void SpiSetup();
void dspRead();

const unsigned  char DATASIZE=22;
char kk;
char readbuff[64],writebuff[64];
unsigned long dat[DATASIZE],prev[DATASIZE];
unsigned long checksum=0;
//send variable is used to synchronize sending
char send=0;
char txt[17];
unsigned long maxReg=0x514791;
unsigned char angle=0;
float ff3;
float Sscale=0.00390,Pscale=0.0038816111;
char b0,b1,b2; 
char txt2[5];

//Sampling interupt
void Timer2_interrupt() iv IVT_INT_TIM2 {

  TIM2_SR.UIF = 0;
 
  if(send==0){
  dspRead(); 
  send=1; 
  }  
}
  
void main() {

    GPIO_Digital_Output(&GPIOC_BASE, _GPIO_PINMASK_13);
    HID_Enable(&readbuff,&writebuff);
    InitTimer2();
    SpiSetup();
    UART1_Init_Advanced(19200, _UART_8_BIT_DATA, _UART_NOPARITY, _UART_ONE_STOPBIT, &_GPIO_MODULE_USART1_PB67);  
    ADE_SpiEnable();
    ADE_Init();
    for(angle=0;angle<DATASIZE;angle++)prev[angle]=0x00FF;
    Delay_ms(200);
    GPIOC_ODRbits.ODR13=0;
  

  while(1)
  {
   
    USB_Polling_Proc();
    kk = HID_Read();
   
    if(kk != 0)
    {
      if((char)readbuff[0]=='O' && send==1){
    //if O is received send 9 packets of 2 periods ( 2 buffers == 128 bytes )

       fetchData();
       readbuff[0]='x';
       send=0;
    
      }
    } 
  
  }
}

//sampling rate setting
void InitTimer2(){

  RCC_APB1ENR.TIM2EN = 1;
  TIM2_CR1.CEN = 0;
  TIM2_PSC = 1999; 
  TIM2_ARR = 35499;
  NVIC_IntEnable(IVT_INT_TIM2);
  TIM2_DIER.UIE = 1;
  TIM2_CR1.CEN = 1;
}

void fetchData(){
   
   unsigned char j=0,i=0;
  
    for(angle=0;angle<DATASIZE;angle++)if(dat[angle]==0 || dat[angle]==0xFFFFFFFF)dat[angle]=prev[angle];                
    for(i=0;i<DATASIZE;i++){
    
     b0=(unsigned char)(dat[i] & 0xFF);
     b1=(unsigned char)(dat[i] >> 8);
     b2=(unsigned char)(dat[i] >> 16);
        
     if(i==17 || i==13){
      writebuff[j++]=b1;
      writebuff[j++]=b0;
     }else{
     writebuff[j++]=b2;
     writebuff[j++]=b1;
     writebuff[j++]=b0;
     }
    
    }
    HID_Write(writebuff,64);
}

void dspRead(){

              dat[0]=getVRMS(2);
              dat[1]=getIRMS(2);
              dat[2]=getVRMS(1);
              dat[3]=getIRMS(1);
              dat[4]=getVRMS(0);
              dat[5]=getIRMS(0);
              dat[6]=getVA(2);
              dat[7]=getWatt(2);
              //dat[7]&=0x000FFFFF;
              dat[8]=getVA(1);
              dat[9]=getWatt(1);
             // dat[9]&=0x000FFFFF;
              dat[10]=getVA(0);
              dat[11]=getWatt(0);
              //dat[11]&=0x000FFFFF;
              dat[12]=ADE_Read32(FVRMS);
              dat[13]=ADE_Read32(HXVRMS);
              dat[14]=ADE_Read32(HYVRMS);
              dat[15]=ADE_Read32(HZVRMS);
              dat[16]=ADE_Read32(FIRMS);
              dat[17]=ADE_Read32(HXIRMS);
              dat[18]=ADE_Read32(HYIRMS);
              dat[19]=ADE_Read32(HZIRMS);
              dat[20]=ADE_Read32(ITHD);
              dat[21]=ADE_Read32(VTHD);
              angle=getPhaseShift(0); 
              
              for(angle=0;angle<DATASIZE;angle++){
              if(dat[angle]!=0 && dat[angle]!=0xFFFFFFFF)prev[angle]=dat[angle];
              }   
}

void SpiSetup(){
   
    GPIO_Digital_Output(&GPIOB_BASE, _GPIO_PINMASK_3| _GPIO_PINMASK_5);
    GPIO_Digital_Input(&GPIOB_BASE, _GPIO_PINMASK_4);
    GPIO_Digital_Output(&GPIOA_BASE, _GPIO_PINMASK_15);// Set CS# pin as Output
    GPIOA_ODRbits.ODR15=1; 
    AFIOEN_bit = 1;
    SWJ_CFG1_bit = 1; 
    SPI1_Init_Advanced(_SPI_FPCLK_DIV32, _SPI_MASTER | _SPI_8_BIT |_SPI_CLK_IDLE_HIGH |
    _SPI_SECOND_CLK_EDGE_TRANSITION | _SPI_MSB_FIRST | _SPI_SS_ENABLE | _SPI_SSM_DISABLE |
    _SPI_SSI_0, &_GPIO_MODULE_SPI1_PB345);
    AFIO_MAPR.SPI1_REMAP=1;
}