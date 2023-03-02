#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
//use pc2 ~ 5
void RCC_Configure_wheel_pinE();//must do
void GPIO_Configure_wheel();//must do
void toFrontRightWheel();
void toFrontLeftWheel();
void toBackRightWheel();
void toBackLeftWheel();
void stopAllWheel();
void setDirectionToFront();
void setDirectionToBack();
void turnToRight();
void turnToLeft();
void turnToRight90();
void turnToLeft90();
void turnToRightUntil();
void turnToLeftUntil();
