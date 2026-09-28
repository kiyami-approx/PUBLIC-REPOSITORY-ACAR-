#ifndef __BT_FIFO_H
#define __BT_FIFO_H

#include "main.h"
#include <stdint.h>

// FIFO缓冲区大小，64字节足够遥控使用
#define BT_FIFO_BUF_SIZE    64U

/**
 * @brief  往FIFO存入1字节，中断回调里面调用，执行极快
 * @param  byte：待存入字节
 * @retval 无
 */
void BT_FIFO_Put(uint8_t byte);

/**
 * @brief 从FIFO取出1字节，主while(1)循环调用
 * @retval 返回字节；0xFF代表FIFO为空，没有数据
 */
uint8_t BT_FIFO_Get(void);

/**
 * @brief 清空FIFO缓冲区
 * @retval 无
 */
void BT_FIFO_Clear(void);

#endif
