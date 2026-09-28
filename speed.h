#ifndef __SPEED_H
#define __SPEED_H

#include "main.h"
#include "tim.h"

extern volatile uint8_t g_force_stop;
/* 硬件参数，和tim.c保持一致 */
#define PWM_MAX_CCR      999U      // Period=999，CCR最大值
#define PWM_PERCENT_MAX  100U

/* 把main.c的循迹相关宏换算成百分比(0‑100) */
#define AVOID_GO_PERCENT       24U
#define TRACK_GO_PERCENT       29U    // 240/999*100 ≈24
#define TRACK_GO_SQUARE_PERCENT       28U
#define TRACK_TURN_SLIGHT_PERC 50U    // 400/999*100 ≈40
#define TRACK_TURN_SHARP_PERC  70U    // 650/999*100 ≈65
#define TRACK_TURN_SQUARE_PERC  90U
#define TRACK_TURN_FW_PERCENT  21U    // 200/999*100 ≈20
#define TRACK_TURN_FW_SQUARE_PERCENT  30U
#define MANUAL_SPEED_PERCENT 40U

/* 电机通道枚举，方便调用 */
typedef enum
{
    MOTOR_TIM2_CH1,     // PA0
    MOTOR_TIM2_CH2,     // PA1
    MOTOR_TIM2_CH3,     // PA2
    MOTOR_TIM2_CH4,     // PA3

    MOTOR_TIM4_CH1,     // PB6
    MOTOR_TIM4_CH2,     // PB7
    MOTOR_TIM4_CH3,     // PB8
    MOTOR_TIM4_CH4      // PB9
}Motor_Channel_t;

/**
 * @brief  初始化所有电机PWM输出，启动全部通道PWM
 * @retval none
 */
void Speed_Init_AllPWM(void);
void Speed_Stop_AllPWM(void);
void Motor_Stop_Percent(void);
void Car_Go_Percent(uint8_t percent);
void Car_TurnLeft_Percent(uint8_t turnPerc);
void Car_TurnRight_Percent(uint8_t turnPerc);
void Car_TurnRight_Square_Percent(uint8_t turnPerc);
void Car_TurnLeft_Square_Percent(uint8_t turnPerc);
void Car_Backward(uint8_t percent);
void Car_Stop(void);

void MOTOR_Control(uint8_t L_percent_FI, uint8_t L_percent_BI, uint8_t R_percent_FI, uint8_t R_percent_BI);
void MOTOR_Front_Control(uint8_t L_percent_FI, uint8_t L_percent_BI, uint8_t R_percent_FI, uint8_t R_percent_BI);
void MOTOR_Back_Control(uint8_t L_percent_FI, uint8_t L_percent_BI, uint8_t R_percent_FI, uint8_t R_percent_BI);
#endif
