#include "bt_fifo.h"

static uint8_t  bt_fifo_buf[BT_FIFO_BUF_SIZE];
static uint16_t bt_fifo_wr = 0U;   //写指针
static uint16_t bt_fifo_rd = 0U;   //读指针

void BT_FIFO_Put(uint8_t byte)
{
    uint16_t next_wr;
    next_wr = (bt_fifo_wr + 1U) % BT_FIFO_BUF_SIZE;

    //缓冲区没满才写入；满则直接丢弃新数据，防止覆盖未读数据
    if(next_wr != bt_fifo_rd)
    {
        bt_fifo_buf[bt_fifo_wr] = byte;
        bt_fifo_wr = next_wr;
    }
}

uint8_t BT_FIFO_Get(void)
{
    uint8_t ret = 0xFFU;
    if(bt_fifo_rd != bt_fifo_wr)
    {
        ret = bt_fifo_buf[bt_fifo_rd];
        bt_fifo_rd = (bt_fifo_rd + 1U) % BT_FIFO_BUF_SIZE;
    }
    return ret;
}

void BT_FIFO_Clear(void)
{
    bt_fifo_wr = 0U;
    bt_fifo_rd = 0U;
}
