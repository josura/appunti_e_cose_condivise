/*
 * adc.h
 */

#ifndef __I2CX_H
#define __I2CX_H

#include <stm32f4xx.h>

void I2C_init(I2C_TypeDef * m_I2C, int m_speed);
int I2C_read_register(I2C_TypeDef * m_I2C, short address, short location, short * data);
int I2C_read_16_bit_register(I2C_TypeDef * m_I2C, short address, short location, int * data);
int I2C_read_buffer(I2C_TypeDef * m_I2C, short address, short location, unsigned char * data, int data_len);
int I2C_write_register(I2C_TypeDef * m_I2C, short address,short location,short data);
int I2C_write_buffer(I2C_TypeDef * m_I2C, short address, short location, unsigned char * data, int len);
int I2C_read_buffer_raw(I2C_TypeDef * m_I2C, short address, unsigned char * data, int data_len);
int I2C_write_buffer_raw(I2C_TypeDef * m_I2C, short address, unsigned char * data, int len);

#endif
