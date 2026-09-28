#include "trace.h"




static uint8_t State_Read(void)
{
	uint8_t state = 0;
	if(X1) state |= (1 << 3);
	if(X2) state |= (1 << 2);
	if(X3) state |= (1 << 1);
	if(X4) state |= (1 << 0);
	return state;
}


/* 自动循迹：状态式，完全复刻main.c逻辑，使用MOTOR_Control百分比接口 */
void Track_Follow_Oval(void)
{
	//========新增强制停止判断========
	if(g_force_stop == 1U)
	{
	    Motor_Stop_Percent();
	    return;   //直接退出循迹状态机
	}

  static uint8_t lastState = 0x06;   /* 记录上一次有效状态，丢线时保持 */
  uint8_t state = State_Read();

  if (state == 0x00)   /* 全无：丢线 */
  {
    if (lastState != 0x06)   /* 上一刻在转弯 → 保持转向 */
    {
      state = lastState;
    }
    else   /* 上一刻是直行 → 直行 */
    {
      state = 0x06;
    }
  }
  else if (state != 0x0F)   /* 记录本次有效状态（十字除外） */
  {
    lastState = state;
  }

  switch (state)
  {
    case 0x06:  /* 中左+中右：居中，直行 */
      Car_Go_Percent(TRACK_GO_PERCENT);
      break;
    case 0x04:  /* 中左：微左转 */
      Car_TurnLeft_Percent(TRACK_TURN_SLIGHT_PERC);
      break;
    case 0x02:  /* 中右：微右转 */
      Car_TurnRight_Percent(TRACK_TURN_SLIGHT_PERC);
      break;
    case 0x08:  /* 最左：急左转 */
    case 0x0C:  /* 最左+中左：急左转 */
      Car_TurnLeft_Percent(TRACK_TURN_SHARP_PERC);
      break;
    case 0x01:  /* 最右：急右转 */
    case 0x03:  /* 中右+最右：急右转 */
      Car_TurnRight_Percent(TRACK_TURN_SHARP_PERC);
      break;
    case 0x07:  /* 中左+中右+最右（缺最左）：直角右转 */
      Car_TurnRight_Percent(TRACK_TURN_SHARP_PERC);
      break;
    case 0x0E:  /* 最左+中左+中右（缺最右）：直角左转 */
      Car_TurnLeft_Percent(TRACK_TURN_SHARP_PERC);
      break;
    case 0x0F:  /* 全有：十字，停车 */
    default:
      Motor_Stop_Percent();
      break;
  }
}
/* 自动循迹：状态式，方形赛道
 * 新增：识别直角后先停机惯性滑行一段时间，再执行后退，再转弯
 * 适配while(1)循环，HAL_GetTick非阻塞计时
 */
void Track_Follow_Square(void)
{
	//========新增强制停止判断========
	if(g_force_stop == 1U)
	{
	   Motor_Stop_Percent();
	   return;   //直接退出循迹状态机
	}
  static uint8_t lastState = 0x06;   /* 记录上一次有效状态，丢线时保持 */

  /* 【全部调参在这里】 */
  #define COAST_BEFORE_BACK_MS    100U     //检测直角后停机惯性滑行时间(ms)，调这个控制往前滑多久
  #define CORNER_BACK_PERCENT     25U     //直角前，后退速度百分比 18~30
  #define CORNER_BACK_MS          300U    //后退持续时间(ms)

  /* 子状态枚举，增加滑行阶段 */
    typedef enum{
        TRACK_NORMAL,                // 普通循迹
        TRACK_COAST_BEFORE_BACK,    //【新增】检测直角，停机惯性滑行
        TRACK_BACK_BEFORE_CORNER,    // 直角预处理：后退消惯性
        TRACK_TURN_CORNER_L,         // 持续直角左转
        TRACK_TURN_CORNER_R          // 持续直角右转
    }TrackSubState_t;

    static TrackSubState_t subState = TRACK_NORMAL;
    static uint32_t phaseStartTick = 0;  //当前阶段开始时刻时间戳（滑行/后退共用）
    static TrackSubState_t pendingTurnDir = TRACK_NORMAL; //保存后退结束后要执行的转弯方向

  uint8_t state = State_Read();
  if (state == 0x00)   /* 全无：丢线 */
  {
    if (lastState != 0x06)   /* 上一刻在转弯 → 保持转向 */
    {
      state = lastState;
    }
    else   /* 上一刻是直行 → 直行 */
    {
      state = 0x06;
    }
  }
  else if (state != 0x0F)   /* 记录本次有效状态（十字除外） */
  {
    lastState = state;
  }

  /* =========子状态机优先执行========= */
    //阶段1：【新增】停机惯性滑行，电机停止，车子靠惯性往前溜
    if(subState == TRACK_COAST_BEFORE_BACK)
    {
        Motor_Stop_Percent(); //停机，靠惯性滑行
        if( (HAL_GetTick() - phaseStartTick) >= COAST_BEFORE_BACK_MS )
        {
            //滑行时间到，切换进入后退阶段
            subState = TRACK_BACK_BEFORE_CORNER;
            phaseStartTick = HAL_GetTick(); //记录后退起始时间戳
        }
        return;
    }

    //阶段2：直角预处理，执行后退
    if(subState == TRACK_BACK_BEFORE_CORNER)
    {
        Car_Backward(CORNER_BACK_PERCENT);
        if( (HAL_GetTick() - phaseStartTick) >= CORNER_BACK_MS )
        {
            Motor_Stop_Percent();
            //后退完成，切到对应的直角转弯状态
            subState = pendingTurnDir;
        }
        return;
    }

    //阶段3：持续直角左转
    if(subState == TRACK_TURN_CORNER_L)
    {
        Car_TurnLeft_Square_Percent(TRACK_TURN_SQUARE_PERC);
        // 判断：中间两个传感器同时看到黑线，代表已经转完直角，退出转弯
        if( (state & 0x06U) == 0x06U )
        {
            subState = TRACK_NORMAL;
        }
        return;
    }

    //阶段4：持续直角右转
    if(subState == TRACK_TURN_CORNER_R)
    {
        Car_TurnRight_Square_Percent(TRACK_TURN_SQUARE_PERC);
        if( (state & 0x06U) == 0x06U )
        {
            subState = TRACK_NORMAL;
        }
        return;
    }

  // =================普通循迹状态=================
  switch (state)
  {
    case 0x06:  /* 中左+中右：居中，直行 */
      Car_Go_Percent(TRACK_GO_SQUARE_PERCENT);
      break;
    case 0x04:  /* 中左：微左转 */
      Car_TurnLeft_Percent(TRACK_TURN_SLIGHT_PERC);
      break;
    case 0x02:  /* 中右：微右转 */
      Car_TurnRight_Percent(TRACK_TURN_SLIGHT_PERC);
      break;

    case 0x08:  /* 最左：急左转 */
    case 0x0C:  /* 最左+中左：急左转 */
    case 0x0E:  /* 最左+中左+中右（缺最右）：直角左转 */
        //触发直角，不再直接后退，先进【惯性滑行】阶段
        pendingTurnDir = TRACK_TURN_CORNER_L;
        subState = TRACK_COAST_BEFORE_BACK;
        phaseStartTick = HAL_GetTick(); //记录滑行开始时刻
        break;

    case 0x01:  /* 最右：急右转 */
    case 0x03:  /* 中右+最右：急右转 */
    case 0x07:  /* 中左+中右+最右（缺最左）：直角右转 */
        //触发直角，先进惯性滑行阶段
        pendingTurnDir = TRACK_TURN_CORNER_R;
        subState = TRACK_COAST_BEFORE_BACK;
        phaseStartTick = HAL_GetTick();
        break;

    case 0x0F:  /* 全有：十字，停车 */
    default:
      Motor_Stop_Percent();
      break;
  }
}

