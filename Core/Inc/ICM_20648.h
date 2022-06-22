/*
 * ICM_20648.h
 *
 *  Created on: Jun 9, 2021
 *      Author: leopi
 */

#ifndef INC_ICM_20648_H_
#define INC_ICM_20648_H_

#include <main.h>
#include <stdint.h>
extern SPI_HandleTypeDef hspi2;
// IMUから取得したデータ
extern volatile int16_t 	xa, ya, za; // 加速度(16bitデータ)
extern volatile int16_t 	xg, yg, zg;	// 角加速度(16bitデータ)
extern float zg_offset, ya_offset;
//extern uint8_t val[2];
//extern int16_t spi_dma_data;
extern float  ZGyro, YAccel;

#define CS_RESET HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define CS_SET   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)

void IMU_DMA_Start();
uint8_t read_byte( uint8_t );
float ReadIMU(uint8_t a, uint8_t b);
void write_byte( uint8_t, uint8_t);
uint8_t IMU_init(void);
void read_gyro_data(void);
void read_zg_data(void);
void read_accel_data(void);
void IMU_Calib();
double lowpass_filter_double(double x, double x0, double r);

float lowpass_filter_float(float x, float x0, float r);
/*
Gyro		レンジ(dps)	スケールファクター(LSB/dps)　dps = 生/スケールファクター
					±250				131
					±500				65.5
					±1000				32.8
					±2000				16.4
Accel		レンジ(g)		スケールファクター(LSB/g)
					±2					16384
					±4					8192
					±8					4096
					±16					2048
*/



#endif /* INC_ICM_20648_H_ */
