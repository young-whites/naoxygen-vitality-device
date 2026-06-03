#ifndef __MOVING_AVERAGE_FILTRE_H

#define __MOVING_AVERAGE_FILTRE_H
#include <applications/macSYS/Inc/macSYS.h>


//滤波器长度
#define         MVF_LENGTH                  5

//定义滤波器结构体
typedef struct {
    float data[MVF_LENGTH]; //存储数据的数组
    int index; //当前数据的索引
    float sum; //当前数据的和
} Filter;

extern Filter Mov_Filtre;



void  Init_filter(Filter *f);
float moving_average_filtre(Filter *f, float x);



#endif /*__MOVING_AVERAGE_FILTRE_H*/
