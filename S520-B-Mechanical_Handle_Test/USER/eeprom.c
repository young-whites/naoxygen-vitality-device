#include "eeprom.h"
#include "stm8s_wwdg.h"
#include "motor.h"
#include "Decode.h"

DataStoreTypeDef  DataStore_Parameter;
DataStoreTypeDef  *ds;
/******如果是第一次启动，就往eeprom里写入初始值*******/


/**
 * @brief  Flash 初始化
 * @param  None
 * @retval None
 */
void EEPROM_Init()
{
    /* DeInitializes the FLASH registers to their default reset values. */
    FLASH_DeInit();
    /*  Sets the fixed programming time */
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}



/**
 * @brief  EEPROM 向指定地址写入数据
 * @param  Address : 要写入的地址
 *         Data    : 要写入的数据
 * @retval None
 */
void EEPROM_WriteData(uint32_t Address,uint8_t Data)
{
    /* Flash 解锁 */
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    /* 向指定地址写入数据 */
    FLASH_ProgramByte(Address, Data);
    /* 等待标志位置位 */
    while (FLASH_GetFlagStatus(FLASH_FLAG_EOP) == RESET);
    /* Flash 解锁 */
    FLASH_Lock(FLASH_MEMTYPE_DATA);
}




/**
 * @brief  EEPROM 从指定地址读数据
 * @param  Address : 要读取的地址
 * @retval 读取的数据
 */
uint8_t  EEPROM_ReadData(uint32_t Address)
{
    uint8_t eeprom_data;

    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    eeprom_data = FLASH_ReadByte(Address);
    FLASH_Lock(FLASH_MEMTYPE_DATA);

    return  eeprom_data;
}


