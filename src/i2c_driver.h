
/**
 * @file    i2c_driver.h
 * @author  Steven Daglish
 * @brief
 * @version 0.1
 * @date    01 August 2019
 *
 */

#ifndef _I2C_DRIVER_H
#define _I2C_DRIVER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __PIC24FJ256GA702__
#include <xc.h>
#endif

void i2c_driver_create(void);

void i2c_driver_start(void);

void i2c_driver_stop(void);

/**
 * @brief
 *
 * @return true     Ack received
 * @return false    Ack not received
 */
bool i2c_driver_send_address_write(uint8_t address);

bool i2c_driver_send_address_read(uint8_t address);

/**
 * @brief
 *
 * @param data
 * @return true     Ack received
 * @return false    Ack not received
 */
bool i2c_driver_send_data(uint8_t data);

uint8_t i2c_driver_read_data(bool ack);

#endif // _I2C_DRIVER_H
