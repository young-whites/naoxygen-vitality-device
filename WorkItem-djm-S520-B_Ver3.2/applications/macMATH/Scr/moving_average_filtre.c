#include <applications/macMATH/Inc/moving_average_filtre.h>




/*
 *  滑动滤波器算法
 *  xn            输入的计算值
 *  MVF_LENGTH    滑动滤波的长度
 */
Filter Mov_Filtre;

//初始化滤波器
void Init_filter(Filter *f) {
    for (int i = 0; i < MVF_LENGTH; i++) {
        f->data[i] = 0; //将数据清零
    }
    f->index = 0; //将索引置零
    f->sum = 0; //将和置零
}

//滤波函数，输入新数据，返回滤波后的数据
float moving_average_filtre(Filter *f, float x) {
    f->sum -= f->data[f->index]; //减去最旧的数据
    f->data[f->index] = x; //存储新数据
    f->sum += x; //加上新数据
    f->index = (f->index + 1) % MVF_LENGTH; //更新索引，循环使用数组
    return f->sum / MVF_LENGTH; //返回平均值
}







