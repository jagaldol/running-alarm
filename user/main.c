#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "lcd.h"
#include "touch.h"
#include "pir.h"
#include "ultrasonic.h"

#include <stdio.h>
#include <stdlib.h>
#include <moveWheel.h>
#include <DS3231_BUZZER.h>
#include <stop_button.h>

void RCC_Configure(void);
void GPIO_Configure(void);

void EXTI_Configure(void);
void NVIC_Configure(void);

void EXTI1_IRQHandler(void);
void EXTI0_IRQHandler(void);
void delay_wheel(int);


int humanDetect1 = 0;
int humanDetect2 = 0;
void setDirection();

int directionFlag = 0;
int onOff = 1;
extern int Alarm_ONOFF;//have to combine isAlramOn
//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    // interrupt
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);  // RCC GPIO D


  TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);//ㅁㅁㅁㅁ??
// Clear TIM2 Capture compare interrupt pending bit

}

//------------------------------------------------- blue tooth below------------
#include "bluetooth.h"

uint16_t receive_string[50];
int string_count = 0;
int day = 0;
int hour = 0;
int minute = 0;
int string_receive_offset = 0;
int start_offset = 0;

//---------------------------------------------------------------------------------------------------

void RCC_Configure_bluetooth(void)
{
   // TODO: Enable the APB2 peripheral clock using the function 'RCC_APB2PeriphClockCmd'
   /* UART TX/RX port clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); //UART 4

   /* USART1 clock enable */ 
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
      
   /* Alternate Function IO clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}

void GPIO_Configure_bluetooth(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

   // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
    
    //TX2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //RX2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART1_Init(void)
{
   USART_InitTypeDef USART_InitStructure;

   // Enable the USART1 peripheral
   USART_Cmd(UART4, ENABLE);
   
   // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
   USART_InitStructure.USART_BaudRate = 9600;
   USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode= USART_Mode_Rx| USART_Mode_Tx;
   USART_Init(UART4, &USART_InitStructure);
   
   // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'
   USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

void NVIC_Configure_bluetooth(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

   // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
   
    // UART1
   // 'NVIC_EnableIRQ' is only required for USART setting
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_EnableIRQ(UART4_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void UART4_IRQHandler() {
   uint16_t word;
    if(USART_GetITStatus(UART4,USART_IT_RXNE)!=RESET){
       // the most recent received data by the USART1 peripheral
       word = USART_ReceiveData(UART4);
       printf("receive4 : %c\n", word);
       
       if (start_offset == 1) {           
         if (word == 0x3b) { //SetAlarm 7-19:20 [0x53, 0x65, 0x74, 0x41, 0x6c, 0x61, 0x72, 0x6d, 0x20, 0x37, 0x2d, 0x31, 0x39, 0x3a, 0x32, 0x30]
           printf("start partition\n");
           string_partition();
           start_offset = 0;
         }
         else {
           receive_string[string_count] = word;
           string_count++;

           // clear 'Read data register not empty' flag
           USART_ClearITPendingBit(UART4,USART_IT_RXNE);
         }
       }
       
       if (word == 0x40) {
         start_offset = 1;
       }
    }
}

void sendDataToUART4(uint16_t data) {
   while ((UART4->SR & USART_SR_TXE) == 0);
   USART_SendData(UART4, data);
}

void string_partition(void) {
  char String[50];
  
  for (int i = 0; i < string_count; i++) {
    String[i] = (char)receive_string[i];
    receive_string[i] = 0;
  }
  
  string_count = 0;
  
  char temp[50], *point;
  char* parti = " -:";
  char* partion_char[10];
  int i = 0;
  
  strcpy(temp, String);
  point = strtok(temp, parti);
  
  while(point) {
    partion_char[i] = point;
    i++;
    point = strtok(NULL, parti);
  }
  
  if (strcmp(partion_char[0], "SetAlarm") == 0) {
    day = atoi(partion_char[1]);
    hour = atoi(partion_char[2]);
    minute = atoi(partion_char[3]);
    printf("DAY : %d ", day);
    printf("HOUR : %d ", hour);
    printf("MINUTE : %d ", minute);
    string_receive_offset = 1;
    
    char msg[] = "Alarm setting Complete";
                        
    for(int i = 0; i < 22; i++) {
         sendDataToUART4(msg[i]);
    }
    sendDataToUART4(day);
    sendDataToUART4(0x20);
    sendDataToUART4(hour);
    sendDataToUART4(0x20);
    sendDataToUART4(minute);
  }
}

void bluetooth_Init(void) {
  RCC_Configure_bluetooth();
  GPIO_Configure_bluetooth();
  USART1_Init();
  NVIC_Configure_bluetooth();
}

int get_day(void) {
  return day;
}

int get_hour(void) {
  return hour;
}

int get_minute(void) {
  string_receive_offset = 0;
  
  return minute;
}

int get_receive_flag(void) {
  return string_receive_offset;
}

//--------------------------------------------------------------------------

void GPIO_Configure(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
 
  // TODO: Initialize the GPIO pins using the structure 'GPIO_InitTypeDef' and the function 'GPIO_Init'
}

void EXTI_Configure(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

   // TODO: Select the GPIO pin (Joystick, button) used as EXTI Line using function 'GPIO_EXTILineConfig'
   // TODO: Initialize the EXTI using the structure 'EXTI_InitTypeDef' and the function 'EXTI_Init'
   
    /* human body1 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource1);
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    /* human body2 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource0);
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Stop button */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
   
   // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

   // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
   
    // human body detect1
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // human body detect2
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    
    /* stop button*/
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//most high priority
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_IRQHandler(void) { // PIR INTERRUPT1
   if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
          humanDetect1 ++;
          //setDirection(2);
          
          EXTI_ClearITPendingBit(EXTI_Line1);
   }
}

void EXTI0_IRQHandler(void) { // PIR INTERRUPT2
   if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
          
          humanDetect2 ++;
            //setDirection(1);
          
          EXTI_ClearITPendingBit(EXTI_Line0);
   }
}

void EXTI3_IRQHandler(void) {//stop button
   if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
     if (Alarm_ONOFF == 1) {
          directionFlag = 0;
          Alarm_ONOFF = 0;
          setDirection(0);
          humanDetect1 = 0;
          humanDetect2 = 0;
          GPIO_ResetBits(GPIOC, GPIO_Pin_8);

     }
     /*
     else {
                 directionFlag = 1;
          Alarm_ONOFF = 1;
          setDirection(directionFlag);

     }*/
          
          EXTI_ClearITPendingBit(EXTI_Line3);
   }
}

//---------------------interrupt button ---------------------------


//-------------------------------------------------------------

void setDirection(int flag){
  switch(flag){
  case 0:
    stopAllWheel();
    break;
  case 1:
    setDirectionToFront();
    break;
  case 2:
    setDirectionToBack();
    break;
  case 3:
    turnToRight();
    break;
  case 4:
    turnToLeft();
    break;
  case 5:
    turnToRight90();
    break;
  case 6:
    turnToLeft90();
    break;
  case 7:
    break;
  }
}


int pattern[7] = {1, 3, 4, 1, 3, 1, 4}; //wheel flag pattern

void delay(int n){
  int i = 0;
  for(i = 0; i < n;i++);
};

int main() {
  int alarm_flag = 0;
  
  SystemInit();
  bluetooth_Init();
  RCC_Configure();
  GPIO_Configure();
  EXTI_Configure();
  GPIO_Configure_wheel();
  RCC_Configure_wheel_pinE();
  Init_BUTTON();
  NVIC_Configure();
  // ------------------------------------
  Init_PIR();
  InitHCSR04();

  LCD_Init();
  LCD_Clear(WHITE);
  
  
  
  //----------------------------------------
  DS3231_Alarm_Init();
  
  ds3231_Alarm1 alarm1_default;
  
  ds3231_time ds_time_default;   //구조체 변수 선언

  //DS3231에 새롭게 입력할 시간 데이터들을 설정 (위에서 선언한 변수 사용)
  ds_time_default.sec=0;
  ds_time_default.min=5;
  ds_time_default.hour_select.am_pm_24=ds3231_24_hour;
  ds_time_default.hour_select.hour=10;
  ds_time_default.day=3;
  ds_time_default.date=19;
  ds_time_default.month=12;
  ds_time_default.year=22;

  ds3231_write_time(&ds_time_default);   //구조체를 이용해 DS3231에 시간 데이터 입력
  ds3231_read_time(&ds_time_default);   
  //시간이 변경됐는지 확인하기 위해 타임 레지스터에 저장되어있는 시간 데이터 읽어옴
  
  alarm1_default.sec=0;
  alarm1_default.min = 6;
  alarm1_default.hour_select.am_pm_24=ds3231_24_hour;
  alarm1_default.hour_select.hour = 10;
  alarm1_default.day_date_select.value = 19;
  alarm1_default.day_date_select.day_or_date=ds3231_date;
  
  ds3231_set_alarm1(&alarm1_default);
  ds3231_read_alarm1(&alarm1_default);
  //GPIO_SetBits(GPIOB, GPIO_Pin_7);
  //GPIO_ResetBits(GPIOB, GPIO_Pin_7);
   int timeNow = delayByTim2();
   while(1){
    //setDirection(wheel_flag);

    //LCD_ShowNum(150, 100, Alarm_ONOFF, 5, BLACK, WHITE);//check alram is on or off

    //---------------------------------------------------
    ds3231_read_time(&ds_time_default);
    ds3231_read_alarm1(&alarm1_default);
    alarm_check(&ds_time_default, &alarm1_default);
    //Delay_little();
    
    //-------------
    
    if (get_receive_flag() == 1) {
      alarm1_default.day_date_select.value = get_day();
      alarm1_default.hour_select.hour = get_hour();
      alarm1_default.min = get_minute();
      ds3231_set_alarm1(&alarm1_default);
      ds3231_read_alarm1(&alarm1_default);
    }
    //--------------    
    //alarm On below
    if(Alarm_ONOFF == 1) GPIO_SetBits(GPIOC, GPIO_Pin_8);
    while(Alarm_ONOFF){
      directionFlag = 1;
      setDirection(directionFlag);
      
      //ds3231_read_time(&ds_time_default);
    
        //GPIO_SetBits(GPIOC, GPIO_Pin_8);//buzzer on
        //setDirection(directionFlag);
        
  //---------------------
        //semi -random using sec in Tim2
       /*
        if(alarm1_default.sec- timeNow>=6){//per 6sec
          int random = alarm1_default.sec%7;
          LCD_ShowNum(65, 240, pattern[random], 5, BLACK, WHITE);//will delete
          setDirection(pattern[random]);
          timeNow = alarm1_default.sec;
        }*/ 
  //-------------------------
        //get distance 
        int32_t dist = HCSR04GetDistance();
        int32_t dist2 = HCSR04GetDistance2();//left 
        int32_t dist3 = HCSR04GetDistance3();
        if(dist >= 1400) dist = 1400;
        if(dist2 >= 1400) dist2 = 1400;
        if(dist3 >= 1400)dist3 = 1400;
        LCD_ShowNum(65, 160, dist, 5, BLACK, WHITE);
        LCD_ShowNum(65, 180, dist2, 5, BLACK, WHITE);
        LCD_ShowNum(65, 200, dist3, 5, BLACK, WHITE);
  //--------------------
        //avoid wall
        int isBiggerLeft = (dist2 >= dist3);
        int isBiggerRight = (dist2 < dist3);
        int timeD = delayByTim2()-timeNow;
        LCD_ShowNum(65, 220, timeD, 5, BLACK, WHITE);

        if(dist <=400 && timeD>10){//front 값 조정 필요
          timeNow = delayByTim2();
          if(isBiggerLeft){
            while(1){//go left
              setDirection(4);
              
              int32_t a = HCSR04GetDistance();
              int32_t b = HCSR04GetDistance2();//left 
              int32_t c = HCSR04GetDistance3();
              int endCondition =(a>=150);
              delay(1000000);        
              LCD_ShowNum(65, 160, a, 5, BLACK, WHITE);
              LCD_ShowNum(65, 180, b, 5, BLACK, WHITE);
              LCD_ShowNum(65, 200, c, 5, BLACK, WHITE);
              if(endCondition || Alarm_ONOFF==0) break;
            }
          }
          else if(isBiggerRight){
            while(1){//go right
              setDirection(3);
              
              int32_t a = HCSR04GetDistance();
              int32_t b = HCSR04GetDistance2();//left 
              int32_t c = HCSR04GetDistance3();
              int endCondition =( a>=150);
              delay(1000000);
              LCD_ShowNum(65, 160, a, 5, BLACK, WHITE);
              LCD_ShowNum(65, 180, b, 5, BLACK, WHITE);
              LCD_ShowNum(65, 200, c, 5, BLACK, WHITE);
              if(endCondition || Alarm_ONOFF==0) break;
            }
          }
        }
    }//end of wihle ...alarm On 
    directionFlag = 0;
    setDirection(0);
  }//end of while usual 
}