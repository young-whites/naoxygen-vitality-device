/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-30     zphu       the first version
 */
#ifndef APPLICATIONS_MACAPP_INC_FALFLASH_TEST_H_
#define APPLICATIONS_MACAPP_INC_FALFLASH_TEST_H_
#include "macSys.h"


void falFlash_Onchip_Optional(uint8_t fal_index,char *dev_name,uint32_t offset,uint8_t size,uint8_t *wr_data);



#endif /* APPLICATIONS_MACAPP_INC_FALFLASH_TEST_H_ */
