#ifndef __DELAY_US_H
#define __DELAY_US_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

// 微秒级延时（HAL标准，依赖SysTick）
void HAL_Delay_us(uint32_t nus);

#ifdef __cplusplus
}
#endif

#endif
