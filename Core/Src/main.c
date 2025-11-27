/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DS1307_I2C_GPIO_PORT			GPIOB
#define DS1307_I2C_SCL_PIN				GPIO_PIN_8
#define DS1307_I2C_SDA_PIN				GPIO_PIN_9

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

//static char *get_day_of_weak(uint8_t);
static void number_to_string(uint8_t, char *);
static char *time_to_string(RTC_time_t *);
static char *date_to_string(RTC_date_t *);
static void saveDataIntoEEPROM(void);
//static void get_data_ds1307(void);
static void get_data(void);
static void update_data(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

RTC_time_t curr_time;
RTC_date_t curr_date;
__vo uint8_t flag_read_rtc = RESET;
char rx_buf[32];
char meas_buf[16];
static uint16_t cuur_data = 0;
static uint32_t counter = 0;
BME280_data_t data;

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
  printf("he\n");

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_TIM2_Init();
  MX_GPIO_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  lcd_display_clear();

  lcd_print_string("HAL testing");

  /* BME280 test */
  bme280_init();
  memset(meas_buf, ' ', strlen(meas_buf));
  update_data();

  lcd_display_clear();
  lcd_set_cursor(1, 1);
  lcd_print_string(meas_buf);

  /* DS1307 initialization */

  ds1307_init();

  /* Timer 2 initialization */
  HAL_TIM_Base_Start_IT(&htim2);

  /* DS1307 set data */
	curr_date.day = THURSDAY;
	curr_date.date = 25;
	curr_date.month = 11;
	curr_date.year = 25;

	curr_time.time_format = DS1307_TIME_FORMAT_24HOUR;
	curr_time.hours = 15;
	curr_time.minutes = 11;
	curr_time.seconds = 50;

	ds1307_set_current_date(&curr_date);

	ds1307_set_current_time(&curr_time);

	lcd_display_clear();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    //MX_USB_HOST_Process();

	  if(flag_read_rtc == SET)
	  {
		counter++;

		flag_read_rtc = RESET;

		saveDataIntoEEPROM();

		lcd_set_cursor(1, 1);

		get_data();

		char *am_pm;
		if(curr_time.time_format != DS1307_TIME_FORMAT_24HOUR)
		{
			am_pm = (curr_time.time_format) ? "PM" : "AM";

			lcd_print_string(rx_buf);
			lcd_print_string(am_pm);
		}
		else
		{
			lcd_print_string(rx_buf);
		}

		// after time get temperature, pressure and humidity
		lcd_set_cursor(2, 1);
		//lcd_print_string(meas_buf);
		lcd_print_string(&rx_buf[16]);

		if(counter >= 9)
		{
			counter = 0;
			update_data();
		}
	  }

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  /* USER CODE BEGIN I2C1_Init 2 */
	__HAL_RCC_I2C1_CLK_ENABLE();

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = DS1307_I2C_SPEED;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	//hi2c1.MemaddSize

	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}

  /* USER CODE END I2C1_Init 2 */
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
  htim2.Init.Prescaler = 8399;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
  	  /*Configure GPIO pin : PD14 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  	GPIO_InitStruct.Pin = DS1307_I2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	GPIO_InitStruct.Pull = DS1307_I2C_PUPD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(DS1307_I2C_GPIO_PORT,&GPIO_InitStruct);

	GPIO_InitStruct.Pin = DS1307_I2C_SDA_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	GPIO_InitStruct.Pull = DS1307_I2C_PUPD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(DS1307_I2C_GPIO_PORT, &GPIO_InitStruct);

	// 1. Configure GPIO pins which used for lcd connections

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	GPIO_InitStruct.Pin = LCD_GPIO_RS;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_GPIO_RW;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_GPIO_EN;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_GPIO_D4;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_GPIO_D5;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_GPIO_D6;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_GPIO_D7;
	HAL_GPIO_Init(LCD_GPIO_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RS, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_RW, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_EN, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_GPIO_PORT, LCD_GPIO_D7, GPIO_PIN_RESET);

	lcd_init();

/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*static char *get_day_of_weak(uint8_t dayCode)
{
	char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	return days[dayCode - 1];
}*/

static void number_to_string(uint8_t num, char *buf)
{
	if(num < 10)
	{
		buf[0] = '0';
		buf[1] = num + 48;
	}else if(num >= 10 && num <= 99)
	{
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
	}
}

// hh:mm:ss
static char *time_to_string(RTC_time_t *curr_time)
{
	static char buf[9];

	buf[2] = ':';
	buf[5] = ':';

	number_to_string(curr_time->hours, buf);
	number_to_string(curr_time->minutes, &buf[3]);
	number_to_string(curr_time->seconds, &buf[6]);

	buf[8] = '\0';

	return buf;
}

// dd.mm.yy
static char *date_to_string(RTC_date_t *curr_date)
{
	static char buf[9];

	buf[2] = '.';
	buf[5] = '.';

	number_to_string(curr_date->date, buf);
	number_to_string(curr_date->month, &buf[3]);
	number_to_string(curr_date->year, &buf[6]);

	buf[8] = '\0';

	return buf;
}

static void saveDataIntoEEPROM(void)
{
	HAL_StatusTypeDef ret;

	// get data
	ds1307_get_current_time(&curr_time);
	ds1307_get_current_date(&curr_date);

	ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), 10, 1000);

	if(ret != HAL_OK)
	{
		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C NOT READY");
	}

	// save into ROM
	at24c32_set_data((uint8_t*)time_to_string(&curr_time), strlen(time_to_string(&curr_time)));

	while(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), 10, 100) != HAL_OK);

	at24c32_set_data((uint8_t*)date_to_string(&curr_date), strlen(date_to_string(&curr_date)));

	while(HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), 10, 100) != HAL_OK);

	at24c32_set_data((uint8_t*)meas_buf, strlen(meas_buf));
}

static void get_data(void)
{
	HAL_StatusTypeDef ret;

	ret = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(AT24C32_I2C_ADDR), 10, 1000);

	if(ret != HAL_OK)
	{
		lcd_display_clear();

		lcd_set_cursor(1, 1);

		lcd_print_string("I2C NOT READY");
	}
	at24c32_get_data((uint8_t*)rx_buf, cuur_data, 32);

	cuur_data += 31;

	rx_buf[31] = '\0';
}

/*static void get_data_ds1307(void)
{
	ds1307_get_current_time(&curr_time);
	ds1307_get_current_date(&curr_date);
}*/

static void update_data(void)
{
	bme280_get_data(&data);

	snprintf(meas_buf, 5, "%.2f", data.temperature);
	meas_buf[4] = ',';

	snprintf(&meas_buf[5], 6, "%.2f", data.pressure);
	meas_buf[10] = ',';

	snprintf(&meas_buf[11], 5, "%.2f", data.humidity);
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  if(htim->Instance == TIM2)
  {
	  flag_read_rtc = SET;
	  //printf("TIM2 is working\n")
  }

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
