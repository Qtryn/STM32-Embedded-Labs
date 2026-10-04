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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* =========================================================
   7-SEGMENT DISPLAY
   Common Anode

   SEG = 0 -> ON
   SEG = 1 -> OFF
   ========================================================= */

void display7SEG(uint8_t num)
{
    /*
     * Segment order:
     *
     * SEG0 = A
     * SEG1 = B
     * SEG2 = C
     * SEG3 = D
     * SEG4 = E
     * SEG5 = F
     * SEG6 = G
     */

    uint8_t seg[10][7] =
    {
        // A  B  C  D  E  F  G

        {0, 0, 0, 0, 0, 0, 1}, // 0
        {1, 0, 0, 1, 1, 1, 1}, // 1
        {0, 0, 1, 0, 0, 1, 0}, // 2
        {0, 0, 0, 0, 1, 1, 0}, // 3
        {1, 0, 0, 1, 1, 0, 0}, // 4
        {0, 1, 0, 0, 1, 0, 0}, // 5
        {0, 1, 0, 0, 0, 0, 0}, // 6
        {0, 0, 0, 1, 1, 1, 1}, // 7
        {0, 0, 0, 0, 0, 0, 0}, // 8
        {0, 0, 0, 0, 1, 0, 0}  // 9
    };

    HAL_GPIO_WritePin(
        SEG0_GPIO_Port,
        SEG0_Pin,
        seg[num][0]
    );

    HAL_GPIO_WritePin(
        SEG1_GPIO_Port,
        SEG1_Pin,
        seg[num][1]
    );

    HAL_GPIO_WritePin(
        SEG2_GPIO_Port,
        SEG2_Pin,
        seg[num][2]
    );

    HAL_GPIO_WritePin(
        SEG3_GPIO_Port,
        SEG3_Pin,
        seg[num][3]
    );

    HAL_GPIO_WritePin(
        SEG4_GPIO_Port,
        SEG4_Pin,
        seg[num][4]
    );

    HAL_GPIO_WritePin(
        SEG5_GPIO_Port,
        SEG5_Pin,
        seg[num][5]
    );

    HAL_GPIO_WritePin(
        SEG6_GPIO_Port,
        SEG6_Pin,
        seg[num][6]
    );
}


/* =========================================================
   7-SEGMENT ENABLE

   PNP transistor:
   LOW  = ON
   HIGH = OFF
   ========================================================= */

void EN0_ON(void)
{
    HAL_GPIO_WritePin(
        EN0_GPIO_Port,
        EN0_Pin,
        GPIO_PIN_RESET
    );
}

void EN0_OFF(void)
{
    HAL_GPIO_WritePin(
        EN0_GPIO_Port,
        EN0_Pin,
        GPIO_PIN_SET
    );
}


void EN1_ON(void)
{
    HAL_GPIO_WritePin(
        EN1_GPIO_Port,
        EN1_Pin,
        GPIO_PIN_RESET
    );
}

void EN1_OFF(void)
{
    HAL_GPIO_WritePin(
        EN1_GPIO_Port,
        EN1_Pin,
        GPIO_PIN_SET
    );
}


void EN2_ON(void)
{
    HAL_GPIO_WritePin(
        EN2_GPIO_Port,
        EN2_Pin,
        GPIO_PIN_RESET
    );
}

void EN2_OFF(void)
{
    HAL_GPIO_WritePin(
        EN2_GPIO_Port,
        EN2_Pin,
        GPIO_PIN_SET
    );
}


void EN3_ON(void)
{
    HAL_GPIO_WritePin(
        EN3_GPIO_Port,
        EN3_Pin,
        GPIO_PIN_RESET
    );
}

void EN3_OFF(void)
{
    HAL_GPIO_WritePin(
        EN3_GPIO_Port,
        EN3_Pin,
        GPIO_PIN_SET
    );
}


/* =========================================================
   TURN OFF ALL 4 DISPLAYS
   ========================================================= */

void All_Display_Off(void)
{
    EN0_OFF();
    EN1_OFF();
    EN2_OFF();
    EN3_OFF();
}

const int MAX_LED = 4;

int index_led = 0;

int led_buffer[4] = {0, 0, 0, 0};
int hour = 15;
int minute = 8;
int second = 50;

uint16_t scan_counter = 0;
uint16_t dot_counter = 0;

void update7SEG(int index)
{
    /*
     * Turn OFF all displays first
     */

    EN0_OFF();
    EN1_OFF();
    EN2_OFF();
    EN3_OFF();


    /*
     * Select which 7-segment to display
     */

    switch (index)
    {
        case 0:

            display7SEG(led_buffer[0]);
            EN0_ON();

            break;


        case 1:

            display7SEG(led_buffer[1]);
            EN1_ON();

            break;


        case 2:

            display7SEG(led_buffer[2]);
            EN2_ON();

            break;


        case 3:

            display7SEG(led_buffer[3]);
            EN3_ON();

            break;


        default:

            EN0_OFF();
            EN1_OFF();
            EN2_OFF();
            EN3_OFF();

            break;
    }
}

void updateClockBuffer(void)
{
    /*
     * Display:
     *
     * HH:MM
     *
     * Example:
     * hour   = 15
     * minute = 08
     *
     * led_buffer = {1, 5, 0, 8}
     */

    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;

    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}


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
  /* USER CODE BEGIN 2 */

  EN0_OFF();
  EN1_OFF();
  EN2_OFF();
  EN3_OFF();

  HAL_GPIO_WritePin(
    DOT_GPIO_Port,
    DOT_Pin,
    GPIO_PIN_SET
  );

  updateClockBuffer();

  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {
        second++;

        if (second >= 60)
        {
            second = 0;
            minute++;
        }

        if (minute >= 60)
        {
            minute = 0;
            hour++;
        }

        if (hour >= 24)
        {
            hour = 0;
        }

        updateClockBuffer();

        HAL_Delay(1000);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DOT_Pin LED_RED_Pin EN0_Pin EN1_Pin
                           EN2_Pin EN3_Pin */
  GPIO_InitStruct.Pin = DOT_Pin|LED_RED_Pin|EN0_Pin|EN1_Pin
                          |EN2_Pin|EN3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG0_Pin SEG1_Pin SEG2_Pin SEG3_Pin
                           SEG4_Pin SEG5_Pin SEG6_Pin */
  GPIO_InitStruct.Pin = SEG0_Pin|SEG1_Pin|SEG2_Pin|SEG3_Pin
                          |SEG4_Pin|SEG5_Pin|SEG6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        /*
         * TIM2 interrupt = 10 ms
         */

        scan_counter++;
        dot_counter++;


        /*
         * ==========================================
         * 4 LED scanning frequency = 1 Hz
         *
         * 1 second / 4 LEDs = 250 ms / LED
         *
         * 250 ms / 10 ms = 25 interrupts
         * ==========================================
         */

        if (scan_counter >= 25)
        {
            scan_counter = 0;

            update7SEG(index_led);

            index_led++;

            if (index_led >= MAX_LED)
            {
                index_led = 0;
            }
        }


        /*
         * ==========================================
         * DOT blinking every 1 second
         *
         * 1 second / 10 ms = 100 interrupts
         * ==========================================
         */

        if (dot_counter >= 100)
        {
            dot_counter = 0;

            HAL_GPIO_TogglePin(
                DOT_GPIO_Port,
                DOT_Pin
            );
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
