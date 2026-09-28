#include "speed.h"

/**
 * @brief 启动全部8路PWM通道，放在main MX_TIM初始化之后调用
 */
void Speed_Init_AllPWM(void)
{
    // TIM2 4通道启动
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);

    // TIM4 4通道启动
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
}

/**
 * @brief 关闭全部8路PWM通道，放在main MX_TIM初始化之后调用
 */
void Speed_Stop_AllPWM(void)
{
    // TIM2 4通道关闭
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);

    // TIM4 4通道关闭
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_3);
    HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
}

/**
 * @brief  设置电机原始CCR占空比(0~999)
 * @param  ch : 电机通道 Motor_Channel_t
 * @param  ccr_val : 比较值，范围 0 ~ 999，超出会自动截断
 * @retval none
 */
static void Speed_SetRawCCR(Motor_Channel_t ch, uint16_t ccr_val)
{
    // 限幅，防止越界
    if(ccr_val > PWM_MAX_CCR)
    {
        ccr_val = PWM_MAX_CCR;
    }

    switch(ch)
    {
        case MOTOR_TIM2_CH1:
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, ccr_val);
            break;
        case MOTOR_TIM2_CH2:
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, ccr_val);
            break;
        case MOTOR_TIM2_CH3:
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, ccr_val);
            break;
        case MOTOR_TIM2_CH4:
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, ccr_val);
            break;

        case MOTOR_TIM4_CH1:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, ccr_val);
            break;
        case MOTOR_TIM4_CH2:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, ccr_val);
            break;
        case MOTOR_TIM4_CH3:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, ccr_val);
            break;
        case MOTOR_TIM4_CH4:
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, ccr_val);
            break;

        default:
            break;
    }
}

/**
 * @brief  按百分比设置占空比 0~100%
 * @param  ch : 电机通道
 * @param  percent : 0~100，超出自动截断
 * @retval none
 */
static void Speed_SetDutyPercent(Motor_Channel_t ch, uint8_t percent)
{
    uint16_t ccr;
    if(percent > PWM_PERCENT_MAX)
    {
        percent = PWM_PERCENT_MAX;
    }
    // 百分比转CCR： percent /100 * 999
    ccr = (uint16_t)(((uint32_t)percent * PWM_MAX_CCR) / 100U);
    Speed_SetRawCCR(ch, ccr);
}


/**
  * @brief 对于电机的控制
  *
  */
void MOTOR_Control(uint8_t L_percent_FI, uint8_t L_percent_BI, uint8_t R_percent_FI, uint8_t R_percent_BI)
{
	Speed_SetDutyPercent(MOTOR_TIM4_CH4,L_percent_FI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH2,L_percent_FI);

	Speed_SetDutyPercent(MOTOR_TIM4_CH3,L_percent_BI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH1,L_percent_BI);

	Speed_SetDutyPercent(MOTOR_TIM4_CH1,R_percent_FI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH3,R_percent_FI);

	Speed_SetDutyPercent(MOTOR_TIM4_CH2,R_percent_BI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH4,R_percent_BI);
}

void MOTOR_Front_Control(uint8_t L_percent_FI, uint8_t L_percent_BI, uint8_t R_percent_FI, uint8_t R_percent_BI)
{
	Speed_SetDutyPercent(MOTOR_TIM4_CH4,L_percent_FI);
	Speed_SetDutyPercent(MOTOR_TIM4_CH3,L_percent_BI);
	Speed_SetDutyPercent(MOTOR_TIM4_CH1,R_percent_FI);
	Speed_SetDutyPercent(MOTOR_TIM4_CH2,R_percent_BI);
}

void MOTOR_Back_Control(uint8_t L_percent_FI, uint8_t L_percent_BI, uint8_t R_percent_FI, uint8_t R_percent_BI)
{
	Speed_SetDutyPercent(MOTOR_TIM2_CH2,L_percent_FI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH1,L_percent_BI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH3,R_percent_FI);
	Speed_SetDutyPercent(MOTOR_TIM2_CH4,R_percent_BI);
}


/**
 * @brief 电机停止
 */
void Motor_Stop_Percent(void)
{
    MOTOR_Control(0U,0U,0U,0U);
}

/**
 * @brief  直行：左右都前进，占空比percent
 */
void Car_Go_Percent(uint8_t percent)
{
	//========新增强制停止判断========
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}
    MOTOR_Control(percent, 0U, percent, 0U);
}


/**
 * @brief 左转，turnPerc：转弯强度百分比
 * Motor_SetSpeed(TRACK_TURN_FORWARD‑turn, TRACK_TURN_FORWARD+turn)
 */
void Car_TurnLeft_Percent(uint8_t turnPerc)
{
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}

    int16_t leftSpd = (int16_t)TRACK_TURN_FW_PERCENT - (int16_t)turnPerc;
    int16_t rightSpd = (int16_t)TRACK_TURN_FW_PERCENT + (int16_t)turnPerc;

    uint8_t l_fi=0, l_bi=0;
    uint8_t r_fi=0, r_bi=0;

    if(leftSpd > 0)
    {
        l_fi = (uint8_t)leftSpd;
    }
    else if(leftSpd < 0)
    {
        l_bi = (uint8_t)(-leftSpd);
    }

    if(rightSpd > 0)
    {
        r_fi = (uint8_t)rightSpd;
    }
    else if(rightSpd < 0)
    {
        r_bi = (uint8_t)(-rightSpd);
    }
    MOTOR_Control(l_fi, l_bi, r_fi, r_bi);
}

/**
 * @brief 右转，turnPerc：转弯强度百分比
 * Motor_SetSpeed(TRACK_TURN_FORWARD+turn, TRACK_TURN_FORWARD‑turn)
 */
void Car_TurnRight_Percent(uint8_t turnPerc)
{
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}

    int16_t leftSpd = (int16_t)TRACK_TURN_FW_PERCENT + (int16_t)turnPerc;
    int16_t rightSpd = (int16_t)TRACK_TURN_FW_PERCENT - (int16_t)turnPerc;

    uint8_t l_fi=0, l_bi=0;
    uint8_t r_fi=0, r_bi=0;

    if(leftSpd > 0)
    {
        l_fi = (uint8_t)leftSpd;
    }
    else if(leftSpd < 0)
    {
        l_bi = (uint8_t)(-leftSpd);
    }

    if(rightSpd > 0)
    {
        r_fi = (uint8_t)rightSpd;
    }
    else if(rightSpd < 0)
    {
        r_bi = (uint8_t)(-rightSpd);
    }
    MOTOR_Control(l_fi, l_bi, r_fi, r_bi);
}

void Car_TurnRight_Square_Percent(uint8_t turnPerc)
{
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}

    int16_t leftSpd = (int16_t)turnPerc;
    int16_t rightSpd = -(int16_t)turnPerc;

    uint8_t l_fi=0, l_bi=0;
    uint8_t r_fi=0, r_bi=0;

    if(leftSpd > 0)
    {
        l_fi = (uint8_t)leftSpd;
    }
    else if(leftSpd < 0)
    {
        l_bi = (uint8_t)(-leftSpd);
    }

    if(rightSpd > 0)
    {
        r_fi = (uint8_t)rightSpd;
    }
    else if(rightSpd < 0)
    {
        r_bi = (uint8_t)(-rightSpd);
    }
    MOTOR_Control(l_fi, l_bi, r_fi, r_bi);
}

void Car_TurnLeft_Square_Percent(uint8_t turnPerc)
{

	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}

    int16_t leftSpd = -(int16_t)turnPerc;
    int16_t rightSpd = (int16_t)turnPerc;

    uint8_t l_fi=0, l_bi=0;
    uint8_t r_fi=0, r_bi=0;

    if(leftSpd > 0)
    {
        l_fi = (uint8_t)leftSpd;
    }
    else if(leftSpd < 0)
    {
        l_bi = (uint8_t)(-leftSpd);
    }

    if(rightSpd > 0)
    {
        r_fi = (uint8_t)rightSpd;
    }
    else if(rightSpd < 0)
    {
        r_bi = (uint8_t)(-rightSpd);
    }
    MOTOR_Control(l_fi, l_bi, r_fi, r_bi);
}

/**
 * @brief 整车后退
 * @param percent 占空比百分比 0~100
 */
void Car_Backward(uint8_t percent)
{
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}

    if(percent > 100) percent = 100;
    MOTOR_Control(0U,percent, 0U,percent);
}

void Car_Stop(void)
{
	MOTOR_Control(100,100, 100,100);
}
