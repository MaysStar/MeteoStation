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
#include "fatfs.h"

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

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_tx;
DMA_HandleTypeDef hdma_sdio_rx;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

static void number_to_string(uint8_t, char *);

static void time_to_string(RTC_time_t *curr_time, char *buf);
static void date_to_string(RTC_date_t *curr_date, char *buf);
static void get_data(char* buf);
static void update_data(char *buf);
static void sd_create_dir_logs(char *path, int year, int month);
static void sd_logs(void);


//static void saveDataIntoEEPROM(void);

int _write(int fd, unsigned char *buf, int len);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

RTC_time_t curr_time;
RTC_date_t curr_date;

BME280_data_t data;

volatile uint8_t flag_read_rtc = RESET;

// variables for directory creation
static uint32_t curr_day = 0;
static uint32_t curr_month = 0;

// buffer for data
static char path[64];
static char file_path[128];

char rx_buf[26];
char date[9];
char date_csv[13];
char time[9];


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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  lcd_display_clear();
  lcd_print_string("HELLO_WORLD");
  lcd_set_cursor(2, 1);
  lcd_print_string("EVERYTHING_OK");
  lcd_display_clear();

  /* BME280 initialization */
  bme280_init();
  update_data(rx_buf);

  /* DS1307 initialization */
  ds1307_init();

  /* Timer 2 initialization */
  HAL_TIM_Base_Start_IT(&htim2);

  /* DS1307 set data */
  curr_date.day = FRIDAY;
  curr_date.date = 28;
  curr_date.month = 12;
  curr_date.year = 25;

  curr_time.time_format = DS1307_TIME_FORMAT_24HOUR;
  curr_time.hours = 6;
  curr_time.minutes = 5;
  curr_time.seconds = 10;

  ds1307_set_current_date(&curr_date);
  ds1307_set_current_time(&curr_time);

  get_data(rx_buf);

  // /LOGS creation
  sd_mount();
  sd_create_directory("/LOGS");
  sd_unmount();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	if(flag_read_rtc == SET)
	{
		flag_read_rtc = RESET;

		sd_logs();

		lcd_set_cursor(1, 1);

		char *am_pm;
		if(curr_time.time_format != DS1307_TIME_FORMAT_24HOUR)
		{
			am_pm = (curr_time.time_format) ? "PM" : "AM";

			lcd_print_string(rx_buf);
			lcd_print_string(am_pm);
		}
		else
		{
			memcpy(time, rx_buf, 8);
			time[8] = '\n';

			lcd_print_string(time);
			lcd_set_cursor(1, 9);
			lcd_print_string(date);
		}

		// after time set temperature, pressure and humidity
		lcd_set_cursor(2, 1);
		lcd_print_string(&rx_buf[9]);

		//sd_write_file(, text)
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
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
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  //1) Initialize SD peripheral (this performs card init in 1-bit) */

  if (HAL_SD_Init(&hsd) != HAL_OK)
  {
	  printf("HAL_SD_Init failed!\r\n");
      Error_Handler();
  }

  printf("HAL_SD_Init OK\n");

  // switch to 4-bit AFTER init
  if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) == HAL_OK)
  {
	  printf("4-bit mode enabled \r\n");
  }
  else
  {
	  printf("Failed to switch to 4-bit mode\r\n");
  }

  /* USER CODE END SDIO_Init 2 */

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
  htim2.Init.Period = 99999;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  	 /*Configure GPIO pin : PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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

  /* SDIO 4-bit pins D0-D3  initialization */
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
static void time_to_string(RTC_time_t *curr_time, char *buf)
{
	buf[2] = ':';
	buf[5] = ':';

	number_to_string(curr_time->hours, buf);
	number_to_string(curr_time->minutes, &buf[3]);
	number_to_string(curr_time->seconds, &buf[6]);

	buf[8] = ';';
}

// dd.mm.yy
static void date_to_string(RTC_date_t *curr_date, char *buf)
{
	buf[2] = '.';
	buf[5] = '.';

	number_to_string(curr_date->date, buf);
	number_to_string(curr_date->month, &buf[3]);
	number_to_string(curr_date->year, &buf[6]);

	buf[8] = '\0';
}

static void sd_create_dir_logs(char *path, int year, int month)
{
	sd_mount();
	year += 2000;

	// /LOGS
    sd_create_directory("/LOGS");

    // /LOGS/YYYY
    sprintf(path, "/LOGS/%04d", year);
    sd_create_directory(path);

    // /LOGS/YYYY/MM
    sprintf(path, "/LOGS/%04d/%02d", year, month);
    sd_create_directory(path);

    sd_unmount();
}

/*static void saveDataIntoEEPROM(void)
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

	cuur_data += 32;
	if(cuur_data >= 4095) cuur_data = 0;

	rx_buf[32] = '\0';
}*/

static void get_data(char* buf)
{
	// get data
	ds1307_get_current_time(&curr_time);
	ds1307_get_current_date(&curr_date);

	/* Seve data into rx_buffer */
	time_to_string(&curr_time, rx_buf);
	date_to_string(&curr_date, date);
}

static void update_data(char *buf)
{
	bme280_get_data(&data);

	memset(&buf[9], 'A', 16);

	snprintf(&buf[9], 16, "%.1f;%.1f;%.2f", data.temperature, data.pressure, data.humidity);
}

static void sd_logs(void)
{
	update_data(rx_buf);
	get_data(rx_buf);

	/* create directory for new month or year */
	if(curr_month != curr_date.month)
	{
		curr_month = curr_date.month;
		sd_create_dir_logs(path, curr_date.year, curr_date.month);

		memcpy(file_path, path, 14);
		file_path[13] = '/';
	}
	// end month directory creation

	// Write data into day file
	sd_mount();

	// if we start new day
	if(curr_day != curr_date.date)
	{
		curr_day = curr_date.date;

		date_csv[0] = 'D';
		memcpy(&date_csv[1], date, 8);
		date_csv[9] = '.';
		date_csv[10] = 'c';
		date_csv[11] = 's';
		date_csv[12] = 'v';

		memcpy(file_path + 14, date_csv, sizeof(date_csv));
		file_path[13] = '/';

		sd_append_file(file_path, "time;temperature;pressure;humidity\r\n");
	}
	rx_buf[24] = '\r';
	rx_buf[25] = '\n';
	sd_append_file(file_path, rx_buf);

	sd_unmount();
	/* end day file write */
}

int _write(int fd, unsigned char *buf, int len) {
  if (fd == 1 || fd == 2) {
    HAL_UART_Transmit(&huart2, buf, len, 999);  // Print to the UART
  }
  return len;
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
