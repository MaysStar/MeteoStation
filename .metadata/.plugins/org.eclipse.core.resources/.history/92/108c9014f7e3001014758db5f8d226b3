#include "bme280.h"

static BME280_calib_t calib;
static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T);
static BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P);
static BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H);

/*********************************************************************
 * @fn      		  - bme_init
 *
 * @brief             - This function initialize bme modul
 *
 * @param[in]         -
 * @param[in]         -
 * @param[in]         -
 *
 * @return            - none
 *
 * @Note              -

 */

HAL_StatusTypeDef bme280_init(void)
{
	HAL_StatusTypeDef ret;
	uint8_t id;
	uint8_t rx_buf1[BME280_CALIB1_SIZE];
	uint8_t rx_buf2[BME280_CALIB2_SIZE];

	/* Proof the id */
	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_I2C_ADDR | 0x01, BME280_ID_ADDR, 1, &id, 1, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR, 10, BME280_TIMEOUT) != HAL_OK);

	/* Get all calibrating data */
	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_I2C_ADDR | 0x01, BME280_CALIB1_ADDR, 1, rx_buf1, BME280_CALIB1_SIZE, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR, 10, BME280_TIMEOUT) != HAL_OK);

	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_I2C_ADDR | 0x01, BME280_CALIB2_ADDR, 1, rx_buf2, BME280_CALIB2_SIZE, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	/* Set all calibrating data into static definition structure ( calib ) */
	calib.dig_T1 = (uint16_t)(rx_buf1[0] | (rx_buf1[1] << 8));
	calib.dig_T2 = (int16_t)(rx_buf1[2] | (rx_buf1[3] << 8));
	calib.dig_T3 = (int16_t)(rx_buf1[4] | (rx_buf1[5] << 8));

	calib.dig_P1 = (uint16_t)(rx_buf1[6] | (rx_buf1[7] << 8));
	calib.dig_P2 = (int16_t)(rx_buf1[8] | (rx_buf1[9] << 8));
	calib.dig_P3 = (int16_t)(rx_buf1[10] | (rx_buf1[11] << 8));
	calib.dig_P4 = (int16_t)(rx_buf1[12] | (rx_buf1[13] << 8));
	calib.dig_P5 = (int16_t)(rx_buf1[14] | (rx_buf1[15] << 8));
	calib.dig_P6 = (int16_t)(rx_buf1[16] | (rx_buf1[17] << 8));
	calib.dig_P7 = (int16_t)(rx_buf1[18] | (rx_buf1[19] << 8));
	calib.dig_P8 = (int16_t)(rx_buf1[20] | (rx_buf1[21] << 8));
	calib.dig_P9 = (int16_t)(rx_buf1[22] | (rx_buf1[23] << 8));

	calib.dig_H1 = (uint8_t)(rx_buf1[24]);
	calib.dig_H2 = (int16_t)(rx_buf2[0] | rx_buf2[1] << 8);
	calib.dig_H3 = (uint8_t)(rx_buf2[2]);
	calib.dig_H4 = (int16_t)((rx_buf2[3] << 4) | (rx_buf2[4] & 0x0F));
	calib.dig_H5 = (int16_t)((rx_buf2[4] >> 4) | (rx_buf2[5] << 4));
	calib.dig_H6 = (int8_t)(rx_buf2[6]);


	/* Configure bme modul setting data into ( ctrl_hum / ctrl_meas / config ) */
	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR, 10, BME280_TIMEOUT) != HAL_OK);

	// CTRL_HUM configuration
	uint8_t tempreg = BME280_OSRS_H;

	ret = HAL_I2C_Mem_Write(&hi2c1, BME280_I2C_ADDR, BME280_CTRL_HUM_ADDR, 1, &tempreg, 1, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR, 10, BME280_TIMEOUT) != HAL_OK);

	// CTRL_MEAS configuration
	tempreg = 0;
	tempreg = BME280_MODE + BME280_OSRS_P + BME280_OSRS_T;

	ret = HAL_I2C_Mem_Write(&hi2c1, BME280_I2C_ADDR, BME280_CTRL_MEAS_ADDR, 1, &tempreg, 1, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR, 10, BME280_TIMEOUT) != HAL_OK);

	// CONFIG configuration
	tempreg = 0;
	tempreg = BME280_SPI3W_EN + BME280_FILTER + BME280_T_SB;

	ret = HAL_I2C_Mem_Write(&hi2c1, BME280_I2C_ADDR, BME280_CONFIG_ADDR, 1, &tempreg, 1, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	return ret;
}

/*
 * BOSCH FUNCTION
 */

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
static BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T)
{
	BME280_S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((BME280_S32_t)calib.dig_T1<<1))) * ((BME280_S32_t)calib.dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((BME280_S32_t)calib.dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)calib.dig_T1)))
	>> 12) *
	 ((BME280_S32_t)calib.dig_T3)) >> 14;
	calib.t_fine = var1 + var2;
	T = (calib.t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8
//fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
static BME280_U32_t BME280_compensate_P_int64(BME280_S32_t adc_P)
{
	BME280_S64_t var1, var2, p;
	var1 = ((BME280_S64_t)calib.t_fine) - 128000;
	var2 = var1 * var1 * (BME280_S64_t)calib.dig_P6;
	var2 = var2 + ((var1*(BME280_S64_t)calib.dig_P5)<<17);
	var2 = var2 + (((BME280_S64_t)calib.dig_P4)<<35);
	var1 = ((var1 * var1 * (BME280_S64_t)calib.dig_P3)>>8) + ((var1 * (BME280_S64_t)calib.dig_P2)<<12);
	var1 = (((((BME280_S64_t)1)<<47)+var1))*((BME280_S64_t)calib.dig_P1)>>33;
	if (var1 == 0)
	 {
	 return 0; // avoid exception caused by division by zero
	 }
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((BME280_S64_t)calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((BME280_S64_t)calib.dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)calib.dig_P7)<<4);
	return (BME280_U32_t)p;
}
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10
//fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
static BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H)
{
	BME280_S32_t v_x1_u32r;

	v_x1_u32r = (calib.t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)calib.dig_H4) << 20) - (((BME280_S32_t)calib.dig_H5) *
	v_x1_u32r)) + ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r *
	((BME280_S32_t)calib.dig_H6)) >> 10) * (((v_x1_u32r * ((BME280_S32_t)calib.dig_H3)) >> 11) +
	((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)calib.dig_H2) +
	8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
	((BME280_S32_t)calib.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (BME280_U32_t)(v_x1_u32r>>12);
}

HAL_StatusTypeDef bme280_get_data(BME280_data_t* data)
{
	/* this value x100 or x1000 */
	BME280_S32_t temp, press, hum;
	HAL_StatusTypeDef ret;
	uint8_t rx_buf[3];

	/* get temperature */
	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_I2C_ADDR | 0x01, BME280_TEMPERATURE_ADDR, 1, rx_buf, 3, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	temp = (BME280_S32_t)((rx_buf[0] << 12) | (rx_buf[1] << 4) | (rx_buf[2] >> 4));

	// DegC
	data->temperature = (BME280_compensate_T_int32(temp) / 100.0);

	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR | 0x01, 10, BME280_TIMEOUT) != HAL_OK);

	/* get humidity */
	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_I2C_ADDR | 0x01, BME280_HUMIDITY_ADDR, 1, rx_buf, 2, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;


	hum = (BME280_S32_t)((rx_buf[0] << 8) | rx_buf[1]);

	// %rH
	data->humidity = (bme280_compensate_H_int32(hum) / 1024.0);

	while(HAL_I2C_IsDeviceReady(&hi2c1, BME280_I2C_ADDR | 0x01, 10, BME280_TIMEOUT) != HAL_OK);

	/* get pressure */
	ret = HAL_I2C_Mem_Read(&hi2c1, BME280_I2C_ADDR | 0x01, BME280_PRESSURE_ADDR, 1, rx_buf, 3, BME280_TIMEOUT);
	if(ret != HAL_OK) return ret;

	// hPa
	press = (BME280_S32_t)((rx_buf[0] << 12) | (rx_buf[1] << 4) | (rx_buf[2] >> 4));

	data->pressure = (BME280_compensate_P_int64(press) / 25600.0);

	return ret;
}
