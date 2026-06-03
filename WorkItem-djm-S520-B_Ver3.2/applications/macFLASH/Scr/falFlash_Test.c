/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-08-30     zphu       the first version
 */
#include "falFlash_Test.h"







/**
 * @brief  擦除片内数据分区测试
 * @param  fal_index ：  要执行的读\写\擦除的索引
 *        *dev_name  :  设备名称（分区名称或者Flash设备名称）
 *         offset    :  地址的偏移量
 *         size      :  要读取\写入\擦除的数据的大小
 *
 * @return None
 */
#define FAL_HEX_WIDTH                 16
void falFlash_Onchip_Optional(uint8_t fal_index,char *dev_name,uint32_t offset,uint8_t size,uint8_t *wr_data)
{
#define __is_print(ch)                ((unsigned int)((ch) - ' ') < 127u - ' ')
    int result = 0;
    size_t i = 0, j = 0;
    static const struct fal_flash_dev *flash_dev = NULL;
    static const struct fal_partition *part_dev = NULL;

    if((flash_dev = fal_flash_device_find(dev_name)) != NULL){
        part_dev = NULL;
    }
    else if((part_dev = fal_partition_find(dev_name)) != NULL){
        flash_dev = NULL;
    }
    else{
        rt_kprintf("Device %s NOT found. Register failed.\n", dev_name);
        flash_dev = NULL;
        part_dev = NULL;
    }

    if (flash_dev)
    {
        rt_kprintf("\r\n");
        rt_kprintf("Register a flash device \n| %s | addr: %ld | len: %d |.\n", flash_dev->name,
                flash_dev->addr, flash_dev->len);
    }
    else if (part_dev)
    {
        rt_kprintf("\r\n");
        rt_kprintf("Register a flash partition \n| %s | flash_dev: %s | offset: %ld | len: %d |.\n",
                part_dev->name, part_dev->flash_name, part_dev->offset, part_dev->len);
    }
    else
    {
        rt_kprintf("No flash device or partition was register.\n");
        fal_show_part_table();
    }

    /*****************************************************************************************************************/
    /* 对Flash进行读操作 */
    /*****************************************************************************************************************/
    if(fal_index == FAL_READ_INDEX){
        /* 创建一个size大小的空间，并将首地址指针赋值给*data变量 */
        uint8_t *data = rt_malloc(size);
        /* 如果空间真的被创建了 */
        if(data)
        {
            if(flash_dev){
                result = flash_dev->ops.read(offset,data,size);
            }
            else if(part_dev){
                result = fal_partition_read(part_dev, offset, data, size);
            }

            if(result >= 0){
                rt_kprintf("Read data success. Start from 0x%08X, size is %ld. The data is:\n", offset,size);
                rt_kprintf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
                for (i = 0; i < size; i += FAL_HEX_WIDTH)
                {
                    rt_kprintf("[%08X] ", offset + i);
                    /* dump hex */
                    for (j = 0; j < FAL_HEX_WIDTH; j++)
                    {
                        if (i + j < size)
                        {
                            rt_kprintf("%02X ", data[i + j]);
                        }
                        else
                        {
                            rt_kprintf("   ");
                        }
                    }
                    /* dump char for hex */
                    for (j = 0; j < FAL_HEX_WIDTH; j++)
                    {
                        if (i + j < size)
                        {
                            rt_kprintf("%c", __is_print(data[i + j]) ? data[i + j] : '.');
                        }
                    }
                    rt_kprintf("\n");
                }
                rt_kprintf("\n");
            }
            rt_free(data);
        }
        else {
            rt_kprintf("Low memory!\n");
        }
    }


    /*****************************************************************************************************************/
    /* 对Flash进行擦除操作 */
    /*****************************************************************************************************************/
    if(fal_index == FAL_ERASE_INDEX){
        if (flash_dev)
        {
            result = flash_dev->ops.erase(offset, size);
        }
        else if (part_dev)
        {
            result = fal_partition_erase(part_dev, offset, size);
        }
        if (result >= 0)
        {
            rt_kprintf("Erase data success. Start from 0x%08X, size is %ld.\n", offset, size);
        }
    }


    /*****************************************************************************************************************/
    /* 对Flash进行写操作 */
    /*****************************************************************************************************************/
    if(fal_index == FAL_WRITE_INDEX){
        uint8_t *data = rt_malloc(size);
        if(data)
        {
            /* 把要写入的数据的值赋值给新开辟的空间data */
            for (i = 0; i < size; i++)
            {
                data[i] = wr_data[i];
            }
            if (flash_dev)
            {
                result = flash_dev->ops.write(offset, data, size);
            }
            else if (part_dev)
            {
                result = fal_partition_write(part_dev, offset, data, size);
            }
            if (result >= 0)
            {
                rt_kprintf("Write data success. Start from 0x%08X, size is %ld.\n", offset, size);
                rt_kprintf("Write data: ");
                for (i = 0; i < size; i++)
                {
                    rt_kprintf("%d ", data[i]);
                }
                rt_kprintf(".\n");
            }
            rt_free(data);
        }
        else
        {
            rt_kprintf("Low memory!\n");
        }
    }
}











int FalWriteTest(void)
{
    Flag.update = 2;
    /* 写入之前先对0x0801 FFF0进行页擦除 */
#if 0
    falFlash_Onchip_Optional(FAL_ERASE_INDEX,"data",0,1,NULL);
    uint8_t wr_updata[1] = {0};
    wr_updata[0] = 1;
    falFlash_Onchip_Optional(FAL_WRITE_INDEX,"data",0,1,wr_updata);
#endif
    macNorFlash_Write_Word(0x0801FFF0, 1);
    rt_kprintf("PRINTF:%d. iap_upgrade_test \r\n",Record.kprintf_cnt++);
    return RT_EOK;
}
MSH_CMD_EXPORT(FalWriteTest,FalWriteData_Test);


int FalReadTest(void)
{
    uint8_t ReadEmpty = 0;
    falFlash_Onchip_Optional(FAL_READ_INDEX, "data", 0, 1, NULL);
    rt_thread_mdelay(500);
    ReadEmpty = macNorFlash_Read_Byte(0x0801FFF0);
    rt_kprintf("ReadEmpty = %d\r\n",ReadEmpty);
    if(ReadEmpty == 1){
        BEEP_Blink(3);
    }
    return RT_EOK;
}
MSH_CMD_EXPORT(FalReadTest,FalReadData_Test);






