#ifndef BSP_DS1307_H_
#define BSP_DS1307_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

/* Application configurable items */
#define DS1307_I2C						I2C1
#define DS1307_I2C_SPEED				100000
#define DS1307_I2C_PUPD					GPIO_NOPULL // we don't use internal pull up
#define DS1307_TIMEOUT					1000

/* Register addresses */
#define DS1307_ADDR_SEC					0x00
#define DS1307_ADDR_MIN					0x01
#define DS1307_ADDR_HOUR				0x02
#define DS1307_ADDR_DAY					0x03
#define DS1307_ADDR_DATA				0x04
#define DS1307_ADDR_MONTH				0x05
#define DS1307_ADDR_YEAR				0x06

/* Time formats */
#define DS1307_TIME_FORMAT_12HOUR_AM	0
#define DS1307_TIME_FORMAT_12HOUR_PM	1
#define DS1307_TIME_FORMAT_24HOUR		2

/* Slave address */
#define DS1307_I2C_ADDR					(0x68 << 1)

/* Day definitions */
#define SUNDAY							1
#define MONDAY							2
#define TUESDAY							3
#define WEDNESDAY						4
#define THURSDAY						5
#define FRIDAY							6
#define SATURDAY						7


/* definitions of some structures of date and time */
typedef struct
{
	uint8_t date;
	uint8_t month;
	uint8_t	year;
	uint8_t day;
}RTC_date_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t	hours;
	uint8_t time_format;
}RTC_time_t;

typedef struct
{
	uint8_t data;
	HAL_StatusTypeDef state;
}DS1307_rx_t;

/* Function prototypes */
HAL_StatusTypeDef ds1307_init(void);

/* Set and Get time */
HAL_StatusTypeDef ds1307_set_current_time(RTC_time_t *time);
HAL_StatusTypeDef ds1307_get_current_time(RTC_time_t *time);

/* Set and Get date */
HAL_StatusTypeDef ds1307_set_current_date(RTC_date_t *date);
HAL_StatusTypeDef ds1307_get_current_date(RTC_date_t *date);

#endif /* BSP_DS1307_H_ */
