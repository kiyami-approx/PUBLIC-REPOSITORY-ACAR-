#include "hc_sr04.h"


//超声波输入捕获
uint16_t  t1_val = 0;    //上升沿时刻
uint16_t  t2_val = 0;    //下降沿时刻
uint32_t  pulse_us = 0;  //高电平脉冲时间(us)
uint16_t  ultra_distance_cm = 0; //距离cm
uint16_t tim_overflow_cnt;
Ultra_State_t ultra_state = ULTRA_IDLE;

uint16_t ultra_dist_cm = 0;
Car_State_t car_state = CAR_STATE_NORMAL;


#define ULTRA_TIMEOUT_MS 40U
#define ULTRA_OBSTACLE_CM 30U
#define ULTRA_OBSTACLE_EMERGENCY_CM 10U
#define ULTRA_OBSTACLE_TURN_CM 20U

/**
 * @brief 发送Trig 10us脉冲，触发测距
 * @retval 0:发送成功；1：Echo还未完成，忙，放弃本次测量
 */
uint8_t HC_SR04_Start(void)
{
	static uint32_t trig_tick = 0;
	if(ultra_state != ULTRA_IDLE)
	{
	    //超时保护：Echo一直高，强制复位
	    if( HAL_GetTick() - trig_tick > ULTRA_TIMEOUT_MS )
	    {
	        ultra_state = ULTRA_IDLE;
	        tim_overflow_cnt = 0U;
	        return 2;
	    }
	    return 1; //上一次测量还没做完，本次不发送Trig
	}

	trig_tick = HAL_GetTick();
	//发送Trig 10us脉冲，发起测距，函数立刻返回，不等待结果！
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_Delay_us(30);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

	//把中断已经算好的最新距离拷贝输出
	ultra_dist_cm = ultra_distance_cm;
	return 0;
}

/**
 * @brief 避障业务逻辑：1米阈值；小于1m原地旋转；大于1m直行
 * @note 电机驱动函数全部注释占位，你替换成自己的电机函数
 */
void HC_SR04_AvoidObstacle(void)
{
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}
    //每一轮循环都尝试发起超声波测量；忙则直接跳过
    HC_SR04_Start();

    switch(car_state)
    {
        case CAR_STATE_NORMAL:
            //正常模式：直行
        	Car_Go_Percent(AVOID_GO_PERCENT);

            //检测到障碍物，切到旋转避障状态
            if(ultra_dist_cm < ULTRA_OBSTACLE_CM)
            {
                car_state = CAR_STATE_AVOID_TURN;
            }
            break;

        case CAR_STATE_AVOID_TURN:

            Car_TurnRight_Percent(TRACK_TURN_SHARP_PERC);

            //持续读超声波：当障碍物消失（距离大于阈值）切回正常直行
            if(ultra_dist_cm > ULTRA_OBSTACLE_CM)
            {
                car_state = CAR_STATE_NORMAL;
            }
            break;
    }
}
