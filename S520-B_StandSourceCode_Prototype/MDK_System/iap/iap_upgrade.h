#ifndef __IAP_UPGRADE_H
#define __IAP_UPGRADE_H
#include "sys.h"





typedef  void (*iapfun)(void);				
void Iap_Jump_app(uint32_t appxaddr);
void soft_reset(void);
void jump_sys_bootloader( void );
void IAP_JumpToApplication(void);
void Update_CommandReceive(uint8_t data);

/* 用于存放IAP升级标志位 */
#define FLASH_APP_UP_ADDR 0x0801E000  




























#endif /*__IAP_UPGRADE_H*/
