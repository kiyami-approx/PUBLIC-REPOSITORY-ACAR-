#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "stm32f1xx_hal.h"
#include "speed.h"
#include "delay_us.h"

//状态机：0=空闲，等待触发；1=已经捕获上升沿，等待下降沿
typedef enum{
    ULTRA_IDLE,
    ULTRA_WAIT_FALL
}Ultra_State_t;

//小车工作状态
typedef enum
{
    CAR_STATE_NORMAL,        //正常直行

    CAR_STATE_AVOID_TURN     //避障原地旋转
}Car_State_t;


// 超声波返回距离，单位cm
extern uint16_t ultra_dist_cm;


extern Car_State_t car_state;
extern uint16_t  t1_val;
extern uint16_t  t2_val;
extern uint32_t  pulse_us;
extern Ultra_State_t ultra_state;

// 超声波返回距离，单位cm
extern uint16_t ultra_distance_cm;

/**
 * @brief  发送Trig触发脉冲，读取一次HC‑SR04距离
 * @retval 0:正常获取; 1:忙，本次不测量
 */
uint8_t HC_SR04_Start(void);

/**
 * @brief 超声波避障逻辑，放在main while(1)循环调用
 * @note 距离小于100cm(1米)，执行原地旋转；大于1m直行
 */
void HC_SR04_AvoidObstacle(void);

#endif
