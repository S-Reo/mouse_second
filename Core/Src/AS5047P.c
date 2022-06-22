
#include "AS5047P.h"

//読み書き対象のアドレスのビットとr/wのビットから偶数parityビットを計算する
//15bit中の1の個数が偶数なら0そうでなければ1

//パリティビットを含めたコマンド値を求めてTransmitに投げる
#if 0
uint16_t as5047p_read(uint16_t reg, int dev_num){
    uint16_t ret,val;

    CS_RESET_1
	HAL_SPI_Transmit(&hspi3,&ret,1,100);
	HAL_SPI_Receive(&hspi3,&val,1,100);
	CS_SET_1
}
uint16_t as5047p_write(uint16_t reg){
    uint16_t ret,val;
    CS_RESET_2
	HAL_SPI_Transmit(&hspi3,&ret,1,100);
	HAL_SPI_Receive(&hspi3,&val,1,100);
	CS_SET_2
}
uint8_t ENC_init(){
    return 0;
}
#else
/**
 * @file as5047p.c
 * @author ZiTe (honmonoh@gmail.com)
 * @brief  A library for AMS AS5047P rotary position sensor/magnetic encoder.
 * @copyright MIT License, Copyright (c) 2022 ZiTe
 *
 */

//#include "as5047p.h"

#define BIT_MODITY(src, n, val) ((src) ^= (-(val) ^ (src)) & (1UL << (n)))
#define BIT_READ(src, n) (((src) >> (n)&1U))
#define BIT_TOGGLE(src, n) ((src) ^= 1UL << (n))

bool is_even_parity(uint16_t data);
void as5047p_spi_transmit(uint16_t data);
uint16_t as5047p_spi_receive();
void delay(volatile uint16_t t);
int spi3_dev_num=0;

void as5047p_dev_num_select(int n){
    spi3_dev_num = n;
}

__attribute__((weak)) void as5047p_spi_send(uint16_t data)
{
  /* NOTE: This function should not be modified,
           it can be implemented in the user file.

     EXAMPLE:
     HAL_SPI_Transmit(&hspi1, (uint8_t *)&data, 1, HAL_MAX_DELAY);
   */
}

__attribute__((weak)) uint16_t as5047p_spi_read(void)
{
    uint16_t data = 0;
  /* NOTE: This function should not be modified,
           it can be implemented in the user file.

     EXAMPLE:
     uint16_t data = 0;
     HAL_SPI_Receive(&hspi1, (uint8_t *)&data, 1, HAL_MAX_DELAY);
    return data;
   */
  return data;
}

__attribute__((weak)) void as5047p_spi_select(void)
{
  /* NOTE: This function should not be modified,
           it can be implemented in the user file.

     EXAMPLE:
     HAL_GPIO_WritePin(AS5047P_SS_GPIO_Port, AS5047P_SS_Pin, GPIO_PIN_RESET);
   */
}

__attribute__((weak)) void as5047p_spi_deselect(void)
{
  /* NOTE: This function should not be modified,
           it can be implemented in the user file.

     EXAMPLE:
     HAL_GPIO_WritePin(AS5047P_SS_GPIO_Port, AS5047P_SS_Pin, GPIO_PIN_SET);
   */
}

void as5047p_send_command(bool is_read_cmd, uint16_t address)
{
  uint16_t frame = address & 0x3FFF;

  /* R/W: 0 for write, 1 for read. */
  BIT_MODITY(frame, 14, is_read_cmd ? 1 : 0);

  /* Parity bit(even) calculated on the lower 15 bits. */
  if (!is_even_parity(frame))
  {
    BIT_TOGGLE(frame, 15);
  }

  as5047p_spi_transmit(frame);
}

void as5047p_send_data(uint16_t address, uint16_t data)
{
  uint16_t frame = data & 0x3FFF;

  /* Data frame bit 14 always low(0). */
  BIT_MODITY(frame, 14, 0);

  /* Parity bit(even) calculated on the lower 15 bits. */
  if (!is_even_parity(frame))
  {
    BIT_TOGGLE(frame, 15);
  }

  as5047p_send_command(false, address);
  as5047p_spi_transmit(frame);
}

uint16_t as5047p_read_data(uint16_t address)
{
  as5047p_send_command(true, address);
  uint16_t received_data = as5047p_spi_receive();
  return received_data;
}

int as5047p_init(uint8_t settings1, uint8_t settings2)
{
  /* SETTINGS1 bit 0 --> Factory Setting: Pre-Programmed to 1. */
  BIT_MODITY(settings1, 0, 1);

  /* SETTINGS1 bit 1 --> Not Used: Pre-Programmed to 0, must not be overwritten. */
  BIT_MODITY(settings1, 1, 0);

  as5047p_send_data(AS5047P_SETTINGS1, (uint16_t)(settings1 & 0x00FF));
  as5047p_send_data(AS5047P_SETTINGS2, (uint16_t)(settings2 & 0x00FF));

  if (as5047p_read_data(AS5047P_ERRFL) != 0)
  {
    return -1; /* Error occurred. */
  }
  return 0; /* No error occurred. */
}

int as5047p_get_angle(bool with_daec, float *angle_degree)
{
  uint16_t address;
  if (with_daec)
  {
    /* Measured angle WITH dynamic angle error compensation(DAEC). */
    address = AS5047P_ANGLECOM;
  }
  else
  {
    /* Measured angle WITHOUT dynamic angle error compensation(DAEC). */
    address = AS5047P_ANGLEUNC;
  }

  uint16_t data = as5047p_read_data(address);
  if (BIT_READ(data, 14) == 0)
  {
    /* Angle in degree = value * ( 360 / 2^14). */
    *angle_degree = (data & 0x3FFF) * (360.0 / 0x4000);
    return 0; /* No error occurred. */
  }
  return -1; /* Error occurred. */
}

void as5047p_set_zero(uint16_t position)
{
  /* 8 most significant bits of the zero position. */
  as5047p_send_data(AS5047P_ZPOSM, ((position >> 6) & 0x00FF));

  /* 6 least significant bits of the zero position. */
  as5047p_send_data(AS5047P_ZPOSL, (position & 0x003F));

  as5047p_nop();
}

inline void as5047p_nop(void)
{
  /*
   * Reading the NOP register is equivalent to a nop (no operation)
   * instruction for the AS5047P.
   */
  as5047p_send_command(true, AS5047P_NOP);
}

void as5047p_spi_transmit(uint16_t data)
{
  delay(T_CSN_DELAY);
  as5047p_spi_select();
  as5047p_spi_send(data);
  as5047p_spi_deselect();
}

uint16_t as5047p_spi_receive()
{
  delay(T_CSN_DELAY);
  as5047p_spi_select();
  uint16_t data = as5047p_spi_read();
  as5047p_spi_deselect();
  return data;
}

void delay(volatile uint16_t t)
{
  while (t--)
  {
    __asm__("nop"); /* Do nothing. */
  }
}

bool is_even_parity(uint16_t data)
{
  uint8_t shift = 1;
  while (shift < (sizeof(data) * 8))
  {
    data ^= (data >> shift);
    shift <<= 1;
  }
  return !(data & 0x1);
}
#endif