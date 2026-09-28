/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "speed.h"
#include "delay_us.h"
#include "trace.h"
#include "blueteeth.h"
#include "bt_fifo.h"
#include "hc_sr04.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile Bt_CmdType_t bt_cmd = BT_CMD_NONE;
uint8_t start_flag = 0;
volatile uint8_t g_force_stop = 0;   //强制停止标志，1=强制停止循迹
uint8_t rx_byte;

/*====新增按键相关变量====*/
uint8_t key_last_state = 1;   //按键上一次状态，默认松开1
uint32_t key_press_tick = 0;  //按键按下时刻计时
#define KEY_LONG_PRESS_MS 3000  //长按3秒阈值
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  //开启USART1接收中断，等待JDY——31蓝牙字节
   HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
   HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint8_t ch;

	  //=============PA4按键处理开始=============
	      uint8_t key_now = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);

	      if(key_now == 0 && key_last_state == 1)
	      {
	          //【按键按下边沿】记录按下时间
	          key_press_tick = HAL_GetTick();
	      }
	      else if(key_now == 1 && key_last_state == 0)
	      {
	          //【按键松开边沿】
	          uint32_t press_duration = HAL_GetTick() - key_press_tick;
	          if(press_duration < KEY_LONG_PRESS_MS)
	          {
	              //短按：开
	              start_flag = 1;
	          }
	          //长按松开不做操作，长按动作在按下持续时检测
	      }

	      //检测持续长按：按键一直按住超过3秒，直接强制关闭
	      if(key_now == 0)
	      {
	          uint32_t press_duration = HAL_GetTick() - key_press_tick;
	          if(press_duration >= KEY_LONG_PRESS_MS)
	          {
	              start_flag = 0;  //长按3s强制关闭
	          }
	      }
	      key_last_state = key_now; //保存本次状态用于下一轮

	      //=============PA4按键处理结束=============


	  if (start_flag)
	  {
		  OPEN_LED_L();
		  OPEN_LED_R();
		  Speed_Init_AllPWM();
		  //Track_Follow();					//循迹
		  //Track_Follow_Square();
		  //MOTOR_Control(50,0,50,0);		//前进
	      ch = BT_FIFO_Get();
	      if(ch != 0xFFU)
	      {
	          BT_UART_InputByte(ch);
	      }

		  switch(bt_cmd)
		  {
	  			case BT_CMD_ULTRASONIC:
	  				g_force_stop = 0;
	  				HC_SR04_AvoidObstacle();
	  				HAL_Delay_us(50000);
	  				break;
		  		case BT_CMD_TRACK_SQUARE:
		  			g_force_stop = 0;
		  			Track_Follow_Square();
		  			break;
		  		case BT_CMD_TRACK_OVAL:
		  			g_force_stop = 0;
		  			Track_Follow_Oval();
		  			break;
		  		case BT_CMD_FORWARD:
		  			g_force_stop = 0;
		  			Car_Go_Percent(MANUAL_SPEED_PERCENT);
		  			break;
		  		case BT_CMD_BACK:
		  			g_force_stop = 0;
		  			Car_Backward(MANUAL_SPEED_PERCENT);
		  			break;
		  		case BT_CMD_LEFT:
		  			g_force_stop = 0;
		  			Car_TurnLeft_Percent(TRACK_TURN_SHARP_PERC);
		  			break;
		  		case BT_CMD_RIGHT:
		  			g_force_stop = 0;
		  			Car_TurnRight_Percent(TRACK_TURN_SHARP_PERC);
		  			break;
		  		case BT_CMD_STOP:
		  			g_force_stop = 1;
		  			Motor_Stop_Percent();
		  			break;
		  		case BT_CMD_NONE:
		  		default:
		  			Motor_Stop_Percent();
		  			break;
		  		  }
	  }
	  else
	  {
		  g_force_stop = 1;
		  CLOSE_LED_L();
		  CLOSE_LED_R();
		  Speed_Stop_AllPWM();
		  bt_cmd = BT_CMD_NONE;
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{

		BT_FIFO_Put(rx_byte);
		//重新挂载中断，持续接收下一个字节
		HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM3)
  {
    if(ultra_state == ULTRA_IDLE)
    {
        t1_val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_4);
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_FALLING);
        ultra_state = ULTRA_WAIT_FALL;
    }
    else if(ultra_state == ULTRA_WAIT_FALL)
    {
        t2_val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_4);
        if(t2_val >= t1_val)
        {
            pulse_us = t2_val - t1_val;
        }
        else
        {
            pulse_us = (0xFFFF - t1_val) + t2_val;
        }
        ultra_distance_cm = pulse_us * 0.017f;


        __HAL_TIM_SET_CAPTUREPOLARITY(&htim3, TIM_CHANNEL_4, TIM_INPUTCHANNELPOLARITY_RISING);
        ultra_state = ULTRA_IDLE;
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
