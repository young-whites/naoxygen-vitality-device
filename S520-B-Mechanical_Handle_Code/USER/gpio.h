#ifndef _GPIO_H
#define _GPIO_H

#include "stm8s.h"
#include "stm8s_conf.h"
#include "stm8s_gpio.h"

#define MOTOR_VO_PIN      GPIO_PIN_4      //VO//POWER
#define MOTOR_VO_PORT     GPIOD

#define MOTOR_ENA_PIN     GPIO_PIN_6      //ENA_MOTOR
#define MOTOR_ENA_PORT    GPIOC

#define MOTOR_SOF_PIN     GPIO_PIN_5      //SOF_MOTOR
#define MOTOR_SOF_PORT    GPIOC

#define PC1_PIN           GPIO_PIN_2      //PC1
#define PC1_PORT          GPIOD

#define PC2_PIN           GPIO_PIN_4      //PC2
#define PC2_PORT          GPIOB

#define IN1_PIN           GPIO_PIN_4      //IN1
#define IN1_PORT          GPIOC

#define IN2_PIN           GPIO_PIN_7      //IN2
#define IN2_PORT          GPIOC



// #define ON                           0
// #define OFF                          1

// #define IN1(statu)   if(statu==ON)  GPIO_WriteHigh(IN1_PORT,IN1_PIN);\
//                          else            GPIO_WriteLow(IN1_PORT,IN1_PIN); 

// #define IN2(statu)   if(statu==ON)  GPIO_WriteHigh(IN2_PORT,IN2_PIN);\
//                          else            GPIO_WriteLow(IN2_PORT,IN2_PIN); 

#define READ_PC1() GPIO_ReadInputPin(PC1_PORT,PC1_PIN)
#define READ_PC2() GPIO_ReadInputPin(PC2_PORT,PC2_PIN)     
                         
void GPIO_Config(void);

#endif