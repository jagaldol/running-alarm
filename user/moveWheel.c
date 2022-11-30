#include <moveWheel.h>
#include "ultrasonic.h"
#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

void RCC_Configure_wheel_pinE(void)//must do
{
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);     // RCC GPIO E
}

void GPIO_Configure_wheel(void)//must do
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//정방향(우)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//역방향(우)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//정방향(좌)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//역방향(좌)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
    GPIO_Init(GPIOD , &GPIO_InitStructure);
}

void toFrontRightWheel(){
    GPIO_SetBits(GPIOE, GPIO_Pin_0);//PE0
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);//PE1
}

void toFrontLeftWheel(){
    GPIO_SetBits(GPIOE, GPIO_Pin_2);//PE2
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);//PE3
}

void toBackRightWheel(){
    GPIO_SetBits(GPIOE, GPIO_Pin_1);//오른바퀴
    GPIO_ResetBits(GPIOE, GPIO_Pin_0);
}

void toBackLeftWheel(){
    GPIO_SetBits(GPIOE, GPIO_Pin_3);//왼바퀴
    GPIO_ResetBits(GPIOE, GPIO_Pin_2);
}

void stopAllWheel(){
    GPIO_ResetBits(GPIOE, GPIO_Pin_0);
    GPIO_ResetBits(GPIOE, GPIO_Pin_1);
    GPIO_ResetBits(GPIOE, GPIO_Pin_2);
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
}
//오른바퀴 : PE0, PE1 ... 왼바퀴: PE2, PE3
void setDirectionToFront(){
    toFrontRightWheel();
    toFrontLeftWheel();

}

void setDirectionToBack(){
  toBackRightWheel();
  toBackLeftWheel();
}

void turnToRight(){
  toFrontRightWheel();
  toBackLeftWheel();
}

void turnToLeft(){
  toFrontLeftWheel();
  toBackRightWheel();
  //어느정도 돌아야하지??
}

void turnToRight90(){
  turnToRight();
  delay(1000);//will be changed
  setDirectionToFront();
}

void turnToLeft90(){
  turnToLeft();
  delay(1000);
  setDirectionToFront();
}


void turnToLeftUntil(int isStop){
    turnToLeft();
    if(isStop){
        setDirectionToFront();
    }
}

void turnToRightUntil(int isStop){
    turnToRight();
    if(isStop){
        setDirectionToFront();
    }
}


