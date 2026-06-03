/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-30     zphu       the first version
 */
#ifndef APPLICATIONS_MACSYS_INC_IAP_UPGRADE_H_
#define APPLICATIONS_MACSYS_INC_IAP_UPGRADE_H_
#include "macSys.h"



typedef  void (*iapfun)(void);
void jump_sys_bootloader( void );




#endif /* APPLICATIONS_MACSYS_INC_IAP_UPGRADE_H_ */
