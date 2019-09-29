/**
 * @file        sht30_driver.h
 * @author      Steven Daglish
 * @brief
 * @version     0.1
 * @date        04 August 2019
 */

// DONE:    Add functionality to read status register
// DONE:    Add functionality to clear status register
// DONE:    Add Break command
// TODO:    Add more stop commands after fails
// TODO:    Add heater control
// TODO:    Add single shot data acquisition

#ifndef _SHT30_DRIVER_H
#define _SHT30_DRIVER_H

#include "i2c_driver.h"
#include <stdbool.h>
#include <stdint.h>

// TODO:    Add additional defines for register addresses
// TODO:    Change name of DEFAULT_ADDRESS to something better.
// TODO:    Remove the need for MSB and LSB defines.
#define DEFAULT_ADDRESS 0x44
#define SOFT_RESET 0x30A2
#define SOFT_RESET_MSB 0x30
#define SOFT_RESET_LSB 0xA2
#define FETCH_DATA 0xE000
#define FETCH_DATA_MSB 0xE0
#define FETCH_DATA_LSB 0x00
#define READ_STATUS_ADDRESS 0xF32D
#define CLEAR_STATUS_ADDRESS 0x3041
#define BREAK_COMMAND_ADDRESS 0x3093
#define SHT_SINGLE_SHOT_MODE_HIGH_CLOCK_STRETCH 0x2C06

void sht30_driver_create(void);

bool sht30_driver_send_soft_reset(void);

/**
 * @brief
 *
 * @param repeatability     0 = low, 1 = medium, 2 = high
 * @param mps               0 = 0.5, 1 = 1, 2 = 2, 3 = 4, 4 = 10
 * @return true
 * @return false
 */
bool sht30_driver_send_periodic_data_aquisition_mode(uint8_t repeatability,
                                                     uint8_t mps);

/**
 * @brief   Sends a call to the SHT30 device to obtain the temperature and
 * humidity. The data is then stored in internal values for later use.
 *
 * @return true
 * @return false
 */
bool sht30_driver_fetch_periodic_data(void);

/**
 * @brief Returns the latest temperature reading. The temperature reading is the
 * full 16bit value returned from the sht30 ic. No computations have been done
 * on it.
 *
 * @return uint16_t
 */
uint16_t sht30_driver_return_temperature(void);

uint16_t sht30_driver_return_humidity(void);

bool sht30_driver_read_status_register(void);

uint16_t sht30_driver_return_status_register(void);

bool sht30_driver_clear_status_register(void);

bool sht30_driver_break_command(void);

bool sht30_driver_get_single_shot_data(void);

#endif // _SHT30_DRIVER_H
