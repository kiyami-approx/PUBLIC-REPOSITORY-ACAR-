#include "blueteeth.h"
#include "main.h"
#include <string.h>

char Serial_RxPacket[BT_RX_BUF_LEN];
extern volatile Bt_CmdType_t bt_cmd;
static uint16_t bt_rx_idx = 0U;
static uint8_t bt_rx_active = 0U;

void BT_UART_InputByte(uint8_t ch)
{
    if(ch == '[')
    {
        bt_rx_idx = 0U;
        bt_rx_active = 1U;
    }
    else if(ch == ']')
    {
        if(bt_rx_active != 0U)
        {
            Serial_RxPacket[bt_rx_idx] = '\0';

            if(strcmp(Serial_RxPacket,"OVAL") == 0)
            {
                bt_cmd = BT_CMD_TRACK_OVAL;
            }
            else if(strcmp(Serial_RxPacket,"SQUARE") == 0)
            {
                bt_cmd = BT_CMD_TRACK_SQUARE;
            }
            else if(strcmp(Serial_RxPacket,"ULTRASONIC") == 0)
            {
                bt_cmd = BT_CMD_ULTRASONIC;
            }
            else if(strcmp(Serial_RxPacket,"STOP") == 0)
            {
                bt_cmd = BT_CMD_STOP;
            }
            else if(strcmp(Serial_RxPacket,"FORWARD") == 0)
            {
                bt_cmd = BT_CMD_FORWARD;
            }
            else if(strcmp(Serial_RxPacket,"BACK") == 0)
            {
                bt_cmd = BT_CMD_BACK;
            }
            else if(strcmp(Serial_RxPacket,"LEFT") == 0)
            {
                bt_cmd = BT_CMD_LEFT;
            }
            else if(strcmp(Serial_RxPacket,"RIGHT") == 0)
            {
                bt_cmd = BT_CMD_RIGHT;
            }
        }
        // 收到结束符，正常复位接收状态，这部分保留不动！
        bt_rx_idx = 0U;
        bt_rx_active = 0U;
    }
    else
    {
        if(bt_rx_active != 0U)
        {
            if(bt_rx_idx < (BT_RX_BUF_LEN - 1U))
            {
                Serial_RxPacket[bt_rx_idx++] = ch;
            }
            else
            {
                //缓冲区溢出，直接重置，防止越界
                bt_rx_idx = 0U;
                bt_rx_active = 0U;
            }
        }
    }
}
