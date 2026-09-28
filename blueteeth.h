#ifndef __BLUETEETH_H
#define __BLUETEETH_H

#include "main.h"

/*蓝牙命令枚举*/
typedef enum
{
    BT_CMD_NONE = 0,
    BT_CMD_STOP,
	BT_CMD_ULTRASONIC,
    BT_CMD_TRACK_SQUARE,
	BT_CMD_TRACK_OVAL,
    BT_CMD_FORWARD,
    BT_CMD_BACK,
    BT_CMD_LEFT,
    BT_CMD_RIGHT
}Bt_CmdType_t;

#define BT_RX_BUF_LEN   100U

extern char Serial_RxPacket[BT_RX_BUF_LEN];
extern uint8_t Serial_RxFlag;

void BT_UART_InputByte(uint8_t ch);

#endif
