#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"
#include "pir.h"
#include "ultrasonic.h"
#include <stdio.h>
#include <stdlib.h>
#include <moveWheel.h>
#include <DS3231_BUZZER.h>

void RCC_Configure(void);
void GPIO_Configure(void);

void EXTI_Configure(void);
void NVIC_Configure(void);

void EXTI1_IRQHandler(void);
void EXTI0_IRQHandler(void);

int humanDetect1 = 0;
int humanDetect2 = 0;
void setDirection();

int directionFlag = 0;
int onOff = 0;

extern int Alarm_ONOFF;
//---------------------------------------------------------------------------------------------------

void RCC_Configure(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);    // interrupt
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);  // RCC GPIO D
}

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

    
   
   // NOTE: do not select the UART GPIO pin used as EXTI Line here
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    // TODO: fill the arg you want
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

   // TODO: Initialize the NVIC using the structure 'NVIC_InitTypeDef' and the function 'NVIC_Init'
   
    // human body detect1
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // human body detect2
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void EXTI1_IRQHandler(void) { // PIR INTERRUPT1
   if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
          
          humanDetect1++;
          
          
          EXTI_ClearITPendingBit(EXTI_Line1);
   }
}

void EXTI0_IRQHandler(void) { // PIR INTERRUPT2
   if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
          
          humanDetect2++;
          
          
          EXTI_ClearITPendingBit(EXTI_Line0);
   }
}


void delay(void) {
   int i;
   for (i = 0; i < 20000000; i++) {}
}

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
  }
}

extern int Alarm_ONOFF;
int main() {
  // LCD 관련 설정은 LCD_Init에 구현되어 있으므로 여기서 할 필요 없음
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  EXTI_Configure();
  GPIO_Configure_wheel();
  RCC_Configure_wheel_pinE();
  
  NVIC_Configure();
  // ------------------------------------
  Init_PIR();
  InitHCSR04();

  LCD_Init();
  LCD_Clear(WHITE);
  
  //----------------------------------------
    DS3231_Alarm_Init();
  
  ds3231_time ds_time_default;   //구조체 변수 선언

  //DS3231에 새롭게 입력할 시간 데이터들을 설정 (위에서 선언한 변수 사용)
  ds_time_default.sec=0;
  ds_time_default.min=55;
  ds_time_default.hour_select.am_pm_24=ds3231_PM;
  ds_time_default.hour_select.hour=8;
  ds_time_default.day=3;
  ds_time_default.date=29;
  ds_time_default.month=11;
  ds_time_default.year=22;

  ds3231_write_time(&ds_time_default);   //구조체를 이용해 DS3231에 시간 데이터 입력
  ds3231_read_time(&ds_time_default);   
  //시간이 변경됐는지 확인하기 위해 타임 레지스터에 저장되어있는 시간 데이터 읽어옴
  
  ds3231_Alarm1 alarm1_default;
  
  alarm1_default.sec=20;
  alarm1_default.min=55;
  alarm1_default.hour_select.am_pm_24=ds3231_PM;
  alarm1_default.hour_select.hour=8;
  alarm1_default.day_date_select.value=29;
  alarm1_default.day_date_select.day_or_date=ds3231_date;
  
  ds3231_set_alarm1(&alarm1_default);
  ds3231_read_alarm1(&alarm1_default);
  

  while(1){
    int32_t dist = HCSR04GetDistance();
    LCD_ShowNum(65, 160, dist, 5, BLACK, WHITE);
    
    LCD_ShowNum(65, 190, humanDetect1, 5, BLACK, WHITE);
    LCD_ShowNum(65, 220, humanDetect2, 5, BLACK, WHITE);
    //---------------------------------------------------
    ds3231_read_time(&ds_time_default);
    alarm_check(&ds_time_default, &alarm1_default);
    Delay_little();
    setDirection(directionFlag);
    if(dist <= 200){
      //todo 초음파 센서 값 비교
      directionFlag = 3;
    }
    
    if (Alarm_ONOFF == 0) {
      
    }
  }
}