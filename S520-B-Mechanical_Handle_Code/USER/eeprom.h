#ifndef __EEPROM_H
#define __EEPROM_H

#include "stm8s.h"
#include "stdio.h"
#include "string.h"
#include "stm8s_flash.h"

typedef struct
{ 
    u8  START_FLAG;
    u8  TIMES_FLAG;
    u16 H2O_Check_Value;
    u16 OIL_Check_Value;
}DataStoreTypeDef;

extern DataStoreTypeDef  DataStore_Parameter;


#define  START_FLAG_ADDR			        0X4000		
/* 电机位置数据低八位 */
#define  MOTOR_DATA_ADDR_LOW8				0x4001
/* 电机位置数据高八位 */
#define  MOTOR_DATA_ADDR_HIGH8				0x4002	    
/* 电机位置校验数据低八位 */
#define  MOTOR_CHECK_DATA_ADDR_LOW8		    0x4003   
/* 电机位置检验数据高八位 */
#define  MOTOR_CHECK_DATA_ADDR_HIGH8        0x4004   
/* 电机位置检验数据 */
#define  MOTOR_CHECK_DATA_ADDR              0x4005          
/* 速度调整模式 */
#define  ADJUST_SPEED_MODE_ADDR             0x4006
/* 速度补偿值 */
#define  ADJUST_SPEED_VALUE_ADDR            0x4007



void EEPROM_Init(void);
s8 EEPROM_DefValue(void);
uint8_t  EEPROM_ReadData(uint32_t Address);
void EEPROM_WriteData(uint32_t Address,uint8_t Data);



#endif
