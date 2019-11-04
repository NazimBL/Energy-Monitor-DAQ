#include "TIM_COMMUN.h"
#include "TIM4.h"
#include "AFIO.h"
#include "GPIO.h"

unsigned int overflow_count = 0;
unsigned long pulse_ticks = 0;
unsigned long start_time = 0;
unsigned long end_time = 0;
unsigned long channelA,channelB=0;

const double PI= 3.14159265358979323846264338327950288419716939937510;

void setup_TIM4();

//1 = postive , 0 negative
int motorDirection=1;

char kk;
char readbuff[64],writebuff[64],writebuff2[64];

void adc_setup();
void timer_setup();
void InitTimer2();

unsigned long l1[64],l2[64],l3[64],i1[64],i2[64],i3[64];
unsigned long vdc1[64],vdc2[64],idc[64];
unsigned long encoder[64];
unsigned long averages[8];
void emgDac();

unsigned char lowB,highB;

char count=0;
void sendSample(int f);
unsigned long average(char index);
char p=0;
//send variable is used to synchronize sending
char send=0;
char n=0;
void averageBF();


//Sampling interupt
void Timer2_interrupt() iv IVT_INT_TIM2 {

  TIM2_SR.UIF = 0;
  if(count>63){
     count=0;
     send=1;
  }
  if(send==0){

              averageBF();
              l3[count]=averages[0];
              i3[count]=averages[1];
              l2[count]=averages[2];
              i2[count]=averages[3];
              l1[count]=averages[4];
              i1[count]=averages[5];
              idc[count]=averages[6];
              vdc1[count]=averages[7];
              vdc2[count]=averages[8];
              encoder[count]=pulse_ticks;

  count++;
  }
}
  
//encoder input capture interrupt
void TIM4_ISR() iv IVT_INT_TIM4 ics ICS_AUTO
{
    if(TIM4_SRbits.UIF == 1)
    {
        TIM4_SRbits.UIF = 0;
        overflow_count++;
    }
    if(TIM4_SRbits.CC1IF == 1)
    {

        TIM4_SRbits.CC1IF = 0;
        end_time = TIM4_CCR1;
        channelA=end_time;
        channelB=TIM4_CCR2;
        if(GPIOB_IDRbits.IDR7)motorDirection=1;
        else motorDirection=0;

        pulse_ticks = ((overflow_count << 16) - start_time + end_time);
        start_time = end_time;
        overflow_count = 0;
    }

}


void main()
{

  HID_Enable(&readbuff,&writebuff);
  adc_setup();
  //debug led
  GPIO_Digital_Output(&GPIOC_BASE, _GPIO_PINMASK_13);
  InitTimer2();
  //Timer 4 input capture pins
  setup_TIM4();

  Delay_ms(200);
  GPIOC_ODR=0x0000;

  while(1)
  {
    USB_Polling_Proc();
    kk = HID_Read();
    if(kk != 0)
    {
       emgDAC();  
    }
}

void emgDac(){

    if(send==1){
    //if O is received send 10 packets of 2 periods ( 2 buffers == 128 bytes )
    if((char)readbuff[0]=='O'){

      for(p=0;p<10;p++)sendSample(p);
      }    
    }
    readbuff[0]='x';
}

void generateSin(){
    
 for(n=0;n<64;n++){

  double arg=(2*PI*n)/32;
  double arg2=arg+(2*PI/3);
  double arg3=arg+(4*PI/3);

  arg2=sin(arg2);
  arg3=sin(arg3);
  arg=sin(arg);

  arg*=2048;
  arg2*=2048;
  arg3*=2048;
  l3[n]=arg+2048;
  l2[n]=arg2+2048;
  l1[n]=arg3+2048;
  }
}

//sampling rate setting
void InitTimer2(){
  RCC_APB1ENR.TIM2EN = 1;
  TIM2_CR1.CEN = 0;
  //0 and 17999
  TIM2_PSC = 0;
  //45
  TIM2_ARR = 44999;
  //TIM2_ARR = 35999;
  NVIC_IntEnable(IVT_INT_TIM2);
  TIM2_DIER.UIE = 1;
  TIM2_CR1.CEN = 1;
}

void adc_setup(){

  ADC_Set_Input_Channel(_ADC_CHANNEL_0);
  ADC_Set_Input_Channel(_ADC_CHANNEL_1);
  ADC_Set_Input_Channel(_ADC_CHANNEL_2);
  ADC_Set_Input_Channel(_ADC_CHANNEL_3);
  ADC_Set_Input_Channel(_ADC_CHANNEL_4);
  ADC_Set_Input_Channel(_ADC_CHANNEL_5);
  ADC_Set_Input_Channel(_ADC_CHANNEL_6);
  ADC_Set_Input_Channel(_ADC_CHANNEL_7);
  ADC_Set_Input_Channel(_ADC_CHANNEL_8);
  ADC1_Init();
}

//sending samples , high and low bytes of adc read
void sendSample(int f){

      char i=0,j=0;
      for(i=0;i<64;i++){
      if(f==0){
      lowB=l3[i]&0xFF;
      highB=l3[i]>>8;
      }else if(f==1){
      lowB=i3[i]&0xFF;
      highB=i3[i]>>8;
      }else if(f==2){
      lowB=l2[i]&0xFF;
      highB=l2[i]>>8;
      }
      else if(f==3){
      lowB=i2[i]&0xFF;
      highB=i2[i]>>8;
      }else if(f==4){
      lowB=l1[i]&0xFF;
      highB=l1[i]>>8;
      }else if(f==5){
      lowB=i1[i]&0xFF;
      highB=i1[i]>>8;
      }else if(f==6){
      lowB=idc[i]&0xFF;
      highB=idc[i]>>8;
      }else if(f==7){
      lowB=vdc1[i]&0xFF;
      highB=vdc1[i]>>8;
      }else if(f==8){
      lowB=vdc2[i]&0xFF;
      highB=vdc2[i]>>8;
      }else if(f==9){
      lowB=encoder[i]&0xFF;
      highB=encoder[i]>>8;
      }
      if(n==0){
      writebuff[j++]=highB;
      writebuff[j++]=lowB;
      }else{
      writebuff2[j++]=highB;
      writebuff2[j++]=lowB;
      }
     
      //send first packet when buffer is full
      if(j==64){
      j=0;
      n=1;
      //send=0;
      }
      }
       HID_Write(writebuff,64);
       HID_Write(writebuff2,64);
       n=0;
       send=0;
}

//input capture timer setting
void setup_TIM4()
{
     GPIO_Config(&GPIOB_BASE,_GPIO_PINMASK_6 | _GPIO_PINMASK_7,_GPIO_CFG_MODE_INPUT | _GPIO_CFG_PULL_UP);
     
     enable_TIM4(true);
     enable_TIM4_counter(false);
     TIM4_ARR = 0xFFFF;
     TIM4_PSC = 719;
     //100 khz
     TIM4_CCERbits.CC1P=0;
     set_TIM4_counting_direction(up_counting);
     set_TIM4_clock_division(clock_division_tCK_INT);
     set_TIM4_CC1_selection(CC1_input_IC1_on_TI1);
     set_TIM4_CC1_state_and_polarity(enable, rising_edge);
     set_TIM4_IC1_input_prescalar(0);
     set_TIM4_IC1_filter(3);

     TIM4_CCERbits.CC2P=0;
     set_TIM4_CC2_selection(CC2_input_IC2_on_TI2);
     set_TIM4_CC2_state_and_polarity(enable, rising_edge);
     set_TIM4_IC2_input_prescalar(0);
     set_TIM4_IC2_filter(3);

     enable_TIM4_CC1_interrupt(true);
     enable_TIM4_update_interrupt(true);
     NVIC_IntEnable(IVT_INT_TIM4);
     EnableInterrupts();
     enable_TIM4_counter(true);
}

//averaging readings for accuracy
unsigned long average(char index){
char i=0;
unsigned long avr=0;

for(i=0;i<60;i++)avr+=ADC1_Get_Sample(index);
avr=(long)(avr/60);

return avr;

}

//averaging readings for accuracy
void averageBF(){

char i=0;
char size=50;
unsigned long avr1=0,avr2=0,avr3=0,avr4=0,avr5=0,avr6=0,avr7=0,avr8=0,avr9=0;

for(i=0;i<size;i++){
avr1+=ADC1_Get_Sample(0);
avr2+=ADC1_Get_Sample(1);
avr3+=ADC1_Get_Sample(2);
avr4+=ADC1_Get_Sample(3);
avr5+=ADC1_Get_Sample(4);
avr6+=ADC1_Get_Sample(5);
avr7+=ADC1_Get_Sample(6);
avr8+=ADC1_Get_Sample(7);
avr9+=ADC1_Get_Sample(8);
}
averages[0]=(long)(avr1/size);
averages[1]=(long)(avr2/size);
averages[2]=(long)(avr3/size);
averages[3]=(long)(avr4/size);
averages[4]=(long)(avr5/size);
averages[5]=(long)(avr6/size);
averages[6]=(long)(avr7/size);
averages[7]=(long)(avr8/size);
averages[8]=(long)(avr9/size);
}
